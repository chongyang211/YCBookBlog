// ============================================================================
// 【文件定位】lexer.cpp —— 词法分析器实现（字符流 → Token 流）
// ----------------------------------------------------------------------------
// 【阅读顺序】scanAll 主循环（分派中枢）→ 三个 readXxx（多字符词类）。
// ============================================================================
#include "mycc/lexer.h"
#include "mycc/error.h"

#include <cctype>           // isdigit/isalpha/isalnum
#include <unordered_map>    // 关键字表

namespace Mycc {

// 主接口：从左到右扫描整个源码，产出 Token vector
std::vector<Token> Lexer::scanAll() {
    std::vector<Token> out;
    while (!isAtEnd()) {
        char c = peek();

        // 1. 跳过空白
        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
        // \r 也丢弃：跨平台源码（Windows CRLF）的 \r 不能变成未知字符
        if (c == '\n') { line++; advance(); continue; }
        // 换行唯一职责：行号 ++（换行本身不产 Token）

        // 2. 跳过单行注释
        if (c == '/' && pos + 1 < src.size() && src[pos + 1] == '/') {
            // 双斜线判定要看"下一个字符"—— 前瞻一眼（不改游标）
            while (!isAtEnd() && peek() != '\n') advance();
            // 吃到行尾（不含 \n，下一轮主循环用它维护行号）
            continue;
        }

        // 3. 数字
        if (std::isdigit(static_cast<unsigned char>(c))) {
            // 【unsigned char 转型】cctype 函数参数必须 0~255，
            //   char 为负（UTF-8 字节）是 UB —— 全项目强制安全习惯
            out.push_back(readNumber());
            continue;
        }

        // 4. 标识符与关键字
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            // 首字符：字母或下划线（数字不行——否则 123abc 的 123 会被误读成标识符）
            out.push_back(readIdent());
            continue;
        }

        // 5. 字符串
        if (c == '"') {
            out.push_back(readString());
            continue;
        }

        // 6. 单字符运算符与分隔符
        switch (c) {
            case '+': out.emplace_back(TokKind::Plus,  line); advance(); continue;
            case '-': out.emplace_back(TokKind::Minus, line); advance(); continue;
            case '*': out.emplace_back(TokKind::Star,  line); advance(); continue;
            case '/': out.emplace_back(TokKind::Slash, line); advance(); continue;
            case '%': out.emplace_back(TokKind::Percent,line);advance();continue;
            case '(': out.emplace_back(TokKind::LParen,line); advance(); continue;
            case ')': out.emplace_back(TokKind::RParen,line); advance(); continue;
            case '{': out.emplace_back(TokKind::LBrace,line); advance(); continue;
            case '}': out.emplace_back(TokKind::RBrace,line); advance(); continue;
            case ';': out.emplace_back(TokKind::Semicolon,line);advance();continue;
            case ',': out.emplace_back(TokKind::Comma, line); advance(); continue;
            default: break;
        }
        // emplace_back 原地构造（无载荷版 Token），先产 Token 再 advance ——
        // 顺序无关但保持"看见即产出，产出即前进"的心智模型

        // 7. 多字符运算符
        // 【最长匹配原则】"==" 绝不能切成 "=" "=" —— 见到 = ! < > 先吃第一字符，
        //   前瞻第二字符决定是双字符还是单字符词类
        if (c == '=' || c == '!' || c == '<' || c == '>') {
            advance();   // 吃掉第一个字符
            char first = c;
            char second = peek();    // 前瞻第二字符（不前进）
            TokKind two;              // 若 second=='=' 的双字符词类
            TokKind one;              // 否则的单字符词类
            switch (first) {
                case '=': two = TokKind::EqEq;  one = TokKind::Assign; break;
                case '!': two = TokKind::BangEq; one = TokKind::Bang;  break;
                case '<': two = TokKind::Le;    one = TokKind::Lt;     break;
                case '>': two = TokKind::Ge;    one = TokKind::Gt;     break;
                default:  two = TokKind::Eof;   one = TokKind::Eof;    break;
                // default 兜底：编译器消"未初始化"告警（逻辑上不可达）
            }
            if (second == '=') { advance(); out.emplace_back(two, line); }
            else               { out.emplace_back(one, line); }
            continue;
        }
        if (c == '&') {
            advance();
            if (peek() == '&') { advance(); out.emplace_back(TokKind::AndAnd, line); }
            else throw LexError("single '&' is not supported", line, file_);
            // 【fail-fast 设计】单 & 不是语言的一部分 → 立即报错，
            //   而非默默当未知字符或位运算（用户需要知道"这里写错了"）
            continue;
        }
        if (c == '|') {
            advance();
            if (peek() == '|') { advance(); out.emplace_back(TokKind::OrOr, line); }
            else throw LexError("single '|' is not supported", line, file_);
            continue;
        }

        // 8. 真正的未知字符
        throw LexError(std::string("unexpected character '") + c + "'", line, file_);
        // 带行号带字符的词法错误 —— 阶段⑧ REPL 能接住它并继续下一行
    }
    out.emplace_back(TokKind::Eof, line);
    // 收尾必附 EOF 哨兵：Parser 的循环条件"读到 Eof 为止"有了明确终点
    return out;
}

