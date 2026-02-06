# 本地网络检测完整指南

## 概述

本地网络检测是网络诊断的核心环节，主要验证设备的基础网络配置是否正确。本文档详细说明了如何通过命令行工具实现LocalNetworkStrategy的所有检测功能，包括默认网关、DNS服务器和本地网络路由的检测。

## 检测架构

```
本地网络检测 (LocalNetworkStrategy)
├── 默认网关检测 (Default Gateway)
├── DNS服务器检测 (DNS Servers) 
└── 本地网络路由检测 (Local Network Routes)
```

## 1. 默认网关检测

### 检测目的
默认网关是设备访问外部网络的关键配置，没有默认网关设备无法访问互联网。

### 命令实现

#### 方法一：使用ip命令
```bash
# 查看默认路由
ip route show default

# 提取网关IP地址
ip route show default | awk '{print $3}' | head -1

# 完整检测脚本
get_gateway_ip() {
    gateway=$(ip route show default 2>/dev/null | awk '{print $3}' | head -1)
    if [ -n "$gateway" ]; then
        echo "$gateway"
        return 0
    else
        echo "UNKNOWN"
        return 1
    fi
}
```

#### 方法二：解析/proc/net/route文件
```bash
# 查看路由表原始数据
cat /proc/net/route

# 解析默认网关（十六进制转IP）
parse_gateway_from_proc() {
    # 查找destination为00000000的路由条目
    gateway_hex=$(awk 'NR>1 && $2=="00000000" && $3!="00000000" {print $3; exit}' /proc/net/route)
    
    if [ -n "$gateway_hex" ]; then
        # 将十六进制转换为IP地址（注意字节序）
        printf "%d.%d.%d.%d\n" \
            $((0x${gateway_hex:6:2})) \
            $((0x${gateway_hex:4:2})) \
            $((0x${gateway_hex:2:2})) \
            $((0x${gateway_hex:0:2}))
    else
        echo "UNKNOWN"
    fi
}
```

#### 方法三：使用route命令
```bash
# 显示路由表
route -n

# 提取默认网关
route -n | awk '$1=="0.0.0.0" && $2!="0.0.0.0" {print $2}' | head -1
```

### 完整检测脚本
```bash
#!/bin/bash

detect_default_gateway() {
    echo "=== 默认网关检测 ==="
    
    # 方法1: ip命令
    if command -v ip >/dev/null 2>&1; then
        gateway=$(ip route show default 2>/dev/null | awk '{print $3}' | head -1)
        if [ -n "$gateway" ]; then
            echo "✅ 默认网关: $gateway (通过ip命令)"
            return 0
        fi
    fi
    
    # 方法2: route命令
    if command -v route >/dev/null 2>&1; then
        gateway=$(route -n 2>/dev/null | awk '$1=="0.0.0.0" && $2!="0.0.0.0" {print $2}' | head -1)
        if [ -n "$gateway" ]; then
            echo "✅ 默认网关: $gateway (通过route命令)"
            return 0
        fi
    fi
    
    # 方法3: 解析/proc/net/route
    if [ -f "/proc/net/route" ]; then
        gateway_hex=$(awk 'NR>1 && $2=="00000000" && $3!="00000000" {print $3; exit}' /proc/net/route)
        if [ -n "$gateway_hex" ]; then
            gateway=$(printf "%d.%d.%d.%d" \
                $((0x${gateway_hex:6:2})) \
                $((0x${gateway_hex:4:2})) \
                $((0x${gateway_hex:2:2})) \
                $((0x${gateway_hex:0:2})))
            echo "✅ 默认网关: $gateway (通过/proc/net/route)"
            return 0
        fi
    fi
    
    echo "❌ 未找到默认网关"
    return 1
}
```

## 2. DNS服务器检测

### 检测目的
DNS服务器负责域名解析，是网络通信的基础服务。现代Linux系统有多种DNS配置方式。

### 命令实现

