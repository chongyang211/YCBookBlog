import { Plugin } from '../events/Plugin.js';

export class ExportCsvPlugin extends Plugin {
  static name = 'ExportCsv';

  install(host) {
    this.host = host;
    this.btn = document.createElement('button');
    this.btn.textContent = '📊 导出 CSV';
    this.btn.onclick = () => this.export();
    host.toolbar.appendChild(this.btn);

    host.on('beforeDestroy', () => this.dispose());
  }

  export() {
    const data = this.host.getData();
    const csv = data.map((row) => row.join(',')).join('\n');
    const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url; a.download = `data-${Date.now()}.csv`; a.click();
    URL.revokeObjectURL(url);    // ⭐ 防内存泄漏
    this.emit('exported');
  }

  dispose() {
    this.btn?.remove();
    super.dispose();
  }
}
