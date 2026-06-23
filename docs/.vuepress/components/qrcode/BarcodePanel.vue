<template>
  <div class="qr-panel">
    <div class="qr-layout">
      <div class="qr-form">
        <label class="qr-label">内容</label>
        <input v-model="text" class="qr-input" placeholder="例如 1234567890128" />

        <div class="qr-form-row">
          <div>
            <label class="qr-label">条码类型</label>
            <select v-model="format" class="qr-select">
              <option value="CODE128">CODE128（推荐，通用）</option>
              <option value="CODE39">CODE39</option>
              <option value="EAN13">EAN13（13 位数字）</option>
              <option value="EAN8">EAN8（8 位数字）</option>
              <option value="UPC">UPC-A（12 位数字）</option>
              <option value="ITF14">ITF-14（14 位数字）</option>
              <option value="MSI">MSI</option>
              <option value="pharmacode">Pharmacode</option>
            </select>
          </div>
          <div>
            <label class="qr-label">高度 (px)</label>
            <input v-model.number="height" type="number" class="qr-input" min="40" max="300" />
          </div>
        </div>

        <div class="qr-form-row">
          <div>
            <label class="qr-label">线宽</label>
            <input v-model.number="width" type="number" class="qr-input" min="1" max="6" />
          </div>
          <div>
            <label class="qr-label">前景色</label>
            <input v-model="fg" type="color" class="qr-color" />
          </div>
          <div>
            <label class="qr-label">背景色</label>
            <input v-model="bg" type="color" class="qr-color" />
          </div>
        </div>

        <label style="font-size:13px;color:#555;">
          <input type="checkbox" v-model="displayValue" /> 显示文字
        </label>

        <div class="qr-toolbar">
          <button class="qr-btn qr-btn-primary" @click="generate" :disabled="loading">
            {{ loading ? '生成中…' : '生成条形码' }}
          </button>
          <button class="qr-btn" @click="download" :disabled="!hasResult">下载 PNG</button>
        </div>

        <div v-if="msg" class="qr-msg" :class="{ 'qr-msg-err': isErr }">{{ msg }}</div>
      </div>

      <div class="qr-preview-box">
        <svg ref="svg" class="qr-preview-canvas"></svg>
        <div v-if="!hasResult" class="qr-preview-empty">预览区</div>
      </div>
    </div>
  </div>
</template>

<script>
import { loadScript, SCRIPTS, downloadSvgAsPng } from './utils.js';

export default {
  name: 'BarcodePanel',
  data() {
    return {
      text: '1234567890128',
      format: 'CODE128',
      width: 2,
      height: 100,
      fg: '#000000',
      bg: '#ffffff',
      displayValue: true,
      loading: false, hasResult: false, msg: '', isErr: false
    };
  },
  methods: {
    async generate() {
      if (!this.text) { this.show('内容不能为空', true); return; }
      this.loading = true; this.msg = '';
      try {
        await loadScript(SCRIPTS.jsbarcode);
        window.JsBarcode(this.$refs.svg, this.text, {
          format: this.format,
          width: this.width,
          height: this.height,
          lineColor: this.fg,
          background: this.bg,
          displayValue: this.displayValue,
          margin: 10
        });
        this.hasResult = true;
        this.show('生成成功', false);
      } catch (e) {
        this.show(e.message || '生成失败（注意类型对内容格式的要求）', true);
      } finally { this.loading = false; }
    },
    download() {
      if (this.hasResult) downloadSvgAsPng(this.$refs.svg, 'barcode.png');
    },
    show(t, err) { this.msg = t; this.isErr = !!err; }
  }
};
</script>
