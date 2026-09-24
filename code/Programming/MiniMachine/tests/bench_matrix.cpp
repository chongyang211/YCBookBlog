#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>
#include <random>

// N = 1024 时数组 4MB, 恰好塞不进一般 L2(256KB) 也塞不满 L3(几 MB)
constexpr int N = 1024;

static long long sumRowMajor(int (*a)[N]) {
    long long s = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            s += a[i][j];       // 顺序:同一行,连续 4B
    return s;
}
static long long sumColMajor(int (*a)[N]) {
    long long s = 0;
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i)
            s += a[i][j];       // 跳读:同一列,间隔 4*N 字节 = 4KB
    return s;
}

// tiling: 把大矩阵切成 32x32 块, 一块一块处理, 每块能塞进 L1
static long long sumTiled(int (*a)[N]) {
    constexpr int T = 32;   // tile 大小: 32*32*4 = 4KB, 塞进 L1
    long long s = 0;
    for (int ii = 0; ii < N; ii += T)
        for (int jj = 0; jj < N; jj += T)
            for (int i = ii; i < ii + T; ++i)
                for (int j = jj; j < jj + T; ++j)
                    s += a[i][j];
    return s;
}

int main() {
    auto* a = new int[N][N];
    std::mt19937 rng(42);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            a[i][j] = rng() & 0xFF;

    auto bench = [](const char* label, auto fn, int (*arr)[N]) {
        // 预热一次 (让数据进 cache)
        volatile long long warm = fn(arr);
        (void)warm;

        // 正式测 5 次取中位数
        std::vector<double> ts;
        for (int r = 0; r < 5; ++r) {
            auto t0 = std::chrono::high_resolution_clock::now();
            volatile long long s = fn(arr);
            auto t1 = std::chrono::high_resolution_clock::now();
            (void)s;
            double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            ts.push_back(ms);
        }
        std::sort(ts.begin(), ts.end());
        std::printf("%-15s median=%8.3f ms  (min %.3f, max %.3f)\n",
            label, ts[2], ts[0], ts[4]);
    };

    // 反直觉:先跑列优先(还没进 cache,冷启动更"公平")
    std::puts("┌─────────────────────────────────────────────────────────────┐");
    std::puts("│  ★ 矩阵 1024×1024 遍历 3 种方式对比                          │");
    std::puts("├─────────────────────────────────────────────────────────────┤");
    bench("col-major   ", sumColMajor,  a);   // 慢
    bench("row-major   ", sumRowMajor,  a);   // 快
    bench("row+tiling  ", sumTiled,     a);   // 更快
    std::puts("└─────────────────────────────────────────────────────────────┘");

    delete[] a;
    return 0;
}
