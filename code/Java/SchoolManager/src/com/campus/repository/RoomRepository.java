package com.campus.repository;

import com.campus.entity.Room;
import com.campus.enums.RoomType;
import com.campus.util.CsvUtil;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.TreeMap;
import java.util.stream.Collectors;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * 房间仓库：TreeMap 字典序 + LinkedHashMap LRU 最近浏览 + CSV 持久化。
 */
public class RoomRepository {

    /** 选用 TreeMap：按 ID 字典序自动排序 */
    private final Map<String, Room> rooms = new TreeMap<>();

    /** 选用 LinkedHashMap (accessOrder=true) 实现 LRU */
    private final LinkedHashMap<String, Room> recentlyViewed =
            new LinkedHashMap<>(8, 0.75f, true) {
        @Override
        protected boolean removeEldestEntry(Map.Entry<String, Room> eldest) {
            return size() > 5;     // 限制 5 个
        }
    };

    public void add(Room r) {
        if (rooms.containsKey(r.getId())) {
            throw new IllegalArgumentException("房间 ID 已存在: " + r.getId());
        }
        rooms.put(r.getId(), r);
    }

    public Optional<Room> findById(String id) {
        Room r = rooms.get(id);
        if (r != null) recentlyViewed.put(id, r);    // 触发 LRU 排序
        return Optional.ofNullable(r);
    }

    public boolean remove(String id) {
        recentlyViewed.remove(id);
        return rooms.remove(id) != null;
    }

    /** 视图 1：按 ID 字典序（TreeMap 自带） */
    public List<Room> findAllOrderById() {
        return new ArrayList<>(rooms.values());
    }

    /** 视图 2：按容量降序（用 Comparable 自然排序） */
    public List<Room> findAllOrderByCapacityDesc() {
        return rooms.values().stream()
                .sorted()
                .collect(Collectors.toList());
    }

    /** 视图 3：按类型分组 */
    public Map<RoomType, List<Room>> groupByType() {
        return rooms.values().stream()
                .collect(Collectors.groupingBy(Room::getType));
    }

    /** 视图 4：找容量最大的 N 个 */
    public List<Room> topNByCapacity(int n) {
        return rooms.values().stream()
                .sorted()
                .limit(n)
                .collect(Collectors.toList());
    }

    /** 视图 5：最近浏览（LRU，最新在前） */
    public List<Room> recentlyViewed() {
        List<Room> list = new ArrayList<>(recentlyViewed.values());
        Collections.reverse(list);
        return list;
    }

    public int size() { return rooms.size(); }

    // ============ CSV 持久化 ============

    public void save(Path path) {
        try {
            List<String> lines = rooms.values().stream()
                    .map(this::toCsv)
                    .collect(Collectors.toList());
            Files.write(path, lines, UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.TRUNCATE_EXISTING);
        } catch (IOException e) {
            throw new RuntimeException("保存房间失败", e);
        }
    }

    public void load(Path path) {
        if (!Files.exists(path)) return;
        try {
            rooms.clear();
            recentlyViewed.clear();
            for (String line : Files.readAllLines(path, UTF_8)) {
                if (line.isBlank()) continue;
                Room r = fromCsv(line);
                rooms.put(r.getId(), r);
            }
        } catch (IOException e) {
            throw new RuntimeException("加载房间失败", e);
        }
    }

    private String toCsv(Room r) {
        return CsvUtil.join(r.getId(), r.getType().name(),
                String.valueOf(r.getCapacity()),
                String.join("|", r.getEquipments()));
    }

    private Room fromCsv(String line) {
        List<String> p = CsvUtil.split(line);
        return new Room(p.get(0), RoomType.valueOf(p.get(1)),
                Integer.parseInt(p.get(2)),
                Arrays.asList(p.get(3).split("\\|")));
    }
}
