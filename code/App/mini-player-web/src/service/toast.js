// 轻量 Toast 系统：全局提示（取代旧版 scanMsg 文字堆积）。
//
// 使用：
//   import { toast } from '@/service/toast'
//   toast.success('已导入 20 条曲目')
//   toast.error('文件已失效，请重新导入')
//   toast.info('提示：点击任意位置启用音频')
//
// 组件侧：ToastHost.vue 读 toasts 响应式列表并渲染
import { reactive } from 'vue';
const state = reactive({ list: [] });
let nextId = 1;
function push(kind, message, duration = 2800) {
    const id = nextId++;
    state.list.push({ id, kind, message, duration });
    if (duration > 0) {
        window.setTimeout(() => dismiss(id), duration);
    }
    return id;
}
function dismiss(id) {
    const i = state.list.findIndex(t => t.id === id);
    if (i >= 0)
        state.list.splice(i, 1);
}
export const toastState = state;
export const toast = {
    success: (msg, duration) => push('success', msg, duration),
    error: (msg, duration) => push('error', msg, duration ?? 4200),
    info: (msg, duration) => push('info', msg, duration),
    warn: (msg, duration) => push('warn', msg, duration ?? 3600),
    dismiss,
};
