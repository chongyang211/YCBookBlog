<template>
  <div class="tm-panel">
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">目标</span>
      <input v-model="target" class="tm-input tm-input-mono" style="min-width: 240px;" placeholder="2025-12-31 23:59:59" />
      <span class="tm-toolbar-label">标题</span>
      <input v-model="title" class="tm-input" style="width: 200px;" placeholder="新年倒计时" />
      <button class="tm-btn tm-btn-primary" @click="set">设置</button>
    </div>

    <div class="tm-toolbar" style="flex-wrap: wrap;">
      <span class="tm-toolbar-label">预设</span>
      <button v-for="p in presets" :key="p.label" class="tm-btn" style="font-size: 12px;" @click="usePreset(p)">{{ p.label }}</button>
    </div>

    <div v-if="error" class="tm-msg tm-msg-err">{{ error }}</div>

    <div v-if="parsed">
      <h3 style="text-align: center; margin: 0; font-size: 18px; color: #16a085;">距 {{ displayTitle }}</h3>
      <p style="text-align: center; color: #888; font-size: 13px; margin: 4px 0 12px;">
        目标：{{ parsedFmt }}
      </p>
      <div class="tm-cd-grid">
        <div>
          <div class="tm-cd-num">{{ cd.days }}</div>
          <div class="tm-cd-label">DAYS</div>
        </div>
        <div>
          <div class="tm-cd-num">{{ pad2(cd.hours) }}</div>
          <div class="tm-cd-label">HOURS</div>
        </div>
        <div>
          <div class="tm-cd-num">{{ pad2(cd.minutes) }}</div>
          <div class="tm-cd-label">MINUTES</div>
        </div>
        <div>
          <div class="tm-cd-num">{{ pad2(cd.seconds) }}</div>
          <div class="tm-cd-label">SECONDS</div>
        </div>
      </div>
      <p v-if="cd.finished" style="text-align: center; color: #c0392b; font-size: 16px; font-weight: 700; margin-top: 12px;">
        ⏰ 时间已到！
      </p>
    </div>
  </div>
</template>

<script>
import { parseDateInput, countdownTo, formatDate } from './utils'

function newYear() {
  const d = new Date(new Date().getFullYear() + 1, 0, 1)
  return { label: '🎊 新年', date: formatDate(d) }
}
function laborDay() {
  const y = new Date().getFullYear()
  let d = new Date(y, 4, 1)
  if (d < new Date()) d = new Date(y + 1, 4, 1)
  return { label: '🚩 五一', date: formatDate(d) }
}
function nationalDay() {
  const y = new Date().getFullYear()
  let d = new Date(y, 9, 1)
  if (d < new Date()) d = new Date(y + 1, 9, 1)
  return { label: '🇨🇳 国庆', date: formatDate(d) }
}

export default {
  name: 'CountdownPanel',
  data() {
    return {
      target: '', title: '', parsed: null, error: '',
      cd: { days: 0, hours: 0, minutes: 0, seconds: 0, finished: false },
      _t: null,
      presets: [newYear(), laborDay(), nationalDay()],
    }
  },
  computed: {
    parsedFmt() { return this.parsed ? formatDate(this.parsed) : '' },
    displayTitle() { return this.title || '目标时间' },
  },
  mounted() {
    this.target = this.presets[0].date
    this.title = '新年'
    this.set()
    this._t = setInterval(() => this.tick(), 1000)
  },
  beforeDestroy() { clearInterval(this._t) },
  methods: {
    set() {
      this.error = ''
      const d = parseDateInput(this.target)
      if (!d) { this.error = '目标时间格式无效'; this.parsed = null; return }
      this.parsed = d
      this.tick()
    },
    tick() {
      if (!this.parsed) return
      this.cd = countdownTo(this.parsed)
    },
    usePreset(p) { this.target = p.date; this.title = p.label.replace(/[^\u4e00-\u9fa5\w]/g, ''); this.set() },
    pad2(n) { return String(n).padStart(2, '0') },
  },
}
</script>
