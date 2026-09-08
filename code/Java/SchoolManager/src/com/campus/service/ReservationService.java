package com.campus.service;

import com.campus.entity.Reservation;
import com.campus.util.CsvUtil;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.NavigableMap;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.TreeMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * 预约服务：双索引（HashMap 主键 + TreeMap 时间）+ 冲突检测 + CSV 持久化。
 */
public class ReservationService {

    /** 索引 1：主键索引 —— 按预约号 O(1) 查 */
    private final Map<String, Reservation> byId = new HashMap<>();

    /** 索引 2：时间索引 —— 按 (roomId → 按 startTime 排序的 TreeMap) */
    private final Map<String, TreeMap<LocalDateTime, Reservation>> byRoomTime = new HashMap<>();

    /** ID 生成器 */
    private static final AtomicLong SEQ = new AtomicLong(0);

    public static String nextResvId() {
        String date = LocalDate.now().format(DateTimeFormatter.ofPattern("yyyyMMdd"));
        return String.format("R%s-%04d", date, SEQ.incrementAndGet());
    }

    /** 检测冲突：返回第一个冲突预约（empty 表示无冲突） */
    public Optional<Reservation> findConflict(String roomId,
                                              LocalDateTime start,
                                              LocalDateTime end) {
        TreeMap<LocalDateTime, Reservation> roomMap = byRoomTime.get(roomId);
        if (roomMap == null) return Optional.empty();

        // 区间查询：找 startTime 在 [start-24h, end) 范围内的候选
        NavigableMap<LocalDateTime, Reservation> candidates =
                roomMap.subMap(start.minusHours(24), true, end, false);

        for (Reservation r : candidates.values()) {
            if (r.getStatus().name().equals("CANCELED") ||
                r.getStatus().name().equals("REJECTED")) {
                continue;       // 已取消/驳回的不算冲突
            }
            // 重叠条件：start < r.end AND end > r.start
            if (start.isBefore(r.getEndTime()) && end.isAfter(r.getStartTime())) {
                return Optional.of(r);
            }
        }
        return Optional.empty();
    }

    /** 创建预约（先检冲突，再原子写双索引） */
    public Reservation reserve(String userId, String roomId,
                               LocalDateTime start, LocalDateTime end) {
        Optional<Reservation> conflict = findConflict(roomId, start, end);
        if (conflict.isPresent()) {
            throw new IllegalStateException(
                    "时段冲突：已存在预约 " + conflict.get().getId());
        }
        Reservation r = new Reservation(nextResvId(), userId, roomId, start, end);
        addReservationConsistently(r);
        return r;
    }

    /** 原子写双索引（任何写操作都必须走这里） */
    private void addReservationConsistently(Reservation r) {
        byId.put(r.getId(), r);
        byRoomTime.computeIfAbsent(r.getRoomId(), k -> new TreeMap<>())
                  .put(r.getStartTime(), r);
    }

    /** 原子删双索引 */
    private void removeReservationConsistently(Reservation r) {
        byId.remove(r.getId());
        TreeMap<LocalDateTime, Reservation> roomMap = byRoomTime.get(r.getRoomId());
        if (roomMap != null) {
            roomMap.remove(r.getStartTime());
            if (roomMap.isEmpty()) byRoomTime.remove(r.getRoomId());
        }
    }

    public void approve(String reservationId) {
        Reservation r = byId.get(reservationId);
        if (r == null) throw new NoSuchElementException("预约不存在: " + reservationId);
        r.approve();
    }

    public void reject(String reservationId) {
        Reservation r = byId.get(reservationId);
        if (r == null) throw new NoSuchElementException("预约不存在: " + reservationId);
        r.reject();
    }

    public void cancel(String reservationId) {
        Reservation r = byId.get(reservationId);
        if (r == null) throw new NoSuchElementException("预约不存在: " + reservationId);
        r.cancel();
        // 取消后是否从索引移除是业务决策——本案例保留以备审计
    }

    public Optional<Reservation> findById(String id) {
        return Optional.ofNullable(byId.get(id));
    }

    public List<Reservation> findByUser(String userId) {
        return byId.values().stream()
                .filter(r -> r.getUserId().equals(userId))
                .sorted(Comparator.comparing(Reservation::getStartTime))
                .collect(Collectors.toList());
    }

    /** 查某房间在 [from, to) 之间的预约 —— TreeMap.subMap 高效区间查询 */
    public List<Reservation> findRoomReservationsBetween(String roomId,
                                                         LocalDateTime from,
                                                         LocalDateTime to) {
        TreeMap<LocalDateTime, Reservation> roomMap = byRoomTime.get(roomId);
        if (roomMap == null) return Collections.emptyList();
        return new ArrayList<>(roomMap.subMap(from, true, to, false).values());
    }

    public int size() { return byId.size(); }

    // ============ CSV 持久化 ============

    public void save(Path path) {
        try {
            List<String> lines = byId.values().stream()
                    .sorted(Comparator.comparing(Reservation::getId))
                    .map(this::toCsv)
                    .collect(Collectors.toList());
            Files.write(path, lines, UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.TRUNCATE_EXISTING);
        } catch (IOException e) {
            throw new RuntimeException("保存预约失败", e);
        }
    }

    public void load(Path path) {
        if (!Files.exists(path)) return;
        try {
            byId.clear();
            byRoomTime.clear();
            for (String line : Files.readAllLines(path, UTF_8)) {
                if (line.isBlank()) continue;
                Reservation r = fromCsv(line);
                addReservationConsistently(r);   // 重建双索引
            }
        } catch (IOException e) {
            throw new RuntimeException("加载预约失败", e);
        }
    }

    private String toCsv(Reservation r) {
        return CsvUtil.join(r.getId(), r.getUserId(), r.getRoomId(),
                r.getStartTime().toString(), r.getEndTime().toString(),
                r.getStatus().name());
    }

    private Reservation fromCsv(String line) {
        List<String> p = CsvUtil.split(line);
        Reservation r = new Reservation(p.get(0), p.get(1), p.get(2),
                LocalDateTime.parse(p.get(3)), LocalDateTime.parse(p.get(4)));
        switch (p.get(5)) {
            case "APPROVED" -> r.approve();
            case "REJECTED" -> r.reject();
            case "CANCELED" -> r.cancel();
            default -> { /* PENDING 保持默认 */ }
        }
        return r;
    }
}
