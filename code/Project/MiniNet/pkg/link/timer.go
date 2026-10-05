// pkg/link/timer.go - 内部 timer 辅助
package link

import "time"

func newTimer(d time.Duration) <-chan time.Time {
	return time.After(d)
}
