<template>
  <div class="ct-panel">
    <div class="ct-toolbar">
      <button class="ct-btn ct-btn-primary" @click="doEncode">编码 →</button>
      <button class="ct-btn" @click="doDecode">← 解码</button>
      <label class="ct-checkbox-label">
        <input v-model="urlSafe" type="checkbox" class="ct-checkbox" />URL-Safe
      </label>
      <button class="ct-btn" @click="swap">↔ 交换</button>
      <button class="ct-btn" @click="clear">清空</button>
      <button class="ct-btn" @click="copyOut">复制结果</button>
    </div>

    <div v-if="error" class="ct-msg ct-msg-err">{{ error }}</div>

    <div class="ct-cols">
      <div class="ct-col">
        <div class="ct-col-head"><span>原文（UTF-8）</span></div>
        <textarea v-model="plain" class="ct-textarea"></textarea>
      </div>
      <div class="ct-col">
        <div class="ct-col-head"><span>Base64</span></div>
        <textarea v-model="encoded" class="ct-textarea"></textarea>
      </div>
    </div>
  </div>
</template>

<script>
import { base64Encode, base64Decode, copyMixin } from './utils'

export default {
  name: 'Base64Panel',
  mixins: [copyMixin],
  data() {
    return { plain: '', encoded: '', urlSafe: false, error: '' }
  },
  methods: {
    doEncode() {
      this.error = ''
      try { this.encoded = base64Encode(this.plain, this.urlSafe) }
      catch (e) { this.error = '编码失败：' + e.message }
    },
    doDecode() {
      this.error = ''
      try { this.plain = base64Decode(this.encoded) }
      catch (e) { this.error = '解码失败：' + e.message }
    },
    swap() { [this.plain, this.encoded] = [this.encoded, this.plain] },
    clear() { this.plain = ''; this.encoded = ''; this.error = '' },
    async copyOut() { await this.ctCopy(this.encoded || this.plain) },
  },
}
</script>
