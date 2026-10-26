/**
 * 解析 m3u8 文本为结构化对象
 * @param {string} text  m3u8 原文
 * @param {string} baseUrl  m3u8 自身的 URL（用于把相对路径转成绝对）
 * @returns {{ live: boolean, targetDuration: number, segments: Array<{url:string, duration:number, seq:number}> }}
 */
export function parseM3u8(text, baseUrl) {
  const lines = text.split(/\r?\n/).map(s => s.trim()).filter(Boolean);
  if (lines[0] !== '#EXTM3U') throw new Error('非合法 m3u8：缺 #EXTM3U');

  let live = true;            // 默认按直播处理，看到 ENDLIST 再改
  let targetDuration = 10;
  let mediaSequence = 0;
  const segments = [];
  let pendingDuration = null; // EXTINF 是"下一行的属性"

  for (const line of lines) {
    if (line.startsWith('#EXT-X-TARGETDURATION:')) {
      targetDuration = parseFloat(line.split(':')[1]);
    } else if (line.startsWith('#EXT-X-MEDIA-SEQUENCE:')) {
      mediaSequence = parseInt(line.split(':')[1], 10);
    } else if (line === '#EXT-X-ENDLIST') {
      live = false;
    } else if (line.startsWith('#EXTINF:')) {
      // #EXTINF:9.967,  → 9.967
      pendingDuration = parseFloat(line.slice(8).split(',')[0]);
    } else if (!line.startsWith('#')) {
      // 不是注释 = URL
      if (pendingDuration == null) {
        console.warn('[m3u8] 跳过 URL（缺 EXTINF）:', line);
        continue;
      }
      // ⭐ 用 URL 构造器把相对路径变绝对——不要自己拼字符串
      const abs = new URL(line, baseUrl).toString();
      segments.push({
        url: abs,
        duration: pendingDuration,
        seq: mediaSequence + segments.length,
      });
      pendingDuration = null;
    }
  }

  return { live, targetDuration, segments };
}
