/**
 * 信号量限流器
 * @param {number} concurrency 同时在飞的最大任务数
 * @returns {(fn: () => Promise<T>) => Promise<T>}
 */
export function pLimit(concurrency) {
  let inFlight = 0;
  const queue = [];

  // ⭐ 取下一个排队任务执行
  const next = () => {
    if (inFlight >= concurrency || queue.length === 0) return;
    inFlight++;
    const { fn, resolve, reject } = queue.shift();
    Promise.resolve()
      .then(fn)              // 调用业务函数（也可能同步抛）
      .then(resolve, reject)
      .finally(() => {
        inFlight--;
        next();              // ⭐ 一个任务完成，立刻调度下一个
      });
  };

  return (fn) =>
    new Promise((resolve, reject) => {
      queue.push({ fn, resolve, reject });
      next();
    });
}
