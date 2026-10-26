import { queueJob } from './scheduler.js';

// Devtools 计数器（Inspector 读取）
export const stats = { effectRuns: 0, triggers: 0 };

// ⭐ 栈：记录嵌套层级
const effectStack = [];
let activeEffect = null;

export function effect(fn, options = {}) {
  const runner = () => {
    try {
      effectStack.push(runner);
      activeEffect = runner;
      cleanup(runner);          // 重跑前清掉旧依赖（解决条件分支问题）
      stats.effectRuns++;
      return fn();
    } finally {
      effectStack.pop();
      activeEffect = effectStack[effectStack.length - 1] ?? null;
    }
  };
  runner.deps = [];             // 这个 runner 被哪些 dep 收录了
  runner.options = options;     // scheduler / lazy 等高级选项
  if (!options.lazy) runner();
  return runner;
}

function cleanup(runner) {
  // 反向移除：让所有持有 runner 的 dep 把 runner 删掉
  runner.deps.forEach((dep) => dep.delete(runner));
  runner.deps.length = 0;
}

// targetMap: 全局依赖图
//   target obj  →  Map<key, Set<effect>>
//   {state}     →  { count: Set[fnA, fnB], name: Set[fnC] }
// ⭐ 必须 WeakMap：强引用 Map 会让被代理对象永远无法被 GC（内存泄漏）
const targetMap = new WeakMap();

export function track(target, key) {
  if (!activeEffect) return;       // 不在 effect 内的读取不需要追踪
  let depsMap = targetMap.get(target);
  if (!depsMap) targetMap.set(target, (depsMap = new Map()));
  let dep = depsMap.get(key);
  if (!dep) depsMap.set(key, (dep = new Set()));
  if (!dep.has(activeEffect)) {
    dep.add(activeEffect);
    activeEffect.deps.push(dep);   // ⭐ 双向记账：runner 也记得自己被哪些 dep 收录
  }
}

export function trigger(target, key) {
  const depsMap = targetMap.get(target);
  if (!depsMap) return;
  const dep = depsMap.get(key);
  if (!dep) return;
  stats.triggers++;
  // 复制一份 Set 再遍历——避免 effect 执行时再触发同 dep 导致死循环
  [...dep].forEach((runner) => {
    // ⭐ scheduler 钩子：让 computed / watch / batch 接管"什么时候真正跑"
    if (runner.options.scheduler) runner.options.scheduler(runner);
    else queueJob(runner);     // ⭐ 默认走微任务队列（批量合并）
  });
}
