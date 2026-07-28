<template>
  <div class="nt-panel">
    <div class="nt-toolbar">
      <button class="nt-btn nt-btn-primary" @click="useMine">使用当前浏览器</button>
      <button class="nt-btn" @click="loadSample">示例</button>
      <button class="nt-btn" @click="clear">清空</button>
    </div>

    <textarea v-model="ua" class="nt-input nt-input-mono" style="min-height: 100px; padding: 10px;" placeholder="Mozilla/5.0..." @input="parse"></textarea>

    <div v-if="result" class="nt-kv-grid">
      <div class="nt-kv-card">
        <div class="nt-kv-label">浏览器</div>
        <div class="nt-kv-val">{{ result.browser || '—' }} <span style="color: #888;">{{ result.browserVer }}</span></div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">引擎</div>
        <div class="nt-kv-val">{{ result.engine || '—' }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">操作系统</div>
        <div class="nt-kv-val">{{ result.os || '—' }} <span style="color: #888;">{{ result.osVer }}</span></div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">设备类型</div>
        <div class="nt-kv-val">{{ result.device }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import { parseUa } from './utils'

const SAMPLE = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36'

export default {
  name: 'UaPanel',
  data() { return { ua: '', result: null } },
  mounted() { this.useMine() },
  methods: {
    parse() { this.result = this.ua ? parseUa(this.ua) : null },
    useMine() { this.ua = navigator.userAgent || ''; this.parse() },
    loadSample() { this.ua = SAMPLE; this.parse() },
    clear() { this.ua = ''; this.result = null },
  },
}
</script>
