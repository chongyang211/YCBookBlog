#pragma once

#include <string>
#include <atomic>

// 商品：库存用 atomic + CAS 循环，防超卖
class Product {
private:
    int               id;
    std::string       name;
    double            price;
    std::atomic<int>  stock;        // 原子库存

public:
    Product(int id_, std::string n, double p, int s)
        : id(id_), name(std::move(n)), price(p), stock(s) {}

    int getId() const { return id; }
    int getStock() const { return stock.load(); }
    double getPrice() const { return price; }
    const std::string& getName() const { return name; }

    // CAS 循环扣库存（防超卖）：判断 + 扣减是原子的
    bool tryDeduct(int n) {
        int cur = stock.load();
        while (cur >= n) {
            // 如果 stock 仍然是 cur，则把它改成 cur - n
            // 失败时 cur 会被自动更新为最新值，进入下一轮
            if (stock.compare_exchange_weak(cur, cur - n)) {
                return true;       // 成功扣减
            }
        }
        return false;             // 库存不够（cur < n）
    }

    void restock(int n) { stock.fetch_add(n); }
};
