export class EventEmitter {
  // 用 Map<string, Set<fn>> 装监听器
  // 私有字段——外部不可访问
  #listeners = new Map();

  on(name, fn) {
    if (!this.#listeners.has(name)) this.#listeners.set(name, new Set());
    this.#listeners.get(name).add(fn);
    return this;             // ⭐ 链式
  }

  off(name, fn) {
    if (!fn) {
      this.#listeners.delete(name);   // 不传 fn 就清光这个事件
    } else {
      this.#listeners.get(name)?.delete(fn);
    }
    return this;
  }

  once(name, fn) {
    const wrapper = (...args) => {
      this.off(name, wrapper);
      fn.apply(this, args);
    };
    return this.on(name, wrapper);
  }

  emit(name, ...args) {
    const set = this.#listeners.get(name);
    if (!set) return false;
    [...set].forEach((fn) => {        // ⭐ 克隆再遍历，防 fn 内 off 自己
      try {
        fn.apply(this, args);
      } catch (e) {
        // ⭐ 一个 listener 出错不影响其他 listener
        console.error(`[EventEmitter] listener of "${name}" threw:`, e);
      }
    });
    return true;
  }

  // 清空所有监听器（dispose 时用）
  removeAllListeners() {
    this.#listeners.clear();
    return this;
  }

  // 调试用：返回某事件 listener 数量
  listenerCount(name) {
    return this.#listeners.get(name)?.size ?? 0;
  }
}
