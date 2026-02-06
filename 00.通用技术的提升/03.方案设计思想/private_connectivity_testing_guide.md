# 私有化网络连接性检测命令测试指南

本文档基于 `src/qrcode/strategy/private_connectivity_strategy.cpp` 中的 `Execute()` 方法，提供每个检测功能对应的命令行测试方法。

## 概述

私有化网络连接性检测主要包含以下几个核心功能：
1. 内网网关连通性检测
2. 内网DNS服务器连通性检测
3. 内网关键服务连通性检测
4. 代理服务器连通性检测
5. 通过代理的外网连通性检测
6. 内网域名解析检测

## 1. 内网网关连通性检测

### 功能说明
检测设备是否能够连接到内网默认网关。

### 对应代码
```cpp
bool PrivateConnectivityStrategy::TestInternalGatewayConnectivity()
```

### 命令测试方法

#### 1.1 获取默认网关
```bash
# 方法1：使用route命令
route -n | grep '^0.0.0.0' | awk '{print $2}'
# 方法2：使用ip命令
ip route | grep default | awk '{print $3}'
# 方法3：使用netstat命令
netstat -rn | grep '^0.0.0.0' | awk '{print $2}'
```

#### 1.2 测试网关连通性

```bash
# 假设网关IP为192.168.1.1
ping -c 3 -W 3 192.168.1.1
# 或者使用更详细的ping测试
ping -c 5 -i 1 -W 3 192.168.1.1
```

#### 1.3 完整测试脚本
```bash
#!/bin/bash
GATEWAY=$(ip route | grep default | awk '{print $3}' | head -1)
if [ -n "$GATEWAY" ]; then
    echo "Testing gateway: $GATEWAY"
    if ping -c 3 -W 3 $GATEWAY > /dev/null 2>&1; then
        echo "Gateway connectivity: OK"
    else
        echo "Gateway connectivity: FAILED"
    fi
else
    echo "No gateway found"
fi
```

## 2. 内网DNS服务器连通性检测

### 功能说明
检测内网DNS服务器是否可达，测试DNS服务器的53端口连通性。

### 对应代码
```cpp
bool PrivateConnectivityStrategy::TestInternalDNSConnectivity(std::string& dns_servers)
```

### 命令测试方法

#### 2.1 获取DNS服务器列表
```bash
# 从/etc/resolv.conf获取DNS服务器
grep "^nameserver" /etc/resolv.conf | awk '{print $2}' | grep -v "127.0.0.1"
```

测试DNS服务器连通性

```bash
# 方法1：使用ping测试
ping -c 3 -W 3 8.8.8.8

# 方法2：使用nc测试DNS端口53
nc -z -v -w3 8.8.8.8 53

# 方法3：使用telnet测试DNS端口
timeout 3 telnet 8.8.8.8 53

# 方法4：使用nmap测试DNS端口
nmap -p 53 8.8.8.8
```

#### 2.3 完整测试脚本
```bash
#!/bin/bash
DNS_SERVERS=$(grep "^nameserver" /etc/resolv.conf | awk '{print $2}' | grep -v "127.0.0.1")
REACHABLE_DNS=""

for dns in $DNS_SERVERS; do
    echo "Testing DNS server: $dns"
    if nc -z -v -w3 $dns 53 2>/dev/null || ping -c 3 -W 3 $dns > /dev/null 2>&1; then
        echo "DNS server $dns: REACHABLE"
        REACHABLE_DNS="$REACHABLE_DNS,$dns"
    else
        echo "DNS server $dns: UNREACHABLE"
    fi
done

echo "Reachable DNS servers: ${REACHABLE_DNS#,}"
```

## 3. 内网关键服务连通性检测

### 功能说明
检测内网关键服务（如Web服务器、API服务器等）的连通性。

### 对应代码
```cpp
bool PrivateConnectivityStrategy::TestInternalServicesConnectivity(std::string& reachable_services)
```

### 命令测试方法

测试特定服务连通性

```bash
# 使用curl测试HTTP服务
curl -I --connect-timeout 3 --max-time 5 http://192.168.1.100

# 使用wget测试HTTP服务
wget --spider --timeout=3 --tries=1 http://192.168.1.100
```

