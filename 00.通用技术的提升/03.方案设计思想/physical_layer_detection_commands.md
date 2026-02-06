# 物理层网络检测命令实现文档

## 概述

本文档详细说明如何使用Linux命令实现物理层网络检测，对应PhysicalLayerStrategy的功能实现。物理层检测主要验证网络接口的基础状态，包括接口存在性、激活状态、IP地址配置和载波信号等。

## 检测项目

### 1. 网络接口存在性检测

#### 目的

检查系统中是否存在指定的网络接口。

```bash
# 方法1：检查/sys/class/net目录
ls /sys/class/net/
# 方法2：使用ip命令
ip link show
# 方法3：检查特定接口是否存在
test -d /sys/class/net/eth0 && echo "eth0 exists" || echo "eth0 not found"
```

检测结果如下所示：

```

```


### 2. 接口状态检测（UP/DOWN）

#### 目的
检查网络接口是否处于激活状态。UP表示接口已激活，可以收发数据；DOWN表示接口未激活，无法通信。

#### 实现方法

##### 方法1：读取flags文件
```bash
# 读取接口flags
cat /sys/class/net/eth0/flags

# 检查IFF_UP标志（第0位）
flags=$(cat /sys/class/net/eth0/flags)
if [ $((0x$flags & 0x1)) -ne 0 ]; then
    echo "Interface UP"
else
    echo "Interface DOWN"
fi
```

##### 
```bash
# 方法1：读取flags文件。读取接口flags
cat /sys/class/net/eth0/flags
# 方法2：使用ip命令。检查状态（查找"UP"关键字）
ip link show eth0 | grep -q "state UP" && echo "UP" || echo "DOWN"
# 方法3：使用ifconfig命令。检查UP状态
ifconfig eth0 | grep -q "UP" && echo "UP" || echo "DOWN"
```



### 3. IP地址检测

#### 目的
确认接口是否获得了有效的IP地址。

#### 实现方法

##### 方法1：使用ip命令
```bash
# 获取接口IP地址
ip addr show eth0


```

##### 方法2：使用ifconfig命令
```bash
# 方法1：使用ip命令。提取IPv4地址
ip addr show eth0 | grep "inet " | awk '{print $2}' | cut -d'/' -f1
# 方法2：使用ifconfig命令。提取IP地址
ifconfig eth0 | grep "inet " | awk '{print $2}'
```

##### 方法3：使用hostname命令
```bash

```

### 4. 载波信号检测（有线接口）

#### 目的
检查有线接口是否有载波信号。有载波信号表示接口已连接到网络；无载波信号表示接口未连接到网络（如网线未插入）。

#### 实现方法
```bash
# 检查载波状态
cat /sys/class/net/eth0/carrier

# 返回值说明：
# 1 = 有载波信号（网线已连接）
# 0 = 无载波信号（网线未连接）

# 脚本检查
carrier=$(cat /sys/class/net/eth0/carrier 2>/dev/null)
if [ "$carrier" = "1" ]; then
    echo "Carrier detected - Cable connected"
else
    echo "No carrier - Cable disconnected"
fi
```

## 完整检测脚本

### 单接口检测脚本

```bash
#!/bin/bash

check_interface() {
    local interface=$1
    echo "=== Checking interface: $interface ==="
    
    # 1. 检查接口是否存在
    if [ ! -d "/sys/class/net/$interface" ]; then
        echo "Status: NOT_EXIST"
        return 1
    fi
    
    # 2. 检查接口状态
    flags=$(cat /sys/class/net/$interface/flags 2>/dev/null)
    if [ $((0x$flags & 0x1)) -ne 0 ]; then
        echo "Status: UP"
        status="UP"
    else
        echo "Status: DOWN"
        status="DOWN"
        return 1
    fi
    
    # 3. 获取IP地址
    ip_addr=$(ip addr show $interface 2>/dev/null | grep "inet " | awk '{print $2}' | cut -d'/' -f1)
    if [ -n "$ip_addr" ]; then
        echo "IP: $ip_addr"
    else
        echo "IP: UNKNOWN"
        return 1
    fi
    
    # 4. 检查载波状态（仅适用于有线接口）
    if [[ $interface == eth* ]] || [[ $interface == enp* ]]; then
        carrier=$(cat /sys/class/net/$interface/carrier 2>/dev/null)
        if [ "$carrier" = "1" ]; then
            echo "Carrier: YES"
        else
            echo "Carrier: NO"
        fi
    fi
    
    echo "Interface $interface is ACTIVE"
    return 0
}

# 使用示例
check_interface "eth0"
```

### 多接口检测脚本

