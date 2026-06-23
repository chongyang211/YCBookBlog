<template>
  <div class="ct-panel">
    <div class="ct-form-grid">
      <div class="ct-form-item">
        <label class="ct-form-label">算法模式</label>
        <select v-model="mode" class="ct-select">
          <option value="AES-GCM">AES-GCM（推荐 · 带认证）</option>
          <option value="AES-CBC">AES-CBC</option>
        </select>
      </div>
      <div class="ct-form-item">
        <label class="ct-form-label">密钥长度</label>
        <select v-model.number="length" class="ct-select">
          <option :value="128">128 位</option>
          <option :value="192">192 位</option>
          <option :value="256">256 位（推荐）</option>
        </select>
      </div>
      <div class="ct-form-item ct-form-item-full">
        <label class="ct-form-label">密码 / 口令</label>
        <input v-model="password" type="text" class="ct-input ct-input-mono" placeholder="输入密码（用 PBKDF2 派生密钥，10 万次迭代）" />
      </div>
    </div>

    <div class="ct-toolbar">
      <button class="ct-btn ct-btn-primary" @click="doEncrypt" :disabled="busy">加密 →</button>
      <button class="ct-btn" @click="doDecrypt" :disabled="busy">← 解密</button>
      <button class="ct-btn" @click="swap">↔ 交换</button>
      <button class="ct-btn" @click="clear">清空</button>
      <button class="ct-btn" @click="copyOut">复制结果</button>
    </div>

    <div v-if="error" class="ct-msg ct-msg-err">{{ error }}</div>
    <div v-if="info" class="ct-msg ct-msg-ok">{{ info }}</div>

    <div class="ct-cols">
      <div class="ct-col">
        <div class="ct-col-head"><span>明文</span></div>
        <textarea v-model="plain" class="ct-textarea" placeholder="原文..."></textarea>
      </div>
      <div class="ct-col">
        <div class="ct-col-head"><span>密文（Base64，含 salt + IV）</span></div>
        <textarea v-model="cipher" class="ct-textarea" placeholder="密文（Base64）..."></textarea>
      </div>
    </div>

    <p style="font-size: 12px; color: #888; margin: 4px 0 0;">
      💡 密文结构：<code>base64( salt(16B) ‖ iv(12/16B) ‖ ciphertext )</code>，解密只需密码。
    </p>
  </div>
</template>

<script>
import { encryptAes, decryptAes, copyMixin } from './utils'

export default {
  name: 'AesPanel',
  mixins: [copyMixin],
  data() {
    return {
      plain: '', cipher: '', password: '',
      mode: 'AES-GCM', length: 256,
      busy: false, error: '', info: '',
    }
  },
  methods: {
    async doEncrypt() {
      this.error = ''; this.info = ''
      if (!this.plain) { this.error = '请输入明文'; return }
      if (!this.password) { this.error = '请输入密码'; return }
      this.busy = true
      try {
        this.cipher = await encryptAes(this.plain, this.password, this.mode, this.length)
        this.info = '加密成功'
      } catch (e) { this.error = '加密失败：' + e.message }
      finally { this.busy = false }
    },
    async doDecrypt() {
      this.error = ''; this.info = ''
      if (!this.cipher) { this.error = '请输入密文'; return }
      if (!this.password) { this.error = '请输入密码'; return }
      this.busy = true
      try {
        this.plain = await decryptAes(this.cipher, this.password, this.mode, this.length)
        this.info = '解密成功'
      } catch (e) { this.error = '解密失败：密码错误或密文损坏（' + e.message + '）' }
      finally { this.busy = false }
    },
    swap() { [this.plain, this.cipher] = [this.cipher, this.plain] },
    clear() { this.plain = ''; this.cipher = ''; this.error = ''; this.info = '' },
    async copyOut() { await this.ctCopy(this.cipher || this.plain) },
  },
}
</script>
