# 服务层网络检测 - 简单命令行操作

## 概述

本文档提供简单直接的命令行操作来检测服务层网络功能，对应ServiceStrategy的功能。无需复杂脚本，直接在终端中输入命令即可完成服务可用性检测。

## 1. 服务主机配置检查

### 目的
确认要测试的服务地址是否正确配置。

### 操作步骤

#### 步骤1：检查环境变量
```bash
# 查看常见的服务配置环境变量
echo $SERVICE_HOST
echo $IOT_SERVICE_URL
echo $API_ENDPOINT
echo $SERVER_URL
```

#### 步骤2：检查配置文件
```bash
# 查看常见配置文件
cat /etc/iot/service.conf 2>/dev/null | grep -i host
cat /opt/app/config/service.json 2>/dev/null | grep -i host
cat ~/.config/iot/service.conf 2>/dev/null | grep -i host
```

#### 步骤3：手动设置测试服务地址
```bash
# 设置要测试的服务地址（示例）
SERVICE_URL="https://device.gz-ty.palm.tencent.com"
echo "测试服务地址: $SERVICE_URL"
```

## 2. 基本HTTP连接测试

### 目的
验证服务器是否可达，能否建立基本的HTTP连接。

### 操作步骤

#### 方法1：使用curl测试HTTP连接
```bash
# 基本HTTP连接测试
curl -I --connect-timeout 5 https://device.gz-ty.palm.tencent.com

# 查看详细连接信息
curl -w "状态码: %{http_code}\n响应时间: %{time_total}s\n响应大小: %{size_download}字节\n" \
     --connect-timeout 5 --max-time 10 -s -o /dev/null \
     https://device.gz-ty.palm.tencent.com
```

#### 方法2：使用wget测试
```bash
# 使用wget测试连接
wget --timeout=5 --tries=1 --spider https://device.gz-ty.palm.tencent.com

# 检查返回状态
echo $?  # 0表示成功，非0表示失败
```

#### 方法3：测试TCP连接
```bash
# 测试HTTPS端口连接（443）
timeout 5 bash -c 'echo > /dev/tcp/device.gz-ty.palm.tencent.com/443' && echo "TCP连接成功" || echo "TCP连接失败"

# 测试HTTP端口连接（80）
timeout 5 bash -c 'echo > /dev/tcp/device.gz-ty.palm.tencent.com/80' && echo "TCP连接成功" || echo "TCP连接失败"
```

### 快速检测命令
```bash
# 快速HTTP连接测试
curl -s --connect-timeout 5 https://device.gz-ty.palm.tencent.com > /dev/null && echo "✅ HTTP连接成功" || echo "❌ HTTP连接失败"
```

## 3. 健康检查端点测试

### 目的
测试服务的健康检查端点，验证服务内部状态。

### 常见健康检查端点
- `/health` - 标准健康检查端点
- `/healthz` - Kubernetes风格健康检查
- `/ping` - 简单ping检查
- `/status` - 状态检查端点
- `/health-check` - 完整健康检查
- `/api/health` - API健康检查
- `/actuator/health` - Spring Boot健康检查

### 操作步骤

#### 步骤1：测试标准健康检查端点
```bash
# 测试 /health 端点
curl -I --connect-timeout 3 https://device.gz-ty.palm.tencent.com/health

# 查看响应内容
curl -s --connect-timeout 3 https://device.gz-ty.palm.tencent.com/health
```

#### 步骤2：测试其他常见端点
```bash
# 测试 /healthz 端点
curl -I --connect-timeout 3 https://device.gz-ty.palm.tencent.com/healthz

# 测试 /ping 端点
curl -I --connect-timeout 3 https://device.gz-ty.palm.tencent.com/ping

# 测试 /status 端点
curl -I --connect-timeout 3 https://device.gz-ty.palm.tencent.com/status
```

#### 步骤3：逐个测试所有端点
```bash
# 逐个测试健康检查端点
for endpoint in /health /healthz /ping /status; do
    echo "测试端点: $endpoint"
    if curl -s --connect-timeout 3 https://device.gz-ty.palm.tencent.com$endpoint > /dev/null 2>&1; then
        echo "✅ $endpoint 可用"
    else
        echo "❌ $endpoint 不可用"
    fi
done
```

### 快速检测命令
```bash
# 快速健康检查测试
curl -s --connect-timeout 3 https://device.gz-ty.palm.tencent.com/health > /dev/null 2>&1 && echo "✅ 健康检查通过" || echo "❌ 健康检查失败"
```

## 4. 服务响应内容分析

### 目的
分析服务响应内容，判断服务健康状态。

### 操作步骤

#### 步骤1：获取响应内容
```bash
# 获取健康检查响应内容
response=$(curl -s --connect-timeout 3 https://device.gz-ty.palm.tencent.com/health)
echo "响应内容: $response"
```

