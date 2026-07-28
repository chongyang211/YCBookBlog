<template>
  <div class="tm-panel">
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">Cron 表达式</span>
      <input v-model="expr" class="tm-input tm-input-mono" style="flex: 1; min-width: 240px;" placeholder="* * * * *" @input="parse" />
      <button class="tm-btn tm-btn-primary" @click="parse">解析</button>
    </div>

    <div class="tm-toolbar" style="flex-wrap: wrap; gap: 6px;">
      <span class="tm-toolbar-label">预设</span>
      <button v-for="p in presets" :key="p.expr" class="tm-btn" style="font-size: 12px;" @click="usePreset(p.expr)">
        {{ p.label }} <span class="tm-input-mono" style="opacity: 0.6;">{{ p.expr }}</span>
      </button>
    </div>

    <div v-if="error" class="tm-msg tm-msg-err">{{ error }}</div>

    <div v-if="parsed" class="tm-cron-fields">
      <div v-for="(f, i) in fields" :key="i" class="tm-cron-field">
        <div class="tm-cron-field-label">{{ f.label }}</div>
        <div class="tm-cron-field-val">{{ parsed.raw[i] }}</div>
        <div style="font-size: 11px; color: #888; margin-top: 4px;">{{ summary(i) }}</div>
      </div>
    </div>

    <div v-if="parsed" class="tm-toolbar">
      <span class="tm-toolbar-label">未来执行次数</span>
      <input v-model.number="count" type="number" min="1" max="50" class="tm-input" style="width: 80px;" />
      <button class="tm-btn" @click="parse">刷新</button>
    </div>

    <div v-if="nextList.length" class="tm-next-list">
      <div v-for="(t, i) in nextList" :key="i" class="tm-next-item">
        <span><span class="tm-next-item-no">#{{ i + 1 }}</span>{{ t }}</span>
        <span style="color: #888; font-size: 12px;">{{ rels[i] }}</span>
      </div>
    </div>
  </div>
</template>

<script>
import { parseCron, nextCronTimes, formatDate } from './utils'

const PRESETS = [
  { label: '每分钟', expr: '* * * * *' },
  { label: '每小时', expr: '0 * * * *' },
  { label: '每日 0 点', expr: '0 0 * * *' },
  { label: '每日 9:30', expr: '30 9 * * *' },
  { label: '工作日 9 点', expr: '0 9 * * 1-5' },
  { label: '每周日 0 点', expr: '0 0 * * 0' },
  { label: '每月 1 号', expr: '0 0 1 * *' },
  { label: '每 15 分钟', expr: '*/15 * * * *' },
]

function relFuture(d) {
  const ms = d.getTime() - Date.now()
  const abs = Math.abs(ms)
  if (abs < 60000) return Math.floor(abs / 1000) + 's 后'
  if (abs < 3600000) return Math.floor(abs / 60000) + 'm 后'
  if (abs < 86400000) return Math.floor(abs / 3600000) + 'h 后'
  return Math.floor(abs / 86400000) + 'd 后'
}

export default {
  name: 'CronPanel',
  data() {
    return {
      expr: '*/15 * * * *',
      parsed: null,
      nextList: [],
      rels: [],
      error: '',
      count: 5,
      fields: [
        { label: '分 (0-59)' },
        { label: '时 (0-23)' },
        { label: '日 (1-31)' },
        { label: '月 (1-12)' },
        { label: '周 (0-6, 0=日)' },
      ],
      presets: PRESETS,
    }
  },
  mounted() { this.parse() },
  methods: {
    parse() {
      this.error = ''
      try {
        this.parsed = parseCron(this.expr)
        const list = nextCronTimes(this.expr, this.count)
        this.nextList = list.map(d => formatDate(d))
        this.rels = list.map(d => relFuture(d))
      } catch (e) {
        this.parsed = null; this.nextList = []; this.rels = []
        this.error = '解析失败：' + e.message
      }
    },
    usePreset(expr) { this.expr = expr; this.parse() },
    summary(i) {
      if (!this.parsed) return ''
      const arr = [this.parsed.minute, this.parsed.hour, this.parsed.dom, this.parsed.month, this.parsed.dow][i]
      if (!arr) return ''
      if (arr.length > 10) return `共 ${arr.length} 个值`
      return arr.join(',')
    },
  },
}
</script>
