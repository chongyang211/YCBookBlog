// 10 频段 Web Audio 均衡器。
// 对应书中 §09 阶段⑧：均衡器 + 睡眠定时 + 构建。
//
// 架构：<audio>/<video> 元素 → MediaElementSource → 10×BiquadFilter(peaking) → GainNode(sleep) → destination
// 教学金点：这是四端里唯一自建 DSP 链路（Android/iOS/Qt 都走系统 EQ）。
//
// 自动播放策略坑（§09 必踩）：AudioContext 必须在"用户手势"里 resume，否则 state 永远卡在 suspended。
import { repository } from '@/data/repository';
export const EQ_FREQUENCIES = [31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000];
export const EQ_PRESETS = {
    flat: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    pop: [-1, -1, 0, 2, 4, 4, 2, 0, -1, -2],
    rock: [4, 3, 2, 0, -1, -1, 0, 2, 3, 4],
    jazz: [2, 2, 1, 2, -2, -2, 0, 1, 2, 3],
    classical: [3, 2, 1, 0, -1, -1, 0, 2, 3, 4],
    bass: [6, 5, 4, 3, 1, 0, 0, 0, 0, 0],
};
export class Equalizer {
    ctx = null;
    source = null;
    filters = [];
    gain = null;
    attachedEl = null;
    _enabled = false;
    async setup(el) {
        if (this.attachedEl === el && this.ctx)
            return;
        if (this.ctx) {
            // 换了元素：销毁重建（MediaElementSource 一次性绑定）
            await this.destroy();
        }
        this.ctx = new (window.AudioContext || window.webkitAudioContext)();
        this.source = this.ctx.createMediaElementSource(el);
        this.filters = EQ_FREQUENCIES.map(freq => {
            const f = this.ctx.createBiquadFilter();
            f.type = 'peaking';
            f.frequency.value = freq;
            f.Q.value = 1;
            f.gain.value = 0;
            return f;
        });
        this.gain = this.ctx.createGain();
        this.gain.gain.value = 1;
        // 串联：source → f0 → f1 → ... → f9 → gain → destination
        let prev = this.source;
        for (const f of this.filters) {
            prev.connect(f);
            prev = f;
        }
        prev.connect(this.gain);
        this.gain.connect(this.ctx.destination);
        this.attachedEl = el;
    }
    /** 自动播放策略：必须在用户手势里调用 */
    async ensureRunning() {
        if (!this.ctx)
            return;
        if (this.ctx.state === 'suspended') {
            try {
                await this.ctx.resume();
            }
            catch { /* noop */ }
        }
    }
    setEnabled(enabled) {
        this._enabled = enabled;
        const target = enabled ? this.filters : null;
        // bypass 实现：enabled=false 把所有 gain 置 0（也可解接，但重建成本高）
        if (!target) {
            for (const f of this.filters)
                f.gain.value = 0;
        }
    }
    get enabled() { return this._enabled; }
    setGains(gains) {
        if (!this._enabled)
            return;
        for (let i = 0; i < this.filters.length && i < gains.length; i++) {
            this.filters[i].gain.value = Math.max(-12, Math.min(12, gains[i]));
        }
    }
    /** sleep timer 用的渐弱（§09 Step 9.4） */
    async fadeOut(durationMs) {
        if (!this.ctx || !this.gain)
            return;
        const now = this.ctx.currentTime;
        this.gain.gain.cancelScheduledValues(now);
        this.gain.gain.setValueAtTime(this.gain.gain.value, now);
        this.gain.gain.linearRampToValueAtTime(0, now + durationMs / 1000);
        await new Promise(r => setTimeout(r, durationMs));
    }
    resetGain() {
        if (!this.gain || !this.ctx)
            return;
        this.gain.gain.cancelScheduledValues(this.ctx.currentTime);
        this.gain.gain.value = 1;
    }
    async destroy() {
        try {
            this.source?.disconnect();
        }
        catch { /* noop */ }
        for (const f of this.filters)
            try {
                f.disconnect();
            }
            catch { /* noop */ }
        try {
            this.gain?.disconnect();
        }
        catch { /* noop */ }
        try {
            await this.ctx?.close();
        }
        catch { /* noop */ }
        this.ctx = null;
        this.source = null;
        this.filters = [];
        this.gain = null;
        this.attachedEl = null;
    }
}
export const equalizer = new Equalizer();
/** 保存 EQ 到 Dexie；并应用到当前链路 */
export async function applyAndPersistEq(opts) {
    equalizer.setEnabled(opts.enabled);
    equalizer.setGains(opts.gains);
    await repository.saveSettings({
        eqEnabled: opts.enabled,
        eqGains: opts.gains,
        eqPreset: opts.preset,
    });
}
