// 通用网络库 —— 迷你测试服务器实现（仅供测试，不用于生产）

#include "network/mini_test_server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <map>
#include <thread>

#include "nlohmann/json.hpp"

namespace http {
namespace test {

namespace {

// 读取直到连接关闭或 \r\n\r\n（请求头结束），再按 Content-Length 读 body
std::string ReadRequest(int fd) {
  std::string buf;
  char chunk[4096];
  while (true) {
    auto pos = buf.find("\r\n\r\n");
    if (pos != std::string::npos) {
      // 检查 body 是否读全
      std::string head = buf.substr(0, pos);
      size_t content_len = 0;
      size_t cl = head.find("Content-Length:");
      if (cl == std::string::npos) cl = head.find("content-length:");
      if (cl != std::string::npos) {
        content_len = static_cast<size_t>(std::stoul(head.substr(head.find(':', cl) + 1)));
      }
      if (buf.size() - pos - 4 >= content_len) break;
    }
    ssize_t n = ::recv(fd, chunk, sizeof(chunk), 0);
    if (n <= 0) break;
    buf.append(chunk, static_cast<size_t>(n));
  }
  return buf;
}

void WriteAll(int fd, const std::string& data) {
  size_t sent = 0;
  while (sent < data.size()) {
    ssize_t n = ::send(fd, data.data() + sent, data.size() - sent, 0);
    if (n <= 0) return;
    sent += static_cast<size_t>(n);
  }
}

void Respond(int fd, int status, const std::string& body) {
  const char* reason = status == 200 ? "OK" : "Internal Server Error";
  std::string resp = "HTTP/1.1 " + std::to_string(status) + " " + reason + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + std::to_string(body.size()) + "\r\n" +
                     "Connection: close\r\n\r\n" + body;
  WriteAll(fd, resp);
}

// 解析 "k1=v1&k2=v2" 为 JSON 对象（值按原字符串返回，不做类型猜测）
nlohmann::json ParseQueryString(const std::string& qs) {
  nlohmann::json obj = nlohmann::json::object();
  size_t start = 0;
  while (start < qs.size()) {
    size_t amp = qs.find('&', start);
    if (amp == std::string::npos) amp = qs.size();
    if (amp > start) {
      std::string kv = qs.substr(start, amp - start);
      size_t eq = kv.find('=');
      if (eq == std::string::npos) {
        obj[kv] = "";
      } else {
        obj[kv.substr(0, eq)] = kv.substr(eq + 1);
      }
    }
    start = amp + 1;
  }
  return obj;
}

}  // namespace

MiniTestServer::MiniTestServer(int port) { port_ = port; }

MiniTestServer::~MiniTestServer() { Stop(); }

bool MiniTestServer::Start() {
  listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd_ < 0) return false;

  int opt = 1;
  ::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(static_cast<uint16_t>(port_));

  if (::bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) return false;
  if (::listen(listen_fd_, 8) < 0) return false;

  // 取实际端口（port=0 时由系统分配）
  socklen_t len = sizeof(addr);
  ::getsockname(listen_fd_, reinterpret_cast<sockaddr*>(&addr), &len);
  port_ = ntohs(addr.sin_port);

  running_ = true;
  thread_ = std::thread(&MiniTestServer::ServeLoop, this);
  return true;
}

void MiniTestServer::Stop() {
  if (!running_.exchange(false)) return;
  ::shutdown(listen_fd_, SHUT_RDWR);  // 唤醒阻塞中的 accept
  ::close(listen_fd_);
  if (thread_.joinable()) thread_.join();
}

void MiniTestServer::ServeLoop() {
  while (running_) {
    int client_fd = ::accept(listen_fd_, nullptr, nullptr);
    if (client_fd < 0) break;  // Stop() 关闭 listen_fd_ 后退出
    HandleClient(client_fd);
    ::close(client_fd);
  }
}

void MiniTestServer::HandleClient(int client_fd) {
  std::string req = ReadRequest(client_fd);
  if (req.empty()) return;

  // 请求行："METHOD /path?query HTTP/1.1"
  size_t line_end = req.find("\r\n");
  std::string request_line = req.substr(0, line_end);
  size_t sp1 = request_line.find(' ');
  size_t sp2 = request_line.find(' ', sp1 + 1);
  std::string method = request_line.substr(0, sp1);
  std::string target = request_line.substr(sp1 + 1, sp2 - sp1 - 1);

  std::string path = target;
  std::string query;
  size_t qmark = target.find('?');
  if (qmark != std::string::npos) {
    path = target.substr(0, qmark);
    query = target.substr(qmark + 1);
  }

  // 请求体（去掉头部）
  std::string body;
  size_t header_end = req.find("\r\n\r\n");
  if (header_end != std::string::npos && req.size() > header_end + 4) {
    body = req.substr(header_end + 4);
  }

  nlohmann::json resp;
  if (path == "/get") {
    resp["code"] = 0;
    resp["method"] = "GET";
    resp["query"] = ParseQueryString(query);
    Respond(client_fd, 200, resp.dump());
  } else if (path == "/echo" && (method == "POST" || method == "PUT")) {
    // body 是合法 JSON 就原样返回（保留类型）；否则作为字符串返回（表单/protobuf 等）
    nlohmann::json parsed = nlohmann::json::parse(body, nullptr, false);
    resp["code"] = 0;
    resp["method"] = method;
    resp["body"] = parsed.is_discarded() ? nlohmann::json(body) : parsed;
    Respond(client_fd, 200, resp.dump());
  } else if (path == "/upload" && method == "POST") {
    // multipart：提取 filename 与内容大小（demo 只关心字段命中的文件名与字节数）
    std::string filename;
    size_t pos = body.find("filename=\"");
    if (pos != std::string::npos) {
      size_t start = pos + 10;
      size_t end = body.find('"', start);
      if (end != std::string::npos) filename = body.substr(start, end - start);
    }
    nlohmann::json resp;
    resp["code"] = 0;
    resp["method"] = "POST";
    resp["filename"] = filename;
    resp["size"] = body.size();
    Respond(client_fd, 200, resp.dump());
  } else if (path == "/slow") {
    // 模拟慢响应：?ms=xxx，用于验证客户端超时控制
    int ms = 300;
    auto q = ParseQueryString(query);
    if (q.contains("ms") && q["ms"].is_string()) ms = std::stoi(q["ms"].get<std::string>());
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    Respond(client_fd, 200, "{\"code\":0,\"msg\":\"slow done\"}");
  } else if (path.rfind("/status/", 0) == 0) {
    int code = std::stoi(path.substr(8));
    Respond(client_fd, code, "{\"code\":-1,\"msg\":\"mock error\"}");
  } else {
    Respond(client_fd, 404, "{\"code\":404,\"msg\":\"not found\"}");
  }
}

}  // namespace test
}  // namespace http
