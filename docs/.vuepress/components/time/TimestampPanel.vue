<template>
  <div class="tm-panel">
    <div class="tm-hero-clock">
      <div class="tm-hero-clock-time">{{ nowStr }}</div>
      <div class="tm-hero-clock-ts">
        当前时间戳：
        <span class="tm-input-mono">{{ Math.floor(now / 1000) }}</span> 秒 ·
        <span class="tm-input-mono">{{ now }}</span> 毫秒
        <button class="tm-btn" style="margin-left: 8px; padding: 2px 10px; font-size: 12px;" @click="tmCopy(String(Math.floor(now / 1000)))">复制秒</button>
        <button class="tm-btn" style="padding: 2px 10px; font-size: 12px;" @click="tmCopy(String(now))">复制毫秒</button>
      </div>
    </div>

    <!-- 时间戳 → 日期 -->
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">时间戳</span>
      <input v-model="ts" class="tm-input tm-input-mono" style="width: 180px;" placeholder="1717000000 / 1717000000000" />
      <span class="tm-toolbar-label">时区</span>
      <select v-model="tz" class="tm-select" style="width: 180px;">
        <option value="">本地</option>
        <option v-for="t in tzs" :key="t.tz" :value="t.tz">{{ t.city }} ({{ t.tz }})</option>
      </select>
      <button class="tm-btn ct-btn-primary tm-btn-primary" @click="parseTs">解析</button>
      <button class="tm-btn" @click="ts = Math.floor(Date.now() / 1000)">填入当前</button>
    </div>

    <div v-if="tsErr" class="tm-msg tm-msg-err">{{ tsErr }}</div>

    <div v-if="parsed" class="tm-result-grid">
      <div class="tm-result-card">
        <div class="tm-result-label">本地时间</div>
        <div class="tm-result-val">{{ parsed.local }}</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">UTC</div>
        <div class="tm-result-val">{{ parsed.utc }}</div>
      </div>
      <div class="tm-result-card" v-if="parsed.tz">
        <div class="tm-result-label">{{ tz }}</div>
        <div class="tm-result-val">{{ parsed.tz }}</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">ISO 8601</div>
        <div class="tm-result-val">{{ parsed.iso }}</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">相对时间</div>
        <div class="tm-result-val">{{ parsed.rel }}</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">星期</div>
        <div class="tm-result-val">{{ parsed.weekday }}</div>
      </div>
    </div>

    <!-- 日期 → 时间戳 -->
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">日期</span>
      <input v-model="dateStr" class="tm-input tm-input-mono" style="width: 220px;" placeholder="2024-06-01 12:00:00" />
      <button class="tm-btn tm-btn-primary" @click="parseDate">→ 转时间戳</button>
    </div>

    <div v-if="dateErr" class="tm-msg tm-msg-err">{{ dateErr }}</div>

    <div v-if="dateResult" class="tm-result-grid">
      <div class="tm-result-card">
        <div class="tm-result-label">秒</div>
        <div class="tm-result-val">{{ dateResult.sec }}</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">毫秒</div>
        <div class="tm-result-val">{{ dateResult.ms }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import { formatDate, formatIso, parseTimestamp, parseDateInput, COMMON_TIMEZONES, copyMixin } from './utils'

const WEEKDAYS = ['周日', '周一', '周二', '周三', '周四', '周五', '周六']

function relTime(ms) {
  const sign = ms < 0 ? '前' : '后'
  const abs = Math.abs(ms)
  if (abs < 60000) return Math.floor(abs / 1000) + ' 秒' + sign
  if (abs < 3600000) return Math.floor(abs / 60000) + ' 分钟' + sign
  if (abs < 86400000) return Math.floor(abs / 3600000) + ' 小时' + sign
  if (abs < 86400000 * 30) return Math.floor(abs / 86400000) + ' 天' + sign
  if (abs < 86400000 * 365) return Math.floor(abs / 86400000 / 30) + ' 个月' + sign
  return Math.floor(abs / 86400000 / 365) + ' 年' + sign
}

export default {
  name: 'TimestampPanel',
  mixins: [copyMixin],
  data() {
    return {
      now: Date.now(),
      ts: '', tz: '',
      tzs: COMMON_TIMEZONES,
      parsed: null, tsErr: '',
      dateStr: '', dateResult: null, dateErr: '',
      _t: null,
    }
  },
  computed: {
    nowStr() { return formatDate(new Date(this.now)) },
  },
  mounted() { this._t = setInterval(() => { this.now = Date.now() }, 1000) },
  beforeDestroy() { clearInterval(this._t) },
  methods: {
    parseTs() {
      this.tsErr = ''
      const d = parseTimestamp(this.ts)
      if (!d) { this.tsErr = '时间戳格式无效'; this.parsed = null; return }
      this.parsed = {
        local: formatDate(d),
        utc: formatDate(d, 'UTC'),
        tz: this.tz ? formatDate(d, this.tz) : '',
        iso: formatIso(d),
        rel: relTime(d.getTime() - Date.now()),
        weekday: WEEKDAYS[d.getDay()],
      }
    },
    parseDate() {
      this.dateErr = ''
      const d = parseDateInput(this.dateStr)
      if (!d) { this.dateErr = '日期格式无效'; this.dateResult = null; return }
      this.dateResult = { sec: Math.floor(d.getTime() / 1000), ms: d.getTime() }
    },
  },
}
</script>
