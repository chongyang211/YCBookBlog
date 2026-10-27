// LRC 歌词解析 + 定位。
// 对应书中 §07 阶段⑥：LRC 歌词同步（三端同构的解析器）。
//
// 格式：
//   [mm:ss.xx] text          普通行
//   [00:12.34][00:45.67] text  多时间戳
//   [ti: Song Title]         元信息（Pxx类）
//   [ar: Artist]
//   [00:12.345] text          精度到毫秒
//
// 二分查找定位当前行（§07 Step 7.2）：ms 升序，lowerBound 后 -1。

import type { LyricLine } from '@/types'

const TAG_RE = /\[(\d{1,2}):(\d{1,2})(?:[.:](\d{1,3}))?\]/g
const META_RE = /^\[(ti|ar|al|by|offset):\s*(.*?)\]\s*$/i

export interface LyricsDocument {
  lines: LyricLine[]
  meta: Record<string, string>
  offset: number  // 全局偏移毫秒
}

export function parseLrc(text: string): LyricsDocument {
  const lines: LyricLine[] = []
  const meta: Record<string, string> = {}
  let offset = 0

  for (const rawLine of text.split(/\r?\n/)) {
    const line = rawLine.trim()
    if (!line) continue

    // 元信息
    const m = line.match(META_RE)
    if (m) {
      const key = m[1].toLowerCase()
      const val = m[2].trim()
      meta[key] = val
      if (key === 'offset') offset = parseInt(val, 10) || 0
      continue
    }

    // 时间戳 + 文字
    const stamps: number[] = []
    let lastEnd = 0
    TAG_RE.lastIndex = 0
    let tagMatch: RegExpExecArray | null
    while ((tagMatch = TAG_RE.exec(line)) !== null) {
      const min = parseInt(tagMatch[1], 10)
      const sec = parseInt(tagMatch[2], 10)
      const frac = tagMatch[3] ?? ''
      // frac 可能是 1-3 位，归一到毫秒
      let ms = 0
      if (frac) {
        const pad = frac.padEnd(3, '0').slice(0, 3)
        ms = parseInt(pad, 10)
      }
      stamps.push(min * 60_000 + sec * 1000 + ms)
      lastEnd = tagMatch.index + tagMatch[0].length
    }
    const content = line.slice(lastEnd).trim()
    if (!stamps.length || !content) continue
    for (const t of stamps) lines.push({ time: t, text: content })
  }

  lines.sort((a, b) => a.time - b.time)
  return { lines, meta, offset }
}

/** 二分查找：返回当前应该高亮的行 index（可能是 -1 表示还没到第一行） */
export function findLineIndex(lines: LyricLine[], positionMs: number): number {
  if (lines.length === 0) return -1
  if (positionMs < lines[0].time) return -1

  let lo = 0, hi = lines.length - 1, ans = 0
  while (lo <= hi) {
    const mid = (lo + hi) >> 1
    if (lines[mid].time <= positionMs) {
      ans = mid
      lo = mid + 1
    } else {
      hi = mid - 1
    }
  }
  return ans
}
