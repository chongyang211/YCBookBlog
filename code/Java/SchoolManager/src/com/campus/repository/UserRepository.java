package com.campus.repository;

import com.campus.entity.Admin;
import com.campus.entity.Student;
import com.campus.entity.Teacher;
import com.campus.entity.User;
import com.campus.enums.Role;
import com.campus.util.CsvUtil;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.stream.Collectors;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * 用户仓库：HashMap 按 ID O(1) 查找 + CSV 持久化。
 */
public class UserRepository {

    /** 选用 HashMap：按 ID 查找 O(1) */
    private final Map<String, User> users = new HashMap<>();

    /** 注册（重复 ID 抛异常） */
    public void register(User u) {
        Objects.requireNonNull(u, "用户不能为 null");
        if (users.containsKey(u.getId())) {
            throw new IllegalArgumentException("用户 ID 已存在: " + u.getId());
        }
        users.put(u.getId(), u);
    }

    /** 按 ID 查找（找不到返回 Optional.empty） */
    public Optional<User> findById(String id) {
        return Optional.ofNullable(users.get(id));
    }

    /** 按 ID 删除（返回是否真的删了） */
    public boolean removeById(String id) {
        return users.remove(id) != null;
    }

    /** 按角色批量删除（用 removeIf，避免 CME） */
    public int removeByRole(Role role) {
        int before = users.size();
        users.values().removeIf(u -> u.getRole() == role);
        return before - users.size();
    }

    /** 总数 */
    public int size() { return users.size(); }

    /** 全部用户（防御性拷贝） */
    public Collection<User> findAll() {
        return new ArrayList<>(users.values());
    }

    /** 按角色筛选（Stream + Lambda） */
    public List<User> findAllByRole(Role role) {
        return users.values().stream()
                .filter(u -> u.getRole() == role)
                .sorted(Comparator.comparing(User::getId))
                .collect(Collectors.toList());
    }

    /** 统计每种角色的人数 */
    public Map<Role, Long> countByRole() {
        return users.values().stream()
                .collect(Collectors.groupingBy(
                        User::getRole,
                        Collectors.counting()));
    }

    // ============ CSV 持久化 ============

    public void save(Path path) {
        try {
            List<String> lines = users.values().stream()
                    .sorted(Comparator.comparing(User::getId))
                    .map(this::toCsv)
                    .collect(Collectors.toList());
            Files.write(path, lines, UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.TRUNCATE_EXISTING);
        } catch (IOException e) {
            throw new RuntimeException("保存用户失败", e);
        }
    }

    public void load(Path path) {
        if (!Files.exists(path)) return;
        try {
            users.clear();
            for (String line : Files.readAllLines(path, UTF_8)) {
                if (line.isBlank()) continue;
                User u = fromCsv(line);
                users.put(u.getId(), u);
            }
        } catch (IOException e) {
            throw new RuntimeException("加载用户失败", e);
        }
    }

    private String toCsv(User u) {
        if (u instanceof Student s) {
            return CsvUtil.join("STUDENT", s.getId(), s.getName(), s.getPassword(),
                    s.getDepartment(), s.getStudentNo());
        } else if (u instanceof Teacher t) {
            return CsvUtil.join("TEACHER", t.getId(), t.getName(), t.getPassword(),
                    t.getDepartment(), t.getTitle());
        } else if (u instanceof Admin a) {
            int bits = (a.canApprove() ? 1 : 0)
                    | ((a.canManageUser() ? 1 : 0) << 1)
                    | ((a.canManageRoom() ? 1 : 0) << 2);
            return CsvUtil.join("ADMIN", a.getId(), a.getName(), a.getPassword(),
                    String.valueOf(bits));
        }
        throw new IllegalStateException("未知用户类型: " + u.getClass());
    }

    private User fromCsv(String line) {
        List<String> p = CsvUtil.split(line);
        return switch (p.get(0)) {
            case "STUDENT" -> new Student(p.get(1), p.get(2), p.get(3), p.get(4), p.get(5));
            case "TEACHER" -> new Teacher(p.get(1), p.get(2), p.get(3), p.get(4), p.get(5));
            case "ADMIN"   -> new Admin(p.get(1), p.get(2), p.get(3), Integer.parseInt(p.get(4)));
            default -> throw new IllegalArgumentException("未知用户类型: " + p.get(0));
        };
    }
}
