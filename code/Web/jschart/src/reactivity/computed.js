import { effect, track, trigger } from './effect.js';

export function computed(getter) {
  let cachedValue;
  let dirty = true;        // ⭐ 脏标记：true 表示需要重新求值

  // 把 getter 包成 effect，但 lazy 不立即执行
  // 它依赖的 reactive 数据变 → scheduler 把 dirty 改回 true
  const runner = effect(getter, {
    lazy: true,
    scheduler: () => {
      if (!dirty) {
        dirty = true;
        trigger(obj, 'value');   // ⭐ 通知"读 c.value 的 effect"重跑
      }
    },
  });

  const obj = {
    get value() {
      if (dirty) {
        cachedValue = runner();   // 重新求值
        dirty = false;
      }
      track(obj, 'value');        // ⭐ 让"读 c.value"也能被外层 effect 收集
      return cachedValue;
    },
  };
  return obj;
}
