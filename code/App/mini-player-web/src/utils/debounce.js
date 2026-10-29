// 轻量防抖（§08 Step 8.1 自实现，不引入 lodash）
export function debounce(fn, delay) {
    let timer = null;
    return (...args) => {
        if (timer !== null)
            clearTimeout(timer);
        timer = window.setTimeout(() => {
            timer = null;
            fn(...args);
        }, delay);
    };
}
