import { effect } from './effect.js';

export function watch(source, cb, options = {}) {
  const { immediate = false, deep = false } = options;

  // 1. 把 source 转换为 getter
  const getter = typeof source === 'function'
    ? source
    : () => deep ? traverse(source) : source;   // 默认只观察一层

  let oldValue;
  let cleanupFn;
  // 用户可在 cb 里 onCleanup(fn) 注册清理（比如取消旧请求）
  const onCleanup = (fn) => { cleanupFn = fn; };

  const job = () => {
    if (cleanupFn) cleanupFn();        // ⭐ 上一次的副作用先清理
    const newValue = runner();
    cb(newValue, oldValue, onCleanup);
    oldValue = newValue;
  };

  const runner = effect(getter, {
    lazy: true,
    scheduler: job,                    // ⭐ 数据变 → 跑 job 而不是直接重跑 getter
  });

  if (immediate) job();
  else oldValue = runner();
}

// 递归读取所有属性，触发 track —— 这就是 deep 的实现
function traverse(value, seen = new Set()) {
  if (value === null || typeof value !== 'object' || seen.has(value)) return value;
  seen.add(value);                     // ⭐ Set 防循环引用（a.b = a 环状对象）
  for (const key in value) traverse(value[key], seen);
  return value;
}
