import java.util.Scanner;

/**
 * 学生成绩管理系统（过程式 Java 风格）
 *
 * 刻意使用入门第 1-6 章的知识：static 方法 + 并行数组，
 * 不用 class Student —— 为的是在 02 案例"银行账户管理"里
 * 对比 class 化 + 多态后的价值。
 */
public class StudentScore {

    // ============ 全局静态字段 ============
    static final int MAX_STUDENTS = 50;
    static final String[] SUBJECT_NAMES = { "语文", "数学", "英语" };
    static final int SUBJECT_COUNT = 3;

    static int[]      ids    = new int[MAX_STUDENTS];      // 学号 int
    static String[]   names  = new String[MAX_STUDENTS];   // 姓名 String
    static double[][] scores = new double[MAX_STUDENTS][SUBJECT_COUNT]; // 三科分数
    static int        count  = 0;                          // 当前学生数量

    static final Scanner SC = new Scanner(System.in);      // 全局 Scanner

    // ============ 主入口 ============
    public static void main(String[] args) {
        initData();

        while (true) {
            showMenu();
            String choice = SC.nextLine().trim();   // 用 nextLine 不用 nextInt
            switch (choice) {
                case "1": addStudent(); break;
                case "2": printTable(); break;
                case "3": deleteStudent(); break;
                case "4": findStudent(); break;
                case "5": updateScore(); break;
                case "6": showStats(); break;
                case "7": sortByTotalDesc(); printTable(); break;
                case "0":
                    System.out.println("再见");
                    SC.close();
                    return;
                default:
                    System.out.println("无效选项，请输入 0-7");
            }
        }
    }

    static void showMenu() {
        System.out.println();
        System.out.println("=========== 学生成绩管理 ===========");
        System.out.println("  1. 添加学生");
        System.out.println("  2. 显示学生");
        System.out.println("  3. 删除学生");
        System.out.println("  4. 查找学生");
        System.out.println("  5. 修改成绩");
        System.out.println("  6. 统计计算");
        System.out.println("  7. 排序展示");
        System.out.println("  0. 退出");
        System.out.println("===================================");
        System.out.print("请选择: ");
    }

    // ============ 初始化假数据 ============
    static void initData() {
        int[] initIds = { 1001, 1002, 1003, 1004, 1005 };
        String[] initNames = { "张三", "李四", "王五", "赵六", "钱七" };
        double[][] initScores = {
            { 85.0, 90.0, 78.0 },
            { 72.5, 88.0, 95.5 },
            { 60.0, 55.0, 70.0 },
            { 95.0, 92.0, 88.0 },
            { 45.0, 50.0, 60.0 },
        };
        for (int i = 0; i < initIds.length; i++) {
            ids[i] = initIds[i];
            names[i] = initNames[i];
            for (int j = 0; j < SUBJECT_COUNT; j++) {
                scores[i][j] = initScores[i][j];
            }
        }
        count = initIds.length;
    }

    // ============ CRUD ============

    // 按学号查索引，找不到返回 -1（-1 哨兵约定）
    static int findIndex(int id) {
        for (int i = 0; i < count; i++) {
            if (ids[i] == id) return i;
        }
        return -1;
    }

    static void addStudent() {
        if (count >= MAX_STUDENTS) {
            System.out.println("（成绩表已满，无法添加）");
            return;
        }

        int id = readInt("请输入学号 (1000-9999): ", 1000, 9999);
        if (findIndex(id) != -1) {
            System.out.println("（学号已存在，无法添加）");
            return;
        }

        String name = readNonEmptyString("请输入姓名: ");
        double chinese = readDouble("请输入语文成绩 (0-100): ", 0, 100);
        double math    = readDouble("请输入数学成绩 (0-100): ", 0, 100);
        double english = readDouble("请输入英语成绩 (0-100): ", 0, 100);

        ids[count] = id;
        names[count] = name;
        scores[count][0] = chinese;
        scores[count][1] = math;
        scores[count][2] = english;
        count++;
        System.out.println("添加成功，当前共 " + count + " 名学生");
    }

    static void deleteStudent() {
        int id = readInt("请输入要删除的学号: ");
        int idx = findIndex(id);
        if (idx == -1) {
            System.out.println("（无此学号）");
            return;
        }
        // 末尾填洞法：把最后一个学生搬到 idx 位置，count--
        int last = count - 1;
        ids[idx] = ids[last];
        names[idx] = names[last];
        for (int j = 0; j < SUBJECT_COUNT; j++) {
            scores[idx][j] = scores[last][j];
        }
        names[last] = null;   // 释放末尾引用，帮助 GC
        count--;
        System.out.println("删除成功，当前共 " + count + " 名学生");
    }

    static void findStudent() {
        int id = readInt("请输入要查找的学号: ");
        int idx = findIndex(id);
        if (idx == -1) {
            System.out.println("（无此学号）");
            return;
        }
        System.out.printf("学号:%d  姓名:%s  语:%.1f 数:%.1f 英:%.1f  总分:%.1f%n",
                ids[idx], names[idx],
                scores[idx][0], scores[idx][1], scores[idx][2],
                calcTotal(idx));
    }

    static void updateScore() {
        int id = readInt("请输入要修改的学号: ");
        int idx = findIndex(id);
        if (idx == -1) {
            System.out.println("（无此学号）");
            return;
        }
        int subject = readInt("选择科目 (1=语文 2=数学 3=英语): ", 1, 3);
        double newScore = readDouble("请输入新分数 (0-100): ", 0, 100);

        double oldScore = scores[idx][subject - 1];
        scores[idx][subject - 1] = newScore;   // 注意是 idx，不是 count
        System.out.printf("%s 的%s成绩从 %.1f 改为 %.1f%n",
                names[idx], SUBJECT_NAMES[subject - 1], oldScore, newScore);
    }

