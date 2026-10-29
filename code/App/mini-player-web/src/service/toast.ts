// 轻量 Toast 系统：全局提示（取代旧版 scanMsg 文字堆积）。
//
// 使用：
//   import { toast } from '@/service/toast'
//   toast.success('已导入 20 条曲目')
//   toast.error('文件已失效，请重新导入')
//   toast.info('提示：点击任意位置启用音频')
//
// 组件侧：ToastHost.vue 读 toasts 响应式列表并渲染
import { reactive } from 'vue'

export type ToastKind = 'success' | 'error' | 'info' | 'warn'

export interface ToastItem {
  id: number
  kind: ToastKind
  message: string
  /** 自动消失毫秒；0 表示手动关闭 */
  duration: number
}

const state = reactive<{ list: ToastItem[] }>({ list: [] })
let nextId = 1

function push(kind: ToastKind, message: string, duration = 2800): number {
  const id = nextId++
  state.list.push({ id, kind, message, duration })
  if (duration > 0) {
    window.setTimeout(() => dismiss(id), duration)
  }
  return id
}

function dismiss(id: number): void {
  const i = state.list.findIndex(t => t.id === id)
  if (i >= 0) state.list.splice(i, 1)
}

export const toastState = state

export const toast = {
  success: (msg: string, duration?: number) => push('success', msg, duration),
  error:   (msg: string, duration?: number) => push('error', msg, duration ?? 4200),
  info:    (msg: string, duration?: number) => push('info', msg, duration),
  warn:    (msg: string, duration?: number) => push('warn', msg, duration ?? 3600),
  dismiss,
}
