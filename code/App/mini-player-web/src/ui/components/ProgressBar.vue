<!-- 进度条 v2：对应 §04 Step 4.3 + §8.5.8
     关键坑：拖动时发射 seeking=true，释放时发射 seek(ms)，这段"拖动期间"UI 自管位置，
     不被 timeupdate 事件覆盖——这是四端同款坑（架构篇 §1.5）。

     ── v0.2.1 架构升级：彻底抛弃 <input type="range"> ──
     旧方案（input + 自绘 .track 叠层）翻车现场：
       全局 style.css 给 input 的 ::-webkit-slider-runnable-track 设了不透明灰背景，
       而 input 层 z-index:2 恰好盖在自绘 fill 之上 → 用户只看到灰线，看不到进度。
     新方案（纯 div + Pointer Events）：
       1. 视觉层（rail/fill/thumb/tip）100% 由自己的 DOM 控制，与全局 input 样式解耦；
       2. setPointerCapture 保证拖出窗口仍能收到 move/up；
       3. role="slider" + keydown 保留键盘无障碍（←→ ±5%，Home/End 首尾）；
       4. touch-action: none 防触摸拖动时页面滚动。 -->
<script setup lang="ts">
import { ref, computed } from 'vue'
import { formatTime } from '@/utils/format'

const props = defineProps<{
  position: number
  duration: number
  disabled?: boolean
  /** 高变体：PlayingPage 用更粗的进度条 */
  tall?: boolean
}>()
const emit = defineEmits<{
  (e: 'seek', ms: number): void
  (e: 'seeking', value: boolean): void
}>()

const rootEl = ref<HTMLElement | null>(null)
const dragging = ref(false)
const hovering = ref(false)
const dragMs = ref(0)
const hoverRatio = ref(0)   // 0-1

/** 拖动期间显示 draft，否则显示引擎的 position */
const displayMs = computed(() => (dragging.value ? dragMs.value : props.position))
const percent = computed(() => {
  if (!props.duration) return 0
  return Math.min(100, Math.max(0, (displayMs.value / props.duration) * 100))
})
const hoverMs = computed(() => Math.floor(hoverRatio.value * props.duration))

/** 气泡防溢出：两端 clamp 到 6%-94%，避免半截气泡 */
const tipLeft = computed(() => `${Math.min(94, Math.max(6, hoverRatio.value * 100))}%`)

function ratioFromEvent(e: PointerEvent): number {
  const el = rootEl.value
  if (!el) return 0
  const rect = el.getBoundingClientRect()
  return Math.min(1, Math.max(0, (e.clientX - rect.left) / rect.width))
}

function onPointerDown(e: PointerEvent) {
  if (props.disabled || props.duration <= 0) return
  dragging.value = true
  dragMs.value = ratioFromEvent(e) * props.duration
  emit('seeking', true)
  // 关键：指针捕获——拖到窗口外也能持续收到 move/up
  ;(e.currentTarget as HTMLElement).setPointerCapture(e.pointerId)
}

function onPointerMove(e: PointerEvent) {
  if (props.disabled) return
  const r = ratioFromEvent(e)
  hoverRatio.value = r
  if (dragging.value) dragMs.value = r * props.duration
}

function onPointerUp(e: PointerEvent) {
  if (!dragging.value) return
  const ms = ratioFromEvent(e) * props.duration
  dragging.value = false
  emit('seek', ms)
  emit('seeking', false)
}

/** 键盘无障碍：role="slider" 的标准交互 */
function onKeydown(e: KeyboardEvent) {
  if (props.disabled || props.duration <= 0) return
  const step = props.duration * 0.05
  switch (e.key) {
    case 'ArrowLeft':  e.preventDefault(); emit('seek', Math.max(0, props.position - step)); break
    case 'ArrowRight': e.preventDefault(); emit('seek', Math.min(props.duration, props.position + step)); break
    case 'Home': e.preventDefault(); emit('seek', 0); break
    case 'End':  e.preventDefault(); emit('seek', props.duration); break
  }
}
</script>

