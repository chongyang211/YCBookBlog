#pragma once

#include <memory>          // std::unique_ptr / make_unique
#include <string>          // std::string
#include <string_view>     // std::string_view（tokenize 零拷贝切词）
#include <vector>          // std::vector

#include "value.hpp"       // Value / valueFromToken

namespace mkv {

// ============================================================================
// 【模块定位】command.hpp —— 命令模式：抽象基类 + 七个派生类 + 工厂 + 切词
// ----------------------------------------------------------------------------
// 【设计思想·教程§05 灵魂三问（OOP 高峰）】
//   ① 为什么不一直 if-else？Step 4.1 的"8 命令 200 行 switch"大爆炸现场：
//      主循环里每加一个命令要改 3 处（解析/执行/AOF 序列化）——
//      命令模式把三者内聚到各命令类，"加新命令 = 新增一个派生类 + 工厂加一行"，
//      主流程零修改（开闭原则的教科书落地）；
//   ② 为什么 unique_ptr？命令对象"造出来 → 执行 → 丢弃"，单线程独占使用，
//      独占所有权语义精确匹配（对比 Entry 的多方共用需要 shared_ptr）；
//   ③ toAofLine 为什么是独立虚函数？AOF 存"原始 token"而非"格式化结果"
//      —— GET 的 "(integer) 30" 不能回放，"SET age 30" 才能 ——
//      序列化与执行必须分离成两个动作。
// ============================================================================

enum class CmdType {
    Set, Get, Del, Expire, Ttl, Keys, Save, Exit, Unknown
};
// 【为什么先过 enum】命令名 → 强类型枚举是"字符串世界"与"类型世界"的分界：
//   后续 switch/分发全部基于枚举，拼错命令名在编译期就暴露（比字符串比较快且安全）

CmdType parseCmdType(const std::string& s);
// 字符串 → 枚举（大小写不敏感）：实现见 command.cpp

// 把一行命令切成多个 string_view，全部指向同一个底层 buffer（零拷贝）
// 【为什么 string_view】切词产物只作为"中转视图"（马上拷进命令对象或用于判断），
//   永不持久保存 —— view 的"不拥有数据"限制在这里恰好是优势：
//   万次切词零内存分配，只有最终要存的 key/value 才付一次 string 拷贝
std::vector<std::string_view> tokenize(std::string_view line);

class Store;   // 前置声明，避免 command.hpp include store.hpp 造成循环
// 【依赖方向】command 依赖 store（execute 调它），store 不依赖 command ——
//   单向。头文件里只需要 Store&（引用声明），完整定义留给 .cpp ——
//   既打破"循环包含"，又让头文件保持轻量

// 命令抽象基类：一条命令自己知道怎么校验、怎么执行、怎么序列化成 AOF
class Command {
public:
    virtual ~Command() = default;
    // 【虚析构铁律·教程§04 现场验证】命令对象经由 Command*（unique_ptr 的
    //   删除器走虚表）销毁 —— 非虚析构时派生类成员（string key_ 等）不释放

    // 用户可见的命令名，例如 "SET"
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    // 返回 string_view：命令名是字面量常量，view 零拷贝且不会悬垂

    // 是否需要写入 AOF（GET / KEYS / TTL / SAVE 这种只读命令返回 false）
    // 【AOF 的分界线】重放时只重演写命令 —— 读命令重放无意义且有副作用风险
    [[nodiscard]] virtual bool isWrite() const noexcept = 0;

    // 执行命令，返回给客户端看的字符串
    [[nodiscard]] virtual std::string execute(Store& store) = 0;

    // 序列化回 AOF 行（仅写命令需实现，读命令直接返回空字符串）
    // 基类默认实现返回 {}（空串）—— 读命令"白拿默认"，写命令必须重写：
    // "纯虚 = 强制契约，普通虚 = 可覆盖的默认"的分寸设计
    [[nodiscard]] virtual std::string toAofLine() const { return {}; }

protected:
    Command() = default;
    // protected 构造：抽象类不该被直接实例化，protected 挡住外部 new
    Command(const Command&) = delete;             // 禁止拷贝
    Command& operator=(const Command&) = delete;
    // 多态对象经指针使用，拷贝会发生"切片"（派生部分被切掉）—— 直接禁掉
};

// SET key value
class SetCmd : public Command {
public:
    SetCmd(std::string key, Value value, std::string rawValueToken)
        : key_(std::move(key)), value_(std::move(value)), rawToken_(std::move(rawValueToken)) {}
    // 【为什么存两份值】value_ 是"推断类型后的值"（执行时用），
    //   rawToken_ 是"原始字符串"（AOF 回写用）——
    //   若 AOF 写 formatValue(value_) 会把 30 变 "(integer) 30"，回放即坏；
    //   存原始 token 保证"回放 == 当时的命令"，一字不差

