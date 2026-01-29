# 网络检测设计方案

## 概述
设计一个分层的网络检测系统，先检查本地网络状况和连通性，在确认网络正常后再调用服务端接口，确保系统的网络通信稳定可靠。

## 现有网络检测能力分析

### 当前系统提供的功能
- `NetworkMonitorService` - 网络状态监控服务
- `NetworkInfo` - 网络类型和连接状态管理
- `SystemInterface::GetNetworkInfo()` - 获取网络详细信息
- `SystemInterface::IsNetworkInterfaceUp()` - 检查接口状态
- 网络信号变化监听 (`DetectedNetworkResultSig`)

### 现有网络类型支持
```cpp
enum class NetworkType {
  kNone = 10000,      // 无网络
  kCellular = 10001,  // 蜂窝网络
  kWifi = 10002,      // WiFi
  kEthernet = 10003,  // 以太网
  kOther = 10004,     // 其他
};

enum class NetworkConnectState {
  kDisconnected = 0,  // 断开连接
  kConnecting = 1,    // 连接中
  kConnected = 2,     // 已连接
  kUnknown = 99,      // 未知状态
};
```

## 网络检测设计方案

### 1. 分层检测架构

```
┌─────────────────────────────────────┐
│           应用层服务调用              │
├─────────────────────────────────────┤
│         网络连通性检测层              │
│  - 外网连通性检测                    │
│  - DNS解析检测                      │
│  - 服务端可达性检测                  │
├─────────────────────────────────────┤
│         本地网络检测层               │
│  - 网络接口状态检测                  │
│  - IP配置检测                       │
│  - 网关可达性检测                    │
├─────────────────────────────────────┤
│         物理层检测                   │
│  - 网络接口物理状态                  │
│  - 信号强度检测                      │
└─────────────────────────────────────┘
```

### 2. 核心检测类设计

#### 2.1 设计思路

**🎯 设计目标**
- **分层检测**：按照网络协议栈从底层到应用层逐步检测
- **快速失败**：一旦某层检测失败，立即返回错误，避免无效检测
- **简单实用**：保持接口简洁，易于理解和使用
- **详细诊断**：提供具体的错误信息和网络状态详情

**🔄 检测流程设计**
```
开始检测
    ↓
物理层检测 → 失败 → 返回物理层错误
    ↓ 成功
本地网络检测 → 失败 → 返回本地网络错误  
    ↓ 成功
连通性检测 → 失败 → 返回连通性错误
    ↓ 成功
服务端检测 → 失败 → 返回服务端错误
    ↓ 成功
检测完成，返回成功结果
```

#### 2.2 核心类实现

```cpp
class NetworkDetector {
public:
    enum class DetectionLevel {
        kPhysical,      // 物理层检测
        kLocal,         // 本地网络检测  
        kConnectivity,  // 连通性检测
        kService        // 服务检测
    };
    
    struct DetectionResult {
        bool success;
        DetectionLevel level;
        std::string error_message;
        std::map<std::string, std::string> details;
    };
    
    // 执行完整网络检测
    static DetectionResult PerformFullDetection();
    
    // 分层检测方法
    static DetectionResult CheckPhysicalLayer();
    static DetectionResult CheckLocalNetwork();
    static DetectionResult CheckConnectivity();
    static DetectionResult CheckServiceReachability();
    
    // 异步检测
    static void PerformDetectionAsync(std::function<void(DetectionResult)> callback);

private:
    // 辅助方法
    static bool TestHostReachability(const std::string& host, int timeout_sec = 5);
    static std::string GetNetworkInterfaceInfo(const std::string& interface);
};
```

#### 2.3 设计优势

**✅ 简洁性**
- 接口简单明了，只包含必要的方法
- 结果结构清晰，包含成功状态、错误级别、错误信息和详细信息

**✅ 实用性**
- 分层检测，快速定位问题所在层级
- 支持同步和异步两种调用方式
- 详细的错误信息便于问题诊断

**✅ 可维护性**
- 每个检测方法职责单一，易于理解和维护
- 使用现有的系统接口，减少依赖复杂度

### 3. 具体检测实现

#### 3.1 物理层检测
```cpp
NetworkDetector::DetectionResult NetworkDetector::CheckPhysicalLayer() {
    DetectionResult result;
    result.level = DetectionLevel::kPhysical;
    
    // 1. 检查网络接口物理状态
    std::string active_interface = SystemInterface::Instance()->GetRawInterfaceName();
    if (active_interface.empty()) {
        result.success = false;
        result.error_message = "No active network interface found";
        return result;
    }
    
    // 2. 检查接口是否UP
    bool interface_up = SystemInterface::Instance()->IsNetworkInterfaceUp(active_interface);
    if (!interface_up) {
        result.success = false;
        result.error_message = "Network interface is down: " + active_interface;
        return result;
    }
    
    // 3. 检查信号强度(WiFi/蜂窝网络)
    NetworkType net_type = NetworkInfo::Instance()->GetNetworkType();
    if (net_type == NetworkType::kWifi || net_type == NetworkType::kCellular) {
        // 获取信号强度信息
        SystemInterface::Instance()->GetNetworkInfo([&result](error_code ec, const std::string& info) {
            if (!ec) {
                auto json = nlohmann::json::parse(info);
                result.details["signal_strength"] = std::to_string(json.value("signal", 0));
            }
        });
    }
    
    result.success = true;
    result.details["interface"] = active_interface;
    result.details["network_type"] = NetworkTypeToString(net_type);
    return result;
}
```

