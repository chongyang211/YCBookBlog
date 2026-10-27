<!-- 10 频段均衡器面板：对应 §09 阶段⑧
     自动播放策略：开关 toggle 时 resume() 必须在用户手势里 -->
<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { equalizer, EQ_FREQUENCIES, EQ_PRESETS, applyAndPersistEq } from '@/service/equalizer'
import { usePlayerStore } from '@/service/playerService'
import { repository } from '@/data/repository'

const player = usePlayerStore()

const enabled = ref(false)
const preset = ref('flat')
const gains = ref<number[]>(new Array(10).fill(0))

onMounted(async () => {
  const s = await repository.loadSettings()
  enabled.value = s.eqEnabled
  preset.value = s.eqPreset
  gains.value = [...s.eqGains]
})

async function toggleEq() {
  // 自动播放策略：必须在"用户手势"里 resume AudioContext
  if (player.audioEl) {
    await equalizer.setup(player.audioEl)
    await equalizer.ensureRunning()
  }
  enabled.value = !enabled.value
  await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: preset.value })
}

async function pickPreset(name: string) {
  preset.value = name
  gains.value = [...(EQ_PRESETS[name] ?? EQ_PRESETS.flat)]
  await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: name })
}

async function onBandChange(index: number, v: number) {
  gains.value[index] = v
  preset.value = 'custom'
  await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: 'custom' })
}

function formatFreq(f: number): string {
  return f >= 1000 ? `${f / 1000}k` : `${f}`
}
</script>

<template>
  <section class="eq">
    <header>
      <h3>均衡器</h3>
      <label class="switch">
        <input type="checkbox" :checked="enabled" @change="toggleEq" />
        <span>{{ enabled ? '已开启' : '已关闭' }}</span>
      </label>
    </header>

    <div class="presets">
      <button
        v-for="name in Object.keys(EQ_PRESETS)"
        :key="name"
        :class="{ active: preset === name }"
        @click="pickPreset(name)"
      >{{ name }}</button>
      <button :class="{ active: preset === 'custom' }" disabled>custom</button>
    </div>

    <div class="bands" :class="{ disabled: !enabled }">
      <div v-for="(f, i) in EQ_FREQUENCIES" :key="f" class="band">
        <span class="gain">{{ gains[i] >= 0 ? '+' : '' }}{{ gains[i] }}dB</span>
        <input
          class="vslider"
          type="range"
          min="-12" max="12" step="1" :value="gains[i]"
          @input="onBandChange(i, Number(($event.target as HTMLInputElement).value))"
          :disabled="!enabled"
        />
        <span class="freq">{{ formatFreq(f) }}</span>
      </div>
    </div>
  </section>
</template>

<style scoped>
.eq {
  background: var(--bg-elev);
  padding: 20px;
  border-radius: var(--radius);
  box-shadow: var(--shadow-sm);
}
header {
  display: flex; justify-content: space-between; align-items: center;
  margin-bottom: 14px;
}
h3 { margin: 0; }
.switch { display: flex; align-items: center; gap: 8px; font-size: 13px; cursor: pointer; }

.presets {
  display: flex; gap: 6px; flex-wrap: wrap;
  margin-bottom: 16px;
}
.presets button {
  padding: 4px 10px;
  border-radius: 999px;
  background: var(--hover);
  color: var(--text-sub);
  font-size: 12px;
}
.presets button.active { background: var(--accent); color: #fff; }

.bands {
  display: flex; justify-content: space-between; align-items: stretch;
  gap: 4px;
  padding: 8px 0;
}
.bands.disabled { opacity: .4; pointer-events: none; }
.band {
  display: flex; flex-direction: column; align-items: center; gap: 6px;
  min-width: 32px; flex: 1;
}
.gain {
  font-size: 10px;
  color: var(--text-mute);
  font-variant-numeric: tabular-nums;
  min-height: 14px;
}
.freq {
  font-size: 11px;
  color: var(--text-mute);
}
.vslider {
  writing-mode: vertical-lr;
  direction: rtl;
  -webkit-appearance: slider-vertical;
  width: 20px;
  height: 110px;
}
</style>
