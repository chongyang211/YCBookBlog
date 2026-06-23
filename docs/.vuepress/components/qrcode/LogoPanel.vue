<template>
  <div class="qr-panel">
    <div class="qr-layout">
      <div class="qr-form">
        <label class="qr-label">内容</label>
        <textarea v-model="text" class="qr-textarea" rows="4" placeholder="https://example.com"></textarea>

        <label class="qr-label">Logo 图片</label>
        <input type="file" accept="image/*" @change="onLogo" class="qr-input" />

        <div class="qr-form-row">
          <div>
            <label class="qr-label">Logo 占比 ({{ Math.round(ratio*100) }}%)</label>
            <input v-model.number="ratio" type="range" min="0.1" max="0.35" step="0.01" class="qr-input" />
          </div>
          <div>
            <label class="qr-label">圆角</label>
            <input v-model.number="radius" type="number" min="0" max="40" class="qr-input" />
          </div>
        </div>

        <div class="qr-form-row">
          <div>
            <label class="qr-label">尺寸 (px)</label>
            <input v-model.number="size" type="number" class="qr-input" min="128" max="1024" step="32" />
          </div>
          <div>
            <label class="qr-label">容错等级</label>
            <select v-model="ecLevel" class="qr-select">
              <option value="M">M</option>
              <option value="Q">Q</option>
              <option value="H">H (推荐)</option>
            </select>
          </div>
        </div>

        <div class="qr-toolbar">
          <button class="qr-btn qr-btn-primary" @click="generate" :disabled="loading">
            {{ loading ? '生成中…' : '生成' }}
          </button>
          <button class="qr-btn" @click="download" :disabled="!hasResult">下载 PNG</button>
        </div>

        <div v-if="msg" class="qr-msg" :class="{ 'qr-msg-err': isErr }">{{ msg }}</div>
        <div class="qr-tip">💡 建议使用 H 级容错以补偿 Logo 遮挡区域</div>
      </div>

      <div class="qr-preview-box">
        <canvas ref="canvas" class="qr-preview-canvas"></canvas>
        <div v-if="!hasResult" class="qr-preview-empty">预览区</div>
      </div>
    </div>
  </div>
</template>

<script>
import { loadScript, SCRIPTS, downloadCanvas, drawLogoOnCanvas } from './utils.js';

export default {
  name: 'LogoPanel',
  data() {
    return {
      text: 'https://github.com',
      size: 360,
      ecLevel: 'H',
      ratio: 0.22,
      radius: 6,
      logoImg: null,
      loading: false, hasResult: false, msg: '', isErr: false
    };
  },
  methods: {
    onLogo(e) {
      const f = e.target.files && e.target.files[0];
      if (!f) return;
      const fr = new FileReader();
      fr.onload = () => {
        const img = new Image();
        img.onload = () => { this.logoImg = img; this.show('Logo 已加载', false); };
        img.onerror = () => this.show('Logo 解码失败', true);
        img.src = fr.result;
      };
      fr.readAsDataURL(f);
    },
    async generate() {
      if (!this.text) { this.show('内容不能为空', true); return; }
      this.loading = true; this.msg = '';
      try {
        await loadScript(SCRIPTS.qrcode);
        await window.QRCode.toCanvas(this.$refs.canvas, this.text, {
          width: this.size, margin: 2, errorCorrectionLevel: this.ecLevel
        });
        if (this.logoImg) {
          drawLogoOnCanvas(this.$refs.canvas, this.logoImg, { ratio: this.ratio, radius: this.radius });
        }
        this.hasResult = true;
        this.show('生成成功', false);
      } catch (e) {
        this.show(e.message || '生成失败', true);
      } finally { this.loading = false; }
    },
    download() { if (this.hasResult) downloadCanvas(this.$refs.canvas, 'qrcode-logo.png'); },
    show(t, err) { this.msg = t; this.isErr = !!err; }
  }
};
</script>
