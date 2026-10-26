export class SortWorker {
  #worker;
  constructor() {
    // ⭐ Vite 5 内置支持 import.meta.url + new Worker，自动构建
    this.#worker = new Worker(
      new URL('./sort.worker.js', import.meta.url),
      { type: 'module' }
    );
  }

  /** 对象数组版（结构化克隆，教学对比用） */
  sort(arr) {
    return new Promise((resolve, reject) => {
      const onMsg = (ev) => {
        this.#worker.removeEventListener('message', onMsg);
        resolve(ev.data);
      };
      this.#worker.addEventListener('message', onMsg);
      this.#worker.addEventListener('error', reject, { once: true });
      this.#worker.postMessage({ type: 'sort-object', arr });
    });
  }

  /** ⭐ Transferable 版：传 ArrayBuffer 零拷贝 */
  sortBuffer(arrayBuffer) {
    return new Promise((resolve, reject) => {
      const onMsg = (ev) => {
        this.#worker.removeEventListener('message', onMsg);
        resolve(ev.data.buffer);
      };
      this.#worker.addEventListener('message', onMsg);
      this.#worker.addEventListener('error', reject, { once: true });
      // ⭐ 第 2 个参数：transfer 列表（所有权转移，主线程原 buffer 立刻 detach）
      this.#worker.postMessage({ type: 'sort-buffer', buffer: arrayBuffer }, [arrayBuffer]);
    });
  }

  destroy() { this.#worker.terminate(); }
}