#### 3.2 批量测试内网服务
```bash
#!/bin/bash
# 定义内网服务列表
SERVICES=(
    "10.0.0.100:80"
    "172.16.1.10:8080"
    "192.168.1.100:80"
    "192.168.1.200:443"
)

REACHABLE_SERVICES=""

for service in "${SERVICES[@]}"; do
    host=$(echo $service | cut -d':' -f1)
    port=$(echo $service | cut -d':' -f2)
    
    echo "Testing service: $service"
    if nc -z -v -w3 $host $port 2>/dev/null || ping -c 3 -W 3 $host > /dev/null 2>&1; then
        echo "Service $service: REACHABLE"
        REACHABLE_SERVICES="$REACHABLE_SERVICES,$service"
    else
        echo "Service $service: UNREACHABLE"
    fi
done

echo "Reachable services: ${REACHABLE_SERVICES#,}"
```

## 4. 代理服务器连通性检测

### 功能说明
检测系统配置的代理服务器是否可达。

### 对应代码
```cpp
bool PrivateConnectivityStrategy::TestProxyConnectivity(std::string& proxy_info)
```

### 命令测试方法

#### 4.1 获取代理配置
```bash
# 检查环境变量中的代理配置
echo "HTTP_PROXY: $HTTP_PROXY"
echo "http_proxy: $http_proxy"
echo "HTTPS_PROXY: $HTTPS_PROXY"
echo "https_proxy: $https_proxy"

# 检查配置文件中的代理设置
if [ -f "/etc/iot/proxy.conf" ]; then
    grep "proxy=" /etc/iot/proxy.conf
fi
```

#### 4.2 测试代理服务器连通性
```bash
# 假设代理服务器为proxy.company.com:8080
PROXY_HOST="proxy.company.com"
PROXY_PORT="8080"
# 方法1：使用telnet测试代理端口
timeout 5 telnet $PROXY_HOST $PROXY_PORT
# 方法2：使用curl测试代理连通性
curl --proxy $PROXY_HOST:$PROXY_PORT --connect-timeout 5 --max-time 10 -I http://www.baidu.com
```

#### 4.3 完整代理测试脚本
```bash
#!/bin/bash
# 获取代理配置
PROXY_CONFIG=""
if [ -n "$http_proxy" ]; then
    PROXY_CONFIG="$http_proxy"
elif [ -n "$HTTP_PROXY" ]; then
    PROXY_CONFIG="$HTTP_PROXY"
elif [ -f "/etc/iot/proxy.conf" ]; then
    PROXY_CONFIG=$(grep "proxy=" /etc/iot/proxy.conf | cut -d'=' -f2)
fi

if [ -z "$PROXY_CONFIG" ]; then
    echo "No proxy configured"
    exit 0
fi

echo "Testing proxy: $PROXY_CONFIG"

# 解析代理配置
CLEAN_PROXY=$(echo $PROXY_CONFIG | sed 's|^https\?://||')
PROXY_HOST=$(echo $CLEAN_PROXY | cut -d':' -f1)
PROXY_PORT=$(echo $CLEAN_PROXY | cut -d':' -f2)

if [ -z "$PROXY_PORT" ]; then
    PROXY_PORT="8080"
fi

echo "Proxy host: $PROXY_HOST, port: $PROXY_PORT"

if nc -z -v -w5 $PROXY_HOST $PROXY_PORT 2>/dev/null; then
    echo "Proxy connectivity: OK"
else
    echo "Proxy connectivity: FAILED"
fi
```

## 5. 通过代理的外网连通性检测

### 功能说明
测试通过代理服务器访问外网的连通性。

### 对应代码
```cpp
bool PrivateConnectivityStrategy::TestExternalConnectivityViaProxy(const std::string& proxy_url)
```

### 命令测试方法

#### 5.1 使用curl通过代理测试外网
```bash
# 测试通过代理访问百度
curl --proxy proxy.company.com:8080 --connect-timeout 10 --max-time 15 -s http://www.baidu.com

# 测试通过代理访问Google
curl --proxy proxy.company.com:8080 --connect-timeout 10 --max-time 15 -s http://www.google.com

# 获取HTTP状态码
curl --proxy proxy.company.com:8080 --connect-timeout 10 --max-time 15 -s -o /dev/null -w "%{http_code}" http://www.baidu.com
```

