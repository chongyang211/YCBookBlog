/**
 * 异步生成器：把 items 数组按 pageSize 分页惰性产出
 * @param {Array<Item>} items
 * @param {number} pageSize
 */
export async function* paginate(items, pageSize = 20) {
  let cursor = 0;
  while (cursor < items.length) {
    // ⭐ 模拟"网络延迟"——真实场景这里可能是 await fetch(nextPage)
    await new Promise((r) => setTimeout(r, 200));
    const page = items.slice(cursor, cursor + pageSize);
    yield page;
    cursor += pageSize;
  }
}
