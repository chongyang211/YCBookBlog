import { reactive } from '../reactivity/reactive.js';

export class FpsMeter {
  state = reactive({ fps: 0, frame: 0, jank: 0 });

  #lastTs = performance.now();
  #frames = 0;
  #running = false;

  start() {
    this.#running = true;
    const tick = (now) => {
      if (!this.#running) return;
      this.#frames++;
      this.state.frame++;
      const dt = now - this.#lastTs;
      if (dt > 33) this.state.jank++;       // > 33ms = 一帧丢了 = 卡顿
      if (now - this.#lastTs >= 1000) {
        this.state.fps = this.#frames;
        this.#frames = 0;
        this.#lastTs = now;
      }
      requestAnimationFrame(tick);
    };
    requestAnimationFrame((t) => { this.#lastTs = t; tick(t); });
  }

  stop() { this.#running = false; }
}
