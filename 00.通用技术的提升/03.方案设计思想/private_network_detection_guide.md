# 私有化网络环境检测指南

## 概述

私有化部署环境通常具有严格的网络隔离、受限的外网访问和自定义的服务配置。本文档详细说明在私有化网络环境中如何调整网络分层检测策略，以及各层检测功能的适用性和替代方案。

## 私有化网络环境特点

### 网络架构特征
- **内网隔离**: 严格的内外网隔离，限制外网访问
- **代理访问**: 通过代理服务器访问外部资源
- **自定义DNS**: 使用内部DNS服务器
- **防火墙限制**: 严格的端口和协议限制
- **VPN连接**: 通过VPN建立安全连接

### 服务部署特征
- **内部服务**: 所有业务服务部署在内网
- **自签名证书**: 使用内部CA签发的证书
- **非标准端口**: 服务可能使用非标准端口
- **负载均衡**: 内部负载均衡器

## 分层检测适应性分析

### 1. 物理层检测 (PhysicalLayerStrategy)

#### 适用性: ✅ 完全适用
私有化环境不影响物理层检测，所有功能正常可用。

#### 检测内容
```bash
# 网络接口检测 - 完全适用
ip addr show
cat /sys/class/net/eth0/flags

# 载波信号检测 - 完全适用
cat /sys/class/net/eth0/carrier

# IP地址检测 - 完全适用
ip addr show eth0 | grep "inet "
```

#### 私有化环境特殊考虑
```bash
# 检查是否使用内网IP段
ip addr show | grep -E "inet (10\.|172\.(1[6-9]|2[0-9]|3[01])\.|192\.168\.)"

# 检查多网卡配置（私有化环境常见）
ip addr show | grep -E "^[0-9]+:" | grep -v lo
```

### 2. 本地网络检测 (LocalNetworkStrategy)

#### 适用性: ✅ 基本适用，需要调整
私有化环境的本地网络检测需要适应内网环境。

#### 默认网关检测 - ✅ 完全适用
```bash
# 检测内网网关
ip route | grep default
ping -c 3 $(ip route | grep default | awk '{print $3}')
```

#### DNS服务器检测 - ⚠️ 需要调整
```bash
# 检查内部DNS服务器
cat /etc/resolv.conf | grep nameserver

# 私有化环境常见DNS配置
# 内部DNS: 10.0.0.1, 172.16.0.1, 192.168.1.1
# 可能包含多级DNS转发
```

#### 本地路由检测 - ✅ 完全适用
```bash
# 检测内网路由
ip route show | grep -v default
```

#### 私有化环境调整建议
```bash
# 检测内网网段路由
ip route show | grep -E "(10\.|172\.(1[6-9]|2[0-9]|3[01])\.|192\.168\.)"

# 检查是否有多个内网段
ip route show | grep -E "dev.*scope link" | wc -l
```

### 3. 连通性检测 (ConnectivityStrategy)

#### 适用性: ⚠️ 部分适用，需要重大调整
这是受私有化环境影响最大的检测层。

#### 网关连通性测试 - ✅ 完全适用
```bash
# 内网网关连通性测试
ping -c 3 $(ip route | grep default | awk '{print $3}')
```

#### 外网连通性测试 - ❌ 需要替代方案

**传统方案（不适用）:**
```bash
# 这些在私有化环境中通常失败
ping -c 3 8.8.8.8          # 外网DNS不可达
ping -c 3 114.114.114.114   # 外网DNS不可达
curl http://www.baidu.com   # 外网网站不可达
```

