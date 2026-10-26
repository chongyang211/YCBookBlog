import { NetworkError, TimeoutError, FeedError } from './errors.js';

/**
 * 带超时的 fetch（单次）
 * @param {string} url
 * @param {{ timeout?: number, signal?: AbortSignal }} opts
 */
export async function httpOnce(url, { timeout = 5000, signal } = {}) {
  const ctrl = new AbortController();
  // ⭐ 把外部 signal 与内部 ctrl 串联——任意一方 abort 都能取消
  if (signal) signal.addEventListener('abort', () => ctrl.abort(signal.reason));
  let timedOut = false;
  const timer = setTimeout(() => { timedOut = true; ctrl.abort(); }, timeout);

  try {
    const resp = await fetch(url, { signal: ctrl.signal });
    // ⭐ fetch 不会因 4xx/5xx reject，必须自己判断
    if (!resp.ok) throw new NetworkError(url, new Error(`HTTP ${resp.status}`));
    return resp;
  } catch (err) {
    if (timedOut) throw new TimeoutError(url, timeout);
    if (err instanceof FeedError) throw err;     // 已经是自定义错误，直接抛
    throw new NetworkError(url, err);
  } finally {
    clearTimeout(timer);
  }
}

/**
 * 带超时 + 指数退避重试的 fetch
 */
export async function http(url, opts = {}) {
  const { retries = 3, baseDelay = 500 } = opts;
  let lastErr;

  for (let attempt = 0; attempt <= retries; attempt++) {
    try {
      return await httpOnce(url, opts);
    } catch (err) {
      lastErr = err;

      // ⭐ 主动取消（用户行为）不应该重试
      if (err.name === 'AbortError' && opts.signal?.aborted) throw err;

      // ⭐ 4xx 客户端错误一般不该重试（参数错了，重试也是错）
      if (/^HTTP 4\d\d/.test(err.message) || /^HTTP 4\d\d/.test(err.cause?.message ?? '')) throw err;

      if (attempt === retries) break;

      // ⭐⭐⭐ 指数退避 + jitter
      const delay = baseDelay * 2 ** attempt + Math.random() * 200;
      console.warn(`[http] 第 ${attempt + 1} 次失败: ${err.message}，${delay.toFixed(0)}ms 后重试`);
      await sleep(delay);
    }
  }
  throw lastErr;
}

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}
