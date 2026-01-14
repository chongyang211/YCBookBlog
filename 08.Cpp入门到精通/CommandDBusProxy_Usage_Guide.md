# CommandDBusProxy 使用指南

## 概述

CommandDBusProxy是IoT服务中用于命令分发的DBus通信代理类，负责向外部应用发送命令信号和接收状态上报。

## 基本使用流程

### 1. 初始化和配置

```cpp
#include "cmds/command_dbus_proxy.h"

class MyService {
private:
    std::unique_ptr<CommandDBusProxy> command_proxy_;

public:
    bool Initialize() {
        // 1. 创建CommandDBusProxy实例
        command_proxy_ = std::make_unique<CommandDBusProxy>();
        
        // 2. 初始化DBus连接
        if (!command_proxy_->Initialize()) {
            LOG_E("Failed to initialize CommandDBusProxy");
            return false;
        }
        
        // 3. 设置状态上报回调函数
        command_proxy_->SetReportCommandStatusCallback(
            [this](const std::string& command_id, int32_t state, 
                   const std::string& biz_result, int32_t code, 
                   const std::string& message) -> int32_t {
                return HandleCommandStatus(command_id, state, biz_result, code, message);
            }
        );
        
        LOG_I("CommandDBusProxy initialized successfully");
        return true;
    }
    
    void Cleanup() {
        if (command_proxy_) {
            command_proxy_->Uninitialize();
        }
    }
};
```

### 2. 发送命令到外部应用

```cpp
bool SendCommandToExternalApp(const std::string& cmd_id, 
                             const std::string& cmd_type,
                             const std::string& target_app,
                             const std::string& cmd_data) {
    if (!command_proxy_) {
        LOG_E("CommandDBusProxy not initialized");
        return false;
    }
    
    // 发送CommandReceived信号
    bool success = command_proxy_->BroadcastCommandReceived(
        cmd_id,      // 命令ID
        cmd_type,    // 命令类型 (如 "clear_voice_cache")
        target_app,  // 目标应用 (如 "palmapp")
        cmd_data     // 命令数据 (JSON格式)
    );
    
    if (success) {
        LOG_I("Command sent successfully: id={} type={} app={}", 
              cmd_id, cmd_type, target_app);
    } else {
        LOG_E("Failed to send command: id={} type={} app={}", 
              cmd_id, cmd_type, target_app);
    }
    
    return success;
}
```

### 3. 处理状态上报

```cpp
int32_t HandleCommandStatus(const std::string& command_id, int32_t state, 
                           const std::string& biz_result, int32_t code, 
                           const std::string& message) {
    LOG_I("Received command status: id={} state={} code={} msg={}", 
          command_id, state, code, message);
    
    // 根据状态码处理不同情况
    if (state >= 1000 && state <= 1999) {
        // 刷掌App状态码范围
        HandlePalmAppStatus(command_id, state, biz_result, message);
    } else if (state >= 2000 && state <= 8999) {
        // 其他第三方App状态码
        HandleThirdPartyAppStatus(command_id, state, biz_result, message);
    } else {
        LOG_W("Unknown status code: {}", state);
        return -1;
    }
    
    return 0; // 返回0表示处理成功
}

void HandlePalmAppStatus(const std::string& command_id, int32_t state, 
                        const std::string& biz_result, const std::string& message) {
    switch (state) {
        case 1000: // PALM_PROCESSING
            LOG_I("Palm app is processing command: {}", command_id);
            break;
            
        case 1001: // PALM_SUCCESS
            LOG_I("Palm app completed command: {}", command_id);
            OnCommandCompleted(command_id, biz_result);
            break;
            
        case 1002: // PALM_FAILED
            LOG_E("Palm app failed to execute command: {} error: {}", command_id, message);
            OnCommandFailed(command_id, message);
            break;
            
        case 1100: // VOICE_CACHE_CLEARING
            LOG_I("Voice cache clearing in progress: {}", command_id);
            break;
            
        case 1101: // VOICE_CACHE_CLEARED
            LOG_I("Voice cache cleared successfully: {}", command_id);
            HandleVoiceCacheCleared(command_id, biz_result);
            break;
            
        case 1103: // VOICE_CACHE_ERROR
            LOG_E("Voice cache clear failed: {} error: {}", command_id, message);
            OnCommandFailed(command_id, message);
            break;
            
        default:
            LOG_W("Unknown palm app status: {}", state);
            break;
    }
}
```