**私有化环境替代方案:**
```bash
# 方案1: 测试代理服务器连通性
PROXY_HOST="proxy.company.com"
PROXY_PORT="8080"
timeout 5 bash -c "echo > /dev/tcp/$PROXY_HOST/$PROXY_PORT" && echo "代理可达" || echo "代理不可达"

# 方案2: 测试内网关键服务
INTERNAL_SERVICES="10.0.0.100 172.16.1.10 192.168.1.50"
for service in $INTERNAL_SERVICES; do
    ping -c 1 -W 3 $service && echo "$service 可达" || echo "$service 不可达"
done

# 方案3: 测试内网DNS解析
nslookup internal.company.com
nslookup service.local

# 方案4: 通过代理测试外网（如果配置了代理）
curl --proxy http://proxy.company.com:8080 -s --connect-timeout 5 http://www.baidu.com > /dev/null && echo "代理外网访问正常" || echo "代理外网访问失败"
```

#### DNS解析测试 - ⚠️ 需要调整

**调整后的DNS测试:**
```bash
# 测试内部域名解析
INTERNAL_DOMAINS="service.company.com api.internal.local db.company.local"
for domain in $INTERNAL_DOMAINS; do
    nslookup $domain > /dev/null 2>&1 && echo "$domain 解析成功" || echo "$domain 解析失败"
done

# 测试外部域名解析（通过内部DNS转发）
EXTERNAL_DOMAINS="www.baidu.com www.qq.com"
for domain in $EXTERNAL_DOMAINS; do
    nslookup $domain > /dev/null 2>&1 && echo "$domain 解析成功（通过内部DNS）" || echo "$domain 解析失败"
done
```

### 4. 服务层检测 (ServiceStrategy)

#### 适用性: ✅ 适用，需要配置调整
私有化环境的服务检测需要适应内部服务地址。

#### 服务地址配置调整
```bash
# 私有化环境服务地址示例
PRIVATE_SERVICE_URLS="
https://api.company.com
http://10.0.0.100:8080
https://service.internal.local:9443
http://192.168.1.100/api
"

# 检查内部服务配置
echo $INTERNAL_SERVICE_HOST
cat /etc/hosts | grep -E "(company|internal|local)"
```

#### HTTP连接测试 - ✅ 适用
```bash
# 测试内部服务HTTP连接
curl -I --connect-timeout 5 https://api.company.com
curl -I --connect-timeout 5 http://10.0.0.100:8080

# 处理自签名证书
curl -k -I --connect-timeout 5 https://service.internal.local:9443
```

#### 健康检查端点测试 - ✅ 适用
```bash
# 测试内部服务健康检查
for endpoint in /health /healthz /ping /status; do
    curl -k -s --connect-timeout 3 https://api.company.com$endpoint > /dev/null 2>&1 && echo "$endpoint 可用" || echo "$endpoint 不可用"
done
```

## 私有化环境检测策略

### 1. 环境识别

#### 自动识别私有化环境
```bash
# 检查是否为私有化环境
check_private_environment() {
    # 检查IP地址段
    private_ip=$(ip addr show | grep -E "inet (10\.|172\.(1[6-9]|2[0-9]|3[01])\.|192\.168\.)" | wc -l)
    
    # 检查外网连通性
    public_access=$(ping -c 1 -W 3 8.8.8.8 > /dev/null 2>&1 && echo "1" || echo "0")
    
    # 检查代理配置
    proxy_config=$(env | grep -i proxy | wc -l)
    
    if [ $private_ip -gt 0 ] && [ $public_access -eq 0 ]; then
        echo "检测到私有化网络环境"
        return 0
    elif [ $proxy_config -gt 0 ]; then
        echo "检测到代理网络环境"
        return 0
    else
        echo "检测到公网环境"
        return 1
    fi
}
```

### 2. 配置文件适配

#### 私有化环境配置示例
```bash
# /etc/iot/private_network.conf
ENVIRONMENT_TYPE="private"
INTERNAL_DNS_SERVERS="10.0.0.1,172.16.0.1"
PROXY_SERVER="proxy.company.com:8080"
INTERNAL_SERVICES="api.company.com,service.internal.local"
HEALTH_CHECK_ENDPOINTS="/health,/ping,/status"
SKIP_EXTERNAL_CONNECTIVITY="true"
```

