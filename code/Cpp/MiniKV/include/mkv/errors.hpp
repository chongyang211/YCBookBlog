#pragma once

#include <stdexcept>    // std::runtime_error：标准运行时异常基类
#include <string>       // std::string / std::to_string

namespace mkv {         // mkv = MiniKV：全项目符号隔离的命名空间

// ============================================================================
// 【模块定位】errors.hpp —— 异常体系（项目的"错误语言"，叶子模块）
// ----------------------------------------------------------------------------
// 【设计思想】单根异常树：根类 KvError 派生四种语义化子异常 ——
//   ① 调用方可按"哪一种错"精确捕获（catch CmdSyntaxError 只接语法错）；
//   ② 顶层 REPL 只需 catch (const KvError&) 一把抓（多态捕获）；
//   ③ 继承 std::runtime_error → 接入标准家族，漏接时仍可被
//     catch (const std::exception&) 兜住，不至于 terminate。
// 【与教程§"异常体系"呼应】KvError 对应"根"，四个子类对应
//   "语法/类型/IO/AOF 坏行"四类失败场景 —— 每类在代码里都有真实抛点。
// ============================================================================

// 所有 KV 内部异常的根
class KvError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
    // 【技术原理】继承构造函数（C++11）：一行让派生类获得基类全部构造的
    //   转发版本 —— 对比手写 JsonError 那种逐个转发再担心漏掉的老写法，
    //   基类新增重载时这里零维护。
};

// 命令语法错误（用户输入问题）
class CmdSyntaxError : public KvError {
public:
    using KvError::KvError;
    // 无自定义逻辑 → 原样继承父类构造集（异常与父类只差"类型名"）
};

// 类型不匹配（如 GET 返回 string 但调用方按 int 用）
class TypeError : public KvError {
public:
    using KvError::KvError;
};

// IO 错误（AOF 写盘失败、目录创建失败）
class IoError : public KvError {
public:
    using KvError::KvError;
};

// AOF 重放时遇到坏行
class AofCorrupted : public KvError {
public:
    // 唯一带自定义构造的子类：错误消息携带"行号 + 细节" ——
    // AOF 恢复失败时用户最想知道"坏在第几行"，定位信息比描述更有价值
    AofCorrupted(std::size_t lineNo, const std::string& detail)
        : KvError("AOF corrupted at line " + std::to_string(lineNo) + ": " + detail) {}
};

}  // namespace mkv