<template>
  <div
    ref="rootEl"
    class="pbar"
    :class="{ disabled, dragging, hovering, tall }"
    role="slider"
    aria-label="播放进度"
    :aria-valuemin="0"
    :aria-valuemax="Math.round(duration)"
    :aria-valuenow="Math.round(displayMs)"
    :aria-disabled="disabled"
    :tabindex="disabled ? -1 : 0"
    @pointerdown="onPointerDown"
    @pointermove="onPointerMove"
    @pointerup="onPointerUp"
    @pointercancel="onPointerUp"
    @mouseenter="hovering = true"
    @mouseleave="hovering = false"
    @keydown="onKeydown"
  >
    <!-- 底轨 + 已播填充（thumb 独立放外层，避免被 rail 的 overflow:hidden 裁剪） -->
    <div class="rail">
      <div class="fill" :style="{ width: percent + '%' }" />
    </div>

    <!-- 圆形 thumb：跟随 percent，独立层 -->
    <div class="thumb" :style="{ left: percent + '%' }" />

    <!-- 悬浮时间气泡 -->
    <div
      v-if="hovering && !dragging && duration > 0"
      class="tip"
      :style="{ left: tipLeft }"
    >
      {{ formatTime(hoverMs) }}
    </div>
  </div>
</template>

<style scoped>
.pbar {
  flex: 1;
  position: relative;
  height: 16px;
  display: flex;
  align-items: center;
  cursor: pointer;
  /* 防触摸拖动时页面跟着滚 */
  touch-action: none;
  -webkit-user-select: none;
  user-select: none;
  outline: none;
}
.pbar.tall { height: 22px; }
.pbar.disabled { opacity: .35; cursor: not-allowed; pointer-events: none; }

/* ── 底轨 ── */
.rail {
  position: absolute;
  left: 0; right: 0;
  top: 50%;
  transform: translateY(-50%);
  height: 4px;
  background: var(--border);
  border-radius: var(--radius-full);
  overflow: hidden;              /* 裁 fill 的圆角，thumb 已在外层不受影响 */
  transition: height .18s cubic-bezier(.2,.8,.3,1);
}
.pbar.tall .rail { height: 6px; }
.pbar:hover .rail,
.pbar.dragging .rail { height: 8px; }
.pbar.tall:hover .rail,
.pbar.tall.dragging .rail { height: 10px; }

/* ── 已播填充：品牌渐变 + 顶部高光 ── */
.fill {
  position: relative;
  height: 100%;
  background: linear-gradient(90deg, var(--brand-1), var(--brand-2));
  border-radius: var(--radius-full);
  /* 播放中 width 随 rAF 每 16ms 更新；拖动时关掉过渡防"拖泥带水" */
  transition: width .12s linear;
}
.pbar.dragging .fill { transition: none; }
/* 顶部 1px 高光：让 fill 有"液体"质感 */
.fill::before {
  content: '';
  position: absolute;
  inset: 0 0 auto 0;
  height: 1px;
  background: rgba(255, 255, 255, 0.35);
  border-radius: var(--radius-full) var(--radius-full) 0 0;
}

/* ── thumb：独立层，默认隐藏，hover/dragging/键盘聚焦浮现 ── */
.thumb {
  position: absolute;
  top: 50%;
  left: 0;
  transform: translate(-50%, -50%) scale(.4);
  width: 14px;
  height: 14px;
  background: #fff;
  border: 2.5px solid var(--brand-2);
  border-radius: 50%;
  box-shadow:
    0 2px 6px rgba(0, 0, 0, 0.25),
    0 0 0 0 rgba(139, 92, 246, 0.35);
  opacity: 0;
  transition:
    opacity .16s ease,
    transform .18s cubic-bezier(.34, 1.56, .64, 1),   /* 带一点回弹 */
    left .12s linear;
  pointer-events: none;          /* 不挡 pointer 事件，拖动逻辑全在 .pbar */
}
.pbar.dragging .thumb { transition: opacity .16s ease; }
.pbar:hover .thumb,
.pbar.dragging .thumb,
.pbar:focus-visible .thumb { opacity: 1; }
.pbar:hover .thumb { transform: translate(-50%, -50%) scale(1); }
.pbar.dragging .thumb {
  transform: translate(-50%, -50%) scale(1.25);
  box-shadow:
    0 3px 10px rgba(0, 0, 0, 0.3),
    0 0 0 6px rgba(139, 92, 246, 0.22);   /* 拖动光环 */
}

/* ── 悬浮时间气泡 ── */
.tip {
  position: absolute;
  bottom: 100%;
  transform: translateX(-50%);
  margin-bottom: 6px;
  padding: 4px 9px;
  background: var(--bg-elev);
  color: var(--text);
  font-size: var(--fs-xs);
  font-weight: 500;
  border-radius: var(--radius-sm);
  border: 1px solid var(--border);
  box-shadow: var(--shadow-md);
  white-space: nowrap;
  pointer-events: none;
  font-variant-numeric: tabular-nums;
  animation: tip-in .15s ease;
}
@keyframes tip-in {
  from { opacity: 0; transform: translateX(-50%) translateY(2px); }
  to   { opacity: 1; transform: translateX(-50%) translateY(0); }
}
</style>
