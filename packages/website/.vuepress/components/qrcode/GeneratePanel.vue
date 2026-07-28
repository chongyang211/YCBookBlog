<template>
  <div class="qr-panel">
    <div class="qr-layout">
      <div class="qr-form">
        <label class="qr-label">内容</label>
        <textarea
          v-model="text"
          class="qr-textarea"
          rows="6"
          placeholder="输入要编码的文字或 URL，如 https://example.com"
        ></textarea>

        <div class="qr-form-row">
          <div>
            <label class="qr-label">尺寸 (px)</label>
            <input v-model.number="size" type="number" class="qr-input" min="64" max="2048" step="32" />
          </div>
          <div>
            <label class="qr-label">边距</label>
            <input v-model.number="margin" type="number" class="qr-input" min="0" max="16" />
          </div>
        </div>

        <div class="qr-form-row">
          <div>
            <label class="qr-label">容错等级</label>
            <select v-model="ecLevel" class="qr-select">
              <option value="L">L (~7%)</option>
              <option value="M">M (~15%)</option>
              <option value="Q">Q (~25%)</option>
              <option value="H">H (~30%)</option>
            </select>
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

        <div class="qr-toolbar">
          <button class="qr-btn qr-btn-primary" @click="generate" :disabled="loading">
            {{ loading ? '生成中…' : '生成二维码' }}
          </button>
          <button class="qr-btn" @click="download" :disabled="!hasResult">下载 PNG</button>
          <button class="qr-btn" @click="copyContent" :disabled="!text">复制内容</button>
        </div>

        <div v-if="msg" class="qr-msg" :class="{ 'qr-msg-err': isErr }">{{ msg }}</div>
      </div>

      <div class="qr-preview-box">
        <canvas ref="canvas" class="qr-preview-canvas"></canvas>
        <div v-if="!hasResult" class="qr-preview-empty">预览区</div>
      </div>
    </div>
  </div>
</template>

<script>
import { loadScript, SCRIPTS, copyText, downloadCanvas } from './utils.js';

export default {
  name: 'GeneratePanel',
  data() {
    return {
      text: 'https://github.com',
      size: 320,
      margin: 2,
      ecLevel: 'M',
      fg: '#1a1a1a',
      bg: '#ffffff',
      loading: false,
      hasResult: false,
      msg: '',
      isErr: false
    };
  },
  mounted() { this.generate(); },
  methods: {
    async generate() {
      if (!this.text) { this.show('内容不能为空', true); return; }
      this.loading = true; this.msg = '';
      try {
        await loadScript(SCRIPTS.qrcode);
        await window.QRCode.toCanvas(this.$refs.canvas, this.text, {
          width: this.size,
          margin: this.margin,
          errorCorrectionLevel: this.ecLevel,
          color: { dark: this.fg, light: this.bg }
        });
        this.hasResult = true;
        this.show('生成成功', false);
      } catch (e) {
        this.show(e.message || '生成失败', true);
      } finally {
        this.loading = false;
      }
    },
    download() {
      if (!this.hasResult) return;
      downloadCanvas(this.$refs.canvas, 'qrcode.png');
    },
    async copyContent() {
      const ok = await copyText(this.text);
      this.show(ok ? '已复制内容' : '复制失败', !ok);
    },
    show(t, err) { this.msg = t; this.isErr = !!err; }
  }
};
</script>