#### 方法一：读取/etc/resolv.conf
```bash
# 查看DNS配置
cat /etc/resolv.conf

# 提取DNS服务器（过滤本地地址）
grep "^nameserver" /etc/resolv.conf | awk '$2!="127.0.0.1" && $2!="::1" {print $2}'

# 格式化输出
get_dns_from_resolv() {
    dns_servers=$(grep "^nameserver" /etc/resolv.conf 2>/dev/null | \
                  awk '$2!="127.0.0.1" && $2!="::1" {print $2}' | \
                  tr '\n' ',' | sed 's/,$//')
    
    if [ -n "$dns_servers" ]; then
        echo "$dns_servers"
        return 0
    else
        return 1
    fi
}
```

#### 方法二：systemd-resolved配置
```bash
# 查看systemd-resolved运行时配置
cat /run/systemd/resolve/resolv.conf

# 使用resolvectl命令
resolvectl status | grep "DNS Servers:" | head -1

# 提取DNS服务器
get_dns_from_systemd() {
    # 尝试运行时配置文件
    if [ -f "/run/systemd/resolve/resolv.conf" ]; then
        dns_servers=$(grep "^nameserver" /run/systemd/resolve/resolv.conf 2>/dev/null | \
                      awk '$2!="127.0.0.1" && $2!="::1" {print $2}' | \
                      tr '\n' ',' | sed 's/,$//')
        
        if [ -n "$dns_servers" ]; then
            echo "$dns_servers"
            return 0
        fi
    fi
    
    # 尝试resolvectl命令
    if command -v resolvectl >/dev/null 2>&1; then
        dns_output=$(resolvectl status 2>/dev/null | grep "DNS Servers:" | head -1)
        if [ -n "$dns_output" ]; then
            dns_servers=$(echo "$dns_output" | cut -d: -f2 | xargs | tr ' ' ',')
            if [ -n "$dns_servers" ]; then
                echo "$dns_servers"
                return 0
            fi
        fi
    fi
    
    return 1
}
```

#### 方法三：NetworkManager配置
```bash
# 查看NetworkManager DNS配置
nmcli dev show | grep IP4.DNS

# 提取DNS服务器
get_dns_from_nm() {
    if command -v nmcli >/dev/null 2>&1; then
        dns_servers=$(nmcli dev show 2>/dev/null | grep "IP4.DNS" | awk '{print $2}' | tr '\n' ',' | sed 's/,$//')
        if [ -n "$dns_servers" ]; then
            echo "$dns_servers"
            return 0
        fi
    fi
    return 1
}
```

### 完整DNS检测脚本
```bash
#!/bin/bash

detect_dns_servers() {
    echo "=== DNS服务器检测 ==="
    
    # 方法1: 标准resolv.conf
    dns_servers=$(grep "^nameserver" /etc/resolv.conf 2>/dev/null | \
                  awk '$2!="127.0.0.1" && $2!="::1" {print $2}' | \
                  tr '\n' ',' | sed 's/,$//')
    
    if [ -n "$dns_servers" ]; then
        echo "✅ DNS服务器: $dns_servers (来源: /etc/resolv.conf)"
        return 0
    fi
    
    # 方法2: systemd-resolved
    if [ -f "/run/systemd/resolve/resolv.conf" ]; then
        dns_servers=$(grep "^nameserver" /run/systemd/resolve/resolv.conf 2>/dev/null | \
                      awk '$2!="127.0.0.1" && $2!="::1" {print $2}' | \
                      tr '\n' ',' | sed 's/,$//')
        
        if [ -n "$dns_servers" ]; then
            echo "✅ DNS服务器: $dns_servers (来源: systemd-resolved)"
            return 0
        fi
    fi
    
    # 方法3: resolvectl命令
    if command -v resolvectl >/dev/null 2>&1; then
        dns_output=$(resolvectl status 2>/dev/null | grep "DNS Servers:" | head -1)
        if [ -n "$dns_output" ]; then
            dns_servers=$(echo "$dns_output" | cut -d: -f2 | xargs | tr ' ' ',')
            if [ -n "$dns_servers" ]; then
                echo "✅ DNS服务器: $dns_servers (来源: resolvectl)"
                return 0
            fi
        fi
    fi
    
    # 方法4: NetworkManager
    if command -v nmcli >/dev/null 2>&1; then
        dns_servers=$(nmcli dev show 2>/dev/null | grep "IP4.DNS" | awk '{print $2}' | tr '\n' ',' | sed 's/,$//')
        if [ -n "$dns_servers" ]; then
            echo "✅ DNS服务器: $dns_servers (来源: NetworkManager)"
            return 0
        fi
    fi
    
    echo "❌ 未找到DNS服务器配置"
    return 1
}
```

