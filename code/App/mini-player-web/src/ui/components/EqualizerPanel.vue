<!-- 10 频段均衡器面板：对应 §09 阶段⑧
     自动播放策略：开关 toggle 时 resume() 必须在用户手势里 -->
<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { equalizer, EQ_FREQUENCIES, EQ_PRESETS, applyAndPersistEq } from '@/service/equalizer'
import { usePlayerStore } from '@/service/playerService'
import { repository } from '@/data/repository'
import { toast } from '@/service/toast'

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
  try {
    if (player.audioEl) {
      await equalizer.setup(player.audioEl)
      await equalizer.ensureRunning()
    }
    enabled.value = !enabled.value
    await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: preset.value })
    toast.success(enabled.value ? '均衡器已启用' : '均衡器已关闭', 1200)
  } catch (e) {
    console.error(e)
    toast.error('均衡器启用失败')
  }
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

async function reset() {
  gains.value = new Array(10).fill(0)
  preset.value = 'flat'
  await applyAndPersistEq({ enabled: enabled.value, gains: gains.value, preset: 'flat' })
}

function formatFreq(f: number): string {
  return f >= 1000 ? `${f / 1000}k` : `${f}`
}
</script>

<template>
  <section class="eq card">
    <header>
      <div class="title-row">
        <h3>均衡器</h3>
        <span class="preset-tag">{{ preset }}</span>
      </div>
      <label class="switch" :class="{ on: enabled }">
        <input type="checkbox" :checked="enabled" @change="toggleEq" />
        <span class="track"><span class="thumb" /></span>
        <span class="label">{{ enabled ? '已启用' : '已关闭' }}</span>
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
      <button class="reset" @click="reset" title="重置为 flat">↺</button>
    </div>

    <div class="bands" :class="{ disabled: !enabled }">
      <div v-for="(f, i) in EQ_FREQUENCIES" :key="f" class="band">
        <span class="gain" :class="{ positive: gains[i] > 0, negative: gains[i] < 0 }">
          {{ gains[i] > 0 ? '+' : '' }}{{ gains[i] }}
        </span>
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
  border-radius: var(--radius-lg);
  border: 1px solid var(--border);
  box-shadow: var(--shadow-sm);
}
header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 16px;
}
.title-row { display: flex; align-items: center; gap: 10px; }
h3 { margin: 0; }
.preset-tag {
  padding: 2px 8px;
  background: var(--accent-soft);
  color: var(--accent);
  font-size: 10px;
  border-radius: var(--radius-full);
  font-weight: 600;
  text-transform: uppercase;
}

/* 自定义开关 */
.switch {
  display: inline-flex;
  align-items: center;
  gap: 10px;
  cursor: pointer;
  font-size: var(--fs-sm);
}
.switch input { display: none; }
.switch .track {
  position: relative;
  width: 40px; height: 22px;
  background: var(--border);
  border-radius: var(--radius-full);
  transition: background .2s;
}
.switch .thumb {
  position: absolute;
  top: 2px; left: 2px;
  width: 18px; height: 18px;
  background: #fff;
  border-radius: 50%;
  transition: left .22s cubic-bezier(.2,.8,.3,1);
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);
}
.switch.on .track { background: linear-gradient(135deg, var(--brand-1), var(--brand-2)); }
.switch.on .thumb { left: 20px; }
.switch .label { color: var(--text-sub); }

.presets {
  display: flex;
  gap: 6px;
  flex-wrap: wrap;
  margin-bottom: 20px;
  align-items: center;
}
.presets button {
  padding: 6px 14px;
  border-radius: var(--radius-full);
  background: var(--bg);
  border: 1px solid var(--border);
  color: var(--text-sub);
  font-size: var(--fs-sm);
  text-transform: capitalize;
  transition: all .15s;
}
.presets button:hover:not(:disabled):not(.active) {
  background: var(--hover);
  color: var(--text);
}
.presets button.active {
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  border-color: transparent;
}
.presets button:disabled { opacity: .4; cursor: default; }
.presets .reset {
  width: 28px; height: 28px;
  padding: 0;
  margin-left: auto;
  display: inline-flex;
  align-items: center;
  justify-content: center;
}

.bands {
  display: flex;
  justify-content: space-between;
  align-items: stretch;
  gap: 6px;
  padding: 10px 0;
}
.bands.disabled { opacity: .4; pointer-events: none; }

.band {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 10px;
  min-width: 32px;
  flex: 1;
}
.gain {
  font-size: var(--fs-xs);
  color: var(--text-mute);
  font-variant-numeric: tabular-nums;
  font-weight: 600;
  min-height: 16px;
  padding: 2px 6px;
  border-radius: var(--radius-sm);
}
.gain.positive { color: var(--success); background: rgba(34, 197, 94, 0.1); }
.gain.negative { color: var(--info); background: rgba(59, 130, 246, 0.1); }

.freq {
  font-size: var(--fs-xs);
  color: var(--text-mute);
  font-weight: 500;
}
.vslider {
  writing-mode: vertical-lr;
  direction: rtl;
  -webkit-appearance: slider-vertical;
  appearance: slider-vertical;
  width: 24px;
  height: 130px;
}
</style>
