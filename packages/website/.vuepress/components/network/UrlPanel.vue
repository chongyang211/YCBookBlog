<template>
  <div class="nt-panel">
    <div class="nt-toolbar">
      <span class="nt-toolbar-label">URL</span>
      <input
        v-model="url"
        class="nt-input nt-input-mono"
        style="flex: 1; min-width: 280px;"
        placeholder="https://example.com/path?a=1&b=2#hash"
        @input="autoParse"
      />
      <button class="nt-btn nt-btn-primary" @click="parse">解析</button>
      <button class="nt-btn" @click="loadSample">示例</button>
      <button class="nt-btn" @click="clear">清空</button>
    </div>

    <div v-if="error" class="nt-msg nt-msg-err">{{ error }}</div>

    <div v-if="parsed" class="nt-kv-grid">
      <div class="nt-kv-card">
        <div class="nt-kv-label">Protocol</div>
        <div class="nt-kv-val">{{ parsed.protocol }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">Hostname</div>
        <div class="nt-kv-val">{{ parsed.hostname }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">Port</div>
        <div class="nt-kv-val">{{ parsed.port || '—' }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">Pathname</div>
        <div class="nt-kv-val">{{ parsed.pathname }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">Search</div>
        <div class="nt-kv-val">{{ parsed.search || '—' }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">Hash</div>
        <div class="nt-kv-val">{{ parsed.hash || '—' }}</div>
      </div>
      <div class="nt-kv-card" v-if="parsed.username || parsed.password">
        <div class="nt-kv-label">User / Pass</div>
        <div class="nt-kv-val">{{ parsed.username }} / {{ parsed.password ? '••••' : '—' }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">Origin</div>
        <div class="nt-kv-val">{{ parsed.origin }}</div>
      </div>
    </div>

    <h3 v-if="parsed && parsed.params.length" style="margin: 14px 0 6px; font-size: 14px; color: #555;">查询参数</h3>
    <table v-if="parsed && parsed.params.length" class="nt-query-table">
      <thead><tr><th style="width: 30%;">Key</th><th>Value</th></tr></thead>
      <tbody>
        <tr v-for="(p, i) in parsed.params" :key="i">
          <td>{{ p.key }}</td>
          <td>{{ p.value }}</td>
        </tr>
      </tbody>
    </table>

    <!-- Encode / Decode -->
    <h3 style="margin: 20px 0 6px; font-size: 14px; color: #555;">URL 编解码</h3>
    <div class="nt-toolbar">
      <input v-model="encInput" class="nt-input nt-input-mono" style="flex: 1; min-width: 240px;" placeholder="原文或已编码字符串" />
      <button class="nt-btn nt-btn-primary" @click="doEncode">编码 →</button>
      <button class="nt-btn" @click="doDecode">← 解码</button>
    </div>
    <div v-if="encErr" class="nt-msg nt-msg-err">{{ encErr }}</div>
    <div v-if="encOutput" class="nt-kv-card">
      <div class="nt-kv-label">结果</div>
      <div class="nt-kv-val">{{ encOutput }}</div>
    </div>

    <!-- Query ↔ JSON -->
    <h3 style="margin: 20px 0 6px; font-size: 14px; color: #555;">QueryString ↔ JSON</h3>
    <div class="nt-toolbar">
      <button class="nt-btn nt-btn-primary" @click="qToJ">QS → JSON</button>
      <button class="nt-btn" @click="jToQ">JSON → QS</button>
    </div>
    <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
      <textarea v-model="qs" class="nt-input nt-input-mono" style="min-height: 120px; padding: 10px;" placeholder="a=1&b=2"></textarea>
      <textarea v-model="qsJson" class="nt-input nt-input-mono" style="min-height: 120px; padding: 10px;" placeholder='{"a":"1","b":"2"}'></textarea>
    </div>
  </div>
</template>

<script>
import { parseUrl, encodeUrl, decodeUrl, qsToJson, jsonToQs, copyMixin } from './utils'

const SAMPLE = 'https://user:pass@www.example.com:8080/path/to/page?name=%E6%9D%A8%E5%85%85&tag=demo&tag=test#section-2'

export default {
  name: 'UrlPanel',
  mixins: [copyMixin],
  data() {
    return {
      url: '', parsed: null, error: '', _t: null,
      encInput: '', encOutput: '', encErr: '',
      qs: '', qsJson: '',
    }
  },
  methods: {
    autoParse() {
      clearTimeout(this._t)
      this._t = setTimeout(() => this.parse(true), 300)
    },
    parse(silent) {
      this.error = ''
      if (!this.url.trim()) { this.parsed = null; return }
      try { this.parsed = parseUrl(this.url) }
      catch (e) { this.parsed = null; if (!silent) this.error = e.message }
    },
    loadSample() { this.url = SAMPLE; this.parse() },
    clear() { this.url = ''; this.parsed = null; this.error = '' },
    doEncode() {
      this.encErr = ''
      try { this.encOutput = encodeUrl(this.encInput) }
      catch (e) { this.encErr = e.message }
    },
    doDecode() {
      this.encErr = ''
      try { this.encOutput = decodeUrl(this.encInput) }
      catch (e) { this.encErr = e.message }
    },
    qToJ() {
      try { this.qsJson = JSON.stringify(qsToJson(this.qs), null, 2) }
      catch (e) { this.qsJson = '' }
    },
    jToQ() {
      try { this.qs = jsonToQs(JSON.parse(this.qsJson)) }
      catch (e) { this.qs = '' }
    },
  },
}
</script>