## 3. 本地网络路由检测

### 检测目的
本地网络路由确保设备能够与同网段的其他设备通信，是局域网通信的基础。

### 命令实现

#### 方法一：使用ip命令
```bash
# 显示所有路由
ip route show

# 显示本地网络路由（排除默认路由）
ip route show | grep -v default

# 显示直连路由
ip route show | grep "scope link"

# 提取本地网络路由
get_local_routes_ip() {
    local_routes=$(ip route show 2>/dev/null | grep -v default | grep "scope link" | head -5)
    if [ -n "$local_routes" ]; then
        echo "$local_routes"
        return 0
    else
        return 1
    fi
}
```

#### 方法二：解析/proc/net/route文件
```bash
# 查看路由表原始数据
cat /proc/net/route

# 解析本地网络路由
parse_local_routes_proc() {
    # 查找本地网络路由：目标不是00000000，网关是00000000，掩码不是00000000
    awk 'NR>1 && $2!="00000000" && $3=="00000000" && $8!="00000000" {
        dest = $2
        mask = $8
        iface = $1
        metric = $7
        
        # 转换十六进制地址为IP
        dest_a = strtonum("0x" substr(dest,7,2))
        dest_b = strtonum("0x" substr(dest,5,2))
        dest_c = strtonum("0x" substr(dest,3,2))
        dest_d = strtonum("0x" substr(dest,1,2))
        dest_ip = dest_a "." dest_b "." dest_c "." dest_d
        
        # 计算CIDR
        mask_int = strtonum("0x" mask)
        cidr = 0
        temp = mask_int
        while (temp > 0) {
            if (temp % 2 == 1) cidr++
            temp = int(temp/2)
        }
        
        # 过滤回环网络和网关主机路由
        if (dest_ip != "127.0.0.0" && cidr != 32) {
            printf "%s/%d dev %s metric %s\n", dest_ip, cidr, iface, metric
        }
    }' /proc/net/route
}
```

#### 方法三：使用route命令
```bash
# 显示路由表
route -n

# 提取本地网络路由
route -n | awk '$1!="0.0.0.0" && $2=="0.0.0.0" && $1!="127.0.0.0" {
    # 简化的CIDR计算（常见掩码）
    cidr = 24
    if ($3 == "255.255.255.255") cidr = 32
    else if ($3 == "255.255.255.0") cidr = 24
    else if ($3 == "255.255.0.0") cidr = 16
    else if ($3 == "255.0.0.0") cidr = 8
    
    printf "%s/%d dev %s\n", $1, cidr, $8
}'
```

### 完整本地路由检测脚本
```bash
#!/bin/bash

detect_local_routes() {
    echo "=== 本地网络路由检测 ==="
    
    # 方法1: ip命令
    if command -v ip >/dev/null 2>&1; then
        local_routes=$(ip route show 2>/dev/null | grep -v default | grep "scope link" | head -5)
        if [ -n "$local_routes" ]; then
            echo "✅ 本地网络路由 (ip命令):"
            echo "$local_routes" | while read -r route; do
                echo "   $route"
            done
            return 0
        fi
    fi
    
    # 方法2: route命令
    if command -v route >/dev/null 2>&1; then
        local_routes=$(route -n 2>/dev/null | awk '$1!="0.0.0.0" && $2=="0.0.0.0" && $1!="127.0.0.0" {
            cidr = 24
            if ($3 == "255.255.255.255") cidr = 32
            else if ($3 == "255.255.255.0") cidr = 24
            else if ($3 == "255.255.0.0") cidr = 16
            else if ($3 == "255.0.0.0") cidr = 8
            printf "%s/%d dev %s\n", $1, cidr, $8
        }')
        
        if [ -n "$local_routes" ]; then
            echo "✅ 本地网络路由 (route命令):"
            echo "$local_routes" | while read -r route; do
                echo "   $route"
            done
            return 0
        fi
    fi
    
    # 方法3: 解析/proc/net/route
    if [ -f "/proc/net/route" ]; then
        local_routes=$(awk 'NR>1 && $2!="00000000" && $3=="00000000" && $8!="00000000" {
            dest = $2; mask = $8; iface = $1; metric = $7
            dest_a = strtonum("0x" substr(dest,7,2))
            dest_b = strtonum("0x" substr(dest,5,2))
            dest_c = strtonum("0x" substr(dest,3,2))
            dest_d = strtonum("0x" substr(dest,1,2))
            dest_ip = dest_a "." dest_b "." dest_c "." dest_d
            mask_int = strtonum("0x" mask)
            cidr = 0; temp = mask_int
            while (temp > 0) { if (temp % 2 == 1) cidr++; temp = int(temp/2) }
            if (dest_ip != "127.0.0.0" && cidr != 32) {
                printf "%s/%d dev %s metric %s\n", dest_ip, cidr, iface, metric
            }
        }' /proc/net/route)
        
        if [ -n "$local_routes" ]; then
            echo "✅ 本地网络路由 (/proc/net/route):"
            echo "$local_routes" | while read -r route; do
                echo "   $route"
            done
            return 0
        fi
    fi
    
    echo "❌ 未找到本地网络路由"
    return 1
}
```