#### 3.2 本地网络检测
```cpp
NetworkDetector::DetectionResult NetworkDetector::CheckLocalNetwork() {
    DetectionResult result;
    result.level = DetectionLevel::kLocal;
    
    // 1. 检查IP配置
    std::string interface = SystemInterface::Instance()->GetRawInterfaceName();
    std::string ip_info = GetResultFromSystemCommand("ip addr show " + interface);
    
    if (ip_info.find("inet ") == std::string::npos) {
        result.success = false;
        result.error_message = "No IP address assigned to interface: " + interface;
        return result;
    }
    
    // 2. 检查默认网关
    std::string gateway = GetResultFromSystemCommand("ip route show default | awk '{print $3}' | head -1");
    if (gateway.empty()) {
        result.success = false;
        result.error_message = "No default gateway configured";
        return result;
    }
    
    // 3. 测试网关可达性
    std::string ping_gateway = GetResultFromSystemCommand("ping -c 1 -W 3 " + gateway + " >/dev/null 2>&1; echo $?");
    if (ping_gateway != "0\n") {
        result.success = false;
        result.error_message = "Gateway unreachable: " + gateway;
        return result;
    }
    
    result.success = true;
    result.details["gateway"] = gateway;
    result.details["interface"] = interface;
    return result;
}
```

#### 3.3 连通性检测
```cpp
NetworkDetector::DetectionResult NetworkDetector::CheckConnectivity() {
    DetectionResult result;
    result.level = DetectionLevel::kConnectivity;
    
    // 1. DNS解析测试
    std::string dns_test = GetResultFromSystemCommand("nslookup www.qq.com >/dev/null 2>&1; echo $?");
    if (dns_test != "0\n") {
        result.success = false;
        result.error_message = "DNS resolution failed";
        return result;
    }
    
    // 2. 外网连通性测试
    std::vector<std::string> test_hosts = {"8.8.8.8", "114.114.114.114", "www.qq.com"};
    int success_count = 0;
    
    for (const auto& host : test_hosts) {
        std::string ping_result = GetResultFromSystemCommand("ping -c 1 -W 5 " + host + " >/dev/null 2>&1; echo $?");
        if (ping_result == "0\n") {
            success_count++;
        }
    }
    
    if (success_count == 0) {
        result.success = false;
        result.error_message = "No external connectivity";
        return result;
    }
    
    result.success = true;
    result.details["reachable_hosts"] = std::to_string(success_count) + "/" + std::to_string(test_hosts.size());
    return result;
}
```

#### 3.4 服务端可达性检测
```cpp
NetworkDetector::DetectionResult NetworkDetector::CheckServiceReachability() {
    DetectionResult result;
    result.level = DetectionLevel::kService;
    
    // 1. 检查服务端域名解析
    std::string service_host = "your-service-domain.com";  // 替换为实际服务域名
    std::string dns_check = GetResultFromSystemCommand("nslookup " + service_host + " >/dev/null 2>&1; echo $?");
    
    if (dns_check != "0\n") {
        result.success = false;
        result.error_message = "Service domain resolution failed: " + service_host;
        return result;
    }
    
    // 2. 测试服务端口连通性
    std::string port_check = GetResultFromSystemCommand("timeout 5 bash -c '</dev/tcp/" + service_host + "/443' >/dev/null 2>&1; echo $?");
    if (port_check != "0\n") {
        result.success = false;
        result.error_message = "Service port unreachable: " + service_host + ":443";
        return result;
    }
    
    // 3. 简单HTTP健康检查
    std::string http_check = GetResultFromSystemCommand("curl -s --connect-timeout 5 --max-time 10 -o /dev/null -w '%{http_code}' https://" + service_host + "/health");
    if (http_check != "200") {
        result.success = false;
        result.error_message = "Service health check failed, HTTP code: " + http_check;
        return result;
    }
    
    result.success = true;
    result.details["service_host"] = service_host;
    result.details["http_status"] = http_check;
    return result;
}
```

### 4. 完整检测流程实现

