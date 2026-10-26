/** 把数据空间的点 (x, y) 映射到画布像素空间 */
export function makeScale({ data, width, height, padding = 40 }) {
  const xs = data.map((p) => p.x);
  const ys = data.map((p) => p.y);
  const xMin = Math.min(...xs), xMax = Math.max(...xs);
  const yMin = Math.min(...ys), yMax = Math.max(...ys);

  const innerW = width  - padding * 2;
  const innerH = height - padding * 2;

  return {
    xScale: (x) => padding + ((x - xMin) / (xMax - xMin || 1)) * innerW,
    yScale: (y) => height - padding - ((y - yMin) / (yMax - yMin || 1)) * innerH,
    xMin, xMax, yMin, yMax, padding, innerW, innerH,
  };
}
