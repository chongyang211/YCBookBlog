package com.campus.service;

import com.campus.entity.Speech;
import com.campus.util.CsvUtil;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.stream.Collectors;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * 演讲服务：PriorityQueue Top-K 排行榜 + Stream 分组聚合 + CSV 持久化。
 */
public class SpeechService {
    private final List<Speech> speeches = new ArrayList<>();

    public void publish(Speech s) { speeches.add(s); }

    public boolean remove(String id) {
        return speeches.removeIf(s -> s.getId().equals(id));
    }

    public List<Speech> findAll() { return new ArrayList<>(speeches); }

    /** Top-K 按评分降序（小顶堆流式算法） */
    public List<Speech> topKByScore(int k) {
        if (k <= 0) return Collections.emptyList();

        // 小顶堆 —— 堆顶是当前 K 个里最小的
        PriorityQueue<Speech> heap = new PriorityQueue<>(
                k, Comparator.comparingDouble(Speech::getScore));

        for (Speech s : speeches) {
            if (heap.size() < k) {
                heap.offer(s);
            } else if (s.getScore() > heap.peek().getScore()) {
                heap.poll();        // 移除堆顶（最小）
                heap.offer(s);      // 加入新元素
            }
        }
        // 堆里是 Top-K，但需要按降序输出
        List<Speech> result = new ArrayList<>(heap);
        result.sort(Comparator.comparingDouble(Speech::getScore).reversed());
        return result;
    }

    /** 按系别求平均分（groupingBy + averagingDouble） */
    public Map<String, Double> averageScoreByDepartment() {
        return speeches.stream()
                .collect(Collectors.groupingBy(
                        Speech::getDepartment,
                        Collectors.averagingDouble(Speech::getScore)));
    }

    /** 按系别分桶（groupingBy + 完整列表） */
    public Map<String, List<Speech>> groupByDepartment() {
        return speeches.stream()
                .collect(Collectors.groupingBy(Speech::getDepartment));
    }

    /** 按是否高分（>= 80）分两组 —— partitioningBy */
    public Map<Boolean, List<Speech>> partitionByHighScore() {
        return speeches.stream()
                .collect(Collectors.partitioningBy(s -> s.getScore() >= 80));
    }

    // ============ CSV 持久化 ============

    public void save(Path path) {
        try {
            List<String> lines = speeches.stream()
                    .map(this::toCsv)
                    .collect(Collectors.toList());
            Files.write(path, lines, UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.TRUNCATE_EXISTING);
        } catch (IOException e) {
            throw new RuntimeException("保存演讲失败", e);
        }
    }

    public void load(Path path) {
        if (!Files.exists(path)) return;
        try {
            speeches.clear();
            for (String line : Files.readAllLines(path, UTF_8)) {
                if (line.isBlank()) continue;
                speeches.add(fromCsv(line));
            }
        } catch (IOException e) {
            throw new RuntimeException("加载演讲失败", e);
        }
    }

    private String toCsv(Speech s) {
        return CsvUtil.join(s.getId(), s.getTitle(), s.getSpeakerId(),
                s.getDepartment(), String.valueOf(s.getScore()));
    }

    private Speech fromCsv(String line) {
        List<String> p = CsvUtil.split(line);
        return new Speech(p.get(0), p.get(1), p.get(2), p.get(3),
                Double.parseDouble(p.get(4)));
    }
}
