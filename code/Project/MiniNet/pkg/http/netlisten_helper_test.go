package http_test

import "net"

// netListen 测试专用别名,避免 import "net" 和 _test 包的名字冲突
func netListen(network, addr string) (net.Listener, error) {
	return net.Listen(network, addr)
}
