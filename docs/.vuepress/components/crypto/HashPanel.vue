<template>
  <div class="ct-panel">
    <div class="ct-toolbar">
      <button class="ct-btn ct-btn-primary" @click="compute">计算全部</button>
      <button class="ct-btn" @click="clear">清空</button>
      <label class="ct-checkbox-label">
        <input v-model="upper" type="checkbox" class="ct-checkbox" />大写
      </label>
    </div>

    <div v-if="error" class="ct-msg ct-msg-err">{{ error }}</div>

    <div class="ct-col">
      <div class="ct-col-head"><span>输入</span></div>
      <textarea v-model="input" class="ct-textarea" placeholder="计算 MD5 / SHA-1 / SHA-256 / SHA-384 / SHA-512..." @input="autoCompute"></textarea>
    </div>

    <div v-if="results.length" class="ct-hash-list">
      <div v-for="r in displayResults" :key="r.name" class="ct-hash-item">
        <span class="ct-hash-name">{{ r.name }}</span>
        <span class="ct-hash-val">{{ r.value }}</span>
        <button class="ct-btn" style="padding: 4px 10px; font-size: 12px;" @click="ctCopy(r.value)">复制</button>
      </div>
    </div>
  </div>
</template>

<script>
import { md5, sha1, sha256, sha384, sha512, copyMixin } from './utils'

export default {
  name: 'HashPanel',
  mixins: [copyMixin],
  data() {
    return {
      input: '',
      results: [],
      error: '',
      upper: false,
      _timer: null,
    }
  },
  computed: {
    displayResults() {
      return this.results.map(r => ({
        ...r, value: this.upper ? r.value.toUpperCase() : r.value
      }))
    },
  },
  methods: {
    autoCompute() {
      clearTimeout(this._timer)
      this._timer = setTimeout(() => this.compute(), 200)
    },
    async compute() {
      this.error = ''
      if (!this.input) { this.results = []; return }
      try {
        const [s1, s2, s3, s5] = await Promise.all([
          sha1(this.input), sha256(this.input), sha384(this.input), sha512(this.input)
        ])
        this.results = [
          { name: 'MD5',     value: md5(this.input) },
          { name: 'SHA-1',   value: s1 },
          { name: 'SHA-256', value: s2 },
          { name: 'SHA-384', value: s3 },
          { name: 'SHA-512', value: s5 },
        ]
      } catch (e) { this.error = e.message }
    },
    clear() { this.input = ''; this.results = []; this.error = '' },
  },
}
</script>
