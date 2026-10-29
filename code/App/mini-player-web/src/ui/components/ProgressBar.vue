<!-- 进度条：对应 §04 Step 4.3
     关键坑：拖动时发射 seeking=true，释放时发射 seek(ms)，这段"拖动期间"UI 自管位置，
     不被 timeupdate 事件覆盖——这是四端同款坑（架构篇 §1.5）。
     美化：悬浮时放大 + 显示悬浮时间气泡 -->
<script setup lang="ts">
import { ref, computed } from 'vue'
import { formatTime } from '@/utils/format'

const props = defineProps<{
  position: number
  duration: number
  disabled?: boolean
  /** 高变体：PlayingPage 可能需要更粗的进度条 */
  tall?: boolean
}>()
const emit = defineEmits<{
  (e: 'seek', ms: number): void
  (e: 'seeking', value: boolean): void
}>()

const draftMs = ref(0)
const dragging = ref(false)
const hovering = ref(false)
const hoverRatio = ref(0)   // 0-1

const displayMs = computed(() => (dragging.value ? draftMs.value : props.position))
const percent = computed(() => {
  if (!props.duration) return 0
  return Math.min(100, Math.max(0, (displayMs.value / props.duration) * 100))
})
const hoverMs = computed(() => Math.floor(hoverRatio.value * props.duration))

function onInput(e: Event) {
  const val = Number((e.target as HTMLInputElement).value)
  draftMs.value = val
  if (!dragging.value) {
    dragging.value = true
    emit('seeking', true)
  }
}

function onChange(e: Event) {
  const val = Number((e.target as HTMLInputElement).value)
  dragging.value = false
  emit('seek', val)
  emit('seeking', false)
}

function onMouseMove(e: MouseEvent) {
  if (props.disabled) return
  const rect = (e.currentTarget as HTMLElement).getBoundingClientRect()
  hoverRatio.value = Math.min(1, Math.max(0, (e.clientX - rect.left) / rect.width))
}
</script>

<template>
  <div
    class="progress"
    :class="{ disabled, dragging, tall }"
    @mouseenter="hovering = !disabled"
    @mouseleave="hovering = false"
    @mousemove="onMouseMove"
  >
    <input
      type="range"
      :min="0"
      :max="duration || 1"
      :value="displayMs"
      :disabled="disabled"
      @input="onInput"
      @change="onChange"
      aria-label="播放进度"
    />
    <div class="track">
      <div class="fill" :style="{ width: percent + '%' }" />
    </div>
    <!-- 悬浮时间提示 -->
    <div
      v-if="hovering && !dragging && duration > 0"
      class="hover-tip"
      :style="{ left: (hoverRatio * 100) + '%' }"
    >
      {{ formatTime(hoverMs) }}
    </div>
  </div>
</template>

<style scoped>
.progress {
  flex: 1;
  position: relative;
  height: 14px;
  display: flex;
  align-items: center;
}
.progress.tall { height: 18px; }
.progress.disabled { opacity: .4; pointer-events: none; }

.track {
  position: absolute;
  left: 0; right: 0;
  height: 4px;
  background: var(--border);
  border-radius: var(--radius-full);
  pointer-events: none;
  overflow: hidden;
  transition: height .15s;
}
.progress.tall .track { height: 6px; }
.progress:hover .track,
.progress.dragging .track { height: 6px; }

.fill {
  height: 100%;
  background: linear-gradient(90deg, var(--brand-1), var(--brand-2));
  border-radius: var(--radius-full);
  transition: width .1s linear;
}

input[type='range'] {
  position: relative;
  z-index: 2;
  background: transparent;
  width: 100%;
  margin: 0;
  height: 14px;
}
.progress.tall input[type='range'] { height: 18px; }

/* 让 thumb 默认看不见、hover/dragging 时显示 */
input[type='range']::-webkit-slider-thumb {
  width: 14px; height: 14px;
  background: #fff;
  border: 2px solid var(--accent);
  border-radius: 50%;
  margin-top: -5px;
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.25);
  opacity: 0;
  transition: opacity .15s, transform .15s;
}
.progress:hover input[type='range']::-webkit-slider-thumb,
.progress.dragging input[type='range']::-webkit-slider-thumb { opacity: 1; }
input[type='range']:hover::-webkit-slider-thumb { transform: scale(1.1); }

input[type='range']::-moz-range-thumb {
  width: 14px; height: 14px;
  background: #fff;
  border: 2px solid var(--accent);
  border-radius: 50%;
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.25);
}

.hover-tip {
  position: absolute;
  bottom: 100%;
  transform: translateX(-50%);
  margin-bottom: 4px;
  padding: 4px 8px;
  background: var(--bg-elev);
  color: var(--text);
  font-size: var(--fs-xs);
  border-radius: var(--radius-sm);
  border: 1px solid var(--border);
  box-shadow: var(--shadow-sm);
  white-space: nowrap;
  pointer-events: none;
  font-variant-numeric: tabular-nums;
}
</style>
