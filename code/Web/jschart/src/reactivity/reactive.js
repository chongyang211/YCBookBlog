import { track, trigger } from './effect.js';

const reactiveMap = new WeakMap();   // ⭐ 同一个 obj 多次 reactive 复用同一个 Proxy

export function reactive(target) {
  if (target === null || typeof target !== 'object') return target;
  if (reactiveMap.has(target)) return reactiveMap.get(target);    // 缓存命中

  const proxy = new Proxy(target, {
    get(obj, key, receiver) {
      const result = Reflect.get(obj, key, receiver);
      track(obj, key);                       // ⭐ 收集依赖
      // ⭐ 关键升级：嵌套对象懒递归
      return typeof result === 'object' && result !== null
        ? reactive(result)
        : result;
    },
    set(obj, key, value, receiver) {
      const oldVal = obj[key];
      const ok = Reflect.set(obj, key, value, receiver);
      if (oldVal !== value) trigger(obj, key);  // ⭐ 值变了才触发
      return ok;
    },
  });
  reactiveMap.set(target, proxy);
  return proxy;
}
