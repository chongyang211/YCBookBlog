<template>
  <section class="hp-section hp-knowledge">
    <div class="hp-section-head">
      <div class="hp-section-eyebrow">KNOWLEDGE MAP</div>
      <h2 class="hp-section-title">完整的知识体系</h2>
      <p class="hp-section-sub">12 大分类 · 覆盖从计算机基础到全栈实战的完整技术地图</p>
    </div>

    <div v-for="group in groups" :key="group.name" class="hp-kgroup">
      <div class="hp-kgroup-head">
        <span class="hp-kgroup-icon">{{ group.icon }}</span>
        <h3 class="hp-kgroup-title">{{ group.name }}</h3>
        <span class="hp-kgroup-line"></span>
        <span class="hp-kgroup-meta">{{ group.cats.length }} 个分类</span>
      </div>

      <div class="hp-kgrid">
        <a
          v-for="cat in group.cats"
          :key="cat.title"
          :href="cat.link"
          class="hp-kcard"
          :style="{ '--c': cat.color, '--c2': cat.color2 }"
        >
          <div class="hp-kcard-bar"></div>
          <div class="hp-kcard-body">
            <div class="hp-kcard-icon">{{ cat.icon }}</div>
            <h4 class="hp-kcard-title">{{ cat.title }}</h4>
            <p class="hp-kcard-desc">{{ cat.desc }}</p>
            <div class="hp-kcard-tags">
              <span v-for="t in cat.tags" :key="t" class="hp-kcard-tag">{{ t }}</span>
            </div>
          </div>
          <div class="hp-kcard-arrow">
            <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2.5">
              <path d="M5 12h14M13 5l7 7-7 7"/>
            </svg>
          </div>
        </a>
      </div>
    </div>
  </section>
</template>

<script>
export default {
  name: 'KnowledgeSection',
  props: {
    groups: { type: Array, required: true },
  },
}
</script>

<style scoped>
.hp-knowledge { background: #fff; }
.hp-kgroup { margin-bottom: 52px; }
.hp-kgroup:last-child { margin-bottom: 0; }

.hp-kgroup-head {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 22px;
}
.hp-kgroup-icon { font-size: 22px; }
.hp-kgroup-title {
  font-size: 20px;
  font-weight: 700;
  margin: 0;
  color: var(--hp-text);
}
.hp-kgroup-line {
  flex: 1;
  height: 1px;
  background: linear-gradient(90deg, var(--hp-border), transparent);
}
.hp-kgroup-meta {
  font-size: 13px;
  color: var(--hp-text-light);
}

.hp-kgrid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 22px;
}

.hp-kcard {
  position: relative;
  display: flex;
  flex-direction: column;
  background: var(--hp-card);
  border: 1px solid var(--hp-border);
  border-radius: 16px;
  overflow: hidden;
  text-decoration: none !important;
  color: inherit !important;
  transition: all 0.3s cubic-bezier(0.16, 1, 0.3, 1);
}
.hp-kcard:hover {
  transform: translateY(-6px);
  border-color: var(--c);
  box-shadow: 0 16px 36px rgba(31, 38, 135, 0.12);
}
.hp-kcard-bar {
  height: 4px;
  background: linear-gradient(90deg, var(--c), var(--c2));
  transition: height 0.3s ease;
}
.hp-kcard:hover .hp-kcard-bar { height: 6px; }

.hp-kcard-body {
  padding: 22px 22px 18px;
  flex: 1;
}
.hp-kcard-icon {
  font-size: 36px;
  margin-bottom: 12px;
  line-height: 1;
  display: inline-block;
  transition: transform 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);
}
.hp-kcard:hover .hp-kcard-icon { transform: scale(1.18) rotate(-6deg); }

.hp-kcard-title {
  font-size: 19px;
  font-weight: 700;
  margin: 0 0 8px;
  color: var(--hp-text);
  letter-spacing: -0.3px;
}
.hp-kcard-desc {
  font-size: 13.5px;
  line-height: 1.7;
  color: var(--hp-text-light);
  margin: 0 0 14px;
  min-height: 46px;
}
.hp-kcard-tags {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}
.hp-kcard-tag {
  font-size: 11.5px;
  padding: 3px 10px;
  background: rgba(74, 144, 217, 0.1);
  color: var(--c);
  border-radius: 999px;
  font-weight: 500;
}
.hp-kcard:hover .hp-kcard-tag {
  background: var(--c);
  color: #fff;
}
.hp-kcard-arrow {
  position: absolute;
  top: 22px;
  right: 18px;
  width: 30px; height: 30px;
  display: flex; align-items: center; justify-content: center;
  background: #f4f6fa;
  color: var(--hp-text-light);
  border-radius: 50%;
  transition: all 0.3s ease;
}
.hp-kcard:hover .hp-kcard-arrow {
  background: var(--c);
  color: #fff;
  transform: translateX(4px);
}

/* 响应式 */
@media (max-width: 960px) {
  .hp-kgrid { grid-template-columns: repeat(2, 1fr); }
}
@media (max-width: 768px) {
  .hp-kgrid { grid-template-columns: 1fr; }
}

/* 暗色 */
.theme--dark .hp-knowledge { background: #181825; }
.theme--dark .hp-kcard {
  background: #1e1e2e;
  border-color: #2a2a3a;
}
.theme--dark .hp-kcard-title { color: #e0e0e0; }
.theme--dark .hp-kcard-arrow {
  background: #2a2a3a;
  color: #888;
}
</style>
