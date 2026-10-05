# Makefile - MiniNet 全局构建脚本
# 第 1 次会话（阶段 ①②③）已启用的 target: build / test / clean / wire-demo
# 其余 target 到对应阶段再启用

.PHONY: build test clean wire-demo \
        demo-ping demo-tcp demo-http demo-tls demo-ws demo-shop \
        bench trace pcap

build:
	@mkdir -p bin
	go build -o bin/mnet ./cmd/mnet
	@echo "✅ built: bin/mnet"

test:
	go test ./pkg/... -v

test-race:
	go test ./pkg/... -race

# 阶段 ③ Step 3.2 / 3.3 的大小端对比 demo
wire-demo:
	go run ./tests/wire_demo

clean:
	rm -rf bin/

# ───── 以下 target 在后续阶段启用 ─────
demo-ping:
	@echo "TODO: 阶段 ⑤ 完成后启用"
demo-tcp:
	@echo "TODO: 阶段 ⑥"
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
