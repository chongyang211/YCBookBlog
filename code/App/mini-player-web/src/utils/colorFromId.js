// 根据稳定字符串（mediaId / filename）生成好看的渐变色，用于封面占位。
//
// 效果：同一首歌每次都是同一个色；不同歌视觉有区分；比纯灰占位好看一个数量级。
// 实现：字符串 → 哈希 → HSL 色相（0-360），固定饱和度和亮度。
function hashString(s) {
    let h = 5381;
    for (let i = 0; i < s.length; i++) {
        h = (h * 33) ^ s.charCodeAt(i);
    }
    return h >>> 0;
}
/** 返回一个 CSS linear-gradient 字符串，可直接做 background */
export function gradientFromId(id) {
    const h1 = hashString(id) % 360;
    const h2 = (h1 + 40 + (hashString(id + '!') % 60)) % 360;
    return `linear-gradient(135deg, hsl(${h1}, 70%, 60%) 0%, hsl(${h2}, 72%, 48%) 100%)`;
}
/** 返回主色（可作为 accent / 文字对比度参考） */
export function colorFromId(id) {
    const h = hashString(id) % 360;
    return `hsl(${h}, 70%, 55%)`;
}