#### 5.2 使用wget通过代理测试外网
```bash
# 设置代理环境变量
export http_proxy=http://proxy.company.com:8080
export https_proxy=http://proxy.company.com:8080

# 使用wget测试
wget --timeout=15 --tries=1 --spider http://www.baidu.com
```

#### 5.3 完整外网连通性测试脚本
```bash
#!/bin/bash
PROXY_URL="$1"
if [ -z "$PROXY_URL" ]; then
    echo "Usage: $0 <proxy_url>"
    exit 1
fi

echo "Testing external connectivity via proxy: $PROXY_URL"

# 测试多个外网站点
SITES=("http://www.baidu.com" "http://www.google.com" "http://httpbin.org/ip")

for site in "${SITES[@]}"; do
    echo "Testing site: $site"
    if curl --proxy $PROXY_URL --connect-timeout 10 --max-time 15 -s $site > /dev/null 2>&1; then
        echo "Site $site: ACCESSIBLE"
    else
        echo "Site $site: INACCESSIBLE"
    fi
done
```

## 6. 内网域名解析检测

### 功能说明
测试内网域名的DNS解析是否正常。

### 对应代码
```cpp
bool PrivateConnectivityStrategy::TestInternalDNSResolution(std::string& resolved_domains)
```

### 命令测试方法

#### 6.1 测试域名解析
```bash
# 使用nslookup测试域名解析
nslookup company.com

# 使用dig测试域名解析
dig company.com

# 使用host测试域名解析
host company.com

# 测试特定DNS服务器的解析
nslookup company.com 192.168.1.1
dig @192.168.1.1 company.com
```

#### 6.2 获取内网域名列表
```bash
# 从/etc/hosts获取内网域名
grep -E "(company\.com|internal\.local|\.local)" /etc/hosts | awk '{print $2}'

# 从配置文件获取内网域名
if [ -f "/etc/iot/internal_domains.conf" ]; then
    grep -v "^#" /etc/iot/internal_domains.conf | grep -v "^$"
fi
```

#### 6.3 完整域名解析测试脚本
```bash
#!/bin/bash
# 定义内网域名列表
DOMAINS=(
    "company.com"
    "internal.local"
    "api.company.com"
    "service.internal.local"
)

# 从/etc/hosts获取额外的内网域名
HOSTS_DOMAINS=$(grep -E "(company\.com|internal\.local|\.local)" /etc/hosts 2>/dev/null | awk '{print $2}' | grep -v "localhost")
if [ -n "$HOSTS_DOMAINS" ]; then
    DOMAINS+=($HOSTS_DOMAINS)
fi

RESOLVED_DOMAINS=""

for domain in "${DOMAINS[@]}"; do
    echo "Testing DNS resolution for: $domain"
    if nslookup $domain > /dev/null 2>&1; then
        echo "Domain $domain: RESOLVED"
        RESOLVED_DOMAINS="$RESOLVED_DOMAINS,$domain"
    else
        echo "Domain $domain: RESOLUTION FAILED"
    fi
done

echo "Resolved domains: ${RESOLVED_DOMAINS#,}"
```

## 7. 综合测试脚本

