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
import HeroSection from '../home/HeroSection.vue'
import KnowledgeSection from '../home/KnowledgeSection.vue'
import ToolsSection from '../home/ToolsSection.vue'
import FeaturedSection from '../home/FeaturedSection.vue'
import BlogInfoSection from '../home/BlogInfoSection.vue'
import ManifestoSection from '../home/ManifestoSection.vue'
import '../home/styles.css'
import {
  typePhrases,
  stats,
  toolCloud,
  groups,
  featured,
  socialIconFallback,
} from '../home/data.js'

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

<!-- ============================================================
 * 全屏击穿样式（非 scoped）
 *
 * 由 mounted -> applyFullWidth() 在 <body> / <html> / .theme-container
 * 上注入 home-fullwidth* class 后生效。
 * 仅作用于首页；离开首页时 beforeDestroy 会清理 class，
 * 这些规则自然失效，不会影响其他页面布局。
 * ============================================================ -->
<style>
.theme-container.home-fullwidth-container {
  max-width: none !important;
  width: 100% !important;
}
/* 清掉所有可能注入的 padding，让 .page 100% 撑满 */
.theme-container.home-fullwidth-container .page,
.theme-container.home-fullwidth-container.sidebar-open .page,
.theme-container.home-fullwidth-container.have-rightmenu .page,
.theme-container.home-fullwidth-container.no-sidebar .page {
  padding-left: 0 !important;
  padding-right: 0 !important;
  padding-bottom: 0 !important;
  padding-top: 3.6rem !important;
  max-width: none !important;
  width: 100% !important;
  margin: 0 !important;
}
/* vdoing-wrapper：清掉限宽和内边距，但务必保留 margin: 0 auto（居中关键） */
.theme-container.home-fullwidth-container .page > .theme-vdoing-wrapper {
  max-width: none !important;
  width: 100% !important;
  margin: 0 auto !important;
  padding: 0 !important;
  background: transparent !important;
  box-shadow: none !important;
  border-radius: 0 !important;
}
/* 屏蔽无关元素 */
.theme-container.home-fullwidth-container .theme-vdoing-wrapper > .placeholder,
.theme-container.home-fullwidth-container .theme-vdoing-wrapper > .content-wrapper,
.theme-container.home-fullwidth-container .theme-vdoing-wrapper > .page-edit,
.theme-container.home-fullwidth-container .theme-vdoing-wrapper > .page-nav,
.theme-container.home-fullwidth-container .page > .update-bar,
.theme-container.home-fullwidth-container .right-menu-wrapper,
.theme-container.home-fullwidth-container .sidebar,
.theme-container.home-fullwidth-container .sidebar-mask,
.theme-container.home-fullwidth-container .sidebar-button,
.theme-container.home-fullwidth-container .sidebar-hover-trigger {
  display: none !important;
}
/* footer 铺满 */
.theme-container.home-fullwidth-container .footer,
.theme-container.home-fullwidth-container.have-rightmenu .footer,
.theme-container.home-fullwidth-container.no-sidebar .footer {
  max-width: none !important;
  width: 100% !important;
  margin: 0 !important;
  padding-left: 0 !important;
  padding-right: 0 !important;
  border-radius: 0 !important;
}
/* 移动端导航栏高度兼容 */
@media (max-width: 719px) {
  .theme-container.home-fullwidth-container .page { padding-top: 3.6rem !important; }
}
</style>
