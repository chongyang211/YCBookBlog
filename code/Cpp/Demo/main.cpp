// Demo — facility 通用库使用示例（线程库 + 网络库）
//
// 编译运行：
//   cd Cpp/Demo && cmake -B build -DCMAKE_BUILD_TYPE=Release
//   cmake --build build -j
//   ./build/demo              # 全部演示
//   ./build/demo thread       # 只看线程库
//   ./build/demo network      # 只看网络库（内置 mini 服务器，无需外网）
//   ./build/demo scheduler    # 只看周期任务调度器
//
// ============================ 线程库 ============================
// 核心模型：每个 Thread 内部有一个「任务队列」，Post() 把任务塞进队列，
// 线程主循环（RunCtx）不断取出并执行。于是：
//   - 投递到「同一个线程」的任务天然串行执行（FIFO），不需要加锁；
//   - 投递到「不同线程」的任务并行执行（各自跑各自的队列）。
//
// 主要 API 一览：
//   ThreadManager::IotThread()/NetworkThread()/...  -> 取全局命名线程
//   Thread::Post(callable)                    -> 把任务投递到该线程
//   Thread::GetName()/Name()/Id()             -> 线程标识
//   this_thread::Handle()                     -> 在任务内部拿到"当前线程"
//   this_thread::Post(callable)               -> 把任务排到当前线程队列尾部
//   ThreadManager::Pool()->Post(callable)          -> 投递任务到线程池（多工人线程并行）
//   ThreadManager::CreateThread(name)              -> 动态创建线程（调用方持有）
//   ThreadManager::CreateExtensibleThread(name)    -> 可扩展线程（支持任务后回调）
//   ExtensibleThread::SetPluginCtxHandler(fn) -> 设置"每执行完一个任务"的回调
//
// ============================ 网络库 ============================
// 设计借鉴：OkHttp（拦截器链）、Retrofit（Response<T>）、axios（链式构造）
//   HttpClient  链式请求：client.Get("/x").Query(...).Header(...).Send()
//   Interceptor 拦截器：日志 / 公共 Header / 鉴权 / 重试 / 签名（横切关注可插拔）
//   ApiClient   JSON API：api.Post<Resp>("/path", req) 一行拿到业务结构体
//   Upload/Download、同步/异步（回调 + future）、超时与重试
//
// ============================ 调度器 ============================
// 通用周期任务调度器：一个基准 tick 驱动所有任务（详见 scheduler/README.md）
//   思路：1 个心跳线程 + 每任务 frequency_multiplier 取模触发 → 线程数与任务数无关
//   sched::PeriodicTaskScheduler::Instance()
//     .RegisterTask(name, multiplier, func, priority, mode, offset, need_time_sync)
//     .SetTaskEnabled / SetTaskMultiplier  动态管理（禁用/改频率）
//     .SetTimeSyncGuard(fn)                注入时间守卫（未同步则跳过敏感任务）
//     .GetStats(name)                      执行次数 / 失败次数 / 耗时
//   ⚠️ 任务 lambda 捕获的对象必须活到"注销之后"（用 shared_ptr，勿捕局部变量引用）

#include "thread/thread_manager.h"

#include "network/api_client.h"
#include "network/http_client.h"
#include "network/interceptor.h"
#include "network/mini_test_server.h"
#include "scheduler/periodic_task_scheduler.h"
#include "nlohmann/json.hpp"

#include <atomic>
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// Demo 自带的极简线程安全日志（用法与 OrderSystem 的 Logger 一致）
struct Logger {
    static std::mutex& mtx() {
        static std::mutex m;
        return m;
    }

    template <typename... Args>
    static void info(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx());          // 整行原子输出
        std::cout << "[" << std::this_thread::get_id() << "] ";
        (std::cout << ... << std::forward<Args>(args));   // C++17 折叠表达式
        std::cout << "\n";
    }
};

