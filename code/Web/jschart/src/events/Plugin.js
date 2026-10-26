import { EventEmitter } from './EventEmitter.js';

// 抽象基类（JS 没有 abstract 关键字，约定子类必须重写 install）
export class Plugin extends EventEmitter {
  // ⭐ 用 Symbol 做唯一 ID（不同插件实例的 id 永远不会冲突）
  static #idCounter = 0;
  id = Symbol(`plugin-${Plugin.#idCounter++}`);

  /** 插件名（必填，子类覆盖） */
  static name = 'AbstractPlugin';

  /** 注册时调用（必须重写） */
  install(host) {
    throw new Error(`${this.constructor.name}.install() must be implemented`);
  }

  /** 卸载时调用（默认空） */
  dispose() {
    this.emit('dispose');
    this.removeAllListeners();
  }
}
