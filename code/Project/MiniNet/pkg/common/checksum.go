// pkg/common/checksum.go - RFC1071 Internet Checksum
// 阶段 ⑤ 真实收发 IP 包时每个包都要算 checksum
package common

// InternetChecksum 计算 RFC1071 校验和
// 调用前应确保 buf 中原 checksum 字段已置 0
//
// 自检性质:如果 header 完整无误,接收方把整个头
// (包括 checksum 字段) 算一遍得到的结果必然是 0。
// 阶段 ⑤ 真实处理 IP 包时直接用这个性质 → 省一行代码,更不易错。
func InternetChecksum(buf []byte) uint16 {
	var sum uint32
	// 两两成对相加
	n := len(buf)
	for i := 0; i < n-1; i += 2 {
		sum += uint32(buf[i])<<8 | uint32(buf[i+1])
	}
	// 奇数字节单独处理
	if n%2 != 0 {
		sum += uint32(buf[n-1]) << 8
	}
	// 把高位的进位加回低位 (carry wrap-around)
	for sum>>16 != 0 {
		sum = (sum & 0xFFFF) + (sum >> 16)
	}
	// 取反码
	return ^uint16(sum)
}