### 3. 检测流程调整

#### 调整后的检测流程
```bash
# 1. 环境识别
echo "=== 环境识别 ==="
if check_private_environment; then
    PRIVATE_ENV=true
    echo "✅ 私有化环境检测模式"
else
    PRIVATE_ENV=false
    echo "✅ 公网环境检测模式"
fi

# 2. 物理层检测（无变化）
echo "=== 物理层检测 ==="
ip addr show | grep -E "inet.*UP"

# 3. 本地网络检测（轻微调整）
echo "=== 本地网络检测 ==="
gateway=$(ip route | grep default | awk '{print $3}')
echo "内网网关: $gateway"
ping -c 1 -W 3 $gateway && echo "✅ 网关可达" || echo "❌ 网关不可达"

# 4. 连通性检测（重大调整）
echo "=== 连通性检测 ==="
if [ "$PRIVATE_ENV" = true ]; then
    # 私有化环境连通性检测
    echo "私有化环境连通性检测..."
    
    # 测试内网关键服务
    INTERNAL_HOSTS="10.0.0.1 172.16.0.1 192.168.1.1"
    for host in $INTERNAL_HOSTS; do
        ping -c 1 -W 3 $host > /dev/null 2>&1 && echo "✅ 内网主机 $host 可达" && break
    done
    
    # 测试内网DNS
    nslookup company.com > /dev/null 2>&1 && echo "✅ 内网DNS解析正常" || echo "❌ 内网DNS解析失败"
    
    # 测试代理（如果配置）
    if [ -n "$http_proxy" ]; then
        curl --proxy $http_proxy -s --connect-timeout 5 http://www.baidu.com > /dev/null && echo "✅ 代理外网访问正常" || echo "⚠️ 代理外网访问异常"
    fi
else
    # 公网环境连通性检测
    ping -c 1 -W 3 8.8.8.8 && echo "✅ 外网连通" || echo "❌ 外网不通"
    nslookup www.baidu.com > /dev/null 2>&1 && echo "✅ DNS解析正常" || echo "❌ DNS解析失败"
fi

# 5. 服务层检测（配置调整）
echo "=== 服务层检测 ==="
if [ "$PRIVATE_ENV" = true ]; then
    # 私有化服务检测
    SERVICE_URL="${INTERNAL_SERVICE_HOST:-https://api.company.com}"
    echo "检测内部服务: $SERVICE_URL"
    curl -k -I --connect-timeout 5 "$SERVICE_URL" && echo "✅ 内部服务可达" || echo "❌ 内部服务不可达"
else
    # 公网服务检测
    SERVICE_URL="${SERVICE_HOST:-https://device.gz-ty.palm.tencent.com}"
    curl -I --connect-timeout 5 "$SERVICE_URL" && echo "✅ 公网服务可达" || echo "❌ 公网服务不可达"
fi
```

## 私有化环境常见问题及解决方案

### 1. 外网连通性问题

#### 问题描述
私有化环境通常无法直接访问外网，导致传统的外网连通性测试失败。

#### 解决方案
```bash
# 方案1: 测试代理服务器
test_proxy_connectivity() {
    if [ -n "$http_proxy" ]; then
        proxy_host=$(echo $http_proxy | sed 's|http://||' | cut -d: -f1)
        proxy_port=$(echo $http_proxy | sed 's|http://||' | cut -d: -f2)
        timeout 5 bash -c "echo > /dev/tcp/$proxy_host/$proxy_port" && return 0 || return 1
    fi
    return 1
}

# 方案2: 测试内网关键基础设施
test_internal_infrastructure() {
    # 测试内网DNS服务器
    dns_servers=$(grep nameserver /etc/resolv.conf | awk '{print $2}')
    for dns in $dns_servers; do
        ping -c 1 -W 3 $dns && echo "DNS服务器 $dns 可达" && return 0
    done
    
    # 测试内网网关
    gateway=$(ip route | grep default | awk '{print $3}')
    ping -c 1 -W 3 $gateway && echo "网关 $gateway 可达" && return 0
    
    return 1
}
```