// 工具：把任务投递到 target（Thread 或 ThreadPool 均可），阻塞等待其完成并取回返回值。
// 原理：用 packaged_task 把"返回值回传"这件事故化，future.get() 负责等待。
template <typename Target, typename Func>
auto PostAndWait(Target* target, Func&& func) {
    using R = std::invoke_result_t<Func&>;
    auto task = std::make_shared<std::packaged_task<R()>>(std::forward<Func>(func));
    auto fut  = task->get_future();
    target->Post([task]() { (*task)(); });   // 任务体：执行 packaged_task
    return fut.get();                        // 阻塞直到目标线程执行完（异常也会在此重抛）
}

// ---------------------------------------------------------------------------
// 1. 全局命名线程：ThreadManager::XxxThread()->Post()
// ---------------------------------------------------------------------------
void demo_global_threads() {
    Logger::info("=== 1. 全局命名线程 ===");

    // 取到线程对象后可以查看它的名字和 id
    facility::Thread* iot = facility::ThreadManager::IotThread();
    Logger::info("Iot 线程: name=", iot->GetName(), ", id=", iot->Id());

    // 1.1 不同线程并行工作：任务最终在各自所属的线程上执行
    facility::ThreadManager::NetworkThread()->Post([]() {
        Logger::info("[network] 收到网络包，当前线程 = ", facility::this_thread::Handle()->Name());
    });
    facility::ThreadManager::HeartBeatThread()->Post([]() {
        Logger::info("[heartbeat] 发送心跳，当前线程 = ", facility::this_thread::Handle()->Name());
    });
    facility::ThreadManager::OtaInstallThread()->Post([]() {
        Logger::info("[ota_install] 开始安装固件，当前线程 = ", facility::this_thread::Handle()->Name());
    });

    // 1.2 同一线程串行：投递到 Iot 的 5 个任务严格按 FIFO 依次执行，
    //     所以这里不用任何锁，也不会有数据竞争（顺序一定是 1..5）
    for (int i = 1; i <= 5; ++i) {
        facility::ThreadManager::IotThread()->Post([i]() {
            Logger::info("[iot] 第 ", i, " 个任务");
        });
    }

    // 1.3 需要结果就用 PostAndWait：它等任务跑完并把返回值带回来
    int sum = PostAndWait(iot, []() {
        int s = 0;
        for (int i = 1; i <= 100; ++i) s += i;
        return s;
    });
    Logger::info("Iot 线程上计算 1+2+...+100 = ", sum);
}

// ---------------------------------------------------------------------------
// 2. this_thread：任务内部再投递任务（把后续动作排回当前线程）
// ---------------------------------------------------------------------------
void demo_this_thread() {
    Logger::info("=== 2. this_thread（当前线程上下文）===");

    auto              done = std::make_shared<std::promise<void>>();
    std::future<void> fut  = done->get_future();

    facility::ThreadManager::IotThread()->Post([done]() {
        Logger::info("[iot] 任务 A 开始，当前线程 = ", facility::this_thread::Handle()->Name());
        // 把任务 B 排到"当前线程"队列的尾部，等 A 结束后由同一个线程执行。
        // 这种写法常用于：把耗时的后续步骤拆开，避免长时间占住队列。
        facility::this_thread::Post([done]() {
            Logger::info("[iot] 任务 B（在 A 内排队，同一线程串行执行）");
            done->set_value();
        });
        Logger::info("[iot] 任务 A 结束（B 紧随其后）");
    });

    fut.wait();  // 等 A、B 都执行完
}

