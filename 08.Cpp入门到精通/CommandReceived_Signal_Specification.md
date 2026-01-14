# CommandReceived 信号详细规范

## 概述

`CommandReceived`信号是IoT服务向外部应用广播命令的核心机制，当IoT服务需要外部应用执行特定任务时，会通过此信号进行通知。

## 信号定义

**接口**: `com.tencent.iotservice.linux.Command`  
**信号名**: `CommandReceived`  
**总线类型**: System Bus

## 参数详细说明

### 1. id (string) - 指令ID

**作用**: 唯一标识一个命令实例，用于状态跟踪和结果上报

**格式**: 通常为UUID或时间戳+随机数组合

**示例**:
```
"cmd_20260114_001234567"
"550e8400-e29b-41d4-a716-446655440000"
"iot_1705201234_abc123"
```

**用途**:
- 外部应用执行完成后，通过此ID调用`ReportCommandStatus`上报状态
- IoT服务用于跟踪命令执行进度
- 日志记录和问题排查

### 2. name (string) - 指令名称

**作用**: 定义具体要执行的操作类型

**常用指令名称**:

| 指令名称 | 功能描述 | 目标应用 |
|----------|----------|----------|
| `clear_voice_cache` | 清除语音缓存 | 刷掌App |
| `reboot_device` | 设备重启 | 系统服务 |
| `update_config` | 更新配置 | 各类App |
| `sync_data` | 数据同步 | 数据服务 |
| `capture_screenshot` | 截屏 | UI服务 |
| `export_logs` | 导出日志 | 日志服务 |

**命名规范**:
- 使用小写字母和下划线
- 动词+名词的格式
- 具有明确的语义

### 3. app_id (string) - 目标应用

**作用**: 指定哪个应用应该处理此命令

**常用应用ID**:

| App ID | 应用名称 | 描述 |
|--------|----------|------|
| `palmapp` | 刷掌应用 | 处理刷掌相关功能 |
| `netserver` | 网络服务 | 处理网络配置和状态 |
| `audioservice` | 音频服务 | 处理音频播放和录制 |
| `systemservice` | 系统服务 | 处理系统级操作 |
| `thirdparty_xxx` | 第三方应用 | 各种第三方集成应用 |

**匹配规则**:
- 外部应用监听信号时，应检查`app_id`是否匹配自己
- 支持通配符匹配（如`*`表示所有应用）
- 大小写敏感

### 4. biz_param (string) - 指令业务数据

**作用**: 包含执行命令所需的具体参数和配置信息

**格式**: JSON字符串

**通用字段**:
```json
{
  "app_id": "palmapp",           // 目标应用ID（冗余字段，与参数3相同）
  "timeout": 60,                 // 超时时间（秒）
  "priority": "high",            // 优先级: high/medium/low
  "retry_count": 3,              // 重试次数
  "callback_url": "...",         // 回调URL（可选）
  "trace_id": "trace_123456"     // 链路追踪ID
}
```

## 具体业务场景示例

### 1. 语音缓存清除命令

```json
{
  "name": "clearAudioCache",
  "app_id": "wx8e6e7b9699424a0d",
  "biz_param": "{
    \"language\": \"zh-CN\"
  }"
}
```

**biz_param详细字段**:
- `language`: 要清除的语言代码（zh-CN, en-US等）
- `cache_type`: 缓存类型（all/tts/asr/temp）
- `force_clear`: 是否强制清除（忽略文件锁定）
- `backup_before_clear`: 清除前是否备份
- `timeout`: 操作超时时间
- `priority`: 操作优先级

### 2. 设备重启命令

```json
{
  "id": "cmd_20260114_reboot_001",
  "name": "reboot_device",
  "app_id": "systemservice",
  "biz_param": "{
    \"delay_seconds\": 30,
    \"reason\": \"system_update\",
    \"force_reboot\": false,
    \"save_state\": true,
    \"notify_users\": true
  }"
}
```