### 2. DNS解析问题

#### 问题描述
私有化环境可能使用内部DNS，外部域名解析可能受限。

#### 解决方案
```bash
# 分层DNS测试
test_dns_resolution() {
    echo "=== DNS解析测试 ==="
    
    # 1. 测试内部域名
    INTERNAL_DOMAINS="localhost company.com internal.local"
    for domain in $INTERNAL_DOMAINS; do
        if nslookup $domain > /dev/null 2>&1; then
            echo "✅ 内部域名 $domain 解析成功"
            internal_dns_ok=true
        fi
    done
    
    # 2. 测试外部域名（通过内部DNS转发）
    EXTERNAL_DOMAINS="www.baidu.com www.qq.com"
    for domain in $EXTERNAL_DOMAINS; do
        if nslookup $domain > /dev/null 2>&1; then
            echo "✅ 外部域名 $domain 解析成功（通过内部DNS）"
            external_dns_ok=true
        fi
    done
    
    # 3. 综合评估
    if [ "$internal_dns_ok" = true ]; then
        echo "✅ DNS基础功能正常"
        return 0
    else
        echo "❌ DNS功能异常"
        return 1
    fi
}
```

### 3. 证书验证问题

#### 问题描述
私有化环境常使用自签名证书，导致HTTPS连接失败。

#### 解决方案
```bash
# 处理自签名证书的HTTP测试
test_https_with_self_signed() {
    local url=$1
    
    # 方案1: 跳过证书验证
    if curl -k -I --connect-timeout 5 "$url" > /dev/null 2>&1; then
        echo "✅ HTTPS连接成功（跳过证书验证）"
        return 0
    fi
    
    # 方案2: 尝试HTTP连接
    http_url=$(echo "$url" | sed 's/https:/http:/')
    if curl -I --connect-timeout 5 "$http_url" > /dev/null 2>&1; then
        echo "✅ HTTP连接成功"
        return 0
    fi
    
    echo "❌ HTTPS/HTTP连接都失败"
    return 1
}
```

### 4. 服务发现问题

#### 问题描述
私有化环境的服务地址可能不固定，需要动态发现。

#### 解决方案
```bash
# 服务发现机制
discover_internal_services() {
    echo "=== 内部服务发现 ==="
    
    # 方案1: 从配置文件发现
    if [ -f "/etc/iot/services.conf" ]; then
        services=$(grep -E "^[^#].*=" /etc/iot/services.conf | cut -d= -f2)
        echo "从配置文件发现服务: $services"
    fi
    
    # 方案2: 从环境变量发现
    services=$(env | grep -E "(SERVICE|API|HOST)" | cut -d= -f2)
    echo "从环境变量发现服务: $services"
    
    # 方案3: 从hosts文件发现
    internal_hosts=$(grep -E "(company|internal|local)" /etc/hosts | awk '{print $2}')
    echo "从hosts文件发现内部主机: $internal_hosts"
    
    # 方案4: 网络扫描发现（谨慎使用）
    # nmap -sn 192.168.1.0/24 | grep -E "Nmap scan report"
}
```

## 私有化环境检测最佳实践

### 1. 配置管理

#### 环境配置文件
```bash
# /etc/iot/network_config.conf
NETWORK_ENVIRONMENT="private"  # private/public/hybrid
INTERNAL_DNS_SERVERS="10.0.0.1,172.16.0.1"
PROXY_SETTINGS="http://proxy.company.com:8080"
INTERNAL_SERVICE_HOSTS="api.company.com,service.internal.local"
SKIP_EXTERNAL_TESTS="true"
CERTIFICATE_VALIDATION="false"
CUSTOM_CA_PATH="/etc/ssl/certs/company-ca.crt"
```

