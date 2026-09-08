import com.minijson.entity.Course;
import com.minijson.entity.Student;
import com.minijson.json.JsonObject;
import com.minijson.json.JsonParser;
import com.minijson.mapper.ObjectMapper;

import java.util.ArrayList;

public class TestMapper {
    public static void main(String[] args) {
        ObjectMapper mapper = new ObjectMapper();

        // 1. Student round-trip（字段名映射 + @JsonIgnore）
        Student s = new Student("S001", "张三", 20, "secret");
        String j1 = mapper.toJson(s);
        System.out.println("toJson: " + j1);
        Student s2 = mapper.fromJson(j1, Student.class);
        System.out.println("还原: " + s2);
        System.out.println("password 被忽略: " + (s2.password == null));

        // 2. Course 泛型集合 List<Student> 类型擦除突破
        Course c = new Course("CS101", "Java 进阶", new ArrayList<>());
        c.students.add(new Student("S001", "张三", 20, "x"));
        c.students.add(new Student("S002", "李四", 21, "y"));
        String j2 = mapper.toJson(c);
        System.out.println("\nCourse JSON: " + j2);
        Course c2 = mapper.fromJson(j2, Course.class);
        System.out.println("students[0] 类型: " + c2.students.get(0).getClass().getSimpleName());
        System.out.println("students[0]: " + c2.students.get(0));

        // 3. Unicode 转义解析（用 char 拼接构造字面 \u4f60\u597d，避免编译器预转义）
        String backslash = String.valueOf((char) 0x5C);   // "\"
        String json = "{\"greet\":\"" + backslash + "u4f60" + backslash + "u597d\"}";
        JsonObject obj = JsonParser.parse(json).asObject();
        System.out.println("\nUnicode 转义解析: " + obj.get("greet").asString());
    }
}