### 完整的私有化网络连接性检测脚本
```bash
#!/bin/bash

echo "=== 私有化网络连接性检测 ==="
echo "开始时间: $(date)"
echo

# 1. 内网网关连通性检测
echo "1. 内网网关连通性检测"
GATEWAY=$(ip route | grep default | awk '{print $3}' | head -1)
if [ -n "$GATEWAY" ]; then
    echo "   网关地址: $GATEWAY"
    if ping -c 3 -W 3 $GATEWAY > /dev/null 2>&1; then
        echo "   结果: ✓ 网关可达"
    else
        echo "   结果: ✗ 网关不可达"
    fi
else
    echo "   结果: ✗ 未找到网关"
fi
echo

# 2. 内网DNS服务器连通性检测
echo "2. 内网DNS服务器连通性检测"
DNS_SERVERS=$(grep "^nameserver" /etc/resolv.conf | awk '{print $2}' | grep -v "127.0.0.1")
REACHABLE_DNS=""
for dns in $DNS_SERVERS; do
    echo "   测试DNS: $dns"
    if nc -z -v -w3 $dns 53 2>/dev/null; then
        echo "   结果: ✓ DNS服务器可达"
        REACHABLE_DNS="$REACHABLE_DNS,$dns"
    else
        echo "   结果: ✗ DNS服务器不可达"
    fi
done
echo "   可达的DNS服务器: ${REACHABLE_DNS#,}"
echo

# 3. 代理服务器连通性检测
echo "3. 代理服务器连通性检测"
PROXY_CONFIG=""
if [ -n "$http_proxy" ]; then
    PROXY_CONFIG="$http_proxy"
elif [ -n "$HTTP_PROXY" ]; then
    PROXY_CONFIG="$HTTP_PROXY"
fi

if [ -n "$PROXY_CONFIG" ]; then
    echo "   代理配置: $PROXY_CONFIG"
    CLEAN_PROXY=$(echo $PROXY_CONFIG | sed 's|^https\?://||')
    PROXY_HOST=$(echo $CLEAN_PROXY | cut -d':' -f1)
    PROXY_PORT=$(echo $CLEAN_PROXY | cut -d':' -f2)
    
    if nc -z -v -w5 $PROXY_HOST $PROXY_PORT 2>/dev/null; then
        echo "   结果: ✓ 代理服务器可达"
        
        # 测试通过代理的外网连通性
        echo "   测试外网连通性..."
        if curl --proxy $PROXY_CONFIG --connect-timeout 10 --max-time 15 -s http://www.baidu.com > /dev/null 2>&1; then
            echo "   结果: ✓ 通过代理可访问外网"
        else
            echo "   结果: ✗ 通过代理无法访问外网"
        fi
    else
        echo "   结果: ✗ 代理服务器不可达"
    fi
else
    echo "   结果: 未配置代理服务器"
fi
echo

# 4. 内网域名解析检测
echo "4. 内网域名解析检测"
DOMAINS=("company.com" "internal.local" "api.company.com")
RESOLVED_DOMAINS=""
for domain in "${DOMAINS[@]}"; do
    echo "   测试域名: $domain"
    if nslookup $domain > /dev/null 2>&1; then
        echo "   结果: ✓ 域名解析成功"
        RESOLVED_DOMAINS="$RESOLVED_DOMAINS,$domain"
    else
        echo "   结果: ✗ 域名解析失败"
    fi
done
echo "   可解析的域名: ${RESOLVED_DOMAINS#,}"
echo

echo "=== 检测完成 ==="
echo "结束时间: $(date)"
```

## 8. 使用说明

### 8.1 单独测试某个功能
```bash
# 测试网关连通性
./test_gateway.sh

# 测试DNS连通性
./test_dns.sh

# 测试代理连通性
./test_proxy.sh

# 测试域名解析
./test_dns_resolution.sh
```

### 8.2 运行完整检测
```bash
# 运行完整的私有化网络连接性检测
./private_connectivity_test.sh

# 将结果保存到文件
./private_connectivity_test.sh > connectivity_test_result.log 2>&1
```

### 8.3 定时检测
```bash
# 添加到crontab，每小时检测一次
echo "0 * * * * /path/to/private_connectivity_test.sh >> /var/log/connectivity_test.log 2>&1" | crontab -
```

## 9. 故障排查

### 9.1 常见问题及解决方法

#### 网关不可达
```bash
# 检查网络接口状态
ip addr show
ifconfig

# 检查路由表
ip route
route -n

# 重启网络服务
sudo systemctl restart networking
```

#### DNS解析失败
```bash
# 检查DNS配置
cat /etc/resolv.conf

# 手动指定DNS服务器测试
nslookup company.com 8.8.8.8

# 清除DNS缓存
sudo systemctl restart systemd-resolved
```

#### 代理连接失败
```bash
# 检查代理配置
env | grep -i proxy

# 测试代理服务器端口
telnet proxy.company.com 8080

# 检查防火墙规则
sudo iptables -L
```

## 10. 总结

本文档提供了完整的私有化网络连接性检测命令测试方法，涵盖了代码中所有检测功能。通过这些命令和脚本，可以有效地验证私有化网络环境的连通性，快速定位网络问题。

建议在实际使用中根据具体的网络环境调整IP地址、域名和端口配置，以确保测试的准确性和有效性。