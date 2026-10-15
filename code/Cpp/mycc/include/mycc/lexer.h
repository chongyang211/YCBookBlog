#pragma once

#include "token.h"   // 产物类型 Token
#include <string>
#include <vector>

namespace Mycc {

// ============================================================================
// 【模块定位】lexer.h —— 词法分析器（教程阶段②：字符流 → Token 流）
// ----------------------------------------------------------------------------
// 【设计思想·状态机】主循环按"首字符"分类路由：
//   空白/注释→丢弃、数字→readNumber、字母/下划线→readIdent、
//   引号→readString、其余→查运算符表 —— 每类一个 readXxx 小函数，
//   "一个函数只管一种词类"（对应 JsonParser 的分派思想，但产物是线性流不是树）。
// 【游标模型】pos（字节下标）+ line（行号）—— advance 是唯一推进口，
//   行号维护收敛在一处（对比 JsonParser 里 parseLiteral 绕过 consume 的补账）。
// ============================================================================
class Lexer {
public:
    // 按值收源码（Lexer 独占源文本所有权）+ 文件名（错误信息用）
    Lexer(std::string source, std::string filename = "<repl>")
        : src(std::move(source)), file_(std::move(filename)) {}

    std::vector<Token> scanAll();      // 主接口：一次切完所有 Token
    // 【一次性 vs 流式】教学版选择"一次切完"：Parser 拿完整 vector 随机访问 ——
    //   真编译器多用流式（边词法边语法，省内存）

private:
    std::string src;     // 源码全文（所有 view/substr 的底层 buffer）
    std::string file_;   // 文件名：随异常上报
    size_t pos = 0;      // 游标：当前扫描到的字节下标
    int    line = 1;     // 行号：随 Token 运输（错误定位）

    // ---- 游标三件套（与 JsonManager 的 peek/consume 同构）----
    char peek()    const { return pos < src.size() ? src[pos] : '\0'; }
    // 越界返回 '\0' 哨兵：主循环 switch 不命中任何已知字符 → 走"未知字符"报错
    char advance()       { return src[pos++]; }
    // 前进唯一出口；行号维护交给调用方（只有主循环遇 '\n' 时 line++）——
    // 这里没像 JsonParser 那样在 consume 里维护行号：readNumber 等"读多字符"
    // 函数保证不吞 '\n'，行号只在两处显式 ++（主循环/readString）
    bool isAtEnd() const { return pos >= src.size(); }

    // ---- 三种"多字符"词类各一个读取函数 ----
    Token readNumber();   // 数字（整数/浮点）
    Token readIdent();    // 标识符 / 关键字
    Token readString();   // 字符串字面量
    // 【共同模式】记 start → 吞字符直到形态结束 → substr 截取 →
    //   构造 Token（payload 装 variant）—— "先看后取"的两段式
};

}  // namespace Mycc