**biz_param详细字段**:
- `delay_seconds`: 延迟重启时间
- `reason`: 重启原因
- `force_reboot`: 是否强制重启
- `save_state`: 是否保存当前状态
- `notify_users`: 是否通知用户

### 3. 配置更新命令

```json
{
  "id": "cmd_20260114_config_001",
  "name": "update_config",
  "app_id": "palmapp",
  "biz_param": "{
    \"config_section\": \"audio\",
    \"config_data\": {
      \"volume\": 80,
      \"sample_rate\": 16000,
      \"channels\": 1
    },
    \"restart_required\": true,
    \"backup_old_config\": true
  }"
}
```

## 信号监听示例

### 1. 命令行监听

```bash
# 监听所有CommandReceived信号
dbus-monitor --system \
  "type='signal',interface='com.tencent.iotservice.linux.Command',member='CommandReceived'"

# 监听特定应用的命令
dbus-monitor --system \
  "type='signal',interface='com.tencent.iotservice.linux.Command',member='CommandReceived'" | \
  grep -A 10 "palmapp"
```

### 2. C++代码监听

```cpp
#include "dbus/dbus_client.h"

class CommandListener {
private:
    std::unique_ptr<DBusClient> dbus_client_;
    boost::signals2::signal<void(std::string, std::string, std::string, std::string)>* command_signal_;

public:
    bool Initialize() {
        dbus_client_ = std::make_unique<DBusClient>();
        
        // 注册信号监听
        command_signal_ = dbus_client_->RegisterSignal<void(std::string, std::string, std::string, std::string)>(
            executor_,
            {"com.tencent.iotservice.linux", "/", "com.tencent.iotservice.linux.Command", "CommandReceived"}
        );
        
        if (!command_signal_) {
            LOG_E("Failed to register CommandReceived signal");
            return false;
        }
        
        // 连接信号处理函数
        command_signal_->connect([this](const std::string& id, const std::string& name, 
                                       const std::string& app_id, const std::string& biz_param) {
            HandleCommandReceived(id, name, app_id, biz_param);
        });
        
        return true;
    }
    
    void HandleCommandReceived(const std::string& id, const std::string& name, 
                              const std::string& app_id, const std::string& biz_param) {
        LOG_I("Received command: id={} name={} app_id={}", id, name, app_id);
        
        // 检查是否是目标应用
        if (app_id != "palmapp" && app_id != "*") {
            LOG_I("Command not for this app, ignoring");
            return;
        }
        
        // 解析业务参数
        try {
            auto json_param = nlohmann::json::parse(biz_param);
            ProcessCommand(id, name, json_param);
        } catch (const std::exception& e) {
            LOG_E("Failed to parse biz_param: {}", e.what());
            ReportCommandStatus(id, 60, "", 1002, "Invalid parameters");
        }
    }
    
    void ProcessCommand(const std::string& id, const std::string& name, 
                       const nlohmann::json& params) {
        if (name == "clear_voice_cache") {
            ProcessVoiceCacheClear(id, params);
        } else if (name == "update_config") {
            ProcessConfigUpdate(id, params);
        } else {
            LOG_E("Unknown command: {}", name);
            ReportCommandStatus(id, 60, "", 1001, "Unknown command");
        }
    }
};
```

### 3. Python代码监听

