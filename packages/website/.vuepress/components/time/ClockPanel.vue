<template>
  <div class="tm-panel">
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">添加城市</span>
      <select v-model="adding" class="tm-select" style="min-width: 220px;">
        <option value="">— 选择城市 —</option>
        <option v-for="t in available" :key="t.tz" :value="t.tz">{{ t.city }} ({{ t.tz }})</option>
      </select>
      <button class="tm-btn tm-btn-primary" :disabled="!adding" @click="add">添加</button>
      <button class="tm-btn" @click="reset">重置</button>
    </div>

    <div class="tm-clocks">
      <div
        v-for="c in clocks"
        :key="c.tz"
        class="tm-clock-card"
        :class="c.night ? 'tm-clock-card-night' : 'tm-clock-card-day'"
      >
        <button class="tm-clock-remove" @click="remove(c.tz)" title="移除">×</button>
        <div class="tm-clock-city">{{ c.city }}</div>
        <div class="tm-clock-tz">{{ c.tz }} · {{ c.offsetStr }}</div>
        <div class="tm-clock-time">{{ c.hm }}</div>
        <div class="tm-clock-date">{{ c.date }} · {{ c.weekday }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import { COMMON_TIMEZONES, formatDate, getTzOffsetMinutes, isNightInTz } from './utils'

const WEEKDAYS = ['周日', '周一', '周二', '周三', '周四', '周五', '周六']

export default {
  name: 'ClockPanel',
  data() {
    return {
      now: Date.now(),
      adding: '',
      selectedTzs: ['Asia/Shanghai', 'Asia/Tokyo', 'Europe/London', 'America/New_York', 'UTC'],
      _t: null,
    }
  },
  computed: {
    available() {
      return COMMON_TIMEZONES.filter(t => !this.selectedTzs.includes(t.tz))
    },
    clocks() {
      const d = new Date(this.now)
      return this.selectedTzs.map(tz => {
        const meta = COMMON_TIMEZONES.find(t => t.tz === tz) || { city: tz, tz }
        const full = formatDate(d, tz)
        const off = getTzOffsetMinutes(tz, d)
        const sign = off >= 0 ? '+' : '-'
        const h = Math.floor(Math.abs(off) / 60)
        const m = Math.abs(off) % 60
        const offsetStr = `UTC${sign}${h}${m ? ':' + String(m).padStart(2, '0') : ''}`
        const date = full.slice(0, 10)
        const hm = full.slice(11, 19)
        // 获取在该时区下的 weekday
        let weekday = ''
        try {
          const parts = new Intl.DateTimeFormat('en-US', { timeZone: tz, weekday: 'short' }).formatToParts(d)
          const w = parts.find(p => p.type === 'weekday').value
          const map = { Sun: 0, Mon: 1, Tue: 2, Wed: 3, Thu: 4, Fri: 5, Sat: 6 }
          weekday = WEEKDAYS[map[w] || 0]
        } catch (e) { /* noop */ }
        return {
          ...meta, hm, date, offsetStr, weekday,
          night: isNightInTz(tz, d),
        }
      })
    },
  },
  mounted() { this._t = setInterval(() => { this.now = Date.now() }, 1000) },
  beforeDestroy() { clearInterval(this._t) },
  methods: {
    add() {
      if (this.adding && !this.selectedTzs.includes(this.adding)) {
        this.selectedTzs.push(this.adding)
      }
      this.adding = ''
    },
    remove(tz) { this.selectedTzs = this.selectedTzs.filter(x => x !== tz) },
    reset() {
      this.selectedTzs = ['Asia/Shanghai', 'Asia/Tokyo', 'Europe/London', 'America/New_York', 'UTC']
    },
  },
}
</script>