## 具体使用场景

### 场景1: 语音缓存清除

```cpp
void ClearVoiceCache(const std::string& language) {
    // 1. 生成命令ID
    std::string cmd_id = GenerateCommandId();
    
    // 2. 构建命令数据
    nlohmann::json cmd_data;
    cmd_data["language"] = language;
    cmd_data["app_id"] = "com.tencent.palmapp";
    
    // 3. 发送命令到刷掌App
    bool success = command_proxy_->BroadcastCommandReceived(
        cmd_id,
        "clear_voice_cache",
        "palmapp", 
        cmd_data.dump()
    );
    
    if (success) {
        LOG_I("Voice cache clear command sent: language={} id={}", language, cmd_id);
        // 可以启动超时定时器
        StartCommandTimeout(cmd_id, 60); // 60秒超时
    } else {
        LOG_E("Failed to send voice cache clear command");
    }
}

void HandleVoiceCacheCleared(const std::string& command_id, const std::string& result) {
    try {
        auto json_result = nlohmann::json::parse(result);
        std::string language = json_result.value("language", "unknown");
        int files_deleted = json_result.value("files_deleted", 0);
        int64_t bytes_freed = json_result.value("bytes_freed", 0);
        
        LOG_I("Voice cache cleared for {}: {} files, {} bytes freed", 
              language, files_deleted, bytes_freed);
        
        // 更新命令状态为成功
        UpdateCommandStatus(command_id, "success", result);
        
    } catch (const std::exception& e) {
        LOG_W("Failed to parse voice cache clear result: {}", e.what());
    }
}
```

### 场景2: 设备重启命令

```cpp
void SendRebootCommand(int delay_seconds) {
    std::string cmd_id = GenerateCommandId();
    
    nlohmann::json cmd_data;
    cmd_data["delay"] = delay_seconds;
    cmd_data["app_id"] = "com.tencent.palmapp";
    
    bool success = command_proxy_->BroadcastCommandReceived(
        cmd_id,
        "reboot_device",
        "palmapp",
        cmd_data.dump()
    );
    
    if (success) {
        LOG_I("Reboot command sent: delay={}s id={}", delay_seconds, cmd_id);
    }
}
```

### 场景3: 批量命令处理

```cpp
void ProcessCommandBatch(const std::vector<CommandInfo>& commands) {
    for (const auto& cmd : commands) {
        // 检查是否为外部App命令
        if (IsExternalAppCommand(cmd.app_id)) {
            // 通过CommandDBusProxy分发
            bool success = command_proxy_->BroadcastCommandReceived(
                cmd.id, cmd.type, GetAppName(cmd.app_id), cmd.data
            );
            
            if (success) {
                LOG_I("External command dispatched: id={} type={} app={}", 
                      cmd.id, cmd.type, cmd.app_id);
                // 记录命令状态
                pending_commands_[cmd.id] = cmd;
            } else {
                LOG_E("Failed to dispatch external command: id={}", cmd.id);
                ReportCommandFailure(cmd.id, "Failed to dispatch to external app");
            }
        } else {
            // 内部命令直接处理
            ProcessInternalCommand(cmd);
        }
    }
}
```

## 错误处理和最佳实践

### 1. 初始化检查

```cpp
bool IsReady() const {
    return command_proxy_ && command_proxy_->IsInitialized();
}

bool SendCommand(const std::string& cmd_id, const std::string& cmd_type,
                const std::string& target_app, const std::string& cmd_data) {
    if (!IsReady()) {
        LOG_E("CommandDBusProxy not ready");
        return false;
    }
    
    return command_proxy_->BroadcastCommandReceived(cmd_id, cmd_type, target_app, cmd_data);
}
```