```python
import dbus
import json
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

class CommandListener:
    def __init__(self):
        DBusGMainLoop(set_as_default=True)
        self.bus = dbus.SystemBus()
        
    def start_listening(self):
        # 监听CommandReceived信号
        self.bus.add_signal_receiver(
            self.handle_command_received,
            signal_name="CommandReceived",
            dbus_interface="com.tencent.iotservice.linux.Command",
            path="/"
        )
        
        print("Listening for CommandReceived signals...")
        
        # 启动主循环
        loop = GLib.MainLoop()
        loop.run()
    
    def handle_command_received(self, cmd_id, name, app_id, biz_param):
        print(f"Received command: id={cmd_id}, name={name}, app_id={app_id}")
        
        # 检查是否是目标应用
        if app_id not in ["palmapp", "*"]:
            print("Command not for this app, ignoring")
            return
        
        try:
            # 解析业务参数
            params = json.loads(biz_param)
            self.process_command(cmd_id, name, params)
        except json.JSONDecodeError as e:
            print(f"Failed to parse biz_param: {e}")
            self.report_command_status(cmd_id, 60, "", 1002, "Invalid parameters")
    
    def process_command(self, cmd_id, name, params):
        if name == "clear_voice_cache":
            self.process_voice_cache_clear(cmd_id, params)
        elif name == "update_config":
            self.process_config_update(cmd_id, params)
        else:
            print(f"Unknown command: {name}")
            self.report_command_status(cmd_id, 60, "", 1001, "Unknown command")
    
    def report_command_status(self, cmd_id, state, biz_result, code, message):
        try:
            iot_service = self.bus.get_object(
                "com.tencent.iotservice.linux", "/"
            )
            
            result = iot_service.ReportCommandStatus(
                cmd_id, state, biz_result, code, message,
                dbus_interface="com.tencent.iotservice.linux.Command"
            )
            
            print(f"Status reported: {result}")
        except Exception as e:
            print(f"Failed to report status: {e}")

# 使用示例
if __name__ == "__main__":
    listener = CommandListener()
    listener.start_listening()
```

## ReportCommandStatus 状态上报

外部应用处理完命令后，必须通过`ReportCommandStatus`方法上报执行状态：

### 状态码定义

| 状态码 | 含义 | 使用场景 |
|--------|------|----------|
| 40 | 执行中 | 长时间任务的进度通知 |
| 50 | 执行成功 | 命令完成且成功 |
| 60 | 执行失败 | 命令执行出错 |

### 扩展状态码（外部App专用）

| 状态码范围 | 应用类型 | 具体状态 |
|------------|----------|----------|
| 1000-1999 | 刷掌App | 1000:处理中, 1001:成功, 1002:失败 |
| 1100-1199 | 语音缓存 | 1100:清除中, 1101:清除完成, 1103:清除失败 |
| 2000-8999 | 第三方App | 自定义状态码 |

### 上报示例

```bash
# 语音缓存清除成功
dbus-send --system \
  --dest=com.tencent.iotservice.linux \
  --type=method_call \
  --print-reply \
  / \
  com.tencent.iotservice.linux.Command.ReportCommandStatus \
  string:"cmd_20260114_voice_001" \
  int32:1101 \
  string:'{"language":"zh-CN","files_deleted":156,"bytes_freed":12345678}' \
  int32:0 \
  string:""

# 命令执行失败
dbus-send --system \
  --dest=com.tencent.iotservice.linux \
  --type=method_call \
  --print-reply \
  / \
  com.tencent.iotservice.linux.Command.ReportCommandStatus \
  string:"cmd_20260114_voice_001" \
  int32:1103 \
  string:"" \
  int32:2001 \
  string:"Permission denied: cannot access cache directory"
```

## 完整交互流程

```
1. IoT服务 → 广播CommandReceived信号
   ├─ id: "cmd_001"
   ├─ name: "clear_voice_cache"  
   ├─ app_id: "palmapp"
   └─ biz_param: '{"language":"zh-CN"}'

2. 外部应用 → 接收信号并处理
   ├─ 解析参数
   ├─ 执行业务逻辑
   └─ 准备结果数据

3. 外部应用 → 上报状态 (ReportCommandStatus)
   ├─ id: "cmd_001"
   ├─ state: 1101 (清除完成)
   ├─ biz_result: '{"files_deleted":156}'
   ├─ code: 0
   └─ message: ""

4. IoT服务 → 接收状态并处理
   ├─ 更新命令状态
   ├─ 记录执行结果
   └─ 可能触发后续操作
```

这个规范为外部应用提供了完整的CommandReceived信号处理指南，确保命令分发和状态上报的标准化和可靠性。