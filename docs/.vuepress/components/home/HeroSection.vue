<template>
  <section class="hp-hero">
    <div class="hp-hero-inner">
      <!-- 左侧：标题区 -->
      <div class="hp-hero-left">
        <div class="hp-hero-badge">
          <span class="hp-badge-dot"></span>
          <span class="hp-badge-text">持续更新中</span>
          <span class="hp-badge-divider"></span>
          <span class="hp-badge-icon" aria-hidden="true">
            <svg viewBox="0 0 24 24" width="14" height="14" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M4 19.5A2.5 2.5 0 0 1 6.5 17H20"/>
              <path d="M6.5 2H20v20H6.5A2.5 2.5 0 0 1 4 19.5v-15A2.5 2.5 0 0 1 6.5 2z"/>
            </svg>
          </span>
          <span class="hp-badge-text">已沉淀</span>
          <span class="hp-badge-num">{{ stats.articles }}+</span>
          <span class="hp-badge-text">篇技术文章</span>
        </div>

        <h1 class="hp-hero-title">
          <span class="hp-title-line-1">编程进阶</span>
          <span class="hp-title-line-2">你值得拥有</span>
        </h1>

        <p class="hp-hero-sub">
          用代码丈量技术的深度，从底层原理到工程实战
        </p>

        <p class="hp-hero-desc">
          从底层原理到工程实战 · 用代码丈量技术的深度<br/>
          涵盖 计算机基础 · 编程语言 · 算法 · 框架 · 工具效率 全栈知识体系
        </p>

        <div class="hp-hero-actions">
          <a href="/computer/" class="hp-btn hp-btn-primary">
            <span>开始阅读</span>
            <svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2.5">
              <path d="M5 12h14M13 5l7 7-7 7"/>
            </svg>
          </a>
          <a href="/tools/" class="hp-btn hp-btn-ghost">
            <span>🛠 在线工具箱</span>
          </a>
        </div>
      </div>

      <!-- 右侧：装饰终端 -->
      <div class="hp-hero-right">
        <div class="hp-terminal">
          <div class="hp-terminal-bar">
            <span class="hp-terminal-dot" style="background:#ff5f56"></span>
            <span class="hp-terminal-dot" style="background:#ffbd2e"></span>
            <span class="hp-terminal-dot" style="background:#27c93f"></span>
            <span class="hp-terminal-title">yccoding.com ~ /</span>
          </div>
          <div class="hp-terminal-body">
            <div class="hp-tl"><span class="hp-tl-prompt">$</span> <span class="hp-tl-cmd">whoami</span></div>
            <div class="hp-tl hp-tl-out">→ YC · 一个想把技术写明白的程序员</div>
            <div class="hp-tl"><span class="hp-tl-prompt">$</span> <span class="hp-tl-cmd">cat manifest.json</span></div>
            <div class="hp-tl hp-tl-out">{</div>
            <div class="hp-tl hp-tl-out">&nbsp;&nbsp;"mission": "<span class="hp-tl-str">让复杂的知识可被检索</span>",</div>
            <div class="hp-tl hp-tl-out">&nbsp;&nbsp;"belief": "<span class="hp-tl-str">原理 &gt; 工具 &gt; 调包</span>",</div>
            <div class="hp-tl hp-tl-out">&nbsp;&nbsp;"stack": [<span class="hp-tl-str">"C/C++"</span>, <span class="hp-tl-str">"Java"</span>, <span class="hp-tl-str">"Go"</span>, <span class="hp-tl-str">"Web"</span>]</div>
            <div class="hp-tl hp-tl-out">}</div>
            <div class="hp-tl"><span class="hp-tl-prompt">$</span> <span class="hp-tl-cmd">npm run start</span></div>
          </div>
        </div>
      </div>
    </div>

    <!-- 数据带 -->
    <div class="hp-hero-stats">
      <div class="hp-stat" v-for="(s, i) in statList" :key="i">
        <div class="hp-stat-num">
          <span>{{ s.display }}</span><sup v-if="s.suffix">{{ s.suffix }}</sup>
        </div>
        <div class="hp-stat-label">{{ s.label }}</div>
      </div>
    </div>
  </section>
