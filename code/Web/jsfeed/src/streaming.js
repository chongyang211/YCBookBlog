/**
 * 流式下载——边下边汇报进度
 * @param {Response} resp
 * @param {(loaded: number, total: number) => void} onProgress
 * @returns {Promise<string>} 完整文本
 */
export async function streamText(resp, onProgress) {
  const total = Number(resp.headers.get('content-length')) || 0;
  const reader = resp.body.getReader();
  const decoder = new TextDecoder('utf-8');     // ⭐ 字节流 → utf-8 字符串
  const chunks = [];
  let loaded = 0;

  while (true) {
    const { value, done } = await reader.read();
    if (done) break;
    chunks.push(value);
    loaded += value.byteLength;
    onProgress?.(loaded, total);
  }

  // ⭐ 把所有 Uint8Array 拼起来再统一 decode（避免跨 chunk 截断 UTF-8 多字节字符）
  const merged = new Uint8Array(loaded);
  let offset = 0;
  for (const c of chunks) { merged.set(c, offset); offset += c.byteLength; }
  return decoder.decode(merged);
}
