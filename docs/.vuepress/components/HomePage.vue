<template>
  <div class="hp-wrapper">
    <HeroSection :stats="stats" :typed="typed" />
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
  typePhrases,
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
      // 打字机状态
      typed: '',
      typeIdx: 0,
      typeSubIdx: 0,
      typeDeleting: false,
      typePhrases,
      typeTimer: null,

      // 静态数据（来自 home/data.js）
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
      return icons.map(i => ({
        ...i,
        emoji: socialIconFallback[i.iconClass] || '🔗',
      }))
    },
    tagList() {
      // 尝试从 vdoing 的 $tags 读取，不可用时返回空数组
      try {
        const tags = this.$tags
        if (tags && tags.list && tags.list.length) {
          return tags.list.slice(0, 10).map(t => ({ name: t.name, path: t.path }))
        }
      } catch (e) { /* ignore */ }
      return []
    },
  },
  mounted() {
    this.applyFullWidth()
    this.startTyping()
    this.observeReveal()
  },
  beforeDestroy() {
    if (this.typeTimer) clearTimeout(this.typeTimer)
    // 离开首页：清理所有标记 class，让其他页面恢复主题默认行为
    if (typeof document !== 'undefined') {
      document.body.classList.remove('home-fullwidth')
      document.documentElement.classList.remove('home-fullwidth')
      const tcEl = document.querySelector('.theme-container')
      if (tcEl) tcEl.classList.remove('home-fullwidth-container')
    }
  },
  methods: {
    applyFullWidth() {
      // ================================================================
      // 首页全屏方案（轻量、无副作用）：
      // 仅添加 home-fullwidth* 标记 class，CSS 通过这些 class
      // 命中 .theme-container.home-fullwidth-container 选择器，
      // 用 !important 覆盖主题的 sidebar-open / have-rightmenu padding。
      // 不再用 MutationObserver 摘 Vue 的响应式 class —— 避免污染其他页面。
      // ================================================================
      if (typeof document === 'undefined') return
      document.body.classList.add('home-fullwidth')
      document.documentElement.classList.add('home-fullwidth')
      const tcEl = document.querySelector('.theme-container')
      if (tcEl) tcEl.classList.add('home-fullwidth-container')
    },
    startTyping() {
      const cur = this.typePhrases[this.typeIdx]
      if (this.typeDeleting) {
        this.typed = cur.substring(0, this.typeSubIdx - 1)
        this.typeSubIdx--
      } else {
        this.typed = cur.substring(0, this.typeSubIdx + 1)
        this.typeSubIdx++
      }
      let delay = this.typeDeleting ? 40 : 90
      if (!this.typeDeleting && this.typeSubIdx === cur.length) {
        delay = 1800
        this.typeDeleting = true
      } else if (this.typeDeleting && this.typeSubIdx === 0) {
        this.typeDeleting = false
        this.typeIdx = (this.typeIdx + 1) % this.typePhrases.length
        delay = 400
      }
      this.typeTimer = setTimeout(this.startTyping, delay)
    },
    observeReveal() {
      if (typeof window === 'undefined' || !('IntersectionObserver' in window)) return
      const io = new IntersectionObserver((entries) => {
        entries.forEach(e => {
          if (e.isIntersecting) {
            e.target.classList.add('hp-in')
            io.unobserve(e.target)
          }
        })
      }, { threshold: 0.12 })
      this.$el.querySelectorAll('.hp-section, .hp-kcard, .hp-fcard, .hp-mf-pillar').forEach(el => {
        el.classList.add('hp-reveal')
        io.observe(el)
      })
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
