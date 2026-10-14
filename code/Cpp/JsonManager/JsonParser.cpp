//
// Created by 杨充 on 2026/6/5.
//
// ============================================================================
// 【文件定位】JsonParser.cpp —— 递归下降解析器实现（游标/字面量/数字/字符串/容器）
// ----------------------------------------------------------------------------
// 【阅读顺序建议】先看游标四件套 → parse 总入口 → parseValue 分派 →
//   各 parse* 子函数 —— 与教程阶段③④的推进顺序一致。
// ============================================================================

#include "JsonParser.h"
#include <cctype>       // std::isspace / std::isdigit：字符分类
#include <iostream>     // debugDump 的 cout

namespace jsonkv {

// ===== 游标工具 =====

// peek：看一眼当前字符，不动游标
char JsonParser::peek() const {
    // 遇到报错：JsonParser.cpp:12:29: error: incompatible operand types ('char' and 'const char *')
    // '\0' 是字符字面量（类型 char），而 "\0" 是字符串字面量（类型 const char[2]，退化为 const char*）。
    // 【技术原理】pos 越界时返回哨兵 '\0'：让调用方"看"到一个不会匹配任何
    //   JSON 语法的字符，自然走错误分支 —— 免去每处都先判越界的啰嗦。
    return pos < src.size() ? src[pos] : '\0';
}

// consume：吃掉当前字符并前进（游标唯一的前进出口）
char JsonParser::consume() {
    char c = src[pos++];    // 取当前字符、pos 后移一步（调用方已保证不越界）
    if (c == '\n') {
        // 跨行就更新行号
        line++;
        col = 1;            // 新行从第 1 列重新数起
    }
    else { col++; }
    // 【设计要点】行号/列号只在 consume 里维护 —— 游标前进的唯一通道，
    //   保证定位信息永不失真（parseLiteral 里手工 pos+=4 绕过 consume，
    //   就得手工补 col+=4 —— 见该函数注释）。
    return c;
}

// match：条件吃 —— "当前是 c 吗？是就吃掉并报告成功"
bool JsonParser::match(char c) {
    if (peek() == c) { consume(); return true; }
    return false;
    // 【使用场景】if (match(']')) break; —— 把"看+吃+判断"三步并一步，
    //   是解析器里出现频率最高的原语。
}

// skipWhitespace：跳过空白（空格/换行/制表符等）
void JsonParser::skipWhitespace() {
    // 【技术细节】static_cast<unsigned char>：isspace 的参数必须是
    //   0~255 的无符号值 —— 直接传 char 时负值（如 UTF-8 中文首字节）是
    //   未定义行为（常见后果：崩溃或误判），转型是强制安全习惯。
    while (pos < src.size() && std::isspace(static_cast<unsigned char>(src[pos]))) {
        consume();
    }
}

// error：抛带定位信息的解析异常（唯一出口，全解析器统一报错格式）
void JsonParser::error(const std::string& msg) {
    throw JsonParseError(msg, line, col);
    // 异常会沿调用栈展开（parseObject → parseValue → parse 一路抛出去），
    // 每层的局部对象（临时 vector/map）由栈展开自动析构 —— RAII 的价值。
}

// debugDump：打印游标状态（教学调试用：观察"解析走到哪了"）
void JsonParser::debugDump() const{
    std::cout << "src len=" << src.size()
          << " pos="    << pos
          << " line="   << line
          << " col="    << col << "\n";
}

// ===== 总入口 =====
// parse：解析"恰好一个 JSON 值"，且后面不许有垃圾
JsonNodePtr JsonParser::parse() {
    skipWhitespace();
    auto node = parseValue();       // 解析第一个（也是唯一一个）值
    skipWhitespace();
    // 尾部垃圾检查：'{"a":1} xxx' 的 xxx 就是垃圾 —— JSON 文档必须
    // "一个值写到底"，多余字符说明输入不是合法 JSON（而不是悄悄忽略）
    if (pos < src.size()) error("trailing characters after value");
    return node;
}

// ===== 数字 =====
// 手写状态机：符号 → 整数位 → [小数点+小数位] → [e/E+符号+指数位]
JsonNodePtr JsonParser::parseNumber() {
    size_t start = pos;             // 记住起点：最后截取这整段文本
    if (match('-')){}               // 可选负号（match 自带"有则吃"语义）
    while (std::isdigit((static_cast<unsigned char>(peek())))) consume();   // 整数部分：连续数字
    if (peek() == '.') {                                                       // 小数部分
        consume();                 // 吃掉 '.'
        while (std::isdigit(static_cast<unsigned char>(peek()))) consume();   // 小数位
    }
    if (peek() == 'e' || peek() == 'E') {                                      // 科学计数法
        consume();                 // 吃掉 e/E
        if (peek() == '+' || peek() == '-') consume();   // 可选指数符号
        while (std::isdigit(static_cast<unsigned char>(peek()))) consume();   // 指数位
    }
    try {
        // 【实现策略】字符收集交给状态机，"文本→double"复用标准库 stod：
        //   自己处理浮点解析精度是深坑（strtod 是几十年的成熟实现）。
        //   src.substr(...) 是 string_view::substr（零拷贝视图），构造
        //   std::string 再交给 stod —— 一次小拷贝换取正确性。
        double v = std::stod(std::string(src.substr(start, pos - start)));
        return makeNumber(v);
    } catch (...) {
        error("invalid number");    // stod 失败（如 "-"、溢出）→ 统一格式报错
    }
    // 注：error 是 [[noreturn]]，此处不需要 return
}

// ===== 字符串 =====
// 状态机：开引号 → [普通字符 | 转义序列]* → 闭引号
JsonNodePtr JsonParser::parseString() {
    if (!match('"')) error("expected '\"'");     // 首字符必须是引号（顺带吃掉）
    std::string result;                          // 逐字符攒结果
    while (pos < src.size() && peek() != '"') {  // 直到闭引号或源耗尽
        char c = consume();
        if (c == '\\') {                                  // ⭐ 转义起始
            char esc = consume();                         // 反斜杠后的转义字符
            switch (esc) {
                // 8 种标准转义：JSON 规范规定"输入这两字符 → 输出一个控制字符"
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case '/':  result += '/';  break;
                case 'n':  result += '\n'; break;
                case 't':  result += '\t'; break;
                case 'r':  result += '\r'; break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                // \uXXXX Unicode 转义留作挑战 B
                default:   error(std::string("bad escape \\") + esc);    // 非法转义立即报错
            }
        } else {
            result += c;      // 普通字符直接收
        }
    }
    // 循环两种出口：① 源耗尽（没等到闭引号）② peek 是 '"' —— 用 match 验证：
    if (!match('"')) error("unterminated string");    // 未闭合（源耗尽出口）报错
    return makeString(std::move(result));             // move：攒好的串零拷贝交给节点
}

// ===== 数组（递归下降的核心之一）=====
// 文法：[ value, value, ... ] 或 []
JsonNodePtr JsonParser::parseArray() {
    if (!match('[')) error("expected '['");       // 顺带吃掉开括号
    auto arr = JsonNode::Array{};                 // 先攒局部数组，最后整体移动入节点
    skipWhitespace();
    if (match(']')) return std::make_unique<JsonNode>(std::move(arr));     // 空数组特判

    while (true) {
        arr.push_back(parseValue());                                       // ⭐ 递归
        // 数组元素可以是任意值 → parseValue；嵌套数组 [1,[2,3]] 的内层
        // [2,3] 又从 parseValue 进 parseArray —— 递归深度 = 嵌套层数
        skipWhitespace();
        if (match(']')) break;                      // 闭括号：正常结束
        if (!match(',')) error("expected ',' or ']' in array");    // 既不是']'也没有','：语法错
    }
    return std::make_unique<JsonNode>(std::move(arr));
}

// ===== 对象（递归下降的另一核心）=====
// 文法：{ "key": value, "key": value, ... } 或 {}
JsonNodePtr JsonParser::parseObject() {
    if (!match('{')) error("expected '{'");        // 顺带吃掉开括号
    auto obj = JsonNode::Object{};                 // 局部 map 攒键值对
    skipWhitespace();
    if (match('}')) return std::make_unique<JsonNode>(std::move(obj));     // 空对象特判

    while (true) {
        skipWhitespace();
        auto keyNode = parseString();                                       // key 必须是字符串
        // 【复用设计】key 的解析完全复用 parseString（含转义处理）——
        //   取出字符串值后丢弃节点包装，只留 key 文本
        std::string key = keyNode->asString();
        skipWhitespace();
        if (!match(':')) error("expected ':'");     // key 后必须有冒号
        auto value = parseValue();                                          // ⭐ 递归
        obj[key] = std::move(value);                // 键值对入 map（移动节点指针）
        // 【语义说明】map 的 operator[]：key 已存在则覆盖（重复键取后者，
        //   JSON 规范允许的实现选择之一）
        skipWhitespace();
        if (match('}')) break;                      // 闭括号：正常结束
        if (!match(',')) error("expected ',' or '}' in object");    // 缺分隔符：语法错
    }
    return std::make_unique<JsonNode>(std::move(obj));
}

// ===== 分派中枢 =====
// parseValue：按首字符路由 —— 递归下降的"总调度台"
JsonNodePtr JsonParser::parseValue() {
    skipWhitespace();
    char c = peek();
    if (c == 'n' || c == 't' || c == 'f') return parseLiteral();   // null/true/false 首字母
    if (c == '"') return parseString();
    if (c == '[') return parseArray();                                                 // ⭐ 新增
    if (c == '{') return parseObject();                                                // ⭐ 新增
    if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();
    error(std::string("unexpected char: ") + c);        // 什么都不是：非法起始（含 '\0' 越界哨兵）
}

// ===== 字面量：null / true / false =====
JsonNodePtr JsonParser::parseLiteral() {
    // 【技术原理】string_view::compare(pos, len, str)：从 pos 起与目标串
    //   比较 —— 一次调用完成"看这 4 个字符是不是 null"。
    // 【手工推进的代价】pos += 4 绕过了 consume()，line/col 不会自动更新，
    //   所以手工 col += 4 补账（字面量不含换行，line 无需动）——
    //   游标维护约定："谁绕过 consume，谁手工补定位"。
    if (src.compare(pos, 4, "null") == 0)  { pos += 4; col += 4; return makeNull(); }
    if (src.compare(pos, 4, "true") == 0)  { pos += 4; col += 4; return makeBool(true); }
    if (src.compare(pos, 5, "false") == 0) { pos += 5; col += 5; return makeBool(false); }
    error("invalid literal");    // 首字母是 n/t/f 但拼不出关键字（如 "nul"）
}


}
