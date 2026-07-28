<template>
  <div class="ct-panel">
    <div class="ct-form-grid">
      <div class="ct-form-item">
        <label class="ct-form-label">算法</label>
        <select v-model="algo" class="ct-select">
          <option value="SHA-256">HMAC-SHA-256（推荐）</option>
          <option value="SHA-1">HMAC-SHA-1</option>
          <option value="SHA-384">HMAC-SHA-384</option>
          <option value="SHA-512">HMAC-SHA-512</option>
        </select>
      </div>
      <div class="ct-form-item ct-form-item-full">
        <label class="ct-form-label">密钥</label>
        <input v-model="key" type="text" class="ct-input ct-input-mono" placeholder="HMAC 密钥" />
      </div>
    </div>

    <div class="ct-toolbar">
      <button class="ct-btn ct-btn-primary" @click="compute" :disabled="busy">计算</button>
      <button class="ct-btn" @click="clear">清空</button>
    </div>

    <div v-if="error" class="ct-msg ct-msg-err">{{ error }}</div>

    <div class="ct-col">
      <div class="ct-col-head"><span>消息</span></div>
      <textarea v-model="msg" class="ct-textarea"></textarea>
    </div>

    <div v-if="result" class="ct-hash-list">
      <div class="ct-hash-item">
        <span class="ct-hash-name">HEX</span>
        <span class="ct-hash-val">{{ result.hex }}</span>
        <button class="ct-btn" style="padding: 4px 10px; font-size: 12px;" @click="ctCopy(result.hex)">复制</button>
      </div>
      <div class="ct-hash-item">
        <span class="ct-hash-name">Base64</span>
        <span class="ct-hash-val">{{ result.base64 }}</span>
        <button class="ct-btn" style="padding: 4px 10px; font-size: 12px;" @click="ctCopy(result.base64)">复制</button>
      </div>
    </div>
  </div>
</template>

<script>
import { hmac, copyMixin } from './utils'

export default {
  name: 'HmacPanel',
  mixins: [copyMixin],
  data() {
    return { algo: 'SHA-256', key: '', msg: '', result: null, error: '', busy: false }
  },
  methods: {
    async compute() {
      this.error = ''; this.result = null
      if (!this.key) { this.error = '请输入密钥'; return }
      this.busy = true
      try { this.result = await hmac(this.algo, this.key, this.msg) }
      catch (e) { this.error = e.message }
      finally { this.busy = false }
    },
    clear() { this.key = ''; this.msg = ''; this.result = null; this.error = '' },
  },
}
</script>