    std::string_view name() const noexcept override { return "SET"; }
    bool isWrite() const noexcept override { return true; }
    std::string execute(Store& store) override;
    std::string toAofLine() const override { return "SET " + key_ + " " + rawToken_ + "\n"; }
    // AOF 行 = 空格分隔的原始命令 + '\n' —— 与用户输入完全同构，
    // 重放时走同一条 tokenize 管道即可还原

private:
    std::string key_;
    Value       value_;
    std::string rawToken_;   // 原始 token，写入 AOF 时不再二次 format
};

// GET key
class GetCmd : public Command {
public:
    explicit GetCmd(std::string key) : key_(std::move(key)) {}
    // explicit：单参构造禁隐式转换（防 string 悄悄变命令对象）
    std::string_view name() const noexcept override { return "GET"; }
    bool isWrite() const noexcept override { return false; }
    std::string execute(Store& store) override;
    // 无 toAofLine 重写 —— 读命令白拿基类默认（空串，永不落盘）

private:
    std::string key_;
};

// DEL key
class DelCmd : public Command {
public:
    explicit DelCmd(std::string key) : key_(std::move(key)) {}
    std::string_view name() const noexcept override { return "DEL"; }
    bool isWrite() const noexcept override { return true; }
    std::string execute(Store& store) override;
    std::string toAofLine() const override { return "DEL " + key_ + "\n"; }
    // 【DEL 必须落盘】重放时"删掉"要重演 —— 否则重启后被删的 key 又复活

private:
    std::string key_;
};

// EXPIRE key seconds
class ExpireCmd : public Command {
public:
    ExpireCmd(std::string key, int seconds) : key_(std::move(key)), seconds_(seconds) {}
    std::string_view name() const noexcept override { return "EXPIRE"; }
    bool isWrite() const noexcept override { return true; }
    std::string execute(Store& store) override;
    std::string toAofLine() const override {
        return "EXPIRE " + key_ + " " + std::to_string(seconds_) + "\n";
        // to_string 规范化数字（用户输入 "EXPIRE k 007" 存 "7"）——
        // 回放语义不变，AOF 内容更干净
    }

private:
    std::string key_;
    int         seconds_;
};

// TTL key
class TtlCmd : public Command {
public:
    explicit TtlCmd(std::string key) : key_(std::move(key)) {}
    std::string_view name() const noexcept override { return "TTL"; }
    bool isWrite() const noexcept override { return false; }
    std::string execute(Store& store) override;

private:
    std::string key_;
};

// KEYS pattern
class KeysCmd : public Command {
public:
    explicit KeysCmd(std::string pattern) : pattern_(std::move(pattern)) {}
    std::string_view name() const noexcept override { return "KEYS"; }
    bool isWrite() const noexcept override { return false; }
    std::string execute(Store& store) override;

private:
    std::string pattern_;
};

// SAVE（强制刷盘；本身不产生 AOF 记录）
class SaveCmd : public Command {
public:
    SaveCmd() = default;
    // 无参构造 = default：类内无成员，编译器版本即可
    std::string_view name() const noexcept override { return "SAVE"; }
    bool isWrite() const noexcept override { return false; }
    // 【语义辨析】isWrite 指"是否产生 AOF 记录"—— SAVE 本身不改数据，
    //   它是"刷盘动作"（main 循环里特判调用 aof.flush()），不落盘自身
    std::string execute(Store& store) override;
};

// 把切好的 tokens 组装成具体的 Command 派生类实例
// 【工厂模式】参数个数校验、类型转换、异常翻译全部收敛在这一个函数 ——
//   "解析规则变更只改工厂"，七个命令类保持纯粹的业务体
[[nodiscard]]
std::unique_ptr<Command> makeCommand(const std::vector<std::string_view>& tokens);
// [[nodiscard]]：返回的命令对象丢弃必是 bug（造了不执行）

}  // namespace mkv
