//
// Created by 杨充 on 2026/6/10.
//
// ============================================================================
// 【文件定位】JsonWriter.h —— 序列化器（JsonNode 树 → JSON 文本）
// ----------------------------------------------------------------------------
// 【设计思路】Parser 的镜像：Parser 是"文本→树"，Writer 是"树→文本"。
//   两种输出模式共用一套递归骨架：
//   - 紧凑模式（step=0）：{"a":1,"b":[1,2]} —— 存储/传输省字节；
//   - 美化模式（step=2）：换行 + 缩进 —— 人眼可读（kv.json 落盘用）。
//   模式切换只是"每处该不该输出 \n 和空格"的布尔判断，递归逻辑零分叉。
// ============================================================================

#pragma once
#include "JsonNode.h"    // 序列化对象：JsonNode
#include <string>        // 返回值 std::string
#include <sstream>       // std::ostringstream：内存流攒输出

namespace jsonkv {

    class JsonWriter {
    private:
        std::ostringstream oss;     // 输出缓冲：往"内存里的流"写，最后一次性导出
        // 【复用说明】oss 是成员：同一个 Writer 对象多次 write() 复用同一块
        //   流缓冲（write 里会重置），省去反复构造/析构流的开销。
        int indent = 0;        // 当前缩进深度
        // 【不变量】indent 始终等于"当前节点在树中的深度 × step"——
        //   进容器 +step、出容器 -step，写换行前先 writeIndent 补空格。
        int step;              // 每级缩进多少空格（0 = 紧凑）
        bool pretty;           // step > 0 即 pretty=true
        // 【派生状态】pretty 不独立配置而是从 step 推导 —— 两个参数
        //   表达同一件事必然出现"step=0 但 pretty=true"的矛盾态，砍掉一个。

        void writeIndent() { for (int i = 0; i < indent; ++i) oss << ' '; }
        // 输出 indent 个空格（当前深度的缩进前缀；紧凑模式下 indent 恒 0）
        void writeNode(const JsonNode& n);      // 递归核心：按类型分派序列化
        void writeString(const std::string& s); // 字符串序列化 + 反向转义

    public:
        // explicit 防隐式转换；默认 2 空格缩进（主流 JSON 美化习惯）
        explicit JsonWriter(int spaces = 2) : step(spaces), pretty(spaces > 0) {}
        std::string write(const JsonNode& root);   // 唯一入口：整树 → 文本
    };

}