// 数字：整数/小数 → double 载荷
Token Lexer::readNumber() {
    size_t start = pos;     // 记住起点（两段读取共用的回溯锚）
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) advance();
    // 整数部分
    if (peek() == '.') {                  // 浮点
        advance();        // 吃 '.'
        while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) advance();
        // 小数部分（整数/浮点统一 Number 词类，见 token.h 注释）
    }
    double v = std::stod(src.substr(start, pos - start));
    // 【复用 stod】字符收集自己来、"文本→double"交给标准库 ——
    //   手写浮点解析是精度深坑（同 JsonManager 的取舍）
    return Token(TokKind::Number, v, line);
}

// 标识符/关键字：字母/数字/下划线的连续串 → 查关键字表定词类
Token Lexer::readIdent() {
    size_t start = pos;
    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        advance();
        // 首字符已由主循环保证是字母/下划线，这里允许数字续写（x1、a_2）
    }
    std::string text = src.substr(start, pos - start);

    // 【static const 查表】关键字表只构造一次（进程生命周期复用）——
    //   每个标识符都要查，静态化省去反复建表的构造开销
    static const std::unordered_map<std::string, TokKind> KEYWORDS = {
        {"let",   TokKind::Let},
        {"print", TokKind::Print},
        {"if",    TokKind::If},     {"else",  TokKind::Else},
        {"while", TokKind::While},
        {"fn",    TokKind::Fn},     {"return",TokKind::Return},
        {"true",  TokKind::True},   {"false", TokKind::False},
    };
    // 【关键字识别的经典做法】先按"标识符"读完整串，再查保留字表 ——
    //   "letx" 读完后查表不中 → 普通标识符（绝不会误切成 let + x）
    if (auto it = KEYWORDS.find(text); it != KEYWORDS.end()) {
        return Token(it->second, line);
        // 命中关键字：无载荷 Token（词类已足够）
    }
    return Token(TokKind::Ident, text, line);
    // 普通标识符：名字装进 string 载荷
}

// 字符串字面量：吃开引号 → 吞内容 → 吃闭引号
Token Lexer::readString() {
    advance();    // 吃掉开头的 "
    std::string s;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') line++;   // 多行字符串里的换行也要计行号
        s.push_back(advance());
        // 【教学简化】无转义序列（\n \" 等）—— 字符串里不能包含引号本身，
        //   延伸挑战可实现（对照 JsonParser 的八种转义表）
    }
    if (isAtEnd()) {
        throw LexError("unterminated string", line, file_);
        // 源码耗尽仍没等到闭引号 —— 典型的词法错误（带行号上报）
    }
    advance();    // 吃掉结尾的 "
    return Token(TokKind::String, s, line);
}

}  // namespace Mycc
