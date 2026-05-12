// mkv/command.hpp —— 命令模式（Command Pattern）
//
// 关键设计（详见 12.迷你KV存储引擎.md §05）：
// - 抽象基类 Command：name() / isWrite() / execute() / toAofLine() 四个虚函数；
// - 派生类 SetCmd / GetCmd / DelCmd / ExpireCmd / TtlCmd / KeysCmd / SaveCmd；
// - 工厂函数 makeCommand：tokens -> unique_ptr<Command>；
// - 主循环只持有 unique_ptr<Command>，加新命令零修改主流程。

#pragma once

#include "mkv/value.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace mkv {

class Store;   // 前置声明：Command::execute 只需要引用

// ============== 命令枚举 ==============
enum class CmdType {
    Set, Get, Del, Expire, Ttl, Keys, Save, Exit, Unknown
};

CmdType parseCmdType(std::string_view s) noexcept;

// 把一行命令切成多个 string_view（零拷贝）
std::vector<std::string_view> tokenize(std::string_view line);

// ============== 抽象基类 ==============
class Command {
public:
    virtual ~Command() = default;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    // 是否需要写入 AOF（GET / KEYS / TTL 等只读命令返回 false）
    [[nodiscard]] virtual bool isWrite() const noexcept = 0;

    // 执行命令，返回给客户端看的字符串
    [[nodiscard]] virtual std::string execute(Store& store) = 0;

    // 序列化回 AOF 行（仅写命令需实现）
    [[nodiscard]] virtual std::string toAofLine() const { return {}; }

protected:
    Command() = default;
    Command(const Command&)            = delete;
    Command& operator=(const Command&) = delete;
};

// ============== 派生类 ==============
class SetCmd : public Command {
public:
    SetCmd(std::string key, Value value, std::string rawValueToken)
        : key_(std::move(key)),
          value_(std::move(value)),
          rawToken_(std::move(rawValueToken)) {}

    std::string_view name()    const noexcept override { return "SET"; }
    bool             isWrite() const noexcept override { return true; }
    std::string      execute (Store& store) override;
    std::string      toAofLine() const override {
        return "SET " + key_ + " " + rawToken_ + "\n";
    }

private:
    std::string key_;
    Value       value_;
    std::string rawToken_;     // 原始 token，写入 AOF 时不再二次 format
};

class GetCmd : public Command {
public:
    explicit GetCmd(std::string key) : key_(std::move(key)) {}
    std::string_view name()    const noexcept override { return "GET"; }
    bool             isWrite() const noexcept override { return false; }
    std::string      execute (Store& store) override;
private:
    std::string key_;
};

class DelCmd : public Command {
public:
    explicit DelCmd(std::string key) : key_(std::move(key)) {}
    std::string_view name()    const noexcept override { return "DEL"; }
    bool             isWrite() const noexcept override { return true; }
    std::string      execute (Store& store) override;
    std::string      toAofLine() const override { return "DEL " + key_ + "\n"; }
private:
    std::string key_;
};

class ExpireCmd : public Command {
public:
    ExpireCmd(std::string key, int seconds)
        : key_(std::move(key)), seconds_(seconds) {}
    std::string_view name()    const noexcept override { return "EXPIRE"; }
    bool             isWrite() const noexcept override { return true; }
    std::string      execute (Store& store) override;
    std::string      toAofLine() const override {
        return "EXPIRE " + key_ + " " + std::to_string(seconds_) + "\n";
    }
private:
    std::string key_;
    int         seconds_;
};

class TtlCmd : public Command {
public:
    explicit TtlCmd(std::string key) : key_(std::move(key)) {}
    std::string_view name()    const noexcept override { return "TTL"; }
    bool             isWrite() const noexcept override { return false; }
    std::string      execute (Store& store) override;
private:
    std::string key_;
};

class KeysCmd : public Command {
public:
    KeysCmd() = default;
    std::string_view name()    const noexcept override { return "KEYS"; }
    bool             isWrite() const noexcept override { return false; }
    std::string      execute (Store& store) override;
};

class SaveCmd : public Command {
public:
    SaveCmd() = default;
    std::string_view name()    const noexcept override { return "SAVE"; }
    bool             isWrite() const noexcept override { return false; }  // 只是 fsync，不产生新行
    std::string      execute (Store& store) override;
};

// ============== 工厂函数 ==============
[[nodiscard]]
std::unique_ptr<Command> makeCommand(const std::vector<std::string_view>& tokens);

}  // namespace mkv