### 2. 超时处理

```cpp
class CommandTimeoutManager {
private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> command_timestamps_;
    std::shared_ptr<boost::asio::steady_timer> cleanup_timer_;

public:
    void StartCommandTimeout(const std::string& cmd_id, int timeout_seconds) {
        command_timestamps_[cmd_id] = std::chrono::steady_clock::now();
        
        // 启动超时检查
        auto timer = std::make_shared<boost::asio::steady_timer>(
            executor_, std::chrono::seconds(timeout_seconds));
        
        timer->async_wait([this, cmd_id](const boost::system::error_code& ec) {
            if (!ec) {
                HandleCommandTimeout(cmd_id);
            }
        });
    }
    
    void HandleCommandTimeout(const std::string& cmd_id) {
        if (command_timestamps_.find(cmd_id) != command_timestamps_.end()) {
            LOG_W("Command timeout: {}", cmd_id);
            command_timestamps_.erase(cmd_id);
            
            // 上报超时状态
            ReportCommandFailure(cmd_id, "Command execution timeout");
        }
    }
    
    void OnCommandCompleted(const std::string& cmd_id) {
        // 命令完成时清除超时记录
        command_timestamps_.erase(cmd_id);
    }
};
```

### 3. 状态同步

```cpp
class CommandStatusTracker {
private:
    std::unordered_map<std::string, CommandStatus> command_status_;

public:
    void UpdateCommandStatus(const std::string& cmd_id, int32_t state, 
                           const std::string& result) {
        CommandStatus& status = command_status_[cmd_id];
        status.current_state = state;
        status.last_update = std::chrono::steady_clock::now();
        status.result = result;
        
        // 如果是终态，可以清理记录
        if (IsFinalState(state)) {
            // 延迟清理，保留一段时间用于查询
            ScheduleCleanup(cmd_id, std::chrono::minutes(5));
        }
    }
    
    bool IsFinalState(int32_t state) {
        return state == 1001 || state == 1002 || state == 1101 || state == 1103;
    }
};
```

## 完整集成示例

```cpp
class IoTCommandService {
private:
    std::unique_ptr<CommandDBusProxy> command_proxy_;
    CommandTimeoutManager timeout_manager_;
    CommandStatusTracker status_tracker_;

public:
    bool Initialize() {
        command_proxy_ = std::make_unique<CommandDBusProxy>();
        
        if (!command_proxy_->Initialize()) {
            return false;
        }
        
        command_proxy_->SetReportCommandStatusCallback(
            [this](const std::string& id, int32_t state, const std::string& result, 
                   int32_t code, const std::string& msg) -> int32_t {
                
                // 更新状态跟踪
                status_tracker_.UpdateCommandStatus(id, state, result);
                
                // 处理命令完成
                if (status_tracker_.IsFinalState(state)) {
                    timeout_manager_.OnCommandCompleted(id);
                }
                
                // 业务逻辑处理
                return HandleCommandStatus(id, state, result, code, msg);
            }
        );
        
        return true;
    }
    
    bool ExecuteExternalCommand(const std::string& cmd_type, 
                               const std::string& target_app,
                               const nlohmann::json& params) {
        std::string cmd_id = GenerateCommandId();
        
        // 发送命令
        bool success = command_proxy_->BroadcastCommandReceived(
            cmd_id, cmd_type, target_app, params.dump()
        );
        
        if (success) {
            // 启动超时监控
            timeout_manager_.StartCommandTimeout(cmd_id, 60);
            LOG_I("External command sent: id={} type={} app={}", cmd_id, cmd_type, target_app);
        }
        
        return success;
    }
};
```

这个使用指南涵盖了CommandDBusProxy的完整使用流程，从初始化到具体的命令分发和状态处理，提供了实用的代码示例和最佳实践。