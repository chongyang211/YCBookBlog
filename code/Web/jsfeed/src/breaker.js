import { CircuitBreakError } from './errors.js';

const failCount = new Map();     // url → 连续失败次数
const openUntil = new Map();     // url → 熔断恢复时间

const THRESHOLD = 3;             // 连续 3 次失败开熔断
const COOLDOWN  = 30_000;        // 熔断 30s

/** 在发请求前调用——返回 true 表示熔断中，应跳过 */
export function isOpen(url) {
  const until = openUntil.get(url);
  if (!until) return false;
  if (Date.now() >= until) {
    openUntil.delete(url);       // 时间到了，半开放尝试
    return false;
  }
  return true;
}

export function reportSuccess(url) {
  failCount.delete(url);
  openUntil.delete(url);
}

export function reportFailure(url) {
  const n = (failCount.get(url) || 0) + 1;
  failCount.set(url, n);
  if (n >= THRESHOLD) {
    openUntil.set(url, Date.now() + COOLDOWN);
    console.warn(`[breaker] ${url} 熔断 ${COOLDOWN / 1000}s`);
  }
}

export { CircuitBreakError };
