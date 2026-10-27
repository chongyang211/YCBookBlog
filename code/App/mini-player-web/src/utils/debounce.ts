// 轻量防抖（§08 Step 8.1 自实现，不引入 lodash）
export function debounce<T extends (...args: unknown[]) => void>(
  fn: T,
  delay: number,
): (...args: Parameters<T>) => void {
  let timer: number | null = null
  return (...args: Parameters<T>) => {
    if (timer !== null) clearTimeout(timer)
    timer = window.setTimeout(() => {
      timer = null
      fn(...args)
    }, delay)
  }
}