// ---------------------------------------------------------------------------
// 3. 线程池：ThreadManager::Pool()，多个工人线程并发消费同一个队列
// ---------------------------------------------------------------------------
void demo_thread_pool() {
    Logger::info("=== 3. 线程池 ===");

    facility::ThreadPool* pool = facility::ThreadManager::Pool();
    Logger::info("线程池工人线程数 = ", facility::ThreadPool::kInitalThreadsNum);

    const int         n       = 8;
    auto              pending = std::make_shared<std::atomic<int>>(n);
    auto              done    = std::make_shared<std::promise<void>>();
    std::future<void> fut     = done->get_future();

    // Post：投递无返回值的任务，8 个任务会被 2 个工人线程并发消费
    for (int i = 0; i < n; ++i) {
        pool->Post([i, pending, done]() {
            Logger::info("[pool] 任务 ", i, " 由线程 ", std::this_thread::get_id(), " 执行");
            std::this_thread::sleep_for(std::chrono::milliseconds(30));  // 模拟耗时
            if (pending->fetch_sub(1) == 1) done->set_value();           // 最后一个任务唤醒主线程
        });
    }
    fut.wait();  // 等 8 个任务全部完成
    Logger::info("[pool] 8 个任务全部完成");
}

// ---------------------------------------------------------------------------
// 4. 动态创建线程：ThreadManager::CreateThread(name)
// ---------------------------------------------------------------------------
void demo_dynamic_thread() {
    Logger::info("=== 4. 动态创建线程 ===");

    // CreateThread 返回裸指针，所有权归调用方 —— 用 unique_ptr 托管，
    // 析构时会自动 Stop() + join()，不会泄漏。
    std::unique_ptr<facility::Thread> thr(facility::ThreadManager::CreateThread("worker"));
    Logger::info("已创建线程 name=", thr->GetName(), ", id=", thr->Id());

    thr->Post([]() { Logger::info("[worker] 任务 1"); });
    thr->Post([]() { Logger::info("[worker] 任务 2"); });

    int r = PostAndWait(thr.get(), []() { return 6 * 7; });
    Logger::info("[worker] 返回结果 = ", r);
}

// ---------------------------------------------------------------------------
// 5. 可扩展线程：ExtensibleThread + SetPluginCtxHandler
//    每执行完一个任务，框架回调 handler —— 适合做插件轮询 / 事件循环
// ---------------------------------------------------------------------------
void demo_extensible_thread() {
    Logger::info("=== 5. 可扩展线程（插件回调）===");

    // 注意：ExtensibleThread 的析构函数是 protected，所以要用基类指针 Thread*
    // 托管（Thread 是虚析构），才能被 unique_ptr 正确释放。
    facility::ExtensibleThread* ext = facility::ThreadManager::CreateExtensibleThread("plugin");
    std::unique_ptr<facility::Thread> thr(ext);
    auto ticks = std::make_shared<std::atomic<int>>(0);

    auto              done = std::make_shared<std::promise<void>>();
    std::future<void> fut  = done->get_future();

    ext->SetPluginCtxHandler([ticks, done]() {
        int n = ticks->fetch_add(1) + 1;
        Logger::info("[plugin] 任务后回调，tick=", n,
                     "，当前线程 = ", facility::this_thread::Handle()->Name());
        if (n == 3) done->set_value();
    });

    for (int i = 1; i <= 3; ++i) {
        ext->Post([i]() { Logger::info("[plugin] 执行业务任务 ", i); });
    }

    fut.wait();  // 等 3 个任务 + 3 次回调执行完
    Logger::info("[plugin] 演示结束");
}

// ===========================================================================
//                          网 络 库 演 示
// ===========================================================================

// 内置测试服务器地址：由 demo_network() 启动服务器后填入
std::string g_net_server_url;

// 业务结构体：与 sched::entity 约定一致（ToJson / FromJson）
namespace entity {

struct CreateUserReq {
    std::string name;
    int         age = 0;
    nlohmann::json ToJson() const { return {{"name", name}, {"age", age}}; }
};

struct CreateUserResp {
    std::string name;
    int         age = 0;
    static CreateUserResp FromJson(const nlohmann::json& j) {
        CreateUserResp r;
        if (j.contains("body")) {
            r.name = j["body"].value("name", "");
            r.age  = j["body"].value("age", 0);
        }
        return r;
    }
};

struct UploadResp {
    std::string filename;
    size_t      size = 0;
    static UploadResp FromJson(const nlohmann::json& j) {
        UploadResp r;
        r.filename = j.value("filename", "");
        r.size     = j.value("size", 0);
        return r;
    }
};

}  // namespace entity

