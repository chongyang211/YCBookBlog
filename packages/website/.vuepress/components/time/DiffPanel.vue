<template>
  <div class="tm-panel">
    <!-- 两日期相差 -->
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">起</span>
      <input v-model="a" class="tm-input tm-input-mono" style="width: 200px;" placeholder="2024-01-01" />
      <span class="tm-toolbar-label">止</span>
      <input v-model="b" class="tm-input tm-input-mono" style="width: 200px;" placeholder="2024-12-31" />
      <button class="tm-btn tm-btn-primary" @click="compute">计算相差</button>
      <button class="tm-btn" @click="useToday">今天</button>
    </div>

    <div v-if="diffErr" class="tm-msg tm-msg-err">{{ diffErr }}</div>

    <div v-if="diff" class="tm-result-grid">
      <div class="tm-result-card">
        <div class="tm-result-label">总天数</div>
        <div class="tm-result-val">{{ diff.days }} 天</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">总小时</div>
        <div class="tm-result-val">{{ Math.floor(Math.abs(diff.ms) / 3600000) }} 小时</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">总分钟</div>
        <div class="tm-result-val">{{ Math.floor(Math.abs(diff.ms) / 60000) }} 分</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">工作日</div>
        <div class="tm-result-val">{{ diff.workDays }} 天</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">分解</div>
        <div class="tm-result-val">{{ diff.days }}d {{ diff.hours }}h {{ diff.minutes }}m {{ diff.seconds }}s</div>
      </div>
      <div class="tm-result-card">
        <div class="tm-result-label">方向</div>
        <div class="tm-result-val">{{ diff.sign > 0 ? '起 → 止（正向）' : '起 ← 止（逆向）' }}</div>
      </div>
    </div>

    <!-- 日期加减 -->
    <h3 style="margin: 20px 0 0; font-size: 16px; color: #555;">日期加减</h3>
    <div class="tm-toolbar">
      <span class="tm-toolbar-label">基准</span>
      <input v-model="base" class="tm-input tm-input-mono" style="width: 200px;" placeholder="2024-06-01 12:00:00" />
      <button class="tm-btn" @click="base = nowStr()">现在</button>
    </div>

    <div class="tm-toolbar" style="flex-wrap: wrap;">
      <label class="tm-toolbar-label">年</label>
      <input v-model.number="add.years" type="number" class="tm-input" style="width: 70px;" />
      <label class="tm-toolbar-label">月</label>
      <input v-model.number="add.months" type="number" class="tm-input" style="width: 70px;" />
      <label class="tm-toolbar-label">日</label>
      <input v-model.number="add.days" type="number" class="tm-input" style="width: 70px;" />
      <label class="tm-toolbar-label">时</label>
      <input v-model.number="add.hours" type="number" class="tm-input" style="width: 70px;" />
      <label class="tm-toolbar-label">分</label>
      <input v-model.number="add.minutes" type="number" class="tm-input" style="width: 70px;" />
      <label class="tm-toolbar-label">秒</label>
      <input v-model.number="add.seconds" type="number" class="tm-input" style="width: 70px;" />
      <button class="tm-btn tm-btn-primary" @click="doAdd">计算</button>
    </div>

    <div v-if="addErr" class="tm-msg tm-msg-err">{{ addErr }}</div>

    <div v-if="addResult" class="tm-result-card" style="max-width: 460px;">
      <div class="tm-result-label">结果</div>
      <div class="tm-result-val">{{ addResult }}</div>
    </div>
  </div>
</template>

<script>
import { parseDateInput, diffDates, addDate, formatDate } from './utils'

export default {
  name: 'DiffPanel',
  data() {
    return {
      a: '', b: '', diff: null, diffErr: '',
      base: '',
      add: { years: 0, months: 0, days: 0, hours: 0, minutes: 0, seconds: 0 },
      addResult: '', addErr: '',
    }
  },
  mounted() { this.useToday() },
  methods: {
    nowStr() { return formatDate(new Date()) },
    useToday() { this.a = this.b = formatDate(new Date()).slice(0, 10) },
    compute() {
      this.diffErr = ''
      const da = parseDateInput(this.a)
      const db = parseDateInput(this.b)
      if (!da || !db) { this.diffErr = '日期格式无效'; this.diff = null; return }
      this.diff = diffDates(da, db)
    },
    doAdd() {
      this.addErr = ''
      const d = parseDateInput(this.base)
      if (!d) { this.addErr = '基准日期无效'; this.addResult = ''; return }
      this.addResult = formatDate(addDate(d, this.add))
    },
  },
}
</script>
