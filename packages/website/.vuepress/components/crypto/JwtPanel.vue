<template>
  <div class="ct-panel">
    <div class="ct-toolbar">
      <button class="ct-btn ct-btn-primary" @click="parse">解码</button>
      <button class="ct-btn" @click="loadSample">示例</button>
      <button class="ct-btn" @click="clear">清空</button>
    </div>

    <div v-if="error" class="ct-msg ct-msg-err">{{ error }}</div>

    <div class="ct-col">
      <div class="ct-col-head"><span>JWT Token</span></div>
      <textarea
        v-model="token"
        class="ct-textarea"
        style="min-height: 120px;"
        placeholder="eyJhbGciOi..."
        @input="autoParse"
      ></textarea>
    </div>

    <div v-if="decoded" class="ct-jwt-segs">
      <div class="ct-jwt-seg">
        <div class="ct-jwt-seg-head ct-jwt-seg-h">HEADER · 算法 / 类型</div>
        <pre class="ct-jwt-seg-body">{{ pretty(decoded.header) }}</pre>
      </div>
      <div class="ct-jwt-seg">
        <div class="ct-jwt-seg-head ct-jwt-seg-p">PAYLOAD · 载荷</div>
        <pre class="ct-jwt-seg-body">{{ pretty(decoded.payload) }}</pre>
      </div>
      <div class="ct-jwt-seg">
        <div class="ct-jwt-seg-head ct-jwt-seg-s">SIGNATURE · 签名</div>
        <pre class="ct-jwt-seg-body">{{ decoded.signature }}</pre>
      </div>
    </div>

    <div v-if="decoded && decoded.meta" class="ct-form-grid">
      <div class="ct-form-item">
        <span class="ct-form-label">签发时间 (iat)</span>
        <span style="font-size: 13px;">{{ decoded.meta.iat || '—' }}</span>
      </div>
      <div class="ct-form-item">
        <span class="ct-form-label">生效时间 (nbf)</span>
        <span style="font-size: 13px;">{{ decoded.meta.nbf || '—' }}</span>
      </div>
      <div class="ct-form-item">
        <span class="ct-form-label">过期时间 (exp)</span>
        <span style="font-size: 13px;">{{ decoded.meta.exp || '—' }}</span>
      </div>
      <div class="ct-form-item">
        <span class="ct-form-label">状态</span>
        <span v-if="decoded.meta.expired" style="color: #c0392b; font-weight: 700;">已过期 · {{ -decoded.meta.expiresIn }}s</span>
        <span v-else-if="decoded.meta.expiresIn !== undefined" style="color: #27ae60; font-weight: 700;">有效 · 剩余 {{ formatLeft(decoded.meta.expiresIn) }}</span>
        <span v-else style="color: #888;">无 exp</span>
      </div>
    </div>

    <div v-if="decoded" class="ct-msg ct-msg-warn">
      ⚠️ 此工具仅解析 JWT 结构，<strong>不验证签名</strong>。请勿信任未验签的 token 内容。
    </div>
  </div>
</template>

<script>
import { decodeJwt } from './utils'

const SAMPLE = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IllhbmcgQ2hvbmciLCJpYXQiOjE3MTcxMjAwMDAsImV4cCI6MTk1Nzg5MzIwMH0.dummysignature'

export default {
  name: 'JwtPanel',
  data() {
    return { token: '', decoded: null, error: '', _timer: null }
  },
  methods: {
    autoParse() {
      clearTimeout(this._timer)
      this._timer = setTimeout(() => this.parse(true), 300)
    },
    parse(silent = false) {
      this.error = ''
      if (!this.token.trim()) { this.decoded = null; return }
      try { this.decoded = decodeJwt(this.token) }
      catch (e) {
        this.decoded = null
        if (!silent) this.error = e.message
      }
    },
    pretty(obj) { try { return JSON.stringify(obj, null, 2) } catch (e) { return String(obj) } },
    loadSample() { this.token = SAMPLE; this.parse() },
    clear() { this.token = ''; this.decoded = null; this.error = '' },
    formatLeft(s) {
      if (s < 60) return s + 's'
      if (s < 3600) return Math.floor(s / 60) + 'm'
      if (s < 86400) return Math.floor(s / 3600) + 'h'
      return Math.floor(s / 86400) + 'd'
    },
  },
}
</script>
