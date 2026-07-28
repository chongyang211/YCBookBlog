<template>
  <div class="ct-wrapper">
    <div class="ct-header">
      <h2 class="ct-title">加解密工具箱 · 浏览器端</h2>
      <p class="ct-desc">基于 Web Crypto API 本地运算，密钥与明文绝不上传，关闭页面即丢失</p>
    </div>

    <div class="ct-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="ct-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span class="ct-tab-icon">{{ tab.emoji }}</span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <keep-alive>
      <component :is="activeComponent" />
    </keep-alive>
  </div>
</template>

<script>
import AesPanel from './crypto/AesPanel.vue'
import Base64Panel from './crypto/Base64Panel.vue'
import HashPanel from './crypto/HashPanel.vue'
import HmacPanel from './crypto/HmacPanel.vue'
import JwtPanel from './crypto/JwtPanel.vue'
import RsaPanel from './crypto/RsaPanel.vue'
import './crypto/styles.css'

export default {
  name: 'CryptoToolbox',
  components: { AesPanel, Base64Panel, HashPanel, HmacPanel, JwtPanel, RsaPanel },
  data() {
    return {
      active: 'aes',
      tabs: [
        { key: 'aes',    label: 'AES',    comp: 'AesPanel',    emoji: '🔒' },
        { key: 'base64', label: 'Base64', comp: 'Base64Panel', emoji: '🔤' },
        { key: 'hash',   label: 'Hash',   comp: 'HashPanel',   emoji: '#️⃣' },
        { key: 'hmac',   label: 'HMAC',   comp: 'HmacPanel',   emoji: '🛡' },
        { key: 'jwt',    label: 'JWT',    comp: 'JwtPanel',    emoji: '🪪' },
        { key: 'rsa',    label: 'RSA',    comp: 'RsaPanel',    emoji: '🔑' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'AesPanel'
    },
  },
}
</script>
