// 全局快捷键（§08 阶段⑦）
// Space/←→/↑↓/M/F，输入框聚焦时放行
import { onMounted, onBeforeUnmount } from 'vue'
import type { PlayerStore } from '@/service/playerService'

export function useGlobalShortcuts(player: PlayerStore) {
  function handler(e: KeyboardEvent) {
    const target = e.target as HTMLElement | null
    if (target) {
      const tag = target.tagName
      if (tag === 'INPUT' || tag === 'TEXTAREA' || target.isContentEditable) return
    }
    switch (e.code) {
      case 'Space': e.preventDefault(); player.togglePlay(); break
      case 'ArrowLeft': e.preventDefault(); player.seek(Math.max(0, player.position - 5000)); break
      case 'ArrowRight': e.preventDefault(); player.seek(player.position + 5000); break
      case 'ArrowUp': e.preventDefault(); player.setVolume(Math.min(1, player.volume + 0.05)); break
      case 'ArrowDown': e.preventDefault(); player.setVolume(Math.max(0, player.volume - 0.05)); break
      case 'KeyM': e.preventDefault(); player.toggleMute(); break
      case 'KeyF':
        if (player.current?.type === 'video' && player.videoEl) {
          e.preventDefault()
          if (document.fullscreenElement) document.exitFullscreen()
          else player.videoEl.requestFullscreen().catch(() => {/* noop */})
        }
        break
      case 'KeyN': e.preventDefault(); player.next(false); break
      case 'KeyP': e.preventDefault(); player.previous(); break
    }
  }

  onMounted(() => window.addEventListener('keydown', handler))
  onBeforeUnmount(() => window.removeEventListener('keydown', handler))
}
