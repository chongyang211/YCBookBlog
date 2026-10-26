import { stats } from '../reactivity/effect.js';
import { FpsMeter } from './FpsMeter.js';
import { reactive } from '../reactivity/reactive.js';
import { effect }   from '../reactivity/effect.js';
import { render }   from '../template/compile.js';

/**
 * Devtools 面板——自举：用响应式系统观察响应式系统
 */
export class Inspector {
  state = reactive({
    fps: 0, frame: 0, jank: 0,
    effectRuns: 0, triggers: 0,
    memMB: 0,
  });
  #fpsMeter = new FpsMeter();
  #panel;
  #timer = null;

  mount(host) {
    this.#panel = document.createElement('div');
    this.#panel.style.cssText = `
      position:fixed; right:10px; bottom:10px; z-index:9999;
      background:#11111b; color:#cdd6f4; padding:10px 14px;
      border:1px solid #45475a; border-radius:6px;
      font:12px/1.5 ui-monospace, monospace; min-width:220px;
    `;
    host.appendChild(this.#panel);
    this.#fpsMeter.start();

    // 每秒采样一次（独立 setInterval，不和 reactive 耦合）
    this.#timer = setInterval(() => {
      this.state.fps        = this.#fpsMeter.state.fps;
      this.state.frame      = this.#fpsMeter.state.frame;
      this.state.jank       = this.#fpsMeter.state.jank;
      this.state.effectRuns = stats.effectRuns;
      this.state.triggers   = stats.triggers;
      // performance.memory 是 Chrome 私有 API
      this.state.memMB = performance.memory
        ? +(performance.memory.usedJSHeapSize / 1024 / 1024).toFixed(1)
        : 0;
    }, 1000);

    // 用 effect + 模板渲染面板
    const tpl = `
      <div style="font-weight:bold;margin-bottom:6px">📊 jschart Devtools</div>
      <div>FPS: <b style="color:{{ fps >= 55 ? '#a6e3a1' : '#f38ba8' }}">{{ fps }}</b></div>
      <div>Jank: {{ jank }} 帧</div>
      <div>effect 重跑: {{ effectRuns }}</div>
      <div>trigger 触发: {{ triggers }}</div>
      <div>内存: {{ memMB }} MB</div>
    `;
    effect(() => { this.#panel.innerHTML = render(tpl, this.state); });
  }

  destroy() {
    clearInterval(this.#timer);
    this.#fpsMeter.stop();
    this.#panel?.remove();
  }
}
