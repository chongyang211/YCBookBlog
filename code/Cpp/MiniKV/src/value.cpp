// 实现文件必须先包含自己的头文件
// 【惯例理由】① 提供实现所需的声明；② 让编译器顺带校验"头文件能否独立
//   编译"（若头文件漏 include，最先在这里报错，定位在源头而非使用处）
#include "mkv/value.hpp"

// namespace mkv 和头文件保持一致，否则链接找不到
// 【机制】类外定义的成员/函数必须与声明同 namespace —— 跨 namespace 的
//   声明与定义是两个不同符号，链接期报 undefined reference
namespace mkv {

    // formatValue 的真正实现（对应 value.hpp 里的那行声明）
    std::string formatValue(const Value &v) {
        // 【std::visit：variant 的"多态访问"】对 v 当前持有的备选类型
        //   调用 lambda 的对应实例化 —— 编译期生成 switch(index) 分派，
        //   漏掉任何一个备选类型直接编译失败（比手写 switch 更安全）
        return std::visit([](const auto &x) -> std::string {
            // auto 形参：对每个备选类型各实例化一份 lambda
            using T = std::decay_t<decltype(x)>;
            // decay_t：剥掉 const & 修饰拿裸类型（variant 传的是 const 引用）
            // 覆盖5个分支
            // 【if constexpr：编译期分支】条件在模板实例化时求值 ——
            //   不成立的分支"不实例化"（连代码都不生成），
            //   这是"一份 lambda 处理五种类型"的核心机制（普通 if 做不到：
            //   to_string(bool) 之类不成立分支照样参与编译就报错）
            if constexpr (std::is_same_v<T, Null>) return "(nil)";
            else if constexpr (std::is_same_v<T, bool>) return x ? "true" : "false";
            else if constexpr (std::is_same_v<T, std::int64_t>) return "(integer) " + std::to_string(x);
            else if constexpr (std::is_same_v<T, double>) return "(double) " + std::to_string(x);
            // 【已知局限】to_string(double) 默认 6 位有效数字 ——
            //   3.1415926 显示 "3.14159"（教学简化；精确版需 ostringstream +
            //   setprecision(max_digits10)，见 JsonManager 的同款注释）
            else if constexpr (std::is_same_v<T, std::string>) return "\"" + x + "\"";
            // 字符串加引号：与 Redis 回复格式对齐（肉眼可区分 "30" 与 30）
        }, v);
    }

    // valueFromToken 的真正实现（对应 value.hpp 里的那行声明）
    // 规则：
    //   "true"/"false" -> bool
    //   "nil"          -> Null
    //   全数字（可带负号）-> int64
    //   含小数点且能 parse -> double
    //   其他 -> string
    // 【设计思想】"动态类型"在静态语言里的落地：SET 时无类型标注，
    //   引擎按 token 字面特征推断 —— Redis 的 set/get 同款行为
    Value valueFromToken(std::string_view s) {
        if (s == "true") return true;
        if (s == "false") return false;
        if (s == "nil") return Null{};
        // 三个关键字优先短路：先排除"字面量语义"，再谈数字解析 ——
        // 顺序保证 "true" 永远是 bool 而不是兜底字符串

        // ─── 分支 2：试探 int64 ───────────────────
        if (!s.empty()) {
            bool allDigit = true;
            // 允许开头一个符号
            size_t start = (s[0] == '-' ? 1 : 0);
            // "-" 后必须还有数字（start < s.size() 检查），单 "-" 不算数
            for (size_t i = start; i < s.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
                    // 【unsigned char 转型】isdigit 参数必须 0~255 ——
                    //   char 为负（UTF-8 首字节）时是 UB，转型是强制安全习惯
                    allDigit = false;
                    break;
                }
            }
            if (allDigit && start < s.size()) {
                // stoll 只能吃 std::string，不能直接吃 string_view，此处一次拷贝可接受
                // 【view→string 的必要拷贝】C 接口（strtoll 系）要 C 字符串，
                //   stoll 要 std::string —— 拷贝不可避免，但只发生在"确认像数字"后
                try { return static_cast<std::int64_t>(std::stoll(std::string(s))); }
                catch (...) { /* 溢出等异常吞掉，落到下一分支 */ }
                // 【异常即分支】stoll 对 "99999999999999999999"（超 int64）抛
                //   out_of_range —— 吞掉后自然落进 string 兜底：
                //   溢出的大数当字符串存，语义完全合理（比丢弃/报错都好）
            }
        }

        // ─── 分支 3：试探 double ──────────────────
        if (s.find('.') != std::string_view::npos) {
            // 只对"含小数点"的 token 尝试 double —— 避免对普通字符串
            // 白做一次 stod（find 一次 O(n) 扫描远快于解析失败抛异常）
            try { return std::stod(std::string(s)); }
            catch (...) { /* 解析失败，落到兜底 */ }
            // "1.2.3" 之类含点但解析失败 → 兜底字符串（如 "v1.2" 版本号）
        }

        // ─── 分支 4：兜底当字符串 ─────────────────
        return std::string(s);
        // 顺序即优先级：关键字 > 整数 > 浮点 > 字符串 ——
        //   越靠后越宽容，兜底分支保证 valueFromToken 永不失败
    }
}