### 2. 检测策略选择

#### 智能检测策略
```bash
# 根据环境自动选择检测策略
select_detection_strategy() {
    # 检测网络环境类型
    if ip addr show | grep -qE "inet (10\.|172\.(1[6-9]|2[0-9]|3[01])\.|192\.168\.)"; then
        if ping -c 1 -W 3 8.8.8.8 > /dev/null 2>&1; then
            echo "hybrid"  # 混合环境：内网+外网
        else
            echo "private"  # 纯内网环境
        fi
    else
        echo "public"  # 公网环境
    fi
}

# 应用对应的检测策略
apply_detection_strategy() {
    strategy=$(select_detection_strategy)
    
    case $strategy in
        "private")
            echo "应用私有化网络检测策略"
            SKIP_EXTERNAL_CONNECTIVITY=true
            USE_INTERNAL_SERVICES=true
            SKIP_CERT_VALIDATION=true
            ;;
        "hybrid")
            echo "应用混合网络检测策略"
            SKIP_EXTERNAL_CONNECTIVITY=false
            USE_INTERNAL_SERVICES=true
            SKIP_CERT_VALIDATION=false
            ;;
        "public")
            echo "应用公网检测策略"
            SKIP_EXTERNAL_CONNECTIVITY=false
            USE_INTERNAL_SERVICES=false
            SKIP_CERT_VALIDATION=false
            ;;
    esac
}
```

### 3. 监控和告警

#### 私有化环境监控
```bash
# 私有化环境网络监控脚本
monitor_private_network() {
    while true; do
        echo "$(date): 开始私有化网络检测"
        
        # 检测内网基础设施
        gateway=$(ip route | grep default | awk '{print $3}')
        if ping -c 1 -W 3 $gateway > /dev/null 2>&1; then
            echo "$(date): ✅ 内网网关正常"
        else
            echo "$(date): ❌ 内网网关异常，需要检查"
        fi
        
        # 检测内部服务
        if curl -k -s --connect-timeout 5 https://api.company.com/health > /dev/null 2>&1; then
            echo "$(date): ✅ 内部服务正常"
        else
            echo "$(date): ❌ 内部服务异常，需要检查"
        fi
        
        # 检测DNS服务
        if nslookup company.com > /dev/null 2>&1; then
            echo "$(date): ✅ 内网DNS正常"
        else
            echo "$(date): ❌ 内网DNS异常，需要检查"
        fi
        
        sleep 300  # 5分钟检测一次
    done
}
```

## 总结

私有化环境对各层检测的影响

| 检测层 | 影响程度 | 主要调整 | 替代方案 |
|--------|----------|----------|----------|
| 物理层 | 无影响 ✅ | 无需调整 | - |
| 本地网络层 | 轻微影响 ⚠️ | DNS配置调整 | 内网DNS测试 |
| 连通性层 | 重大影响 ❌ | 外网测试替换 | 内网服务测试、代理测试 |
| 服务层 | 中等影响 ⚠️ | 服务地址调整 | 内部服务、证书处理 |

### 关键适配要点

1. **环境识别**: 自动识别私有化环境，选择合适的检测策略
2. **配置调整**: 使用内部服务地址、DNS服务器、代理配置
3. **测试替换**: 用内网连通性测试替代外网连通性测试
4. **证书处理**: 处理自签名证书和内部CA
5. **服务发现**: 实现内部服务的自动发现机制

### 最佳实践建议

1. **分环境配置**: 为不同环境准备不同的配置文件
2. **智能检测**: 实现环境自动识别和策略选择
3. **渐进式检测**: 从基础设施到应用服务逐层检测
4. **监控告警**: 建立适合私有化环境的监控体系
5. **文档维护**: 维护详细的私有化环境网络拓扑文档