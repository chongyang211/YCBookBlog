# Makefile - MiniNet 全局构建脚本
# 第 1 次会话（阶段 ①②③）已启用的 target: build / test / clean / wire-demo
# 其余 target 到对应阶段再启用

.PHONY: build test clean wire-demo \
        demo-ping demo-tcp demo-http demo-tls demo-ws demo-shop \
        bench trace pcap

build:
	@mkdir -p bin
	go build -o bin/mnet ./cmd/mnet
	go build -o bin/mnet-ping ./cmd/mnet-ping
	go build -o bin/mnet-dig ./cmd/mnet-dig
	go build -o bin/mnet-curl ./cmd/mnet-curl
	go build -o bin/mnet-proxy ./cmd/mnet-proxy
	go build -o bin/mnet-chat ./cmd/mnet-chat
	@echo "✅ built: 6 binaries (mnet · ping · dig · curl · proxy · chat)"

test:
	go test ./pkg/... -v

test-race:
	go test ./pkg/... -race

# 阶段 ③ Step 3.2 / 3.3 的大小端对比 demo
wire-demo:
	go run ./tests/wire_demo

# 阶段 ⑤ Step 5.4 的 🔥 BUG-2 现场
bug2-demo:
	go run ./tests/bug2_demo

# 阶段 ⑦ Step 7.3-7.4 的 🔥 BUG-3 现场
bug3-demo:
	go run ./tests/bug3_demo

# 阶段 ⑧ Step 8.3 的 🔥 BUG-4 现场 (需 Linux)
bug4-demo:
	go run ./tests/bug4_demo

# 阶段 ⑨ Step 9.3-9.4 DNS 本地演示
dns-demo:
	go run ./tests/dns_demo

# 阶段 ⑪ Step 11.5 HTTP 本地演示
http-demo:
	go run ./tests/http_demo

# 阶段 ⑬ 🔥 BUG-5 缓存击穿现场
bug5-demo:
	go run ./tests/bug5_demo

# 阶段 ⑭ 🔥 BUG-6 WebSocket 无心跳被 LB 杀
bug6-demo:
	go run ./tests/bug6_demo

# 阶段 ⑮ 🔥 BUG-7 TCP 队头阻塞 vs QUIC 多流
bug7-demo:
	go run ./tests/bug7_demo

# 阶段 ⑯ 总装: shop 全栈 demo (浏览器可点)
demo-shop:
	@echo "启动 shop demo, 然后用浏览器打开 http://127.0.0.1:8080/"
	@echo "Ctrl+C 停止"
	go run ./tests/shop_demo

# 阶段 ⑤ Step 5.3 ping 独立工具
demo-ping: build
	./bin/mnet-ping -c 4

# 阶段 ⑥ TCP 握手挥手测试
demo-tcp:
	go test ./pkg/transport/ -run 'TestHandshake|TestDataTransfer|TestFourWayClose' -v

clean:
	rm -rf bin/

# ───── 以下 target 在后续阶段启用 ─────
demo-http:
	@echo "TODO: 阶段 ⑪"
demo-tls:
	@echo "TODO: 阶段 ⑩"
demo-ws:
	@echo "TODO: 阶段 ⑭"
demo-shop:
	@echo "TODO: 阶段 ⑯ 总装"
bench:
	@echo "TODO: 阶段 ⑦/⑧/⑬/⑮ 启用"
trace:
	./bin/mnet --log=trace
pcap:
	@echo "TODO: 阶段 ⑤ 后启用,导出 /tmp/mnet-*.pcap"
