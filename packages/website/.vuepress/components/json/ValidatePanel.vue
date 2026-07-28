<template>
  <div class="jt-panel">
    <div class="jt-toolbar">
      <div class="jt-field">
        <label>模式</label>
        <select v-model="validMode">
          <option value="strict">严格 JSON</option>
          <option value="loose">宽松（允许尾逗号/注释/单引号）</option>
        </select>
      </div>
      <div class="jt-actions">
        <button class="jt-btn jt-btn-primary" @click="doValidate">✓ 立即校验</button>
      </div>
    </div>
    <div class="jt-io jt-io-full">
      <div class="jt-io-head">
        <span>待校验 JSON</span>
        <span class="jt-io-cnt">{{ validIn.length }} 字符 · {{ validIn.split('\n').length }} 行</span>
      </div>
      <textarea v-model="validIn" class="jt-textarea-large" placeholder="粘贴 JSON 后点击「立即校验」..." spellcheck="false"></textarea>
    </div>
    <div v-if="validResult" class="jt-valid-result" :class="validResult.ok ? 'jt-valid-ok' : 'jt-valid-err'">
      <div class="jt-valid-head">
        <span class="jt-valid-icon">{{ validResult.ok ? '✅' : '❌' }}</span>
        <span class="jt-valid-title">{{ validResult.ok ? '校验通过' : '校验失败' }}</span>
        <span class="jt-valid-meta" v-if="validResult.ok">类型：{{ validResult.type }} · 顶级元素：{{ validResult.topCount }}</span>
      </div>
      <div v-if="!validResult.ok" class="jt-valid-body">
        <p class="jt-valid-msg">{{ validResult.message }}</p>
        <div v-if="validResult.line" class="jt-valid-loc">
          <span>位置：</span>
          <strong>第 {{ validResult.line }} 行，第 {{ validResult.col }} 列</strong>
        </div>
        <pre v-if="validResult.snippet" class="jt-valid-snippet" v-html="validResult.snippet"></pre>
      </div>
    </div>
  </div>
</template>

<script>
import { looseParse, locateError } from './utils.js'

export default {
  name: 'ValidatePanel',
  data() {
    return { validIn: '', validMode: 'strict', validResult: null }
  },
  methods: {
    parse(text) {
      return this.validMode === 'loose' ? looseParse(text) : JSON.parse(text)
    },
    doValidate() {
      if (!this.validIn.trim()) { this.validResult = null; return }
      try {
        const obj = this.parse(this.validIn)
        const type = Array.isArray(obj) ? 'Array' : obj === null ? 'null' : typeof obj
        const topCount = Array.isArray(obj)
          ? obj.length
          : (obj && typeof obj === 'object' ? Object.keys(obj).length : 1)
        this.validResult = { ok: true, type, topCount }
      } catch (e) {
        const loc = locateError(this.validIn, e)
        this.validResult = { ok: false, ...loc }
      }
    },
  },
}
</script>
