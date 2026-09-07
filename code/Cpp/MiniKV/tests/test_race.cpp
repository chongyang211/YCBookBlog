// 数据竞争现场：两线程各加 1000 次，期望 2000，实际可能 < 2000
// 这是"为什么需要锁"的最直观演示，跑几次能看到不同结果。
#include <iostream>
#include <thread>

int counter = 0;   // 裸变量，未保护

int main() {
    std::thread t1([] { for (int i = 0; i < 1000; ++i) ++counter; });
    std::thread t2([] { for (int i = 0; i < 1000; ++i) ++counter; });
    t1.join();
    t2.join();
    std::cout << "counter = " << counter << " (expected 2000)\n";
    return 0;
}
