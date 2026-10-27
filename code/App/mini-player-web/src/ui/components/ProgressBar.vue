<!-- 进度条：对应 §04 Step 4.3
     关键坑：拖动时发射 seeking=true，释放时发射 seek(ms)，这段"拖动期间"UI 自管位置，
     不被 timeupdate 事件覆盖——这是四端同款坑（架构篇 §1.5）。 -->
<script setup lang="ts">
import { ref, computed } from 'vue'

const props = defineProps<{
  position: number
  duration: number
  disabled?: boolean
}>()
const emit = defineEmits<{
  (e: 'seek', ms: number): void
  (e: 'seeking', value: boolean): void
}>()

const draftMs = ref(0)
const dragging = ref(false)

const displayMs = computed(() => (dragging.value ? draftMs.value : props.position))
const percent = computed(() => {
  if (!props.duration) return 0
  return Math.min(100, Math.max(0, (displayMs.value / props.duration) * 100))
})

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
</script>

<template>
  <div class="progress" :class="{ disabled }">
    <input
      type="range"
      :min="0"
      :max="duration || 1"
      :value="displayMs"
      :disabled="disabled"
      @input="onInput"
      @change="onChange"
    />
    <div class="track"><div class="fill" :style="{ width: percent + '%' }" /></div>
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
.progress.disabled { opacity: .4; pointer-events: none; }

.track {
  position: absolute;
  left: 0; right: 0;
  height: 4px;
  background: var(--border);
  border-radius: 2px;
  pointer-events: none;
}
.fill {
  height: 100%;
  background: linear-gradient(90deg, var(--accent) 0%, var(--accent-strong) 100%);
  border-radius: 2px;
  transition: width .1s linear;
}

input[type='range'] {
  position: relative;
  z-index: 1;
  background: transparent;
  width: 100%;
  margin: 0;
  height: 14px;
}
input[type='range']::-webkit-slider-runnable-track { background: transparent; }
input[type='range']::-moz-range-track { background: transparent; }
</style>
