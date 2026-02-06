# 网络连接性检测 - 简单命令行操作

## 概述

本文档提供简单直接的命令行操作来检测网络连接性，对应ConnectivityStrategy的功能。无需复杂脚本，直接在终端中输入命令即可完成检测。

## 1. 网关连通性测试

### 目的
测试设备是否能够与默认网关通信。

### 操作步骤

#### 步骤1：查看默认网关
```bash
# 查看默认网关地址
ip route | grep default

# 或者使用route命令
route -n | grep "^0.0.0.0"
```

#### 步骤2：测试网关连通性
```bash
# 假设网关是192.168.1.1，替换为实际网关地址
ping -c 3 192.168.1.1

# 快速测试（1次ping，3秒超时）
ping -c 1 -W 3 192.168.1.1
```

#### 步骤3：检查结果
- ✅ **成功**: 显示ping统计信息，0% packet loss
- ❌ **失败**: 显示"Destination Host Unreachable"或100% packet loss

### 一键检测命令
```bash
# 自动获取网关并测试（一行命令）
ping -c 1 -W 3 $(ip route | grep default | awk '{print $3}' | head -1)
```

## 2. 外网连通性测试

### 目的
测试设备是否能够访问互联网。

### 操作步骤

#### 方法1：ping公共DNS服务器
```bash
# 测试Google DNS
ping -c 3 8.8.8.8

# 测试114 DNS
ping -c 3 114.114.114.114

# 测试Cloudflare DNS
ping -c 3 1.1.1.1
```

#### 方法2：测试多个主机（逐个尝试）
```bash
# 如果第一个失败，尝试第二个
ping -c 1 -W 3 8.8.8.8 || ping -c 1 -W 3 114.114.114.114 || ping -c 1 -W 3 1.1.1.1
```

#### 方法3：HTTP连接测试
```bash
# 测试HTTP连接
curl -s --connect-timeout 5 http://www.baidu.com > /dev/null && echo "HTTP连接成功" || echo "HTTP连接失败"

# 测试HTTPS连接
curl -s --connect-timeout 5 https://www.baidu.com > /dev/null && echo "HTTPS连接成功" || echo "HTTPS连接失败"
```

### 快速检测命令
```bash
# 快速外网连通性测试
ping -c 1 -W 3 8.8.8.8 && echo "✅ 外网连通" || echo "❌ 外网不通"
```

## 3. DNS解析功能测试

### 目的
测试DNS解析功能是否正常工作。

### 操作步骤

#### 步骤1：检查DNS配置
```bash
# 查看DNS服务器配置
cat /etc/resolv.conf | grep nameserver
```

#### 步骤2：测试域名解析

##### 使用nslookup
```bash
# 测试百度域名解析
nslookup www.baidu.com

# 测试QQ域名解析
nslookup www.qq.com

# 指定DNS服务器测试
nslookup www.baidu.com 8.8.8.8
```

##### 使用dig（如果可用）
```bash
# 测试域名解析
dig www.baidu.com

# 简短输出
dig +short www.baidu.com

# 指定DNS服务器
dig @8.8.8.8 www.baidu.com
```

##### 使用host（如果可用）
```bash
# 测试域名解析
host www.baidu.com

# 指定DNS服务器
host www.baidu.com 8.8.8.8
```

### 快速检测命令
```bash
# 快速DNS解析测试
nslookup www.baidu.com > /dev/null 2>&1 && echo "✅ DNS解析正常" || echo "❌ DNS解析失败"
```

## 4. 综合连接性检测

### 完整检测流程（逐步执行）

#### 步骤1：检测网关
```bash
echo "=== 网关连通性测试 ==="
gateway=$(ip route | grep default | awk '{print $3}' | head -1)
echo "默认网关: $gateway"
if ping -c 1 -W 3 $gateway > /dev/null 2>&1; then
    echo "✅ 网关连通性: 正常"
else
    echo "❌ 网关连通性: 失败"
fi
```

#### 步骤2：检测外网
```bash
echo "=== 外网连通性测试 ==="
if ping -c 1 -W 3 8.8.8.8 > /dev/null 2>&1; then
    echo "✅ 外网连通性: 正常 (8.8.8.8)"
elif ping -c 1 -W 3 114.114.114.114 > /dev/null 2>&1; then
    echo "✅ 外网连通性: 正常 (114.114.114.114)"
elif ping -c 1 -W 3 1.1.1.1 > /dev/null 2>&1; then
    echo "✅ 外网连通性: 正常 (1.1.1.1)"
else
    echo "❌ 外网连通性: 失败"
fi
```

#### 步骤3：检测DNS
```bash
echo "=== DNS解析测试 ==="
if nslookup www.baidu.com > /dev/null 2>&1; then
    echo "✅ DNS解析: 正常 (www.baidu.com)"
elif nslookup www.qq.com > /dev/null 2>&1; then
    echo "✅ DNS解析: 正常 (www.qq.com)"
else
    echo "❌ DNS解析: 失败"
fi
```

## 5. 常用检测命令速查

### 基础网络信息
```bash
# 查看网络接口
ip addr show

# 查看路由表
ip route show

# 查看DNS配置
cat /etc/resolv.conf
```

### 连通性测试
```bash
# 测试网关（替换为实际网关IP）
ping -c 3 192.168.1.1

# 测试外网
ping -c 3 8.8.8.8

# 测试域名解析
nslookup www.baidu.com

# 测试HTTP连接
curl -I www.baidu.com
```

