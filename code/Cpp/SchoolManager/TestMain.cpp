//
// Created by 杨充 on 2026/10/4.
//

#include <future>
#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <thread>

using namespace std;

class MyClass {
public:
    MyClass(int value) : value_(value) {
        std::cout << "MyClass constructed with value: " << value_ << std::endl;
    }
    ~MyClass() {
        std::cout << "MyClass destroyed with value: " << value_ << std::endl;
    }
    void printValue() const {
        std::cout << "Value: " << value_ << std::endl;
    }
private:
    int value_;
};

void test() {
    std::shared_ptr<MyClass> my_class;
}

void test2() {

}

void test3() {

}

// g++ -std==c++17 Test.cpp
// g++ TestMain.cpp
// g++ -std=c++17 TestMain.cpp
// 身份 + 账号 + 密码
int main() {
    test();
    test2();
    return 0;
}