<template>
  <div class="qr-panel">
    <div
      class="qr-drop"
      :class="{ active: dragOver }"
      @dragover.prevent="dragOver = true"
      @dragleave.prevent="dragOver = false"
      @drop.prevent="onDrop"
      @click="$refs.file.click()"
    >
      <div class="qr-drop-icon">📷</div>
      <div class="qr-drop-text">点击选择或拖入二维码图片</div>
      <div class="qr-drop-sub">支持 PNG / JPG / WebP / GIF 等</div>
      <input ref="file" type="file" accept="image/*" style="display:none" @change="onPick" />
    </div>

    <div class="qr-toolbar" style="margin-top:12px;">
      <button class="qr-btn" @click="$refs.file.click()">选择图片</button>
      <button class="qr-btn" @click="pasteImage">从剪贴板粘贴</button>
      <button class="qr-btn" @click="clear" :disabled="!result">清空</button>
    </div>

    <div v-if="msg" class="qr-msg" :class="{ 'qr-msg-err': isErr }">{{ msg }}</div>

    <div v-if="result" class="qr-decode-result">
      <div class="qr-decode-title">识别结果</div>
      <textarea :value="result" class="qr-decode-text" readonly rows="4"></textarea>
      <div class="qr-toolbar">
        <button class="qr-btn qr-btn-primary" @click="copy">复制结果</button>
        <button class="qr-btn" v-if="isUrl" @click="openUrl">打开链接</button>
      </div>
    </div>
  </div>
</template>

<script>
import { loadScript, SCRIPTS, copyText, fileToImageData } from './utils.js';

export default {
  name: 'DecodePanel',
  data() {
    return { dragOver: false, result: '', msg: '', isErr: false };
  },
  computed: {
    isUrl() { return /^https?:\/\//i.test(this.result); }
  },
  methods: {
    async decodeFile(file) {
      this.msg = ''; this.result = '';
      try {
        await loadScript(SCRIPTS.jsqr);
        const img = await fileToImageData(file);
        const code = window.jsQR(img.data, img.width, img.height);
        if (!code) { this.show('未识别到二维码', true); return; }
        this.result = code.data;
        this.show('识别成功', false);
      } catch (e) { this.show(e.message || '识别失败', true); }
    },
    onPick(e) {
      const f = e.target.files && e.target.files[0];
      if (f) this.decodeFile(f);
    },
    onDrop(e) {
      this.dragOver = false;
      const f = e.dataTransfer.files && e.dataTransfer.files[0];
      if (f) this.decodeFile(f);
    },
    async pasteImage() {
      try {
        const items = await navigator.clipboard.read();
        for (const it of items) {
          for (const type of it.types) {
            if (type.startsWith('image/')) {
              const blob = await it.getType(type);
              this.decodeFile(blob);
              return;
            }
          }
        }
        this.show('剪贴板中没有图片', true);
      } catch (e) {
        this.show('需要 HTTPS 或浏览器授权才能读取剪贴板', true);
      }
    },
    async copy() {
      const ok = await copyText(this.result);
      this.show(ok ? '已复制' : '复制失败', !ok);
    },
    openUrl() {
      // 仅在用户主动点击时打开
      window.open(this.result, '_blank', 'noopener,noreferrer');
    },
    clear() { this.result = ''; this.msg = ''; },
    show(t, err) { this.msg = t; this.isErr = !!err; }
  }
};
</script>
