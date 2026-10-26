import { makeScale } from './Coordinate.js';

export class BarChart {
  constructor(container) {
    this.container = container;
    this.canvas = document.createElement('canvas');
    container.appendChild(this.canvas);
    this.ctx = this.canvas.getContext('2d');
    this.resize(600, 300);
    this.ro = new ResizeObserver((entries) => {
      const { width, height } = entries[0].contentRect;
      this.resize(width || this.cssW, height || this.cssH);
      if (this.lastData) this.render(this.lastData);
    });
    this.ro.observe(container);
  }

  resize(cssW, cssH) {
    const dpr = window.devicePixelRatio || 1;
    this.canvas.width  = cssW * dpr;
    this.canvas.height = cssH * dpr;
    this.canvas.style.width  = cssW + 'px';
    this.canvas.style.height = cssH + 'px';
    this.ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    this.cssW = cssW; this.cssH = cssH;
  }

  render(data) {
    this.lastData = data;
    const ctx = this.ctx;
    const width = this.cssW, height = this.cssH;
    ctx.clearRect(0, 0, width, height);

    const { xScale, yScale, padding, innerW } = makeScale({ data, width, height });
    const barWidth = (innerW / data.length) * 0.6;

    // 坐标轴
    ctx.strokeStyle = '#6c7086';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(padding, padding);
    ctx.lineTo(padding, height - padding);
    ctx.lineTo(width - padding, height - padding);
    ctx.stroke();

    // 柱子
    data.forEach((p) => {
      const x = xScale(p.x) - barWidth / 2;
      const y = yScale(p.y);
      const h = (height - padding) - y;
      ctx.fillStyle = '#a6e3a1';
      ctx.fillRect(x, y, barWidth, h);
    });
  }

  destroy() { this.ro?.disconnect(); this.canvas.remove(); }
}
