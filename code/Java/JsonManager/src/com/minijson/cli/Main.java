package com.minijson.cli;

import com.minijson.db.MiniDB;
import com.minijson.db.Repository;
import com.minijson.entity.Course;
import com.minijson.entity.Student;

import java.nio.file.Path;
import java.util.Scanner;

/**
 * Mini JSON DB REPL 入口。
 */
public class Main {
    public static void main(String[] args) throws Exception {
        MiniDB db = new MiniDB(Path.of("data"));
        Repository<Student> stuRepo = db.register("student", Student.class, "id");
        db.register("course", Course.class, "code");
        db.loadAll();

        seedIfEmpty(stuRepo);
        QueryEngine engine = new QueryEngine(db);

        Scanner sc = new Scanner(System.in);
        System.out.println("Mini JSON DB > 输入 HELP 查看命令，QUIT 退出");
        while (true) {
            System.out.print("\n> ");
            if (!sc.hasNextLine()) break;
            String line = sc.nextLine().trim();
            if (line.isEmpty()) continue;
            String upper = line.toUpperCase();

            try {
                if (upper.equals("QUIT") || upper.equals("EXIT")) {
                    db.saveAll();
                    System.out.println("已保存。再见");
                    return;
                }
                if (upper.equals("HELP")) {
                    System.out.println("""
                            支持命令：
                              SHOW TABLES
                              SELECT <table>
                              SELECT <table> WHERE <field> <op> <value>
                                op: =  !=  >  >=  <  <=  ~（字符串包含）
                              INSERT student <id> <name> <age>
                              DELETE <table> <id>
                              QUIT / EXIT""");
                    continue;
                }
                if (upper.equals("SHOW TABLES")) {
                    db.tableNames().forEach(t ->
                            System.out.println(" - " + t + " (" + db.table(t).size() + " 行)"));
                    continue;
                }
                if (upper.startsWith("INSERT")) {
                    String[] p = line.split("\\s+");
                    if (p.length != 5 || !p[1].equalsIgnoreCase("student")) {
                        System.out.println("仅支持: INSERT student <id> <name> <age>");
                        continue;
                    }
                    stuRepo.save(new Student(p[2], p[3], Integer.parseInt(p[4]), ""));
                    System.out.println("已插入");
                    continue;
                }
                if (upper.startsWith("DELETE")) {
                    String[] p = line.split("\\s+");
                    if (p.length != 3) { System.out.println("用法: DELETE <table> <id>"); continue; }
                    boolean ok = db.table(p[1]).delete(p[2]);
                    System.out.println(ok ? "已删除" : "不存在");
                    continue;
                }

                // SELECT
                engine.execute(line).forEach(o -> System.out.println("  " + o));

            } catch (Exception e) {
                System.out.println("✗ " + e.getClass().getSimpleName() + ": " + e.getMessage());
            }
        }
    }

    static void seedIfEmpty(Repository<Student> r) {
        if (r.size() > 0) return;
        r.save(new Student("S001", "张三", 20, ""));
        r.save(new Student("S002", "李四", 21, ""));
        r.save(new Student("S003", "王五", 19, ""));
        r.save(new Student("S004", "赵六", 22, ""));
        System.out.println("[初始化] 种子数据加载完毕");
    }
}