// 计数拦截器：放在重试拦截器之后（更靠近网络层），每次真实请求 +1
class CountingInterceptor : public http::Interceptor {
 public:
    explicit CountingInterceptor(int& count) : count_(count) {}
    http::Response Intercept(http::Chain& chain) override {
        count_++;
        return chain.Proceed();
    }

 private:
    int& count_;
};

int net_failed = 0;
#define NET_CHECK(cond)                                                              \
    do {                                                                             \
        if (cond) {                                                                  \
            Logger::info("  [PASS] ", #cond);                                        \
        } else {                                                                     \
            Logger::info("  [FAIL] ", #cond, "  (line ", __LINE__, ")");             \
            net_failed++;                                                            \
        }                                                                            \
    } while (0)

// 1. 链式请求构造（requests/axios 风格）
void demo_net_fluent() {
    Logger::info("=== 网络 1. 链式构造 Fluent API ===");

    http::HttpClient client(g_net_server_url);
    auto r1 = client.Get("/get")
                  .Query({{"id", "42"}, {"verbose", "true"}})
                  .Header("X-Client", "demo")
                  .Timeout(std::chrono::seconds(5))
                  .Send();
    auto j1 = nlohmann::json::parse(r1.body);
    NET_CHECK(r1.Ok());
    NET_CHECK(j1["query"]["id"] == "42");

    auto r2 = client.Post("/echo")
                  .Body(R"({"name":"robot","ver":3})")
                  .Header("X-Trace", "abc-123")
                  .Send();
    auto j2 = nlohmann::json::parse(r2.body);
    NET_CHECK(r2.Ok());
    NET_CHECK(j2["body"]["name"] == "robot");
}

// 2. 拦截器链（OkHttp 风格）
void demo_net_interceptor() {
    Logger::info("=== 网络 2. 拦截器链（日志/Header/鉴权/签名/重试）===");

    std::vector<std::string> logs;
    http::ClientConfig cfg;
    cfg.logger         = [&logs](const std::string& s) { logs.push_back(s); };
    cfg.enable_logging = true;
    cfg.retry          = http::RetryPolicy{3, std::chrono::milliseconds(50)};

    http::HttpClient client(g_net_server_url, cfg);
    client.AddInterceptor(std::make_shared<http::interceptor::HeaderInterceptor>(
        http::Headers{{"X-App", "demo-app"}}));
    client.AddInterceptor(std::make_shared<http::interceptor::BearerAuthInterceptor>("tk-9527"));
    // 签名外置：不同接口需要不同签名算法时，挂多个 SigningInterceptor 即可
    client.AddInterceptor(std::make_shared<http::interceptor::SigningInterceptor>(
        [](const std::string& m, const std::string& path, const std::string& body) {
            return "SIGN(" + m + "," + path + "," + std::to_string(body.size()) + ")";
        }));

    auto r3 = client.Get("/get").Query({{"auth", "1"}}).Send();
    NET_CHECK(r3.Ok());
    NET_CHECK(logs.size() >= 2);
    Logger::info("  日志样例: ", logs.empty() ? "" : logs.front());

    int attempts = 0;
    client.AddInterceptor(std::make_shared<CountingInterceptor>(attempts));
    size_t before = logs.size();
    client.Get("/status/500").Send();
    size_t log_added = logs.size() - before;
    NET_CHECK(attempts == 3);    // 真实请求 3 次
    NET_CHECK(log_added == 2);   // 外层日志只呈现 1 组（重试对它透明，与 OkHttp 一致）
    Logger::info("  真实请求次数=", attempts, "，外层日志条数=", log_added, "（重试对最外层透明）");
}

// 3. Response<T> 泛型响应（Retrofit 风格）
void demo_net_generic() {
    Logger::info("=== 网络 3. Response<T> 泛型响应 ===");

    api::ApiClient api(g_net_server_url);
    entity::CreateUserReq req{"xiaoming", 18};
    auto resp = api.Post<entity::CreateUserResp>("/echo", req);
    NET_CHECK(resp.IsSuccess());
    NET_CHECK(resp.Data().name == "xiaoming");
    NET_CHECK(resp.Data().age == 18);
    Logger::info("  Post<CreateUserResp> -> name=", resp.Data().name, " age=", resp.Data().age);

    auto bad = api.Get<entity::CreateUserResp>("/status/500");
    NET_CHECK(!bad.IsSuccess());
    NET_CHECK(bad.status_code == 500);
}

// 4. 上传 / 下载
void demo_net_file() {
    Logger::info("=== 网络 4. 上传（Multipart）/ 下载 ===");

    {
        std::ofstream("/tmp/netclient_upload.txt") << "hello-netclient";
    }
    api::ApiClient api(g_net_server_url);
    auto up = api.Upload("/upload", "file", "/tmp/netclient_upload.txt");
    NET_CHECK(up.Ok());
    NET_CHECK(up.data["filename"] == "netclient_upload.txt");
    Logger::info("  upload -> ", up.data.dump());

    auto dl = api.Download("/get", "/tmp/netclient_download.json");
    NET_CHECK(dl.Ok());
    std::ifstream in("/tmp/netclient_download.json");
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    NET_CHECK(!content.empty());
}

// 5. 超时控制
void demo_net_timeout() {
    Logger::info("=== 网络 5. 超时控制 ===");

    http::HttpClient client(g_net_server_url);
    auto slow = client.Get("/slow")
                    .Query({{"ms", "800"}})
                    .Timeout(std::chrono::milliseconds(200))
                    .Send();
    NET_CHECK(!slow.Ok());
    NET_CHECK(slow.status_code == 0);  // 网络层失败
    Logger::info("  timeout error: ", slow.error);
}

// 6. 异步：回调 + future
void demo_net_async() {
    Logger::info("=== 网络 6. 异步（回调 / future）===");

    http::HttpClient client(g_net_server_url);
    std::atomic<int> cb_count{0};
    client.Get("/get").Query({{"async", "cb"}}).SendAsync([&](http::Response r) {
        if (r.Ok()) cb_count++;
    });
    for (int i = 0; i < 100 && cb_count == 0; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    NET_CHECK(cb_count == 1);

    auto fut = client.Post("/echo").Body(R"({"future":true})").SendAsync();
    auto fr  = fut.get();
    NET_CHECK(fr.Ok());
    NET_CHECK(nlohmann::json::parse(fr.body)["body"]["future"] == true);
}

int demo_network() {
    // 内置 mini 服务器：无需外网即可验证全部网络能力
    http::test::MiniTestServer server;
    if (!server.Start()) {
        Logger::info("[mini server] 启动失败");
        return 1;
    }
    g_net_server_url = server.base_url();
    Logger::info("########## 网络库用法演示 开始（内置服务器 ", g_net_server_url, "）##########");

    demo_net_fluent();
    demo_net_interceptor();
    demo_net_generic();
    demo_net_file();
    demo_net_timeout();
    demo_net_async();

    Logger::info("########## 网络库用法演示 ", (net_failed == 0 ? "ALL PASS" : "FAILED"),
                 " ##########");
    server.Stop();
    return net_failed == 0 ? 0 : 1;
}

// ===========================================================================
//                       周 期 任 务 调 度 器 演 示
// ===========================================================================

int sched_failed = 0;
#define SCHED_CHECK(cond)                                                            \
    do {                                                                             \
        if (cond) {                                                                  \
            Logger::info("  [PASS] ", #cond);                                        \
        } else {                                                                     \
            Logger::info("  [FAIL] ", #cond, "  (line ", __LINE__, ")");             \
            sched_failed++;                                                          \
        }                                                                            \
    } while (0)

// 1. 注册任务 + 倍频触发 + 优先级排序 + 错峰
//
// ⚠️ 关键坑：任务 lambda 捕获的对象，生命周期必须长于"任务注册期"。
//    如果捕获函数局部变量的引用（如 std::atomic<int>&），函数返回后引用悬垂，
//    下一个场景再 Start() 就会踩到已销毁的对象（崩溃）。
//    这里统一用 shared_ptr 持有计数器，并在场景结束注销任务（双保险）。
void demo_sched_basic() {
    Logger::info("=== 调度 1. 注册任务 / 倍频触发 / 优先级 / 错峰 ===");

    auto& s = sched::PeriodicTaskScheduler::Instance();
    s.SetTickInterval(std::chrono::milliseconds(200));  // 演示加速：tick = 200ms
    s.SetWorkerCount(2);

    auto critical = std::make_shared<std::atomic<int>>(0);
    auto normal   = std::make_shared<std::atomic<int>>(0);
    auto low      = std::make_shared<std::atomic<int>>(0);

    // 每 tick 都执行，同步模式（短任务），优先级最高
    s.RegisterTask("heartbeat", 1, [critical] { (*critical)++; },
                   sched::TaskPriority::kCritical, sched::TaskExecutionMode::kSync);
    // 每 2 个 tick 执行一次，异步模式
    s.RegisterTask("log_upload", 2, [normal] { (*normal)++; },
                   sched::TaskPriority::kNormal, sched::TaskExecutionMode::kAsync);
    // 每 3 个 tick 执行一次，且前 1 个 tick 跳过（错峰）
    s.RegisterTask("firmware_check", 3, [low] { (*low)++; },
                   sched::TaskPriority::kLow, sched::TaskExecutionMode::kAsync,
                   /*initial_offset=*/1);

    s.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(2100));  // 约 10 个 tick
    s.Stop();

    uint64_t ticks = s.tick_count();
    Logger::info("  已发生 tick = ", ticks, "，heartbeat=", critical->load(),
                 " log_upload=", normal->load(), " firmware_check=", low->load());

    SCHED_CHECK(ticks >= 9);                       // 心跳累计
    SCHED_CHECK(critical->load() >= 9);            // 倍频 1：每 tick 都执行
    SCHED_CHECK(normal->load() >= 4);              // 倍频 2：约一半
    SCHED_CHECK(low->load() >= 2 && low->load() <= 4);  // 倍频 3 + 错峰 1 tick
    SCHED_CHECK(normal->load() < critical->load());  // 倍频越高执行越少

    s.UnregisterTask("heartbeat");
    s.UnregisterTask("log_upload");
    s.UnregisterTask("firmware_check");
}

// 2. 动态管理：改频率、禁用/启用、注销
void demo_sched_dynamic() {
    Logger::info("=== 调度 2. 动态改频率 / 禁用 / 注销 ===");

    auto& s = sched::PeriodicTaskScheduler::Instance();
    s.SetTickInterval(std::chrono::milliseconds(200));

    auto n = std::make_shared<std::atomic<int>>(0);
    s.RegisterTask("report", 1, [n] { (*n)++; }, sched::TaskPriority::kNormal,
                   sched::TaskExecutionMode::kSync);

    s.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(650));  // ~3 tick
    int before_disable = n->load();

    s.SetTaskEnabled("report", false);                            // 禁用
    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    int after_disable = n->load();
    Logger::info("  禁用前执行=", before_disable, "，禁用后执行=", after_disable);
    SCHED_CHECK(before_disable >= 2);
    SCHED_CHECK(after_disable == before_disable);                 // 禁用后不再执行

    s.SetTaskEnabled("report", true);                             // 重新启用
    std::this_thread::sleep_for(std::chrono::milliseconds(450));
    SCHED_CHECK(n->load() > after_disable);                       // 恢复执行

    s.SetTaskMultiplier("report", 5);                             // 频率调低（每 5 tick）
    int before_slow = n->load();
    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    SCHED_CHECK(n->load() - before_slow <= 2);                    // 明显变慢

    s.Stop();
    s.UnregisterTask("report");
}

// 3. 时间同步守卫：时间未同步时跳过敏感任务（可注入 = 可测试）
void demo_sched_time_guard() {
    Logger::info("=== 调度 3. 时间同步守卫（TimeSyncGuard 注入）===");

    auto& s = sched::PeriodicTaskScheduler::Instance();
    s.SetTickInterval(std::chrono::milliseconds(200));

    auto sensitive = std::make_shared<std::atomic<int>>(0);
    auto ordinary  = std::make_shared<std::atomic<int>>(0);
    auto time_synced = std::make_shared<std::atomic<bool>>(false);
    s.SetTimeSyncGuard([time_synced] { return time_synced->load(); });  // 模拟"时间未同步"

    s.RegisterTask("cert_refresh", 1, [sensitive] { (*sensitive)++; },
                   sched::TaskPriority::kCritical, sched::TaskExecutionMode::kSync,
                   0, /*requires_time_synced=*/true);
    s.RegisterTask("metrics", 1, [ordinary] { (*ordinary)++; },
                   sched::TaskPriority::kNormal, sched::TaskExecutionMode::kSync,
                   0, /*requires_time_synced=*/false);

    s.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    SCHED_CHECK(sensitive->load() == 0);      // 时间未同步 → 敏感任务全程跳过
    SCHED_CHECK(ordinary->load() >= 2);       // 普通任务不受影响

    time_synced->store(true);                  // 时间恢复同步
    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    Logger::info("  时间同步后 cert_refresh 执行=", sensitive->load(), " 次");
    SCHED_CHECK(sensitive->load() >= 2);      // 恢复后开始执行

    s.Stop();
    s.UnregisterTask("cert_refresh");
    s.UnregisterTask("metrics");
}

// 4. 统计：执行次数 / 失败次数 / 耗时
void demo_sched_stats() {
    Logger::info("=== 调度 4. 任务统计（次数 / 失败 / 耗时）===");

    auto& s = sched::PeriodicTaskScheduler::Instance();
    s.SetTickInterval(std::chrono::milliseconds(200));

    s.RegisterTask("flaky_job", 1,
                   [] {
                       std::this_thread::sleep_for(std::chrono::milliseconds(10));
                       throw std::runtime_error("simulated failure");
                   },
                   sched::TaskPriority::kNormal, sched::TaskExecutionMode::kSync);

    s.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    s.Stop();

    auto st = s.GetStats("flaky_job");
    Logger::info("  flaky_job: 成功=", st.executions, " 失败=", st.failures,
                 " 总耗时=", st.total_ms, "ms 最大=", st.max_ms, "ms 最小=", st.min_ms, "ms");
    SCHED_CHECK(st.executions == 0);       // 抛异常的任务不计成功
    SCHED_CHECK(st.failures >= 2);         // 记为失败（不会拖垮调度器）
    SCHED_CHECK(st.max_ms >= 10);          // 耗时统计到

    s.UnregisterTask("flaky_job");
}

int demo_scheduler() {
    Logger::info("########## 周期任务调度器用法演示 开始 ##########");
    demo_sched_basic();
    demo_sched_dynamic();
    demo_sched_time_guard();
    demo_sched_stats();
    Logger::info("########## 周期任务调度器用法演示 ",
                 (sched_failed == 0 ? "ALL PASS" : "FAILED"), " ##########");
    return sched_failed == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
    const std::string which = (argc > 1) ? argv[1] : "all";
    const bool run_thread     = (which == "all" || which == "thread");
    const bool run_network    = (which == "all" || which == "network");
    const bool run_scheduler  = (which == "all" || which == "scheduler");

    int rc = 0;
    if (run_thread) {
        Logger::info("########## 线程库用法演示 开始 ##########");
        demo_global_threads();
        demo_this_thread();
        demo_thread_pool();
        demo_dynamic_thread();
        demo_extensible_thread();
        Logger::info("########## 线程库用法演示 结束 ##########");
    }
    if (run_network) {
        rc = demo_network();
    }
    if (run_scheduler) {
        int r = demo_scheduler();
        if (r != 0) rc = r;
    }
    return rc;
}
