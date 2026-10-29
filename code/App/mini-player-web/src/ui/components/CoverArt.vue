<!-- 统一的封面组件：有封面显示图，没封面用渐变占位（基于 id 稳定取色）。
     支持尺寸 size prop 和"旋转动画 spinning"。 -->
<script setup lang="ts">
import { computed } from 'vue'
import { gradientFromId } from '@/utils/colorFromId'

const props = defineProps<{
  src?: string
  id?: string
  /** px 尺寸 */
  size?: number
  /** 是否旋转（正在播放）*/
  spinning?: boolean
  /** 是否圆形 */
  circle?: boolean
  /** 占位 emoji */
  placeholder?: string
}>()

const bgStyle = computed(() => {
  if (props.src) return { backgroundImage: `url(${props.src})`, backgroundSize: 'cover', backgroundPosition: 'center' }
  return { background: gradientFromId(props.id ?? 'default') }
})

const sizeStyle = computed(() => {
  const s = props.size ?? 54
  return { width: `${s}px`, height: `${s}px`, fontSize: `${s * 0.4}px` }
})
</script>

<template>
  <div
    class="cover-art"
    :class="{ circle, spinning: spinning && !!src, 'is-placeholder': !src }"
    :style="{ ...bgStyle, ...sizeStyle }"
  >
    <span v-if="!src" class="ph">{{ placeholder ?? '♪' }}</span>
  </div>
</template>

<style scoped>
.cover-art {
  flex-shrink: 0;
  border-radius: var(--radius);
  overflow: hidden;
  display: flex;
  align-items: center;
  justify-content: center;
  color: rgba(255, 255, 255, 0.92);
  text-shadow: 0 1px 4px rgba(0, 0, 0, 0.25);
  font-weight: 300;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.08);
  transition: box-shadow .3s, transform .3s;
}
.cover-art.circle { border-radius: 50%; }
.cover-art.spinning {
  animation: spin 22s linear infinite;
}
.cover-art.is-placeholder .ph {
  display: block;
  filter: drop-shadow(0 2px 6px rgba(0, 0, 0, 0.2));
}
</style>
