// 主题切换：对应书中 §08 阶段⑦
// 三态：auto（跟随系统）/ light / dark，持久化到 localStorage（和 Dexie 分开，避免首屏异步）
const KEY = 'mp.theme'
export type ThemeMode = 'auto' | 'light' | 'dark'

export function readThemeMode(): ThemeMode {
  const v = localStorage.getItem(KEY) as ThemeMode | null
  return v && ['auto', 'light', 'dark'].includes(v) ? v : 'auto'
}

export function writeThemeMode(mode: ThemeMode): void {
  localStorage.setItem(KEY, mode)
  applyTheme(mode)
}

export function applyTheme(mode: ThemeMode): void {
  const root = document.documentElement
  if (mode === 'auto') {
    root.removeAttribute('data-theme')
  } else {
    root.setAttribute('data-theme', mode)
  }
}

export function applyInitialTheme(): void {
  applyTheme(readThemeMode())
}
