export class PieChart {
  constructor(container) {
    this.container = container;
    this.canvas = document.createElement('canvas');
    container.appendChild(this.canvas);
    this.ctx = this.canvas.getContext('2d');
    this.resize(400, 400);
    this.colors = ['#89b4fa', '#a6e3a1', '#f9e2af', '#fab387', '#cba6f7', '#f38ba8'];
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
    const cx = this.cssW / 2, cy = this.cssH / 2;
    const radius = Math.min(cx, cy) * 0.8;
    ctx.clearRect(0, 0, this.cssW, this.cssH);

    const total = data.reduce((s, d) => s + d.value, 0);
    let startAngle = -Math.PI / 2;     // 从 12 点钟方向开始

    data.forEach((d, i) => {
      const angle = (d.value / total) * Math.PI * 2;
      ctx.fillStyle = this.colors[i % this.colors.length];
      ctx.beginPath();
      ctx.moveTo(cx, cy);
      ctx.arc(cx, cy, radius, startAngle, startAngle + angle);
      ctx.closePath();
      ctx.fill();

      // 标签（极坐标三件套：cos/sin × r）
      const midAngle = startAngle + angle / 2;
      const tx = cx + Math.cos(midAngle) * radius * 0.7;
      const ty = cy + Math.sin(midAngle) * radius * 0.7;
      ctx.fillStyle = '#1e1e2e';
      ctx.font = '14px sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText(`${d.label} ${(d.value / total * 100).toFixed(1)}%`, tx, ty);

      startAngle += angle;
    });
  }
}