#### 步骤2：分析响应状态
```bash
# 检查响应中的健康状态指示器
echo "$response" | grep -qi "ok\|healthy\|up\|running\|success" && echo "✅ 服务状态健康" || echo "⚠️ 服务状态异常"

# 检查错误指示器
echo "$response" | grep -qi "error\|down\|fail\|unhealthy" && echo "❌ 服务状态异常" || echo "✅ 无明显错误"
```

#### 步骤3：检查JSON格式响应
```bash
# 如果响应是JSON格式，可以使用jq解析（如果安装了jq）
if command -v jq > /dev/null; then
    echo "$response" | jq .
else
    echo "响应内容: $response"
fi
```

## 5. 综合服务检测

### 完整检测流程（逐步执行）

#### 步骤1：设置服务地址
```bash
# 设置要测试的服务地址
SERVICE_URL="https://device.gz-ty.palm.tencent.com"
echo "=== 服务层网络检测 ==="
echo "服务地址: $SERVICE_URL"
```

#### 步骤2：基本HTTP连接测试
```bash
echo "=== 基本HTTP连接测试 ==="
if curl -s --connect-timeout 5 --max-time 10 "$SERVICE_URL" > /dev/null 2>&1; then
    echo "✅ 基本HTTP连接: 成功"
    basic_http="true"
else
    echo "❌ 基本HTTP连接: 失败"
    basic_http="false"
fi
```

#### 步骤3：健康检查端点测试
```bash
echo "=== 健康检查端点测试 ==="
health_endpoint="none"
for endpoint in /health /healthz /ping /status; do
    echo "测试端点: $endpoint"
    if curl -s --connect-timeout 3 "$SERVICE_URL$endpoint" > /dev/null 2>&1; then
        echo "✅ 健康检查成功: $endpoint"
        health_endpoint="$endpoint"
        service_healthy="true"
        break
    else
        echo "❌ 健康检查失败: $endpoint"
    fi
done

if [ "$health_endpoint" = "none" ]; then
    echo "💥 所有健康检查端点都失败"
    service_healthy="false"
fi
```

#### 步骤4：综合结果评估
```bash
echo "=== 检测结果汇总 ==="
echo "基本HTTP连接: $basic_http"
echo "服务健康检查: $service_healthy"
echo "成功的健康端点: $health_endpoint"

# 判断整体服务状态
if [ "$service_healthy" = "true" ]; then
    echo "🎉 服务状态: 完全健康"
    overall_status="healthy"
elif [ "$basic_http" = "true" ]; then
    echo "⚠️ 服务状态: 部分可用（HTTP连接正常，但健康检查失败）"
    overall_status="partial"
else
    echo "💥 服务状态: 完全不可用"
    overall_status="unavailable"
fi
```

## 6. 常用检测命令速查

### 基础服务信息
```bash
# 检查服务配置
echo $SERVICE_HOST
cat /etc/iot/service.conf 2>/dev/null | grep host

# 设置测试服务
SERVICE_URL="https://device.gz-ty.palm.tencent.com"
```

### 连接测试
```bash
# HTTP连接测试
curl -I --connect-timeout 5 $SERVICE_URL

# TCP连接测试
timeout 5 bash -c 'echo > /dev/tcp/device.gz-ty.palm.tencent.com/443'

# 健康检查测试
curl -s --connect-timeout 3 $SERVICE_URL/health
```

### 服务诊断
```bash
# 查看详细响应信息
curl -w "状态码:%{http_code} 时间:%{time_total}s\n" -s -o /dev/null $SERVICE_URL

# 测试多个端点
for ep in /health /healthz /ping; do curl -I $SERVICE_URL$ep; done
```

## 7. 故障排查命令

### HTTP连接问题
```bash
# 检查域名解析
nslookup device.gz-ty.palm.tencent.com

# 检查端口连通性
telnet device.gz-ty.palm.tencent.com 443

# 详细连接信息
curl -v https://device.gz-ty.palm.tencent.com
```

### 健康检查问题
```bash
# 查看具体错误信息
curl -v https://device.gz-ty.palm.tencent.com/health

# 测试不同的健康检查端点
curl -I https://device.gz-ty.palm.tencent.com/health
curl -I https://device.gz-ty.palm.tencent.com/healthz
curl -I https://device.gz-ty.palm.tencent.com/ping
curl -I https://device.gz-ty.palm.tencent.com/status
```

### 网络层面问题
```bash
# 路由跟踪
traceroute device.gz-ty.palm.tencent.com

# 检查防火墙
sudo iptables -L | grep -i drop

# 检查网络连接
netstat -an | grep :443
```

## 8. 快速诊断命令组合

