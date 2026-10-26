// MSE 引擎——只关心"喂字节"，不关心"字节从哪来"
export class MseEngine extends EventTarget {
  #video;
  #ms = null;             // MediaSource
  #sb = null;             // SourceBuffer
  #queue = [];            // 待 append 的字节队列
  #mime;
  #ended = false;

  constructor(videoEl, mime) {
    super();
    this.#video = videoEl;
    this.#mime  = mime;
  }

  // 浏览器是否支持当前 mime
  static isSupported(mime) {
    return 'MediaSource' in window && MediaSource.isTypeSupported(mime);
  }

  // ⭐ Step A：开始一个新的 MediaSource
  open() {
    return new Promise((resolve, reject) => {
      if (!MseEngine.isSupported(this.#mime)) {
        return reject(new Error(`MSE 不支持 ${this.#mime}`));
      }
      const ms = new MediaSource();
      this.#ms = ms;
      this.#video.src = URL.createObjectURL(ms);
      // 注意：sourceopen 事件**只触发一次**——必须 once
      ms.addEventListener('sourceopen', () => {
        URL.revokeObjectURL(this.#video.src);   // 立刻回收 blob URL
        try {
          this.#sb = ms.addSourceBuffer(this.#mime);
          // ⭐ updateend ≈ "上一段字节消化完了"
          this.#sb.addEventListener('updateend', () => this.#drain());
          this.#sb.addEventListener('error', e => this.#fail(e));
          resolve();
        } catch (err) { reject(err); }
      }, { once: true });
    });
  }

  // ⭐ Step B：业务层喂数据进来
  appendChunk(arrayBuffer) {
    if (!this.#sb) throw new Error('MSE 未 open');
    if (this.#ended) return;
    this.#queue.push(arrayBuffer);
    this.#drain();
  }

  // ⭐ Step C：声明"我喂完了"
  endOfStream() {
    this.#ended = true;
    if (!this.#sb || this.#sb.updating || this.#queue.length) return;
    if (this.#ms.readyState === 'open') this.#ms.endOfStream();
  }

  // 排队消费 —— 因为 SourceBuffer.updating=true 时不能 append
  #drain() {
    if (!this.#sb || this.#sb.updating) return;
    if (this.#queue.length) {
      const buf = this.#queue.shift();
      try { this.#sb.appendBuffer(buf); }
      catch (err) { this.#fail(err); }
      return;
    }
    // 队列空且业务声明结束 → 关流
    if (this.#ended && this.#ms.readyState === 'open') {
      this.#ms.endOfStream();
    }
  }

  #fail(err) {
    this.dispatchEvent(new CustomEvent('mse-error', { detail: err }));
  }

  destroy() {
    if (this.#ms?.readyState === 'open') {
      try { this.#ms.endOfStream(); } catch {}
    }
    this.#queue.length = 0;
    this.#sb = null;
    this.#ms = null;
  }
}