</template>

<script>
export default {
  name: 'HeroSection',
  props: {
    stats: { type: Object, required: true },
  },
  computed: {
    statList() {
      return [
        { display: this.stats.articles, suffix: '+', label: '技术文章' },
        { display: this.stats.categories, suffix: '', label: '知识分类' },
        { display: this.stats.tools, suffix: '+', label: '在线工具' },
        { display: this.stats.days, suffix: '+', label: '建站天数' },
      ]
    },
  },
}
</script>

<style scoped>
.hp-hero {
  position: relative;
  min-height: 660px;
  padding: 80px clamp(24px, 4vw, 64px) 60px;
  overflow: hidden;
  background: linear-gradient(135deg, #f5f7fb 0%, #eef2f9 50%, #ebe9fc 100%);
}

.hp-hero-inner {
  position: relative;
  max-width: 1200px;
  width: 100%;
  margin: 0 auto;
  display: grid;
  grid-template-columns: minmax(0, 1.05fr) minmax(420px, 520px);
  gap: clamp(40px, 5vw, 80px);
  align-items: center;
}
.hp-hero-left { max-width: 640px; }

.hp-hero-badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 7px 16px 7px 12px;
  background: rgba(255, 255, 255, 0.92);
  border: 1px solid transparent;
  background-image:
    linear-gradient(rgba(255,255,255,0.92), rgba(255,255,255,0.92)),
    linear-gradient(135deg, var(--hp-brand) 0%, var(--hp-brand-2) 100%);
  background-origin: border-box;
  background-clip: padding-box, border-box;
  border-radius: 999px;
  font-size: 13px;
  font-weight: 500;
  color: var(--hp-text-light);
  margin-bottom: 24px;
  box-shadow: 0 4px 16px rgba(74, 144, 217, 0.12);
}
.hp-badge-dot {
  width: 8px; height: 8px;
  background: #22c55e;
  border-radius: 50%;
  box-shadow: 0 0 0 4px rgba(34, 197, 94, 0.15);
  flex-shrink: 0;
}
.hp-badge-text { color: var(--hp-text-light); }
.hp-badge-divider {
  width: 1px;
  height: 12px;
  background: linear-gradient(to bottom, transparent, rgba(74, 144, 217, 0.35), transparent);
  margin: 0 2px;
}
.hp-badge-icon {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  color: var(--hp-brand);
}
.hp-badge-num {
  display: inline-flex;
  align-items: center;
  padding: 2px 8px;
  background: linear-gradient(135deg, var(--hp-brand) 0%, var(--hp-brand-2) 100%);
  color: #fff;
  border-radius: 999px;
  font-weight: 700;
  font-size: 12px;
  letter-spacing: 0.3px;
  box-shadow: 0 2px 8px rgba(74, 144, 217, 0.35);
}

.hp-hero-title {
  font-size: 64px;
  font-weight: 800;
  line-height: 1.05;
  letter-spacing: -2px;
  margin: 0 0 22px;
}
.hp-title-line-1 {
  display: block;
  color: #1a202c;
}
.hp-title-line-2 {
  display: block;
  color: var(--hp-brand-2);
}

.hp-hero-sub {
  font-size: 20px;
  font-weight: 500;
  color: var(--hp-text);
  margin: 0 0 16px;
}

.hp-hero-desc {
  font-size: 15px;
  line-height: 1.85;
  color: var(--hp-text-light);
  margin: 0 0 36px;
}

.hp-hero-actions {
  display: flex;
  gap: 14px;
  flex-wrap: wrap;
}

