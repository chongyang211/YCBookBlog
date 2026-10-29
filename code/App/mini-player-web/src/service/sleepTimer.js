// 睡眠定时器。
// 对应书中 §09 Step 9.4：setTimeout + GainNode 渐弱。
//
// 后台节流坑（§06 阶段⑤）：标签页切后台时 setTimeout 被节流到 1Hz，所以精度只保证到 ±1 秒。
import { equalizer } from './equalizer';
export class SleepTimer {
    timerId = null;
    endAt = 0;
    /** 启动：minutes 分钟后暂停播放，前 N 秒做音量淡出 */
    start(minutes, player, fadeMs = 10_000) {
        this.cancel();
        const totalMs = Math.max(1, minutes) * 60_000;
        this.endAt = Date.now() + totalMs;
        const fadeStart = Math.max(0, totalMs - fadeMs);
        this.timerId = window.setTimeout(async () => {
            try {
                await equalizer.fadeOut(fadeMs);
            }
            catch { /* 没启用 EQ 就跳过，直接暂停 */ }
            player.pause();
            equalizer.resetGain();
            this.endAt = 0;
            this.timerId = null;
        }, fadeStart + fadeMs);
    }
    cancel() {
        if (this.timerId !== null) {
            clearTimeout(this.timerId);
            this.timerId = null;
        }
        this.endAt = 0;
        equalizer.resetGain();
    }
    get isActive() { return this.timerId !== null; }
    get remainingMs() {
        return this.endAt ? Math.max(0, this.endAt - Date.now()) : 0;
    }
}
export const sleepTimer = new SleepTimer();
