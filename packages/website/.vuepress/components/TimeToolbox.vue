<template>
  <div class="tm-wrapper">
    <div class="tm-header">
      <h2 class="tm-title">时间日期工具箱 · 浏览器端</h2>
      <p class="tm-desc">时间戳 / 世界时钟 / Cron 解析 / 日期计算 / 倒计时，全部本地运算</p>
    </div>

    <div class="tm-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="tm-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span>{{ tab.emoji }}</span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <keep-alive>
      <component :is="activeComponent" />
    </keep-alive>
  </div>
</template>

<script>
import TimestampPanel from './time/TimestampPanel.vue'
import ClockPanel from './time/ClockPanel.vue'
import CronPanel from './time/CronPanel.vue'
import DiffPanel from './time/DiffPanel.vue'
import CountdownPanel from './time/CountdownPanel.vue'
import './time/styles.css'

export default {
  name: 'TimeToolbox',
  components: { TimestampPanel, ClockPanel, CronPanel, DiffPanel, CountdownPanel },
  data() {
    return {
      active: 'ts',
      tabs: [
        { key: 'ts',    label: '时间戳',   comp: 'TimestampPanel', emoji: '🕐' },
        { key: 'clock', label: '世界时钟', comp: 'ClockPanel',     emoji: '🌍' },
        { key: 'cron',  label: 'Cron',     comp: 'CronPanel',      emoji: '⚙️' },
        { key: 'diff',  label: '日期计算', comp: 'DiffPanel',      emoji: '📅' },
        { key: 'cd',    label: '倒计时',   comp: 'CountdownPanel', emoji: '⏳' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'TimestampPanel'
    },
  },
}
</script>
