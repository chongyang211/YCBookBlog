import { paginate } from './pager.js';

const app = document.querySelector('#app');
const list = document.createElement('ul');
list.id = 'feed-list';
app.appendChild(list);

const sentinel = document.createElement('div');
sentinel.id = 'sentinel';
sentinel.textContent = '正在加载下一页…';
app.appendChild(sentinel);

let pager = null;          // 当前生成器
let loading = false;

export async function setItems(items) {
  list.innerHTML = '';
  pager = paginate(items, 20);
  await loadNext();
}

async function loadNext() {
  if (!pager || loading) return;
  loading = true;
  const { value: page, done } = await pager.next();
  loading = false;
  if (done) {
    sentinel.textContent = '— 没有更多了 —';
    observer.disconnect();
    return;
  }
  const frag = document.createDocumentFragment();
  for (const item of page) {
    const li = document.createElement('li');
    const t = document.createElement('a');
    t.href = item.link;
    t.target = '_blank';
    t.rel = 'noopener noreferrer';   // ⭐ 防 reverse-tabnabbing 攻击
    t.textContent = item.title;       // ⭐⭐⭐ textContent 而非 innerHTML——抗 XSS
    li.appendChild(t);
    const meta = document.createElement('small');
    meta.textContent = ' ' + item.pubDate;
    li.appendChild(meta);
    frag.appendChild(li);
  }
  list.appendChild(frag);
}

// ⭐⭐ 用 IntersectionObserver 监测哨兵元素是否进入视口
const observer = new IntersectionObserver((entries) => {
  for (const e of entries) {
    if (e.isIntersecting) loadNext();
  }
}, { rootMargin: '200px' });    // 离视口 200px 时就开始加载（提前预取）
observer.observe(sentinel);
