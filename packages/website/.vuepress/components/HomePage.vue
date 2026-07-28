<template>
  <div class="hp-wrapper">
    <HeroSection :stats="stats" />
    <KnowledgeSection :groups="groups" />
    <ToolsSection :tool-cloud="toolCloud" />
    <FeaturedSection :featured="featured" />
    <BlogInfoSection
      :blog-info="blogInfo"
      :social-icons="socialIcons"
      :tag-list="tagList"
    />
    <ManifestoSection />
  </div>
</template>

<script>
import HeroSection from './home/HeroSection.vue'
import KnowledgeSection from './home/KnowledgeSection.vue'
import ToolsSection from './home/ToolsSection.vue'
import FeaturedSection from './home/FeaturedSection.vue'
import BlogInfoSection from './home/BlogInfoSection.vue'
import ManifestoSection from './home/ManifestoSection.vue'
import './home/styles.css'
import {
  stats,
  toolCloud,
  groups,
  featured,
  socialIconFallback,
} from './home/data.js'

export default {
  name: 'HomePage',
  components: {
    HeroSection,
    KnowledgeSection,
    ToolsSection,
    FeaturedSection,
    BlogInfoSection,
    ManifestoSection,
  },
  data() {
    return {
      stats,
      toolCloud,
      groups,
      featured,
    }
  },
  computed: {
    blogInfo() {
      const cfg = (this.$themeConfig && this.$themeConfig.blogger) || {}
      return {
        avatar: cfg.avatar || '/img/logo.png',
        name: cfg.name || 'YC',
        slogan: cfg.slogan || '专注编程 · 终身学习者',
      }
    },
    socialIcons() {
      const cfg = (this.$themeConfig && this.$themeConfig.social) || {}
      const icons = cfg.icons || []
      return icons.map(i => ({ ...i, emoji: socialIconFallback[i.iconClass] || '🔗' }))
    },
    tagList() {
      try {
        const tags = this.$tags
        if (tags && tags.list && tags.list.length)
          return tags.list.slice(0, 10).map(t => ({ name: t.name, path: t.path }))
      } catch (e) { /* ignore */ }
      return []
    },
  },
  mounted() {
    this.applyFullWidth()
  },
  beforeDestroy() {
    if (typeof document !== 'undefined') {
      document.body.classList.remove('home-fullwidth')
      document.documentElement.classList.remove('home-fullwidth')
      const tcEl = document.querySelector('.theme-container')
      if (tcEl) tcEl.classList.remove('home-fullwidth-container')
    }
  },
  methods: {
    applyFullWidth() {
      if (typeof document === 'undefined') return
      document.body.classList.add('home-fullwidth')
      document.documentElement.classList.add('home-fullwidth')
      const tcEl = document.querySelector('.theme-container')
      if (tcEl) tcEl.classList.add('home-fullwidth-container')
    },
  },
}
</script>

<!--
  全屏击穿样式已迁移至 docs/.vuepress/styles/index.styl
  由 mounted -> applyFullWidth() 在 <html>/<body>/.theme-container
  上注入 home-fullwidth* class 后生效；离开首页时 beforeDestroy
  会清理 class，规则自然失效，不影响其他页面。

  迁移原因：index.styl 是 vuepress 主题级样式入口，注入位置在
  vdoing 主题样式之后，能可靠覆盖主题里
    .theme-container.sidebar-open .page { padding-left: ($sidebarWidth + .8rem) }
  这条没有 !important 的规则，避免首页"被侧栏推向左侧"的问题。
-->
