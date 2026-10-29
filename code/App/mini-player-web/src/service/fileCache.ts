// 文件缓存：内存池，关键作用是把 File 对象和 mediaId 关联起来。
//
// 为什么必须有这一层？
//   repository.addMediaBatch() 存库前会 stripRuntimeFields —— 把 file 字段删掉
//   （File 对象无法安全跨 session 存 IndexedDB，关浏览器指针就断了）。
//   导入成功后 LibraryPage reload() 从 db 读回 items，item.file 已经 undefined，
//   直接点播放 → loadIntoElement 判为"文件已失效"。
//
// 本模块就是补这个洞：
//   1. scanner.filesToMedia() 完成后，调 fileCache.registerAll(items) 把 File 存进内存 Map
//   2. playerService.loadIntoElement() 优先从 cache 取 file，再退化到 item.file
//
// 关浏览器后，Map 清空 —— 对应"文件已失效"是真实状态，不欺骗用户。
import type { MediaItem } from '@/types'

const map = new Map<string, File>()

export const fileCache = {
  /** 单条注册 */
  put(id: string, file: File): void {
    map.set(id, file)
  },

  /** 批量注册（扫描完成后调一次） */
  registerAll(items: MediaItem[]): void {
    for (const it of items) {
      if (it.file) map.set(it.id, it.file)
    }
  },

  /** 取 File（没有就 null） */
  get(id: string): File | null {
    return map.get(id) ?? null
  },

  has(id: string): boolean {
    return map.has(id)
  },

  /** 把 item.file 补上（如果 cache 有的话） */
  hydrate(item: MediaItem): MediaItem {
    if (!item.file) {
      const f = map.get(item.id)
      if (f) item.file = f
    }
    return item
  },

  /** 批量 hydrate */
  hydrateAll(items: MediaItem[]): MediaItem[] {
    for (const it of items) this.hydrate(it)
    return items
  },

  /** 删除一条 */
  remove(id: string): void {
    map.delete(id)
  },

  clear(): void {
    map.clear()
  },

  get size(): number {
    return map.size
  },
}