    // ============ 统计计算 ============

    static double calcTotal(int idx) {
        double total = 0;
        for (int j = 0; j < SUBJECT_COUNT; j++) {
            total += scores[idx][j];
        }
        return total;
    }

    static double calcAverage(int idx) {
        return calcTotal(idx) / SUBJECT_COUNT;
    }

    static double calcSubjectAvg(int subject) {
        if (count == 0) return 0;   // 除零守卫
        double total = 0;
        for (int i = 0; i < count; i++) {
            total += scores[i][subject];
        }
        return total / count;
    }

    // 打擂台算法找第一名
    static int findTopStudent() {
        if (count == 0) return -1;
        int topIdx = 0;
        double topTotal = calcTotal(0);
        for (int i = 1; i < count; i++) {
            double cur = calcTotal(i);
            if (cur > topTotal) {
                topTotal = cur;
                topIdx = i;
            }
        }
        return topIdx;
    }

    static int countPassed(int subject) {
        int n = 0;
        for (int i = 0; i < count; i++) {
            if (scores[i][subject] >= 60) {   // 及格边界用 >=，不用 > 59
                n++;
            }
        }
        return n;
    }

    static void showStats() {
        if (count == 0) {
            System.out.println("（暂无学生）");
            return;
        }
        System.out.println("--- 统计 ---");
        int top = findTopStudent();
        System.out.printf("第一名: %s (总分 %.1f)%n", names[top], calcTotal(top));
        for (int j = 0; j < SUBJECT_COUNT; j++) {
            System.out.printf("%s 平均分: %.2f, 及格人数: %d/%d%n",
                    SUBJECT_NAMES[j], calcSubjectAvg(j), countPassed(j), count);
        }
    }

    // ============ 排序与展示 ============

    // 冒泡排序：按总分降序
    static void sortByTotalDesc() {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - 1 - i; j++) {
                if (calcTotal(j) < calcTotal(j + 1)) {
                    swapAll(j, j + 1);
                }
            }
        }
    }

    // ⚠️ 并行数组同步交换：三个数组必须一起换，漏一个就数据错乱
    static void swapAll(int a, int b) {
        int tmpId = ids[a];     ids[a] = ids[b];     ids[b] = tmpId;
        String tmpName = names[a]; names[a] = names[b]; names[b] = tmpName;
        double[] tmpScore = scores[a]; scores[a] = scores[b]; scores[b] = tmpScore;
    }

    // 可变参数 char...：调用方可传任意个 char
    static void printSeparator(char... chars) {
        StringBuilder sb = new StringBuilder();
        for (char c : chars) {
            for (int i = 0; i < 12; i++) sb.append(c);
        }
        System.out.println(sb);
    }

    static void printTable() {
        if (count == 0) {
            System.out.println("（暂无学生）");
            return;
        }
        printSeparator('=', '-', '=', '-', '=');
        System.out.printf("%-6s %-10s %8s %8s %8s %8s %8s%n",
                "学号", "姓名", "语文", "数学", "英语", "总分", "平均");
        printSeparator('-', '-', '-', '-', '-');
        for (int i = 0; i < count; i++) {
            System.out.printf("%-6d %-10s %8.1f %8.1f %8.1f %8.1f %8.2f%n",
                    ids[i], names[i],
                    scores[i][0], scores[i][1], scores[i][2],
                    calcTotal(i), calcAverage(i));
        }
        printSeparator('=', '-', '=', '-', '=');
    }

    // ============ 输入校验工具 ============

    static int readInt(String prompt, int min, int max) {
        while (true) {
            System.out.print(prompt);
            String line = SC.nextLine().trim();
            if (!isInteger(line)) {
                System.out.println("（请输入整数）");
                continue;
            }
            int v = Integer.parseInt(line);
            if (v < min || v > max) {
                System.out.printf("（请输入 %d~%d 的整数）%n", min, max);
                continue;
            }
            return v;
        }
    }

    // 重载：不带范围版本
    static int readInt(String prompt) {
        return readInt(prompt, Integer.MIN_VALUE, Integer.MAX_VALUE);
    }

    static double readDouble(String prompt, double min, double max) {
        while (true) {
            System.out.print(prompt);
            String line = SC.nextLine().trim();
            if (!isDouble(line)) {
                System.out.println("（请输入数字）");
                continue;
            }
            double v = Double.parseDouble(line);
            if (v < min || v > max) {
                System.out.printf("（请输入 %.1f~%.1f 的数字）%n", min, max);
                continue;
            }
            return v;
        }
    }

    static String readNonEmptyString(String prompt) {
        while (true) {
            System.out.print(prompt);
            String s = SC.nextLine().trim();
            if (!s.isEmpty()) return s;
            System.out.println("（输入不能为空）");
        }
    }

    static boolean isInteger(String s) {
        if (s == null || s.isEmpty()) return false;
        int i = 0;
        if (s.charAt(0) == '-' || s.charAt(0) == '+') {
            if (s.length() == 1) return false;
            i = 1;
        }
        for (; i < s.length(); i++) {
            if (!Character.isDigit(s.charAt(i))) return false;
        }
        return true;
    }

    static boolean isDouble(String s) {
        if (s == null || s.isEmpty()) return false;
        boolean dot = false;
        int i = 0;
        if (s.charAt(0) == '-' || s.charAt(0) == '+') {
            if (s.length() == 1) return false;
            i = 1;
        }
        for (; i < s.length(); i++) {
            char c = s.charAt(i);
            if (c == '.') {
                if (dot) return false;   // 不能有两个点
                dot = true;
            } else if (!Character.isDigit(c)) {
                return false;
            }
        }
        return true;
    }
}
