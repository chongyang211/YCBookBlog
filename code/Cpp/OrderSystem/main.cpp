
// ============================================================================
// 【文件定位】main.cpp —— 入口层（UI）：菜单 + 策略分发 + 组件组装
// ----------------------------------------------------------------------------
// 【设计思路】main 只做三件事：
//   ① 组装组件：线程池/管理器/商品/三个策略（依赖注入：pool 传给策略们）；
//   ② 菜单循环：读选项 → 策略多态分发 / 查询 / 看库存；
//   ③ 顶层 try-catch 兜底：单次操作失败不炸整个循环。
// 【多态高光】三目链把三个具体策略统一成 OrderStrategy& ——
//   一句 placeOrders 调用，运行时按用户选择进入不同实现（策略模式收口）。
// ============================================================================

#include "include/ThreadPool.h"
#include "include/OrderManager.h"
#include "include/Product.h"
#include "include/NormalStrategy.h"
#include "include/FlashStrategy.h"
#include "include/BatchStrategy.h"
#include "include/Logger.h"
#include <iostream>




int main() {
    ThreadPool   pool(4);       // 4 个工人线程（三个策略共享这一个池）
    OrderManager mgr;           // 订单存储（读写锁保护）
    Product      iphone(1, "iPhone 15", 7999.0, 100);   // 唯一商品：初始库存 100

    NormalStrategy normal;          // 策略一：顺序（无依赖）
    FlashStrategy  flash(pool);     // 策略二：全并发（注入共享池）
    BatchStrategy  batch(pool, 10); // 策略三：分批（批大小 10）

    while (true) {      // 事件循环：每轮打印菜单 → 读选项 → 分发
        std::cout << "\n========== 订单系统 ==========\n"
                     "1=普通下单  2=秒杀下单  3=批量下单\n"
                     "4=查询订单  5=查看库存  0=退出\n> ";
        int op;
        if (!(std::cin >> op)) break;      // 流失败（EOF/非法输入）→ 退出循环
        if (op == 0) break;                // 0 = 用户主动退出

        try {      // 兜底任何异常（策略内的 future 重抛/管理器异常等）——
            // 出错只打 ERR 继续下一轮，程序不崩
            if (op == 1 || op == 2 || op == 3) {
                int userId, count;
                std::cout << "用户 ID: ";    std::cin >> userId;
                std::cout << "下单次数: ";    std::cin >> count;

                std::vector<int> qtys(count, 1);     // count 单、每单 1 件
                // 【简化假设】所有单数量都是 1（秒杀场景的典型粒度）——
                // 接口的 quantities 向量保留了"每单任意数量"的能力

                // ⭐ 策略多态分发：三目链把具体策略"向上转型"为基类引用
                // static_cast<OrderStrategy&> 显式转型表达"这是向上转型"
                // （无运行时开销，纯类型视角统一 —— 也可直接赋给基类引用变量）
                OrderStrategy& s = (op == 1)
                    ? static_cast<OrderStrategy&>(normal)
                    : (op == 2)
                        ? static_cast<OrderStrategy&>(flash)
                        : static_cast<OrderStrategy&>(batch);

                auto ids = s.placeOrders(iphone, mgr, userId, qtys);
                // 同一句调用按 op 进入三种实现 —— 策略模式的运行时分发
                int success = 0;
                for (int id : ids) if (id > 0) success++;   // 统计成功数（-1 哨兵过滤）
                Logger::info("用户 ", userId, " 提交 ", count, " 单，成功 ", success);
                // 线程安全日志：整行原子（时间戳 + 线程号 + 内容）
            }
            else if (op == 4) {
                int userId;
                std::cout << "用户 ID: "; std::cin >> userId;
                auto v = mgr.ordersByUser(userId);     // 读锁下的快照拷贝
                std::cout << "用户 " << userId << " 共 " << v.size() << " 单：\n";
                for (const auto& o : v) {
                    std::cout << "  订单 #" << o.id
                              << " - 商品 " << o.productId
                              << " - 数量 " << o.quantity << "\n";
                }
            }
            else if (op == 5) {
                std::cout << "iPhone 15 当前库存: " << iphone.getStock() << "\n";
                // 原子 load 读库存 —— 并发下单进行中也读到一致值
            }
        } catch (const std::exception& e) {
            std::cout << "ERR " << e.what() << "\n";    // 异常局部化：只影响本轮操作
        }
    }
    return 0;
    // main 返回触发析构链：pool.shutdown()（等工人收尾）→ mgr/iphone 析构
}
