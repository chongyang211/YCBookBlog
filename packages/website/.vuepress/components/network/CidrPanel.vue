<template>
  <div class="nt-panel">
    <div class="nt-toolbar">
      <span class="nt-toolbar-label">CIDR</span>
      <input v-model="cidr" class="nt-input nt-input-mono" style="width: 240px;" placeholder="192.168.1.0/24" @input="autoParse" />
      <button class="nt-btn nt-btn-primary" @click="parse">计算</button>
      <button class="nt-btn" @click="loadSample">示例</button>
      <button class="nt-btn" @click="clear">清空</button>
    </div>

    <div v-if="error" class="nt-msg nt-msg-err">{{ error }}</div>

    <div v-if="result" class="nt-kv-grid">
      <div class="nt-kv-card">
        <div class="nt-kv-label">网络地址</div>
        <div class="nt-kv-val">{{ result.network }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">广播地址</div>
        <div class="nt-kv-val">{{ result.broadcast }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">子网掩码</div>
        <div class="nt-kv-val">{{ result.mask }} (/{{ result.prefix }})</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">通配符掩码</div>
        <div class="nt-kv-val">{{ result.wildcard }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">起止主机</div>
        <div class="nt-kv-val">{{ result.firstHost }} ~ {{ result.lastHost }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">主机数 / 可用</div>
        <div class="nt-kv-val">{{ result.total }} / {{ result.usable }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">IP 类别</div>
        <div class="nt-kv-val">{{ result.class }} {{ result.isPrivate ? '· 私有 IP' : '· 公网 IP' }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">IP 二进制</div>
        <div class="nt-kv-val" style="font-size: 11.5px;">{{ result.binaryIp }}</div>
      </div>
      <div class="nt-kv-card">
        <div class="nt-kv-label">掩码二进制</div>
        <div class="nt-kv-val" style="font-size: 11.5px;">{{ result.binaryMask }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import { parseCidr } from './utils'

export default {
  name: 'CidrPanel',
  data() { return { cidr: '192.168.1.0/24', result: null, error: '', _t: null } },
  mounted() { this.parse() },
  methods: {
    autoParse() {
      clearTimeout(this._t)
      this._t = setTimeout(() => this.parse(true), 300)
    },
    parse(silent) {
      this.error = ''
      try { this.result = parseCidr(this.cidr) }
      catch (e) { this.result = null; if (!silent) this.error = e.message }
    },
    loadSample() { this.cidr = '10.0.0.0/16'; this.parse() },
    clear() { this.cidr = ''; this.result = null; this.error = '' },
  },
}
</script>
