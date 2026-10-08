//
// Created by 杨充 on 2026/10/4.
//

#include <future>
#include <iostream>
#include <map>
#include <string>
#include <memory>
using namespace std;


// 后台线程：接收 promise，算完后投递结果
void produceData(std::promise<int> p) {
    std::cout << "[后台] 开始处理数据..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟耗时计算
    int result = 42;
    p.set_value(result);              // ① 把结果塞给 future
    std::cout << "[后台] 结果已投递" << std::endl;
}

void test() {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();
    std::thread worker(produceData, std::move(prom)); // ④ promise 所有权移交线程（不可拷贝）
    std::cout << "[主] 线程已启动，先干点别的..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int value = fut.get();             // ⑤ 阻塞等待结果（此刻才真正等满 2 秒）
    std::cout << "[主] 收到结果：" << value << std::endl;
    worker.join();
}

// g++ -std==c++17 Test.cpp
// g++ TestMain.cpp
// g++ -std=c++17 TestMain.cpp
// 身份 + 账号 + 密码
int main() {
    test();
    return 0;
}