/* 终端 */
.hp-hero-right {
  position: relative;
  display: flex;
  justify-content: center;
}
.hp-terminal {
  width: 100%;
  max-width: 520px;
  background: #1e1e2e;
  border-radius: 14px;
  overflow: hidden;
  box-shadow:
    0 20px 50px rgba(31, 38, 135, 0.25),
    0 8px 16px rgba(0, 0, 0, 0.08);
  font-family: 'SF Mono', Menlo, Consolas, 'Liberation Mono', monospace;
}
.hp-terminal-bar {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 10px 14px;
  background: #2a2a3a;
  border-bottom: 1px solid #1a1a26;
}
.hp-terminal-dot {
  width: 12px; height: 12px;
  border-radius: 50%;
  display: inline-block;
}
.hp-terminal-title {
  flex: 1;
  text-align: center;
  font-size: 12px;
  color: #888;
  margin-left: -50px;
}
.hp-terminal-body {
  padding: 18px 20px 22px;
  font-size: 13px;
  line-height: 1.85;
  color: #cdd6f4;
}
.hp-tl { white-space: nowrap; }
.hp-tl-prompt { color: #a6e3a1; font-weight: 700; margin-right: 6px; }
.hp-tl-cmd { color: #89b4fa; }
.hp-tl-out { color: #bac2de; opacity: 0.85; }
.hp-tl-str { color: #f9e2af; }

/* 数据带 */
.hp-hero-stats {
  position: relative;
  max-width: 1200px;
  width: 100%;
  margin: 64px auto 0;
  padding: 28px clamp(24px, 3vw, 40px);
  background: rgba(255, 255, 255, 0.82);
  border: 1px solid rgba(255, 255, 255, 0.8);
  border-radius: 20px;
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 24px;
  box-shadow: 0 10px 40px rgba(31, 38, 135, 0.08);
}
.hp-stat { text-align: center; }
.hp-stat-num {
  font-size: 38px;
  font-weight: 800;
  color: var(--hp-brand-2);
  line-height: 1;
  letter-spacing: -1px;
}
.hp-stat-num sup { font-size: 22px; margin-left: 2px; }
.hp-stat-label {
  margin-top: 8px;
  font-size: 13px;
  color: var(--hp-text-light);
  letter-spacing: 0.5px;
}

/* 响应式 */
@media (max-width: 1280px) and (min-width: 961px) {
  .hp-hero-inner { grid-template-columns: minmax(0, 1fr) 440px; gap: 50px; }
  .hp-terminal { max-width: 440px; }
  .hp-hero-title { font-size: 56px; }
}
@media (max-width: 960px) {
  .hp-hero-inner { grid-template-columns: 1fr; gap: 40px; }
  .hp-hero-left { max-width: none; }
  .hp-hero-right { display: none; }
  .hp-hero-title { font-size: 48px; }
}
@media (max-width: 768px) {
  .hp-hero { padding: 60px 18px 40px; min-height: auto; }
  .hp-hero-title { font-size: 40px; letter-spacing: -1px; }
  .hp-hero-sub { font-size: 18px; }
  .hp-hero-stats {
    grid-template-columns: repeat(2, 1fr);
    gap: 18px;
    padding: 20px 18px;
  }
  .hp-stat-num { font-size: 30px; }
}
@media (max-width: 480px) {
  .hp-hero-title { font-size: 34px; }
  .hp-hero-actions { flex-direction: column; align-items: stretch; }
  .hp-hero-actions .hp-btn { justify-content: center; }
  .hp-hero-badge {
    flex-wrap: wrap;
    padding: 6px 12px;
    font-size: 12px;
  }
  .hp-badge-divider, .hp-badge-icon { display: none; }
}

/* 暗色模式 */
.theme--dark .hp-hero {
  background: linear-gradient(135deg, #11111b 0%, #1a1a2e 50%, #1f1a3a 100%);
}
.theme--dark .hp-hero-badge {
  background-image:
    linear-gradient(rgba(30, 30, 46, 0.85), rgba(30, 30, 46, 0.85)),
    linear-gradient(135deg, var(--hp-brand) 0%, var(--hp-brand-2) 100%);
  color: #aaa;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.3);
}
.theme--dark .hp-badge-text { color: #bbb; }
.theme--dark .hp-badge-divider {
  background: linear-gradient(to bottom, transparent, rgba(123, 95, 217, 0.45), transparent);
}
.theme--dark .hp-title-line-1 { color: #f0f0f0; }
.theme--dark .hp-hero-sub { color: #ddd; }
.theme--dark .hp-hero-stats {
  background: rgba(30, 30, 46, 0.85);
  border-color: rgba(123, 95, 217, 0.2);
}
</style>
