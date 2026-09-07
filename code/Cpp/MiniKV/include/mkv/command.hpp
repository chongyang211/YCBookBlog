#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "value.hpp"

namespace mkv {

enum class CmdType {
    Set, Get, Del, Expire, Ttl, Keys, Save, Exit, Unknown
};

CmdType parseCmdType(const std::string& s);

// 把一行命令切成多个 string_view，全部指向同一个底层 buffer（零拷贝）
std::vector<std::string_view> tokenize(std::string_view line);

class Store;   // 前置声明，避免 command.hpp include store.hpp 造成循环

// 命令抽象基类：一条命令自己知道怎么校验、怎么执行、怎么序列化成 AOF
class Command {
public:
    virtual ~Command() = default;

    // 用户可见的命令名，例如 "SET"
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    // 是否需要写入 AOF（GET / KEYS / TTL / SAVE 这种只读命令返回 false）
    [[nodiscard]] virtual bool isWrite() const noexcept = 0;

    // 执行命令，返回给客户端看的字符串
    [[nodiscard]] virtual std::string execute(Store& store) = 0;

    // 序列化回 AOF 行（仅写命令需实现，读命令直接返回空字符串）
    [[nodiscard]] virtual std::string toAofLine() const { return {}; }

protected:
    Command() = default;
    Command(const Command&) = delete;             // 禁止拷贝
    Command& operator=(const Command&) = delete;
};

// SET key value
class SetCmd : public Command {
public:
    SetCmd(std::string key, Value value, std::string rawValueToken)
        : key_(std::move(key)), value_(std::move(value)), rawToken_(std::move(rawValueToken)) {}

    std::string_view name() const noexcept override { return "SET"; }
    bool isWrite() const noexcept override { return true; }
    std::string execute(Store& store) override;
    std::string toAofLine() const override { return "SET " + key_ + " " + rawToken_ + "\n"; }

private:
    std::string key_;
    Value       value_;
    std::string rawToken_;   // 原始 token，写入 AOF 时不再二次 format
};

// GET key
class GetCmd : public Command {
public:
    explicit GetCmd(std::string key) : key_(std::move(key)) {}
    std::string_view name() const noexcept override { return "GET"; }
    bool isWrite() const noexcept override { return false; }
    std::string execute(Store& store) override;

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
    std::string_view name() const noexcept override { return "SAVE"; }
    bool isWrite() const noexcept override { return false; }
    std::string execute(Store& store) override;
};

// 把切好的 tokens 组装成具体的 Command 派生类实例
[[nodiscard]]
std::unique_ptr<Command> makeCommand(const std::vector<std::string_view>& tokens);

}  // namespace mkv
