import { fetchFeeds } from './pipeline.js';
import { setItems } from './view.js';
import { list } from './feeds.js';

// ⭐ 用 codetabs 代理一下，避开 CORS（教学用，生产请走自己的代理）
const PROXY = (u) => `https://api.codetabs.com/v1/proxy?quest=${encodeURIComponent(u)}`;
const SOURCES = list().map(PROXY);

(async function () {
  console.time('fetch');
  const results = await fetchFeeds(SOURCES, { concurrency: 3 });
  console.timeEnd('fetch');

  // 状态栏统计
  const ok = results.filter((r) => r.ok);
  const cached = ok.filter((r) => r.fromCache).length;
  console.log(`拉取 ${ok.length}/${results.length} 源 · 失败 ${results.length - ok.length} · 命中缓存 ${cached}`);

  for (const r of results) {
    if (!r.ok) console.warn('❌', r.url, r.error.constructor.name, r.error.message);
  }

  // ⭐ 把所有源的 items 拍平 + 按时间倒排
  const merged = results
    .filter((r) => r.ok)
    .flatMap((r) => r.data.items)
    .sort((a, b) => new Date(b.pubDate) - new Date(a.pubDate));

  console.log(`合并 ${merged.length} 条`);
  await setItems(merged);
})();
