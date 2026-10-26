import { makeScale } from './Coordinate.js';

export class LineChart {
  constructor(container) {
    this.container = container;
    this.canvas = document.createElement('canvas');
    container.appendChild(this.canvas);
    this.ctx = this.canvas.getContext('2d');

    this.cssW = 600;
    this.cssH = 300;
    this.resize(this.cssW, this.cssH);

    // 监听容器 resize → 重新 setup（ResizeObserver）
    this.ro = new ResizeObserver((entries) => {
      const { width, height } = entries[0].contentRect;
      this.resize(width || this.cssW, height || this.cssH);
      if (this.lastData) this.render(this.lastData);
    });
    // 让父容器决定大小
    this.canvas.style.width = '100%';
    this.canvas.style.maxWidth = '600px';
    this.ro.observe(container);
  }

  // ⭐ dpr 修复三步走：位图 = CSS × dpr；CSS 大小不变；坐标系缩放回 CSS 单位
  resize(cssW, cssH) {
    const dpr = window.devicePixelRatio || 1;
    // ⭐ 1. 位图大小 = CSS × dpr
    this.canvas.width  = Math.floor(cssW * dpr);
    this.canvas.height = Math.floor(cssH * dpr);
    // ⭐ 2. CSS 显示大小不变
    this.canvas.style.width  = cssW + 'px';
    this.canvas.style.height = cssH + 'px';
    // ⭐ 3. 缩放绘图坐标系 → 后续按 CSS 单位画
    this.ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    this.cssW = cssW; this.cssH = cssH;
  }

  render(data) {
    this.lastData = data;
    const ctx = this.ctx;
    const width = this.cssW, height = this.cssH;   // ⭐ 用 CSS 单位
    ctx.clearRect(0, 0, width, height);

    const { xScale, yScale, padding } = makeScale({ data, width, height });

    // 画坐标轴
    ctx.strokeStyle = '#6c7086';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(padding, padding);
    ctx.lineTo(padding, height - padding);
    ctx.lineTo(width - padding, height - padding);
    ctx.stroke();

    // 画折线
    ctx.strokeStyle = '#89b4fa';
    ctx.lineWidth = 2;
    ctx.beginPath();
    data.forEach((p, i) => {
      const x = xScale(p.x);
      const y = yScale(p.y);
      i === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    });
    ctx.stroke();

    // 画数据点
    ctx.fillStyle = '#f9e2af';
    data.forEach((p) => {
      ctx.beginPath();
      ctx.arc(xScale(p.x), yScale(p.y), 3, 0, Math.PI * 2);
      ctx.fill();
    });
  }

  destroy() {
    this.ro?.disconnect();          // ⭐ 卸载 observer 防内存泄漏
    this.canvas.remove();
  }
}
