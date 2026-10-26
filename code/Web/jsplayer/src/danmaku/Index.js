/**
 * 时间索引：按整数秒分桶
 * 查询"当前秒新出现的弹幕" 从 O(n) 变成 O(1)
 */
export class DanmakuIndex {
  #buckets = new Map();   // second(int) → DanmakuItem[]
  #total = 0;

  load(items) {
    this.#buckets.clear();
    this.#total = 0;
    for (const it of items) {
      const sec = Math.floor(it.t);
      let bucket = this.#buckets.get(sec);
      if (!bucket) { bucket = []; this.#buckets.set(sec, bucket); }
      bucket.push(it);
      this.#total++;
    }
  }

  // 取 [from, to) 区间内出现的所有弹幕
  range(from, to) {
    const out = [];
    for (let s = Math.floor(from); s < Math.ceil(to); s++) {
      const b = this.#buckets.get(s);
      if (b) for (const it of b) {
        if (it.t >= from && it.t < to) out.push(it);
      }
    }
    return out;
  }

  get total() { return this.#total; }
}