### 一键服务状态检查
```bash
SERVICE_URL="https://device.gz-ty.palm.tencent.com"; \
echo "服务地址: $SERVICE_URL"; \
curl -s --connect-timeout 5 "$SERVICE_URL" > /dev/null && echo "HTTP连接: ✅" || echo "HTTP连接: ❌"; \
curl -s --connect-timeout 3 "$SERVICE_URL/health" > /dev/null && echo "健康检查: ✅" || echo "健康检查: ❌"
```

### 服务详细信息检查
```bash
SERVICE_URL="https://device.gz-ty.palm.tencent.com"; \
echo "=== 服务详细检查 ==="; \
echo "服务地址: $SERVICE_URL"; \
curl -w "HTTP状态码: %{http_code}\n响应时间: %{time_total}s\n" -s -o /dev/null "$SERVICE_URL"; \
echo "健康检查端点测试:"; \
for ep in /health /healthz /ping /status; do \
  echo -n "$ep: "; \
  curl -s --connect-timeout 3 "$SERVICE_URL$ep" > /dev/null && echo "✅" || echo "❌"; \
done
```

### 模拟ServiceStrategy输出
```bash
SERVICE_URL="https://device.gz-ty.palm.tencent.com"
basic_http=$(curl -s --connect-timeout 5 "$SERVICE_URL" > /dev/null 2>&1 && echo "true" || echo "false")
service_healthy="false"
health_endpoint="none"

for endpoint in /health /healthz /ping /status; do
    if curl -s --connect-timeout 3 "$SERVICE_URL$endpoint" > /dev/null 2>&1; then
        service_healthy="true"
        health_endpoint="$endpoint"
        break
    fi
done

echo "{"
echo "  \"strategy_name\": \"Service\","
echo "  \"success\": $([ "$service_healthy" = "true" ] && echo "true" || echo "false"),"
echo "  \"details\": {"
echo "    \"service_host\": \"$SERVICE_URL\","
echo "    \"basic_http\": \"$basic_http\","
echo "    \"service_healthy\": \"$service_healthy\""
if [ "$health_endpoint" != "none" ]; then
    echo "    ,\"health_endpoint\": \"$health_endpoint\""
else
    echo "    ,\"health_endpoints_tested\": \"/health,/healthz,/ping,/status\""
fi
echo "  },"
if [ "$service_healthy" = "true" ]; then
    echo "  \"error_message\": \"Service reachability OK\""
elif [ "$basic_http" = "true" ]; then
    echo "  \"error_message\": \"Service reachable but health check failed\""
else
    echo "  \"error_message\": \"Service unreachable: $SERVICE_URL\""
fi
echo "}"
```

## 9. 使用示例

### 场景1：新服务部署验证
```bash
# 1. 设置服务地址
SERVICE_URL="https://your-new-service.com"

# 2. 基本连接测试
curl -I --connect-timeout 5 $SERVICE_URL

# 3. 健康检查测试
curl -s --connect-timeout 3 $SERVICE_URL/health

# 4. 查看响应内容
curl -s $SERVICE_URL/health | head -5
```

### 场景2：服务故障排查
```bash
# 1. 快速状态检查
SERVICE_URL="https://device.gz-ty.palm.tencent.com"
curl -s --connect-timeout 5 "$SERVICE_URL" > /dev/null && echo "服务可达" || echo "服务不可达"

# 2. 详细诊断
curl -v $SERVICE_URL 2>&1 | grep -E "Connected|HTTP"

# 3. 健康检查诊断
for ep in /health /healthz /ping; do
    echo -n "$ep: "
    curl -w "%{http_code}" -s -o /dev/null $SERVICE_URL$ep
    echo
done
```

### 场景3：服务监控
```bash
# 定期检查服务状态（每60秒）
SERVICE_URL="https://device.gz-ty.palm.tencent.com"
while true; do
    status=$(curl -s --connect-timeout 5 "$SERVICE_URL" > /dev/null && echo "正常" || echo "异常")
    echo "$(date): 服务状态 - $status"
    sleep 60
done
```

## 10. 总结

### 核心检测命令
1. **HTTP连接测试**: `curl -I --connect-timeout 5 <SERVICE_URL>`
2. **健康检查测试**: `curl -s --connect-timeout 3 <SERVICE_URL>/health`
3. **TCP连接测试**: `timeout 5 bash -c 'echo > /dev/tcp/<HOST>/<PORT>'`

### 快速诊断流程
1. 设置服务地址
2. 测试基本HTTP连接
3. 测试健康检查端点
4. 分析响应内容
5. 综合评估服务状态

### 优势
- **简单直接**: 无需复杂脚本，直接命令行操作
- **快速有效**: 几秒钟内完成服务可用性检测
- **易于理解**: 每个命令都有明确的目的和输出
- **灵活使用**: 可以单独使用或组合使用
- **故障定位**: 能够精确定位服务层问题