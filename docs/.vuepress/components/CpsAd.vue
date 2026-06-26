<template>
  <div class="cps-ad" v-if="visible">
    <p class="cps-ad-tag">推荐</p>
    <slot>
      <p class="cps-ad-text">{{ text }}</p>
    </slot>
    <a :href="link" target="_blank" rel="nofollow sponsored" class="cps-ad-btn">
      {{ btnText }}
    </a>
  </div>
</template>

<script>
export default {
  name: 'CpsAd',
  props: {
    text: { type: String, default: '' },
    link: { type: String, required: true },
    btnText: { type: String, default: '了解详情' },
  },
  data() { return { visible: false } },
  mounted() {
    // 延迟渲染，不抢首屏性能
    if (typeof window === 'undefined') return
    if ('IntersectionObserver' in window) {
      const io = new IntersectionObserver(([e]) => {
        if (e.isIntersecting) { this.visible = true; io.disconnect() }
      }, { rootMargin: '200px' })
      io.observe(this.$el)
    } else {
      this.visible = true
    }
  },
}
</script>

<style scoped>
.cps-ad {
  margin: 28px 0;
  padding: 18px 20px;
  background: linear-gradient(135deg, #f0f7ff 0%, #f8f3ff 100%);
  border: 1px solid rgba(74, 144, 217, 0.2);
  border-radius: 12px;
  display: flex;
  align-items: center;
  gap: 14px;
  flex-wrap: wrap;
  min-height: 0; /* 固定容器防止 CLS */;
}
.cps-ad-tag {
  background: linear-gradient(135deg, #4a90d9, #7b5fd9);
  color: #fff;
  font-size: 11px;
  font-weight: 700;
  padding: 3px 10px;
  border-radius: 999px;
  flex-shrink: 0;
  margin: 0;
}
.cps-ad-text {
  flex: 1;
  min-width: 160px;
  font-size: 14px;
  color: #2c3e50;
  margin: 0;
  line-height: 1.6;
}
.cps-ad-btn {
  display: inline-flex;
  align-items: center;
  padding: 8px 20px;
  background: linear-gradient(135deg, #4a90d9, #7b5fd9);
  color: #fff !important;
  border-radius: 8px;
  font-size: 13px;
  font-weight: 600;
  text-decoration: none !important;
  flex-shrink: 0;
  transition: opacity 0.2s;
}
.cps-ad-btn:hover { opacity: 0.88; }

.theme--dark .cps-ad {
  background: linear-gradient(135deg, #1a1a2e 0%, #1f1a3a 100%);
  border-color: rgba(123, 95, 217, 0.3);
}
.theme--dark .cps-ad-text { color: #ddd; }
</style>
