const microQueue = new Set();
const rafQueue   = new Set();
let microFlushing = false;
let rafScheduled  = false;

/** 微任务队列：本帧内合并多次 mutation（Vue 3 / React Concurrent 同款思路） */
export function queueJob(job) {
  microQueue.add(job);
  if (!microFlushing) {
    microFlushing = true;
    Promise.resolve().then(() => {
      try { [...microQueue].forEach((j) => j()); }
      finally { microQueue.clear(); microFlushing = false; }
    });
  }
}

// ⭐ 屏幕级任务（render / layout）走 rAF——一帧只执行一次
export function queueRender(job) {
  rafQueue.add(job);
  if (!rafScheduled) {
    rafScheduled = true;
    requestAnimationFrame(() => {
      try { [...rafQueue].forEach((j) => j()); }
      finally { rafQueue.clear(); rafScheduled = false; }
    });
  }
}
