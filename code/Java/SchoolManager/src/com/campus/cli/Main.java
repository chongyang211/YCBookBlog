package com.campus.cli;

import com.campus.entity.Admin;
import com.campus.entity.Reservation;
import com.campus.entity.Room;
import com.campus.entity.Speech;
import com.campus.entity.Student;
import com.campus.entity.Teacher;
import com.campus.entity.User;
import com.campus.enums.RoomType;
import com.campus.repository.RoomRepository;
import com.campus.repository.UserRepository;
import com.campus.service.ReservationService;
import com.campus.service.SpeechService;

import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;
import java.util.Scanner;

/**
 * 校园身份预约系统 CLI 入口。
 */
public class Main {

    static final Scanner SC = new Scanner(System.in);
    static final UserRepository USERS  = new UserRepository();
    static final RoomRepository ROOMS  = new RoomRepository();
    static final SpeechService  SPEECH = new SpeechService();
    static final ReservationService RESV = new ReservationService();

    public static void main(String[] args) throws Exception {
        Path data = Path.of("data");
        Files.createDirectories(data);
        loadAll(data);
        seedIfEmpty();    // 第一次启动种子数据

        while (true) {
            System.out.println("\n========= 校园身份预约系统 =========");
            System.out.print("请输入用户 ID（或 q 退出）: ");
            String id = SC.nextLine().trim();
            if ("q".equalsIgnoreCase(id)) {
                saveAll(data);
                System.out.println("再见");
                return;
            }
            Optional<User> opt = USERS.findById(id);
            if (opt.isEmpty()) { System.out.println("✗ 用户不存在"); continue; }

            User u = opt.get();
            System.out.print("密码: ");
            String pwd = SC.nextLine().trim();
            if (!u.checkPassword(pwd)) { System.out.println("✗ 密码错误"); continue; }

            System.out.println("✅ 登录成功，欢迎 " + u.getName());
            dispatchByRole(u);     // 多态分发
        }
    }

    // ===== 多态分发 =====

    static void dispatchByRole(User u) {
        while (true) {
            u.showMenu();        // 多态：每个子类自己的菜单
            System.out.print("请选择: ");
            String c = SC.nextLine().trim();
            if ("0".equals(c)) return;

            try {
                switch (u.getRole()) {
                    case STUDENT -> studentAction((Student) u, c);
                    case TEACHER -> teacherAction((Teacher) u, c);
                    case ADMIN   -> adminAction  ((Admin)   u, c);
                }
            } catch (Exception e) {
                System.out.println("✗ " + e.getMessage());
            }
        }
    }

    static void studentAction(Student s, String c) {
        switch (c) {
            case "1" -> ROOMS.findAllOrderByCapacityDesc().forEach(System.out::println);
            case "2" -> {
                System.out.print("房间 ID: ");
                String roomId = SC.nextLine().trim();
                System.out.print("开始时间(yyyy-MM-dd HH:mm): ");
                LocalDateTime st = parseTime(SC.nextLine());
                System.out.print("结束时间(yyyy-MM-dd HH:mm): ");
                LocalDateTime et = parseTime(SC.nextLine());
                Reservation r = RESV.reserve(s.getId(), roomId, st, et);
                System.out.println("✅ 预约成功: " + r.getId());
            }
            case "3" -> RESV.findByUser(s.getId()).forEach(System.out::println);
            case "4" -> {
                System.out.print("预约号: ");
                RESV.cancel(SC.nextLine().trim());
                System.out.println("✅ 取消成功");
            }
            default -> System.out.println("无效选项");
        }
    }

    static void teacherAction(Teacher t, String c) {
        switch (c) {
            case "1" -> {
                System.out.print("演讲 ID: ");
                String id = SC.nextLine().trim();
                System.out.print("标题: ");
                String title = SC.nextLine().trim();
                System.out.print("评分: ");
                double score = Double.parseDouble(SC.nextLine().trim());
                SPEECH.publish(new Speech(id, title, t.getId(), t.getDepartment(), score));
                System.out.println("✅ 发布成功");
            }
            case "2" -> {
                System.out.print("Top K: ");
                SPEECH.topKByScore(Integer.parseInt(SC.nextLine().trim()))
                      .forEach(System.out::println);
            }
            case "3" -> SPEECH.averageScoreByDepartment().forEach(
                    (dept, avg) -> System.out.printf("  %s: %.2f%n", dept, avg));
            default -> System.out.println("无效选项");
        }
    }

    static void adminAction(Admin a, String c) {
        switch (c) {
            case "1" -> {   // 审批预约
                if (!a.canApprove()) { System.out.println("✗ 无审批权限"); return; }
                System.out.print("预约号 + 操作(approve/reject)，空格分隔: ");
                String[] parts = SC.nextLine().trim().split("\\s+");
                if (parts.length != 2) { System.out.println("✗ 格式错误"); return; }
                if ("approve".equals(parts[1])) RESV.approve(parts[0]);
                else                            RESV.reject(parts[0]);
                System.out.println("✅ 操作完成");
            }
            case "2" -> {   // 增删用户
                if (!a.canManageUser()) { System.out.println("✗ 无用户管理权限"); return; }
                System.out.print("删除用户 ID: ");
                if (USERS.removeById(SC.nextLine().trim()))
                    System.out.println("✅ 已删除");
                else System.out.println("✗ 不存在");
            }
            case "3" -> {   // 增删房间
                if (!a.canManageRoom()) { System.out.println("✗ 无房间管理权限"); return; }
                System.out.print("删除房间 ID: ");
                if (ROOMS.remove(SC.nextLine().trim()))
                    System.out.println("✅ 已删除");
                else System.out.println("✗ 不存在");
            }
            case "4" -> ROOMS.recentlyViewed().forEach(System.out::println);
            case "5" -> System.out.printf("用户:%d 房间:%d 演讲:%d 预约:%d%n",
                    USERS.size(), ROOMS.size(), SPEECH.findAll().size(), RESV.size());
            default -> System.out.println("无效选项或无权限");
        }
    }

    // ===== 工具方法 =====

    static LocalDateTime parseTime(String s) {
        return LocalDateTime.parse(s.trim().replace(' ', 'T'));   // yyyy-MM-ddTHH:mm
    }

    static void loadAll(Path data) {
        USERS .load(data.resolve("user.csv"));
        ROOMS .load(data.resolve("room.csv"));
        SPEECH.load(data.resolve("speech.csv"));
        RESV  .load(data.resolve("reservation.csv"));
    }

    static void saveAll(Path data) {
        RESV  .save(data.resolve("reservation.csv"));
        SPEECH.save(data.resolve("speech.csv"));
        ROOMS .save(data.resolve("room.csv"));
        USERS .save(data.resolve("user.csv"));
    }

    /** 第一次启动种子数据 */
    static void seedIfEmpty() {
        if (USERS.size() > 0) return;
        USERS.register(new Student("S001", "张三",   "p", "计算机系", "20240001"));
        USERS.register(new Student("S002", "李四",   "p", "数学系",   "20240002"));
        USERS.register(new Teacher("T001", "王教授", "p", "计算机系", "教授"));
        USERS.register(new Admin  ("A001", "管理员", "p", 0b111));
        ROOMS.add(new Room("R001", RoomType.COMPUTER, 60, List.of("投影")));
        ROOMS.add(new Room("R002", RoomType.MEETING,  30, List.of("白板")));
        ROOMS.add(new Room("R003", RoomType.LAB,     100, List.of("实验台")));
        System.out.println("[初始化] 种子数据加载完毕");
    }
}