### 网络诊断
```bash
# 路由跟踪
traceroute 8.8.8.8

# 网络统计
netstat -i

# 查看网络连接
ss -tuln
```

## 6. 故障排查命令

### 网关问题
```bash
# 检查路由表
ip route show

# 检查ARP表
arp -a

# 手动添加默认路由（临时）
sudo ip route add default via 192.168.1.1
```

### 外网连接问题
```bash
# 检查防火墙
sudo iptables -L

# 检查网络服务状态
systemctl status networking

# 重启网络服务
sudo systemctl restart networking
```

### DNS问题
```bash
# 检查DNS配置
cat /etc/resolv.conf

# 临时修改DNS
echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf

# 清除DNS缓存
sudo systemctl restart systemd-resolved
```

## 7. 快速诊断命令组合

### 一键网络状态检查
```bash
echo "网关: $(ip route | grep default | awk '{print $3}')"; \
ping -c 1 -W 3 $(ip route | grep default | awk '{print $3}') > /dev/null && echo "网关: ✅" || echo "网关: ❌"; \
ping -c 1 -W 3 8.8.8.8 > /dev/null && echo "外网: ✅" || echo "外网: ❌"; \
nslookup www.baidu.com > /dev/null 2>&1 && echo "DNS: ✅" || echo "DNS: ❌"
```

### 网络配置概览
```bash
echo "=== 网络配置概览 ==="; \
echo "接口信息:"; ip addr show | grep -E "inet |UP"; \
echo "默认网关: $(ip route | grep default | awk '{print $3}')"; \
echo "DNS服务器: $(grep nameserver /etc/resolv.conf | awk '{print $2}' | tr '\n' ' ')"
```

### 连通性快速测试
```bash
echo "=== 连通性测试 ==="; \
echo -n "网关连通性: "; ping -c 1 -W 3 $(ip route | grep default | awk '{print $3}') > /dev/null && echo "✅" || echo "❌"; \
echo -n "外网连通性: "; ping -c 1 -W 3 8.8.8.8 > /dev/null && echo "✅" || echo "❌"; \
echo -n "DNS解析: "; nslookup www.baidu.com > /dev/null 2>&1 && echo "✅" || echo "❌"
```

## 8. 模拟ServiceStrategy输出

### 手动生成JSON格式结果
```bash
# 检测并生成类似ServiceStrategy的输出
gateway=$(ip route | grep default | awk '{print $3}' | head -1)
gateway_ok=$(ping -c 1 -W 3 $gateway > /dev/null 2>&1 && echo "true" || echo "false")
internet_ok=$(ping -c 1 -W 3 8.8.8.8 > /dev/null 2>&1 && echo "success" || echo "failed")
dns_ok=$(nslookup www.baidu.com > /dev/null 2>&1 && echo "success" || echo "failed")

echo "{"
echo "  \"strategy_name\": \"Connectivity\","
echo "  \"success\": $([ "$gateway_ok" = "true" ] && [ "$internet_ok" = "success" ] && [ "$dns_ok" = "success" ] && echo "true" || echo "false"),"
echo "  \"details\": {"
echo "    \"gateway_reachable\": \"$gateway_ok\","
echo "    \"internet_connectivity\": \"$internet_ok\","
echo "    \"dns_resolution_www.baidu.com\": \"$dns_ok\""
echo "  },"
echo "  \"error_message\": \"$([ "$gateway_ok" = "true" ] && [ "$internet_ok" = "success" ] && [ "$dns_ok" = "success" ] && echo "Connectivity OK" || echo "Connectivity issues detected")\""
echo "}"
```

## 9. 使用示例

### 场景1：新设备网络检测
```bash
# 1. 检查网络配置
ip addr show
ip route show
cat /etc/resolv.conf

# 2. 测试连通性
ping -c 3 $(ip route | grep default | awk '{print $3}')
ping -c 3 8.8.8.8
nslookup www.baidu.com
```

### 场景2：网络故障排查
```bash
# 1. 快速诊断
ping -c 1 -W 3 $(ip route | grep default | awk '{print $3}') && echo "网关OK" || echo "网关异常"
ping -c 1 -W 3 8.8.8.8 && echo "外网OK" || echo "外网异常"
nslookup www.baidu.com > /dev/null 2>&1 && echo "DNS OK" || echo "DNS异常"

# 2. 详细检查（如果有问题）
traceroute 8.8.8.8
dig www.baidu.com
```

### 场景3：定期监控
```bash
# 每隔30秒检查一次网络状态
while true; do
    echo "$(date): $(ping -c 1 -W 3 8.8.8.8 > /dev/null && echo "网络正常" || echo "网络异常")"
    sleep 30
done
```

## 10. 总结

### 核心检测命令
1. **网关测试**: `ping -c 1 -W 3 $(ip route | grep default | awk '{print $3}')`
2. **外网测试**: `ping -c 1 -W 3 8.8.8.8`
3. **DNS测试**: `nslookup www.baidu.com`

### 快速诊断
- 使用组合命令一次性检查所有连接性
- 根据输出结果判断网络状态
- 针对性地进行故障排查

### 优势
- **简单直接**: 无需复杂脚本，直接命令行操作
- **快速有效**: 几秒钟内完成全面检测
- **易于理解**: 每个命令都有明确的目的和输出
- **灵活使用**: 可以单独使用或组合使用