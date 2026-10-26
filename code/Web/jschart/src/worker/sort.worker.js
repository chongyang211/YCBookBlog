// Worker 是独立线程，没有 window / document
// 支持 Float64Array 编码传输（Transferable 零拷贝）

// 对象数组版：结构化克隆传整个数组（教学对比用）
self.addEventListener('message', (ev) => {
  if (!ev.data || !ev.data.type) return;

  if (ev.data.type === 'sort-object') {
    const t0 = performance.now();
    const arr = ev.data.arr;            // ⚠️ 结构化克隆传整个数组（慢）
    arr.sort((a, b) => a.ts - b.ts);
    const elapsed = performance.now() - t0;
    self.postMessage({ type: 'sort-object', arr, elapsed });
    return;
  }

  if (ev.data.type === 'sort-buffer') {
    // ⭐ Transferable 版：直接拿到内存指针，零拷贝
    const buf = new Float64Array(ev.data.buffer);
    const n = buf.length / 2;
    // 把 (ts, value) 二元组按 ts 排序
    const arr = new Array(n);
    for (let i = 0; i < n; i++) arr[i] = { ts: buf[i * 2], value: buf[i * 2 + 1] };
    arr.sort((a, b) => a.ts - b.ts);
    for (let i = 0; i < n; i++) { buf[i * 2] = arr[i].ts; buf[i * 2 + 1] = arr[i].value; }
    // ⭐ 传回也用 transfer
    self.postMessage({ type: 'sort-buffer', buffer: buf.buffer }, [buf.buffer]);
  }
});
