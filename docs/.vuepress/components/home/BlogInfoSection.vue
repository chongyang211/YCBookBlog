<template>
  <section class="hp-section hp-bloginfo">
    <div class="hp-bi-grid">
      <!-- 博主卡片 -->
      <div class="hp-bi-card hp-bi-about">
        <img class="hp-bi-avatar" :src="$withBase(blogInfo.avatar)" :alt="blogInfo.name" />
        <div class="hp-bi-text">
          <h4 class="hp-bi-name">{{ blogInfo.name }}</h4>
          <p class="hp-bi-slogan">{{ blogInfo.slogan }}</p>
        </div>
        <div class="hp-bi-social">
          <a
            v-for="(s, i) in socialIcons"
            :key="i"
            :href="s.link"
            :title="s.title"
            target="_blank"
            rel="noopener"
            class="hp-bi-si"
          >
            <span v-if="s.iconClass" :class="['iconfont', s.iconClass]"></span>
            <span v-else>{{ s.emoji }}</span>
          </a>
        </div>
      </div>

      <!-- 二维码卡片 -->
      <div class="hp-bi-card hp-bi-qr">
        <p class="hp-bi-qr-tip">创作不易，如果对你有帮助<br/>欢迎请我喝杯咖啡 ☕</p>
        <img class="hp-bi-qr-img" src="/img/pay.webp" alt="赞赏码" />
      </div>

      <!-- 标签云 -->
      <div class="hp-bi-card hp-bi-tags" v-if="tagList.length">
        <h4 class="hp-bi-tag-title">热门标签</h4>
        <div class="hp-bi-tag-wrap">
          <a
            v-for="t in tagList"
            :key="t.name"
            :href="t.path"
            class="hp-bi-tag"
          >{{ t.name }}</a>
        </div>
      </div>
    </div>
  </section>
</template>

<script>
export default {
  name: 'BlogInfoSection',
  props: {
    blogInfo:    { type: Object, required: true }, // { avatar, name, slogan }
    socialIcons: { type: Array,  default: () => [] }, // [{ iconClass, emoji, title, link }]
    tagList:     { type: Array,  default: () => [] }, // [{ name, path }]
  },
}
</script>

<style scoped>
.hp-bloginfo {
  background: #fff;
  padding: 60px 24px 0;
}
.hp-bi-grid {
  max-width: 1200px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  gap: 24px;
}
.hp-bi-card {
  background: var(--hp-bg);
  border: 1px solid var(--hp-border);
  border-radius: 16px;
  padding: 28px 24px;
  display: flex;
  flex-direction: column;
}

/* 博主信息 */
.hp-bi-about {
  align-items: center;
  text-align: center;
}
.hp-bi-avatar {
  width: 72px; height: 72px;
  border-radius: 50%;
  object-fit: cover;
  border: 3px solid var(--hp-brand);
  margin-bottom: 12px;
}
.hp-bi-text { margin-bottom: 14px; }
.hp-bi-name {
  font-size: 18px;
  font-weight: 700;
  margin: 0 0 4px;
  color: var(--hp-text);
}
.hp-bi-slogan {
  font-size: 13px;
  color: var(--hp-text-light);
  margin: 0;
}
.hp-bi-social {
  display: flex;
  gap: 12px;
  justify-content: center;
}
.hp-bi-si {
  width: 36px; height: 36px;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 50%;
  background: #f0f2f8;
  color: var(--hp-text-light);
  text-decoration: none !important;
  font-size: 16px;
  transition: all 0.2s;
}
.hp-bi-si:hover {
  background: var(--hp-brand);
  color: #fff;
  transform: translateY(-2px);
}

/* 二维码 */
.hp-bi-qr { align-items: center; text-align: center; }
.hp-bi-qr-tip {
  font-size: 14px;
  color: var(--hp-text-light);
  line-height: 1.8;
  margin: 0 0 16px;
}
.hp-bi-qr-img {
  width: 160px;
  border-radius: 10px;
}

/* 标签云 */
.hp-bi-tags { justify-content: flex-start; }
.hp-bi-tag-title {
  font-size: 15px;
  font-weight: 700;
  margin: 0 0 14px;
  color: var(--hp-text);
}
.hp-bi-tag-wrap {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}
.hp-bi-tag {
  display: inline-block;
  padding: 5px 14px;
  background: rgba(74, 144, 217, 0.08);
  color: var(--hp-brand);
  border-radius: 999px;
  font-size: 12.5px;
  font-weight: 500;
  text-decoration: none !important;
  transition: all 0.2s;
}
.hp-bi-tag:hover {
  background: var(--hp-brand);
  color: #fff;
}

/* 响应式 */
@media (max-width: 960px) {
  .hp-bi-grid { grid-template-columns: 1fr 1fr; }
}
@media (max-width: 768px) {
  .hp-bi-grid { grid-template-columns: 1fr; }
}

/* 暗色 */
.theme--dark .hp-bloginfo { background: #181825; }
.theme--dark .hp-bi-card { background: #1e1e2e; border-color: #2a2a3a; }
.theme--dark .hp-bi-name { color: #e0e0e0; }
.theme--dark .hp-bi-tag-title { color: #e0e0e0; }
.theme--dark .hp-bi-si { background: #2a2a3a; color: #888; }
.theme--dark .hp-bi-si:hover { background: var(--hp-brand); color: #fff; }
.theme--dark .hp-bi-tag { background: rgba(123, 95, 217, 0.15); }
</style>
