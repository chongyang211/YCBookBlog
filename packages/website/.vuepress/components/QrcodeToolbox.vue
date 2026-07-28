<template>
  <client-only>
    <div class="qr-wrapper">
      <div class="qr-header">
        <h2 class="qr-title">🔲 二维码工具箱</h2>
        <p class="qr-desc">浏览器端生成 / 解码 / 美化，数据不上传，依赖按需懒加载</p>
      </div>

      <div class="qr-tabs">
        <button
          v-for="t in tabs"
          :key="t.id"
          class="qr-tab"
          :class="{ active: active === t.id }"
          @click="active = t.id"
        >{{ t.icon }} {{ t.label }}</button>
      </div>

      <keep-alive>
        <component :is="currentPanel" />
      </keep-alive>
    </div>
  </client-only>
</template>

<script>
import './qrcode/styles.css';
import GeneratePanel from './qrcode/GeneratePanel.vue';
import TemplatePanel from './qrcode/TemplatePanel.vue';
import LogoPanel from './qrcode/LogoPanel.vue';
import DecodePanel from './qrcode/DecodePanel.vue';
import BarcodePanel from './qrcode/BarcodePanel.vue';

export default {
  name: 'QrcodeToolbox',
  components: { GeneratePanel, TemplatePanel, LogoPanel, DecodePanel, BarcodePanel },
  data() {
    return {
      active: 'generate',
      tabs: [
        { id: 'generate', label: '生成', icon: '📱' },
        { id: 'template', label: '模板', icon: '📋' },
        { id: 'logo',     label: '带 Logo', icon: '🎨' },
        { id: 'decode',   label: '识别', icon: '🔍' },
        { id: 'barcode',  label: '条形码', icon: '📊' }
      ]
    };
  },
  computed: {
    currentPanel() {
      return {
        generate: 'GeneratePanel',
        template: 'TemplatePanel',
        logo: 'LogoPanel',
        decode: 'DecodePanel',
        barcode: 'BarcodePanel'
      }[this.active];
    }
  }
};
</script>