```bash
#!/bin/bash

# 物理层网络检测脚本
physical_layer_detection() {
    echo "=== Physical Layer Network Detection ==="
    
    # 定义要检测的接口
    interfaces=("eth0" "wlan0" "enp0s3" "wlp2s0")
    found_active=false
    
    for interface in "${interfaces[@]}"; do
        echo
        echo "--- Checking $interface ---"
        
        # 检查接口是否存在
        if [ ! -d "/sys/class/net/$interface" ]; then
            echo "$interface: NOT_EXIST"
            continue
        fi
        
        # 检查接口状态
        flags=$(cat /sys/class/net/$interface/flags 2>/dev/null)
        if [ $((0x$flags & 0x1)) -ne 0 ]; then
            echo "$interface: UP"
            
            # 获取IP地址
            ip_addr=$(ip addr show $interface 2>/dev/null | grep "inet " | awk '{print $2}' | cut -d'/' -f1)
            if [ -n "$ip_addr" ]; then
                echo "$interface IP: $ip_addr"
                found_active=true
                
                # 检查载波状态（有线接口）
                if [[ $interface == eth* ]] || [[ $interface == enp* ]]; then
                    carrier=$(cat /sys/class/net/$interface/carrier 2>/dev/null)
                    echo "$interface Carrier: $carrier"
                fi
            else
                echo "$interface IP: UNKNOWN"
            fi
        else
            echo "$interface: DOWN"
        fi
    done
    
    echo
    if [ "$found_active" = true ]; then
        echo "✅ Physical layer OK - Found active network interfaces"
        return 0
    else
        echo "❌ Physical layer FAILED - No active network interfaces found"
        return 1
    fi
}

# 执行检测
physical_layer_detection
```

## 故障排查命令

### 常见问题诊断

#### 1. 接口不存在
```bash
# 列出所有网络接口
ls /sys/class/net/

# 查看PCI网络设备
lspci | grep -i network

# 查看USB网络设备
lsusb | grep -i network
```

#### 2. 接口DOWN状态
```bash
# 激活接口
sudo ip link set eth0 up

# 或使用ifconfig
sudo ifconfig eth0 up
```

#### 3. 无IP地址
```bash
# 使用DHCP获取IP
sudo dhclient eth0

# 手动配置IP
sudo ip addr add 192.168.1.100/24 dev eth0
```

#### 4. 无载波信号
```bash
# 检查网线连接
cat /sys/class/net/eth0/carrier

# 检查网卡驱动
ethtool eth0

# 查看网卡详细信息
sudo ethtool -i eth0
```

## 高级检测命令

### 网络接口详细信息
```bash
# 查看接口统计信息
cat /proc/net/dev

# 查看接口详细配置
ip addr show

# 查看接口硬件信息
ethtool eth0

# 查看接口驱动信息
ethtool -i eth0
```

### 网络拓扑检测
```bash
# 查看路由表
ip route show

# 查看ARP表
ip neigh show

# 查看网络命名空间
ip netns list
```

## 模拟故障场景

### 1. 模拟接口DOWN
```bash
# 关闭接口
sudo ip link set eth0 down

# 检查状态
ip link show eth0
```

### 2. 模拟无IP地址
```bash
# 删除IP地址
sudo ip addr del 192.168.1.100/24 dev eth0

# 检查IP
ip addr show eth0
```

### 3. 模拟载波丢失
```bash
# 物理方法：拔掉网线
# 软件方法：某些网卡支持软件控制载波
```

## 输出格式示例

### 成功检测输出
```json
{
  "details": {
    "eth0_status": "UP",
    "eth0_ip": "192.168.9.109", 
    "eth0_carrier": "1",
    "wlan0_status": "NOT_EXIST",
    "enp0s3_status": "NOT_EXIST",
    "wlp2s0_status": "NOT_EXIST"
  },
  "error_message": "Physical layer OK - Found active network interfaces",
  "strategy_name": "PhysicalLayer",
  "success": true
}
```

### 失败检测输出
```json
{
  "details": {
    "eth0_status": "DOWN",
    "eth0_ip": "UNKNOWN",
    "eth0_carrier": "0",
    "wlan0_status": "NOT_EXIST",
    "enp0s3_status": "NOT_EXIST", 
    "wlp2s0_status": "NOT_EXIST"
  },
  "error_message": "Network interfaces found but not activated",
  "strategy_name": "PhysicalLayer",
  "success": false
}
```

## 总结

物理层检测是网络诊断的基础，通过检查接口存在性、状态、IP配置和载波信号，可以快速定位网络连接的基础问题。使用本文档提供的命令和脚本，可以有效地进行物理层网络检测和故障排查。