import { Plugin } from '../events/Plugin.js';

export class ExportPngPlugin extends Plugin {
  static name = 'ExportPng';

  install(host) {
    this.host = host;
    // host 是图表宿主，提供 toolbar / canvas 等接口
    this.btn = document.createElement('button');
    this.btn.textContent = '📷 导出 PNG';
    this.btn.onclick = () => this.export();
    host.toolbar.appendChild(this.btn);

    host.on('beforeDestroy', () => this.dispose());
  }

  export() {
    const url = this.host.canvas.toDataURL('image/png');
    const a = document.createElement('a');
    a.href = url;
    a.download = `chart-${Date.now()}.png`;
    a.click();
    this.emit('exported', { url });
  }

  dispose() {
    this.btn?.remove();
    super.dispose();
  }
}