```cpp
NetworkDetector::DetectionResult NetworkDetector::PerformFullDetection() {
    DetectionResult final_result;
    
    // 1. 物理层检测
    auto physical_result = CheckPhysicalLayer();
    if (!physical_result.success) {
        return physical_result;
    }
    
    // 2. 本地网络检测
    auto local_result = CheckLocalNetwork();
    if (!local_result.success) {
        return local_result;
    }
    
    // 3. 连通性检测
    auto connectivity_result = CheckConnectivity();
    if (!connectivity_result.success) {
        return connectivity_result;
    }
    
    // 4. 服务端可达性检测
    auto service_result = CheckServiceReachability();
    if (!service_result.success) {
        return service_result;
    }
    
    // 所有检测通过
    final_result.success = true;
    final_result.level = DetectionLevel::kService;
    final_result.error_message = "All network checks passed";
    
    // 合并所有检测详情
    final_result.details.insert(physical_result.details.begin(), physical_result.details.end());
    final_result.details.insert(local_result.details.begin(), local_result.details.end());
    final_result.details.insert(connectivity_result.details.begin(), connectivity_result.details.end());
    final_result.details.insert(service_result.details.begin(), service_result.details.end());
    
    return final_result;
}
```

### 5. 异步检测实现

```cpp
void NetworkDetector::PerformDetectionAsync(std::function<void(DetectionResult)> callback) {
    Threads::MainThread()->Post([callback]() {
        auto result = PerformFullDetection();
        callback(result);
    });
}
```

### 6. 集成到现有系统

#### 6.1 在服务调用前进行检测
```cpp
class ServiceClient {
public:
    template<typename T>
    void CallServiceWithNetworkCheck(std::function<T()> service_call, 
                                   std::function<void(bool, T)> callback) {
        NetworkDetector::PerformDetectionAsync([=](NetworkDetector::DetectionResult result) {
            if (result.success) {
                // 网络检测通过，执行服务调用
                try {
                    T response = service_call();
                    callback(true, response);
                } catch (const std::exception& e) {
                    LOG_E("Service call failed: {}", e.what());
                    callback(false, T{});
                }
            } else {
                // 网络检测失败，记录错误并返回
                LOG_E("Network detection failed at level {}: {}", 
                      static_cast<int>(result.level), result.error_message);
                callback(false, T{});
            }
        });
    }
};
```

#### 6.2 定期网络健康检查
```cpp
class NetworkHealthMonitor {
private:
    std::shared_ptr<asio::steady_timer> health_check_timer_;
    std::chrono::seconds check_interval_ = std::chrono::seconds(300); // 5分钟检查一次
    
public:
    void StartHealthCheck() {
        health_check_timer_ = std::make_shared<asio::steady_timer>(
            Threads::MainThread()->Executor(), check_interval_);
        
        ScheduleNextCheck();
    }
    
private:
    void ScheduleNextCheck() {
        health_check_timer_->async_wait([this](const error_code& error) {
            if (!error) {
                NetworkDetector::PerformDetectionAsync([this](NetworkDetector::DetectionResult result) {
                    if (!result.success) {
                        LOG_W("Network health check failed: {}", result.error_message);
                        // 可以触发网络修复或告警
                    } else {
                        LOG_I("Network health check passed");
                    }
                    
                    // 安排下次检查
                    health_check_timer_->expires_after(check_interval_);
                    ScheduleNextCheck();
                });
            }
        });
    }
};
```

## 使用示例

### 基本使用
```cpp
// 同步检测
auto result = NetworkDetector::PerformFullDetection();
if (result.success) {
    LOG_I("Network is healthy, proceeding with service calls");
    // 执行服务端API调用
} else {
    LOG_E("Network issue detected: {}", result.error_message);
    // 处理网络问题
}

// 异步检测
NetworkDetector::PerformDetectionAsync([](NetworkDetector::DetectionResult result) {
    if (result.success) {
        // 网络正常，执行业务逻辑
    } else {
        // 网络异常，执行错误处理
    }
});
```

### 集成到现有服务
```cpp
// 在IoT服务中集成网络检测
void IotService::RequestPullcmdWithNetworkCheck() {
    NetworkDetector::PerformDetectionAsync([this](NetworkDetector::DetectionResult result) {
        if (result.success) {
            // 网络正常，执行原有的拉取命令逻辑
            auto [ec, instructions] = RequestPullcmd();
            // ... 处理指令
        } else {
            LOG_W("Network detection failed, skipping command pull: {}", result.error_message);
            // 可以设置重试机制或告警
        }
    });
}
```

## 总结

该网络检测设计方案提供了：

1. **分层检测**：从物理层到服务层的全面检测
2. **异步支持**：不阻塞主线程的异步检测机制
3. **详细诊断**：提供具体的错误信息和网络状态详情
4. **易于集成**：可以轻松集成到现有的服务调用流程中
5. **健康监控**：支持定期的网络健康检查

通过这个方案，可以确保在网络状况良好时才进行服务端API调用，提高系统的稳定性和用户体验。