## 4. 综合检测脚本

### 完整的本地网络检测脚本
```bash
#!/bin/bash

# 本地网络配置综合检测脚本
# 对应 LocalNetworkStrategy 的功能实现

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_status() {
    local status=$1
    local message=$2
    case $status in
        "OK")
            printf "${GREEN}✅ %s${NC}\n" "$message"
            ;;
        "WARN")
            printf "${YELLOW}⚠️  %s${NC}\n" "$message"
            ;;
        "ERROR")
            printf "${RED}❌ %s${NC}\n" "$message"
            ;;
        "INFO")
            printf "${BLUE}ℹ️  %s${NC}\n" "$message"
            ;;
    esac
}

# 检测默认网关
check_default_gateway() {
    print_status "INFO" "检测默认网关..."
    
    # 使用ip命令
    if command -v ip >/dev/null 2>&1; then
        gateway=$(ip route show default 2>/dev/null | awk '{print $3}' | head -1)
        if [ -n "$gateway" ]; then
            print_status "OK" "默认网关: $gateway"
            echo "$gateway"
            return 0
        fi
    fi
    
    # 使用route命令
    if command -v route >/dev/null 2>&1; then
        gateway=$(route -n 2>/dev/null | awk '$1=="0.0.0.0" && $2!="0.0.0.0" {print $2}' | head -1)
        if [ -n "$gateway" ]; then
            print_status "OK" "默认网关: $gateway"
            echo "$gateway"
            return 0
        fi
    fi
    
    # 解析/proc/net/route
    if [ -f "/proc/net/route" ]; then
        gateway_hex=$(awk 'NR>1 && $2=="00000000" && $3!="00000000" {print $3; exit}' /proc/net/route)
        if [ -n "$gateway_hex" ]; then
            gateway=$(printf "%d.%d.%d.%d" \
                $((0x${gateway_hex:6:2})) \
                $((0x${gateway_hex:4:2})) \
                $((0x${gateway_hex:2:2})) \
                $((0x${gateway_hex:0:2})))
            print_status "OK" "默认网关: $gateway"
            echo "$gateway"
            return 0
        fi
    fi
    
    print_status "ERROR" "未找到默认网关"
    echo "UNKNOWN"
    return 1
}

# 检测DNS服务器
check_dns_servers() {
    print_status "INFO" "检测DNS服务器..."
    
    # 方法1: /etc/resolv.conf
    dns_servers=$(grep "^nameserver" /etc/resolv.conf 2>/dev/null | \
                  awk '$2!="127.0.0.1" && $2!="::1" {print $2}' | \
                  tr '\n' ',' | sed 's/,$//')
    
    if [ -n "$dns_servers" ]; then
        print_status "OK" "DNS服务器: $dns_servers (来源: /etc/resolv.conf)"
        echo "$dns_servers"
        return 0
    fi
    
    # 方法2: systemd-resolved
    if [ -f "/run/systemd/resolve/resolv.conf" ]; then
        dns_servers=$(grep "^nameserver" /run/systemd/resolve/resolv.conf 2>/dev/null | \
                      awk '$2!="127.0.0.1" && $2!="::1" {print $2}' | \
                      tr '\n' ',' | sed 's/,$//')
        
        if [ -n "$dns_servers" ]; then
            print_status "OK" "DNS服务器: $dns_servers (来源: systemd-resolved)"
            echo "$dns_servers"
            return 0
        fi
    fi
    
    # 方法3: resolvectl
    if command -v resolvectl >/dev/null 2>&1; then
        dns_output=$(resolvectl status 2>/dev/null | grep "DNS Servers:" | head -1)
        if [ -n "$dns_output" ]; then
            dns_servers=$(echo "$dns_output" | cut -d: -f2 | xargs | tr ' ' ',')
            if [ -n "$dns_servers" ]; then
                print_status "OK" "DNS服务器: $dns_servers (来源: resolvectl)"
                echo "$dns_servers"
                return 0
            fi
        fi
    fi
    
    print_status "ERROR" "未找到DNS服务器配置"
    echo "NONE"
    return 1
}

# 检测本地网络路由
check_local_routes() {
    print_status "INFO" "检测本地网络路由..."
    
    # 使用ip命令
    if command -v ip >/dev/null 2>&1; then
        local_routes=$(ip route show 2>/dev/null | grep -v default | grep "scope link" | head -3)
        if [ -n "$local_routes" ]; then
            print_status "OK" "本地网络路由:"
            echo "$local_routes" | while read -r route; do
                echo "   $route"
            done
            # 格式化为单行输出
            formatted_routes=$(echo "$local_routes" | tr '\n' ';' | sed 's/;$//')
            echo "$formatted_routes"
            return 0
        fi
    fi
    
    # 使用route命令作为备选
    if command -v route >/dev/null 2>&1; then
        local_routes=$(route -n 2>/dev/null | awk '$1!="0.0.0.0" && $2=="0.0.0.0" && $1!="127.0.0.0" {
            printf "%s dev %s; ", $1, $8
        }' | sed 's/; $//')
        
        if [ -n "$local_routes" ]; then
            print_status "OK" "本地网络路由: $local_routes"
            echo "$local_routes"
            return 0
        fi
    fi
    
    print_status "ERROR" "未找到本地网络路由"
    echo "NONE"
    return 1
}

# 主检测函数
main_detection() {
    echo "========================================="
    echo "       本地网络配置检测"
    echo "========================================="
    echo
    
    # 检测结果变量
    gateway_result=""
    dns_result=""
    routes_result=""
    
    # 1. 检测默认网关
    gateway_result=$(check_default_gateway)
    gateway_status=$?
    echo
    
    # 2. 检测DNS服务器
    dns_result=$(check_dns_servers)
    dns_status=$?
    echo
    
    # 3. 检测本地路由
    routes_result=$(check_local_routes)
    routes_status=$?
    echo
    
    # 综合结果
    echo "========================================="
    echo "           检测结果汇总"
    echo "========================================="
    
    success_count=0
    total_count=3
    
    if [ $gateway_status -eq 0 ]; then
        print_status "OK" "默认网关检测: 通过"
        success_count=$((success_count + 1))
    else
        print_status "ERROR" "默认网关检测: 失败"
    fi
    
    if [ $dns_status -eq 0 ]; then
        print_status "OK" "DNS服务器检测: 通过"
        success_count=$((success_count + 1))
    else
        print_status "ERROR" "DNS服务器检测: 失败"
    fi
    
    if [ $routes_status -eq 0 ]; then
        print_status "OK" "本地路由检测: 通过"
        success_count=$((success_count + 1))
    else
        print_status "ERROR" "本地路由检测: 失败"
    fi
    
    echo
    echo "检测完成: $success_count/$total_count 项通过"
    
    # JSON格式输出（模拟LocalNetworkStrategy的输出）
    echo
    echo "JSON格式结果:"
    echo "{"
    echo "  \"strategy_name\": \"LocalNetwork\","
    echo "  \"success\": $([ $success_count -eq $total_count ] && echo "true" || echo "false"),"
    echo "  \"details\": {"
    echo "    \"default_gateway\": \"$gateway_result\","
    echo "    \"dns_servers\": \"$dns_result\","
    echo "    \"local_network_route\": \"$([ $routes_status -eq 0 ] && echo "true" || echo "false")\","
    echo "    \"local_network_info\": \"$routes_result\""
    echo "  },"
    
    if [ $success_count -eq $total_count ]; then
        echo "  \"error_message\": \"Local network configuration OK\""
    else
        issues=""
        [ $gateway_status -ne 0 ] && issues="${issues}No default gateway found, "
        [ $dns_status -ne 0 ] && issues="${issues}No DNS servers configured, "
        [ $routes_status -ne 0 ] && issues="${issues}No local network route found, "
        issues=$(echo "$issues" | sed 's/, $//')
        echo "  \"error_message\": \"Local network configuration issues: $issues\""
    fi
    
    echo "}"
    
    # 返回状态
    if [ $success_count -eq $total_count ]; then
        return 0
    else
        return 1
    fi
}

# 显示帮助信息
show_help() {
    echo "本地网络检测脚本"
    echo
    echo "用法: $0 [选项]"
    echo
    echo "选项:"
    echo "  -h, --help     显示帮助信息"
    echo "  -g, --gateway  仅检测默认网关"
    echo "  -d, --dns      仅检测DNS服务器"
    echo "  -r, --routes   仅检测本地路由"
    echo "  -j, --json     仅输出JSON格式结果"
    echo
    echo "示例:"
    echo "  $0              # 完整检测"
    echo "  $0 --gateway    # 仅检测网关"
    echo "  $0 --json       # JSON格式输出"
}

# 解析命令行参数
case "${1:-}" in
    -h|--help)
        show_help
        exit 0
        ;;
    -g|--gateway)
        check_default_gateway >/dev/null
        exit $?
        ;;
    -d|--dns)
        check_dns_servers >/dev/null
        exit $?
        ;;
    -r|--routes)
        check_local_routes >/dev/null
        exit $?
        ;;
    -j|--json)
        # 静默模式，只输出JSON
        exec 2>/dev/null
        main_detection | tail -n 10
        exit ${PIPESTATUS[0]}
        ;;
    "")
        # 默认完整检测
        main_detection
        exit $?
        ;;
    *)
        echo "未知选项: $1"
        echo "使用 $0 --help 查看帮助信息"
        exit 1
        ;;
esac
```

