#include <iostream>
#include "JsonExceptions.h"
#include "JsonNode.h"
#include "JsonParser.h"
#include "JsonWriter.h"

void demo(int which) {
    switch (which) {
        case 1: throw jsonkv::JsonParseError("bad token", 3, 12);
        case 2: throw jsonkv::TypeMismatchError("string", "number");
        case 3: throw jsonkv::KeyNotFoundError("user.name");
        case 4: throw jsonkv::JsonIoError("cannot open kv.json");
        default: throw jsonkv::JsonError("base class works!");
    }
}

const char* typeStr(jsonkv::JsonNode::Type t) {
    switch (t) {
        case jsonkv::JsonNode::Type::Null:   return "null";
        case jsonkv::JsonNode::Type::Bool:   return "bool";
        case jsonkv::JsonNode::Type::Number: return "number";
        case jsonkv::JsonNode::Type::String: return "string";
        case jsonkv::JsonNode::Type::Array:  return "array";
        case jsonkv::JsonNode::Type::Object: return "object";
    }
    return "?";
}

void test() {
    // for (int i = 1; i <= 5; ++i) {
    //     try { demo(i); }
    //     catch (const jsonkv::JsonError& e) {                     // ⭐ 父类一次接住
    //         std::cout << "[" << i << "] " << e.what() << "\n";
    //     }
    // }


    using namespace jsonkv;
    JsonNode n_null;
    JsonNode n_bool(true);
    JsonNode n_num(3.14);
    JsonNode n_str(std::string("hello"));
    std::cout << "create 4 nodes OK\n";

    using namespace jsonkv;
    JsonNode nodes[] = { JsonNode{}, JsonNode{true}, JsonNode{3.14}, JsonNode{std::string("hi")} };
    for (auto& n : nodes) {
        std::cout << typeStr(n.type()) << "\n";
    }

    JsonNode b(true);
    std::cout << b.asBool() << "\n";       // OK：1
    try {
        double x = b.asNumber();           // ⚠️ 类型不对
        std::cout << x << "\n";
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "\n";
    }

    JsonNode c(true);
    try {
        c.asString();
    } catch (const TypeMismatchError& e) {
        std::cout << "friendly: " << e.what() << "\n";
    }


    // 手搭：{ "user": { "name": "Alice", "age": 30 } }
    auto user = makeObject();
    user->asObject()["name"] = makeString("Alice");
    user->asObject()["age"]  = makeNumber(30);

    auto root = makeObject();
    root->asObject()["user"] = std::move(user);

    // 用 at 路径查询
    std::cout << "name = " << root->at("user.name").asString() << "\n";
    std::cout << "age  = " << root->at("user.age").asNumber() << "\n";

    // 故意查不存在的路径
    try { root->at("user.email"); }
    catch (const KeyNotFoundError& e) { std::cout << "miss: " << e.what() << "\n"; }

    jsonkv::JsonParser p("hello");
    p.debugDump();

    using namespace jsonkv;
    for (const char* input : {"null", "true", "false"}) {
        JsonParser p(input);
        auto node = p.parse();
        std::cout << input << " => isNull=" << node->isNull()
                  << " isBool=" << node->isBool();
        if (node->isBool()) std::cout << " val=" << node->asBool();
        std::cout << "\n";
    }

    for (const char* s : {"42", "-3.14", "1e3", "-2.5e-2"}) {
        JsonParser p(s);
        std::cout << s << " => " << p.parse()->asNumber() << "\n";
    }

    for (const char* s : {"\"hello\"", "\"\"", "\"abc 123\""}) {
        JsonParser p(s);
        std::cout << "[" << p.parse()->asString() << "]\n";
    }

    JsonParser p3(R"("a\nb\t\"c\"")");                        // 原始字符串字面量更直观
    std::string s = p3.parse()->asString();
    std::cout << "len=" << s.size() << "\n";
    std::cout << s << "\n";

    const char* json = R"([
      {"name":"Alice","age":30,"vip":true},
      {"name":"Bob","age":25,"vip":false}
    ])";

    JsonParser p4(json);
    auto root4 = p4.parse();

    std::cout << "size = " << root4->size() << "\n";                      // 2
    for (size_t i = 0; i < root4->size(); ++i) {
        auto& item = (*root4)[i];
        std::cout << item.at("name").asString()
                  << " / " << item.at("age").asNumber()
                  << " / vip=" << item.at("vip").asBool() << "\n";
    }

    JsonParser p5(R"({"a":null,"b":true,"c":3.14,"d":"hi\nyou"})");
    auto root5 = p5.parse();

    JsonWriter w(0);                 // 紧凑模式
    std::cout << w.write(*root5) << "\n";
}

void test5() {
    using namespace jsonkv;
    const char* json = R"({"users":[{"name":"Alice","age":30},{"name":"Bob","age":25}]})";

    JsonParser p(json);
    auto root = p.parse();

    std::cout << "--- 紧凑模式 ---\n";
    JsonWriter w0(0);
    std::cout << w0.write(*root) << "\n\n";

    std::cout << "--- 缩进 2 空格 ---\n";
    JsonWriter w2(2);
    std::cout << w2.write(*root) << "\n";
}

// g++ -std=c++17 main.cpp JsonExceptions.cpp JsonNode.cpp JsonParser.cpp JsonWriter.cpp
int main() {
    test();
    test5();
    return 0;
}
