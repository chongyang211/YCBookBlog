<template>
  <section class="hp-section hp-manifesto">
    <div class="hp-mf-inner">
      <div class="hp-mf-quote">"</div>
      <h2 class="hp-mf-title">为什么做这个网站？</h2>
      <p class="hp-mf-text">
        我相信好的技术内容应该 <strong>免费</strong>、<strong>开放</strong>、<strong>持续迭代</strong>。<br/>
        这里没有快餐式标题党，只有沉下心来打磨的<strong>原理推演</strong>与<strong>实战复盘</strong>。
      </p>

      <div class="hp-mf-pillars">
        <div class="hp-mf-pillar">
          <div class="hp-mf-pi">📖</div>
          <h4>深度优先</h4>
          <p>讲清"为什么"，而不只是"怎么做"</p>
        </div>
        <div class="hp-mf-pillar">
          <div class="hp-mf-pi">🔄</div>
          <h4>持续更新</h4>
          <p>知识会过时，文章会迭代，绝不一锤子买卖</p>
        </div>
        <div class="hp-mf-pillar">
          <div class="hp-mf-pi">🌱</div>
          <h4>开源免费</h4>
          <p>所有内容 MIT 开源，无付费墙，无广告弹窗</p>
        </div>
      </div>

      <div class="hp-mf-links">
        <a href="/pages/d14ddf/" class="hp-mf-link">关于我 →</a>
        <a href="/friends/" class="hp-mf-link">友情链接 →</a>
        <a href="https://github.com/yc-coding" target="_blank" rel="noopener" class="hp-mf-link">GitHub →</a>
      </div>
    </div>
  </section>
</template>

<script>
export default {
  name: 'ManifestoSection',
}
</script>

<style scoped>
.hp-manifesto {
  background: #fff;
  text-align: center;
}
.hp-mf-inner { max-width: 880px; margin: 0 auto; }
.hp-mf-quote {
  font-family: Georgia, serif;
  font-size: 100px;
  line-height: 1;
  color: var(--hp-brand);
  opacity: 0.2;
  margin-bottom: -30px;
}
.hp-mf-title {
  font-size: 38px;
  font-weight: 800;
  margin: 0 0 22px;
  letter-spacing: -1px;
  color: var(--hp-text);
}
.hp-mf-text {
  font-size: 17px;
  line-height: 1.95;
  color: var(--hp-text-light);
  margin: 0 0 50px;
}
.hp-mf-text strong { color: var(--hp-brand-2); }

.hp-mf-pillars {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 22px;
  margin-bottom: 50px;
}
.hp-mf-pillar {
  padding: 28px 22px;
  background: var(--hp-bg);
  border-radius: 16px;
  border: 1px solid var(--hp-border);
  transition: all 0.3s ease;
}
.hp-mf-pillar:hover {
  transform: translateY(-4px);
  border-color: var(--hp-brand);
  box-shadow: 0 12px 28px rgba(74, 144, 217, 0.12);
}
.hp-mf-pi {
  font-size: 36px;
  margin-bottom: 12px;
}
.hp-mf-pillar h4 {
  font-size: 17px;
  font-weight: 700;
  margin: 0 0 8px;
  color: var(--hp-text);
}
.hp-mf-pillar p {
  font-size: 13px;
  line-height: 1.7;
  color: var(--hp-text-light);
  margin: 0;
}

.hp-mf-links {
  display: flex;
  justify-content: center;
  gap: 32px;
  flex-wrap: wrap;
}
.hp-mf-link {
  color: var(--hp-brand-2) !important;
  text-decoration: none !important;
  font-weight: 600;
  font-size: 14.5px;
  transition: color 0.2s;
}
.hp-mf-link:hover { color: var(--hp-brand) !important; }

/* 响应式 */
@media (max-width: 960px) {
  .hp-mf-pillars { grid-template-columns: 1fr; }
}
@media (max-width: 768px) {
  .hp-mf-title { font-size: 28px; }
}

/* 暗色 */
.theme--dark .hp-manifesto { background: #181825; }
.theme--dark .hp-mf-title { color: #e0e0e0; }
.theme--dark .hp-mf-pillar {
  background: #1e1e2e;
  border-color: #2a2a3a;
}
.theme--dark .hp-mf-pillar h4 { color: #e0e0e0; }
</style>
