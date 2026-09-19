// 通用网络库 —— 迷你 HTTP 测试服务器（POSIX socket，单线程逐连接处理）。
//
// 用途：在无外网环境下验证 HttpClient / ApiClient 的全部能力
//      （链式构造、拦截器、泛型响应、上传下载、超时、异步）。
// 定位：测试替身（Test Double），只求行为可控，不追求性能与并发，不上生产。
//
// 路由：
//   GET  /get?k=v  → {"code":0,"method":"GET","query":{"k":"v"}}
//   POST /echo     → {"code":0,"method":"POST","body":<原样 JSON>}
//   GET  /status/500 → 返回 HTTP 500（验证错误处理/重试）

#pragma once

#include <atomic>
#include <string>
#include <thread>

namespace http {
namespace test {

class MiniTestServer {
 public:
  // @param port 0 表示由系统分配空闲端口（用 port() 取实际值）
  explicit MiniTestServer(int port = 0);
  ~MiniTestServer();

  bool Start();
  void Stop();

  int port() const { return port_; }
  std::string base_url() const { return "http://127.0.0.1:" + std::to_string(port_); }

 private:
  void ServeLoop();
  // 处理单个连接：读取请求行与 body，返回响应
  void HandleClient(int client_fd);

  int listen_fd_ = -1;
  int port_ = 0;
  std::atomic<bool> running_{false};
  std::thread thread_;
};

}  // namespace test
}  // namespace http
