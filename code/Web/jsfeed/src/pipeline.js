import { http } from './http.js';
import { parse } from './parser.js';
import { pLimit } from './pLimit.js';
import { readOne, writeOne } from './cache.js';
import { isOpen, reportSuccess, reportFailure, CircuitBreakError } from './breaker.js';
import { streamText } from './streaming.js';

/**
 * 并发拉取多个订阅源（限流 + 容错 + 熔断降级 + 流式进度）
 * @param {string[]} urls
 * @param {{ concurrency?: number, signal?: AbortSignal }} opts
 * @returns {Promise<Array<{ url, ok, data?, error?, fromCache? }>>}
 */
export async function fetchFeeds(urls, { concurrency = 5, signal } = {}) {
  const limit = pLimit(concurrency);

  // ⭐ Promise.all + 内部 catch：任意失败不会中断整个流水线（错误转数据）
  return Promise.all(
    urls.map((url) =>
      limit(async () => {
        // ① 熔断中？直接降级缓存
        if (isOpen(url)) {
          const cached = readOne(url);
          return cached
            ? { url, ok: true, data: cached, fromCache: true }
            : { url, ok: false, error: new CircuitBreakError(url) };
        }
        try {
          const resp = await http(url, { signal });
          // 流式拉取——边下边汇报进度
          const text = await streamText(resp, (loaded, total) => {
            if (total > 0) console.log(`[${url}] ${(loaded / total * 100).toFixed(0)}%`);
            else console.log(`[${url}] 已下载 ${(loaded / 1024).toFixed(0)} KB`);
          });
          const data = await parse(url, text);
          // ② 成功：写缓存 + 复位熔断
          writeOne(url, data);
          reportSuccess(url);
          return { url, ok: true, data };
        } catch (error) {
          reportFailure(url);
          // ③ 失败：尝试降级缓存
          const cached = readOne(url);
          if (cached) return { url, ok: true, data: cached, fromCache: true, staleError: error };
          return { url, ok: false, error };
        }
      })
    )
  );
}
