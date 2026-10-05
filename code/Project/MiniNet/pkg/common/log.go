// pkg/common/log.go - 全项目公用的结构化日志
package common

import (
	"fmt"
	"os"
	"sync/atomic"
	"time"
)

type LogLv int32

const (
	LvTrace LogLv = iota
	LvInfo
	LvWarn
	LvError
)

// 用 atomic 存全局级别,支持运行时动态切换;
// 阶段 ⑧ Reactor 会并发打日志,atomic 保证读写无数据竞争
var gLv atomic.Int32

func init() { gLv.Store(int32(LvInfo)) } // 默认 Info

// SetLogLevel 从 main 的 flag 调用
func SetLogLevel(lv LogLv) { gLv.Store(int32(lv)) }

// ParseLogLevel 把 "trace"/"info" 转成 LogLv
func ParseLogLevel(s string) (LogLv, error) {
	switch s {
	case "trace":
		return LvTrace, nil
	case "info":
		return LvInfo, nil
	case "warn":
		return LvWarn, nil
	case "error":
		return LvError, nil
	}
	return LvInfo, fmt.Errorf("unknown log level: %s", s)
}

// Log 带 tag 和级别的格式化输出
// 用法:common.Log(LvInfo, "arp", "who-has %s → %s", ip, mac)
func Log(lv LogLv, tag string, format string, args ...any) {
	if lv < LogLv(gLv.Load()) {
		return
	}
	tagStr := []string{"TRACE", "INFO ", "WARN ", "ERROR"}[lv]
	// 时间戳精确到毫秒,后续分析 pcap 对齐用
	ts := time.Now().Format("15:04:05.000")
	fmt.Fprintf(os.Stderr, "[%s][%s][%-5s] %s\n",
		ts, tagStr, tag, fmt.Sprintf(format, args...))
}

// 简洁的级别函数,后续代码几乎只调这四个
func Trace(tag, f string, a ...any) { Log(LvTrace, tag, f, a...) }
func Info(tag, f string, a ...any)  { Log(LvInfo, tag, f, a...) }
func Warn(tag, f string, a ...any)  { Log(LvWarn, tag, f, a...) }
func Error(tag, f string, a ...any) { Log(LvError, tag, f, a...) }