## 5. 故障排查指南

### 常见问题及解决方案

#### 问题1: 没有默认网关
```bash
# 诊断命令
ip route show
route -n

# 解决方案
sudo ip route add default via 192.168.1.1  # 替换为实际网关IP
# 或
sudo route add default gw 192.168.1.1
```

#### 问题2: DNS服务器未配置
```bash
# 诊断命令
cat /etc/resolv.conf
systemctl status systemd-resolved

# 解决方案
echo "nameserver 8.8.8.8" | sudo tee -a /etc/resolv.conf
echo "nameserver 114.114.114.114" | sudo tee -a /etc/resolv.conf
```

#### 问题3: 本地路由缺失
```bash
# 诊断命令
ip addr show
ip route show

# 解决方案
sudo ip route add 192.168.1.0/24 dev eth0  # 添加本地网络路由
```

### 网络服务重启
```bash
# 重启网络服务
sudo systemctl restart networking
sudo systemctl restart NetworkManager
sudo systemctl restart systemd-resolved

# 重新获取IP地址
sudo dhclient -r  # 释放
sudo dhclient     # 重新获取
```

## 6. 总结

本地网络检测是网络诊断的基础，通过检测默认网关、DNS服务器和本地路由，可以确保设备具备基本的网络通信能力。本文档提供的命令和脚本可以有效地进行本地网络配置检测和故障排查，完全对应LocalNetworkStrategy的功能实现。

### 关键要点
1. **默认网关**: 外网访问的必要条件
2. **DNS服务器**: 域名解析的基础服务
3. **本地路由**: 局域网通信的保障
4. **多种检测方法**: 确保在不同环境下都能正常工作
5. **详细的故障排查**: 快速定位和解决问题