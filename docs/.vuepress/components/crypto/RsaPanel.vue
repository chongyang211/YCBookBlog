<template>
  <div class="ct-panel">
    <div class="ct-toolbar">
      <span class="ct-toolbar-label">密钥长度</span>
      <select v-model.number="modulusLength" class="ct-select" style="width: 120px;">
        <option :value="2048">2048</option>
        <option :value="3072">3072</option>
        <option :value="4096">4096</option>
      </select>
      <button class="ct-btn ct-btn-primary" @click="genKey" :disabled="busy">生成密钥对</button>
      <button class="ct-btn" @click="downloadPub" :disabled="!publicKey">下载公钥</button>
      <button class="ct-btn" @click="downloadPri" :disabled="!privateKey">下载私钥</button>
    </div>

    <div v-if="error" class="ct-msg ct-msg-err">{{ error }}</div>
    <div v-if="info" class="ct-msg ct-msg-ok">{{ info }}</div>

    <div class="ct-cols">
      <div class="ct-col">
        <div class="ct-col-head">
          <span>公钥（PEM）</span>
          <span><button class="ct-btn" style="padding: 2px 8px; font-size: 11px;" @click="ctCopy(publicKey)">复制</button></span>
        </div>
        <textarea v-model="publicKey" class="ct-textarea" placeholder="-----BEGIN PUBLIC KEY-----"></textarea>
      </div>
      <div class="ct-col">
        <div class="ct-col-head">
          <span>私钥（PEM）</span>
          <span><button class="ct-btn" style="padding: 2px 8px; font-size: 11px;" @click="ctCopy(privateKey)">复制</button></span>
        </div>
        <textarea v-model="privateKey" class="ct-textarea" placeholder="-----BEGIN PRIVATE KEY-----"></textarea>
      </div>
    </div>

    <div class="ct-toolbar">
      <button class="ct-btn ct-btn-primary" @click="doEncrypt" :disabled="busy">用公钥加密 →</button>
      <button class="ct-btn" @click="doDecrypt" :disabled="busy">← 用私钥解密</button>
      <button class="ct-btn" @click="clearText">清空文本</button>
    </div>

    <div class="ct-cols">
      <div class="ct-col">
        <div class="ct-col-head"><span>明文</span></div>
        <textarea v-model="plain" class="ct-textarea" placeholder="待加密文本（长度受密钥长度限制）"></textarea>
      </div>
      <div class="ct-col">
        <div class="ct-col-head"><span>密文（Base64）</span></div>
        <textarea v-model="cipher" class="ct-textarea"></textarea>
      </div>
    </div>

    <p style="font-size: 12px; color: #888; margin: 4px 0 0;">
      💡 RSA-OAEP / SHA-256。2048 位密钥最多加密 ~190 字节，更长内容请用 AES 加密内容、用 RSA 加密 AES 密钥（混合加密）。
    </p>
  </div>
</template>

<script>
import { generateRsaKeyPair, rsaEncrypt, rsaDecrypt, copyMixin, downloadText } from './utils'

export default {
  name: 'RsaPanel',
  mixins: [copyMixin],
  data() {
    return {
      modulusLength: 2048,
      publicKey: '', privateKey: '',
      plain: '', cipher: '',
      busy: false, error: '', info: '',
    }
  },
  methods: {
    async genKey() {
      this.error = ''; this.info = ''
      this.busy = true
      try {
        const pair = await generateRsaKeyPair(this.modulusLength)
        this.publicKey = pair.publicKey
        this.privateKey = pair.privateKey
        this.info = `${this.modulusLength} 位 RSA 密钥对生成成功`
      } catch (e) { this.error = '生成失败：' + e.message }
      finally { this.busy = false }
    },
    async doEncrypt() {
      this.error = ''; this.info = ''
      if (!this.publicKey) { this.error = '请先生成或填入公钥'; return }
      this.busy = true
      try { this.cipher = await rsaEncrypt(this.plain, this.publicKey); this.info = '加密成功' }
      catch (e) { this.error = '加密失败：' + e.message }
      finally { this.busy = false }
    },
    async doDecrypt() {
      this.error = ''; this.info = ''
      if (!this.privateKey) { this.error = '请先生成或填入私钥'; return }
      this.busy = true
      try { this.plain = await rsaDecrypt(this.cipher, this.privateKey); this.info = '解密成功' }
      catch (e) { this.error = '解密失败：' + e.message }
      finally { this.busy = false }
    },
    clearText() { this.plain = ''; this.cipher = ''; this.error = ''; this.info = '' },
    downloadPub() { if (this.publicKey) downloadText('public.pem', this.publicKey) },
    downloadPri() { if (this.privateKey) downloadText('private.pem', this.privateKey) },
  },
}
</script>
