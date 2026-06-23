<template>
  <section class="hp-section hp-featured">
    <div class="hp-section-head">
      <div class="hp-section-eyebrow">FEATURED POSTS</div>
      <h2 class="hp-section-title">编辑精选</h2>
      <p class="hp-section-sub">从千篇文章中精挑细选 · 真正值得读的硬核内容</p>
    </div>

    <div class="hp-fgrid">
      <a
        v-for="post in featured"
        :key="post.link"
        :href="post.link"
        class="hp-fcard"
        :style="{ '--c': post.color }"
      >
        <div class="hp-fcard-cover">
          <span class="hp-fcard-emoji">{{ post.emoji }}</span>
          <span class="hp-fcard-tag">{{ post.tag }}</span>
        </div>
        <div class="hp-fcard-body">
          <h4 class="hp-fcard-title">{{ post.title }}</h4>
          <p class="hp-fcard-desc">{{ post.desc }}</p>
          <div class="hp-fcard-meta">
            <span class="hp-fcard-author">✍️ {{ post.author }}</span>
            <span class="hp-fcard-date">{{ post.date }}</span>
          </div>
        </div>
      </a>
    </div>
  </section>
</template>

<script>
export default {
  name: 'FeaturedSection',
  props: {
    featured: { type: Array, required: true },
  },
}
</script>

<style scoped>
.hp-featured { background: var(--hp-bg); }
.hp-fgrid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 24px;
}
.hp-fcard {
  display: flex;
  flex-direction: column;
  background: #fff;
  border-radius: 16px;
  overflow: hidden;
  text-decoration: none !important;
  color: inherit !important;
  box-shadow: 0 2px 8px rgba(31, 38, 135, 0.04);
  transition: all 0.35s cubic-bezier(0.16, 1, 0.3, 1);
}
.hp-fcard:hover {
  transform: translateY(-8px);
  box-shadow: 0 20px 40px rgba(31, 38, 135, 0.15);
}
.hp-fcard-cover {
  position: relative;
  height: 160px;
  background: linear-gradient(135deg, var(--c), rgba(255,255,255,0.4));
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: hidden;
}
.hp-fcard-cover::after {
  content: '';
  position: absolute; inset: 0;
  background-image: radial-gradient(circle at 30% 40%, rgba(255,255,255,0.3), transparent 50%);
}
.hp-fcard-emoji {
  font-size: 64px;
  filter: drop-shadow(0 4px 12px rgba(0, 0, 0, 0.15));
  position: relative;
  z-index: 1;
  transition: transform 0.4s ease;
}
.hp-fcard:hover .hp-fcard-emoji {
  transform: scale(1.15) rotate(-6deg);
}
.hp-fcard-tag {
  position: absolute;
  top: 14px; left: 14px;
  padding: 4px 12px;
  background: rgba(255, 255, 255, 0.95);
  color: var(--c);
  border-radius: 999px;
  font-size: 11.5px;
  font-weight: 700;
  letter-spacing: 0.5px;
  z-index: 1;
}
.hp-fcard-body { padding: 22px 22px 20px; flex: 1; display: flex; flex-direction: column; }
.hp-fcard-title {
  font-size: 18px;
  font-weight: 700;
  margin: 0 0 10px;
  color: var(--hp-text);
  line-height: 1.4;
}
.hp-fcard-desc {
  font-size: 13.5px;
  line-height: 1.75;
  color: var(--hp-text-light);
  margin: 0 0 18px;
  flex: 1;
}
.hp-fcard-meta {
  display: flex;
  justify-content: space-between;
  font-size: 12.5px;
  color: var(--hp-text-light);
  padding-top: 14px;
  border-top: 1px solid var(--hp-border);
}

/* 响应式 */
@media (max-width: 960px) {
  .hp-fgrid { grid-template-columns: repeat(2, 1fr); }
}
@media (max-width: 768px) {
  .hp-fgrid { grid-template-columns: 1fr; }
}

/* 暗色 */
.theme--dark .hp-featured { background: #11111b; }
.theme--dark .hp-fcard {
  background: #1e1e2e;
  border: 1px solid #2a2a3a;
}
.theme--dark .hp-fcard-title { color: #e0e0e0; }
.theme--dark .hp-fcard-meta { border-top-color: #2a2a3a; }
</style>
