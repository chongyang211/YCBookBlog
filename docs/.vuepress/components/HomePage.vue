<template>
  <div class="hp-wrapper">
    <!-- ==================== 屏 1：英雄区 ==================== -->
    <section class="hp-hero">
      <div class="hp-hero-bg">
        <span class="hp-blob hp-blob-1"></span>
        <span class="hp-blob hp-blob-2"></span>
        <span class="hp-blob hp-blob-3"></span>
        <span class="hp-grid-bg"></span>
      </div>

      <div class="hp-hero-inner">
        <!-- 左侧：标题区 -->
        <div class="hp-hero-left">
          <div class="hp-hero-badge">
            <span class="hp-badge-dot"></span>
            持续更新 · 已沉淀 {{ stats.articles }}+ 篇技术文章
          </div>

          <h1 class="hp-hero-title">
            <span class="hp-title-line-1">编程进阶</span>
            <span class="hp-title-line-2">你值得拥有</span>
          </h1>

          <p class="hp-hero-sub">
            <span class="hp-typewriter">{{ typed }}</span>
            <span class="hp-cursor">|</span>
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
              <div class="hp-tl"><span class="hp-tl-prompt">$</span> <span class="hp-tl-cmd">npm run start</span> <span class="hp-cursor hp-cursor-term">▊</span></div>
            </div>
          </div>
          <!-- 浮动小标签 -->
          <span class="hp-float-tag hp-tag-1">{ JSON }</span>
          <span class="hp-float-tag hp-tag-2">&lt;/&gt;</span>
          <span class="hp-float-tag hp-tag-3">⚡ async</span>
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

    <!-- ==================== 屏 2：知识体系 ==================== -->
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

    <!-- ==================== 屏 3：工具箱速达 ==================== -->
    <section class="hp-section hp-tools">
      <div class="hp-tools-inner">
        <div class="hp-tools-left">
          <div class="hp-section-eyebrow hp-eyebrow-light">DEVELOPER TOOLS</div>
          <h2 class="hp-tools-title">浏览器端在线工具箱</h2>
          <p class="hp-tools-sub">
            18+ 个高频开发工具，无需注册，无需下载，<br/>
            所有数据本地处理，永不上传。
          </p>

          <div class="hp-tools-features">
            <div class="hp-tf">
              <div class="hp-tf-icon">🔒</div>
              <div class="hp-tf-text">
                <strong>零数据上传</strong>
                <span>全部浏览器端 JS 运算，隐私无忧</span>
              </div>
            </div>
            <div class="hp-tf">
              <div class="hp-tf-icon">⚡</div>
              <div class="hp-tf-text">
                <strong>开箱即用</strong>
                <span>无需注册账号，无需安装客户端</span>
              </div>
            </div>
            <div class="hp-tf">
              <div class="hp-tf-icon">🎯</div>
              <div class="hp-tf-text">
                <strong>聚焦高频场景</strong>
                <span>JSON · 文本 · 图片 · 加解密 · 二维码 ...</span>
              </div>
            </div>
          </div>

          <a href="/tools/" class="hp-btn hp-btn-primary hp-btn-lg">
            进入工具箱
            <svg viewBox="0 0 24 24" width="20" height="20" fill="none" stroke="currentColor" stroke-width="2.5">
              <path d="M5 12h14M13 5l7 7-7 7"/>
            </svg>
          </a>
        </div>

        <div class="hp-tools-right">
          <div class="hp-tools-cloud">
            <div
              v-for="(t, i) in toolCloud"
              :key="t.name"
              class="hp-tcloud"
              :class="'hp-tcloud-' + (i % 3 + 1)"
              :style="{ '--d': (i * 0.15) + 's' }"
            >
              <span class="hp-tcloud-icon">{{ t.icon }}</span>
              <span class="hp-tcloud-name">{{ t.name }}</span>
            </div>
          </div>
        </div>
      </div>
    </section>

    <!-- ==================== 屏 4：精选推荐 ==================== -->
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

    <!-- ==================== 屏 4.5：博主信息 + 二维码（原右侧栏内容） ==================== -->
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

        <!-- 标签云（从 themeConfig 读取） -->
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

    <!-- ==================== 屏 5：站点哲学 ==================== -->
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
  </div>
</template>

<script>
export default {
  name: 'HomePage',
  data() {
    return {
      // 打字机
      typed: '',
      typeIdx: 0,
      typeSubIdx: 0,
      typeDeleting: false,
      typePhrases: [
        '原理 > 工具 > 调包',
        '把复杂讲简单，把简单做透彻',
        '每一行代码都有它的成本',
        'Read the f**king source code',
      ],
      typeTimer: null,

      // 统计数据
      stats: {
        articles: 1000,
        categories: 12,
        tools: 18,
        days: 1800,
      },

      // 工具云
      toolCloud: [
        { icon: '📋', name: 'JSON' },
        { icon: '📝', name: '文本' },
        { icon: '🖼', name: '图片' },
        { icon: '🔐', name: '加解密' },
        { icon: '🔲', name: '二维码' },
        { icon: '⏰', name: 'Cron' },
        { icon: '🎨', name: '颜色' },
        { icon: '🔢', name: '进制' },
        { icon: '🌐', name: 'URL' },
        { icon: '#️⃣', name: 'Hash' },
        { icon: '📅', name: '日期' },
        { icon: '🔍', name: '正则' },
      ],

      // 12 分类（按 4 大分组）
      groups: [
        {
          name: '基础理论',
          icon: '🎯',
          cats: [
            { title: '计算机', desc: '组成原理、网络协议、CPU 体系结构', icon: '💻', color: '#2962ff', color2: '#448aff', link: '/computer/', tags: ['操作系统', '网络', '体系'] },
            { title: '算法', desc: '数据结构与算法，从线性表到动态规划', icon: '🧮', color: '#00897b', color2: '#26a69a', link: '/algorithm/', tags: ['LeetCode', 'DP', '图论'] },
            { title: '编程', desc: '面向对象、设计原则、23 种设计模式', icon: '⚙️', color: '#7b5fd9', color2: '#9575cd', link: '/programming/', tags: ['OOP', '设计模式', 'SOLID'] },
          ],
        },
        {
          name: '编程语言',
          icon: '🔤',
          cats: [
            { title: 'C/C++', desc: 'C 语言与 C++ 从入门到精通', icon: '🔧', color: '#5d4037', color2: '#795548', link: '/c-cpp/', tags: ['指针', 'STL', 'Modern C++'] },
            { title: 'Java', desc: 'JVM、并发编程、集合源码深度解析', icon: '☕', color: '#ef6c00', color2: '#ff8a00', link: '/java/', tags: ['JVM', '并发', 'Spring'] },
            { title: 'Go', desc: 'Goroutine 调度、GC、内存模型', icon: '🐹', color: '#0277bd', color2: '#039be5', link: '/go/', tags: ['Goroutine', 'GC', 'Channel'] },
          ],
        },
        {
          name: '实战开发',
          icon: '🚀',
          cats: [
            { title: 'Web', desc: 'HTML、JavaScript、TypeScript、Vue', icon: '🌐', color: '#43a047', color2: '#66bb6a', link: '/web-dev/', tags: ['Vue', 'TS', 'Node'] },
            { title: 'App', desc: 'Android 进阶与 iOS 开发实战', icon: '📱', color: '#8e24aa', color2: '#ab47bc', link: '/app/', tags: ['Android', 'iOS', 'Flutter'] },
            { title: 'Linux', desc: 'QML、QT、Linux 应用开发', icon: '🐧', color: '#fbc02d', color2: '#fdd835', link: '/linux/', tags: ['QT', 'Shell', '内核'] },
          ],
        },
        {
          name: '效率 & 沉淀',
          icon: '✨',
          cats: [
            { title: '技术', desc: '技术文档、GitHub 技巧、Nodejs、博客搭建', icon: '🛠', color: '#546e7a', color2: '#78909c', link: '/technology/', tags: ['Git', 'Docker', '运维'] },
            { title: '专栏', desc: '性能优化、编程原理、方案设计思想', icon: '📖', color: '#e91e63', color2: '#ec407a', link: '/column/', tags: ['性能', '架构', '原理'] },
            { title: '更多', desc: '学习、面试、心情杂货、实用技巧', icon: '📦', color: '#ad1457', color2: '#c2185b', link: '/more/', tags: ['面试', '随笔', '友链'] },
          ],
        },
      ],

      // 精选推荐
      featured: [
        {
          emoji: '🎨',
          tag: '深度长文',
          color: '#7b5fd9',
          title: '自定义 View 绘制实践',
          desc: '从 onMeasure 到 onDraw，把 Android 自定义 View 的完整生命周期讲透。',
          author: 'YC',
          date: '2024',
          link: '/pages/2c1d49/',
        },
        {
          emoji: '⚡',
          tag: '原理剖析',
          color: '#ef6c00',
          title: 'JVM 内存模型与 GC 算法',
          desc: '从对象头到三色标记，搞懂 JVM 是如何管理你那几个 G 的堆内存的。',
          author: 'YC',
          date: '2024',
          link: '/java/',
        },
        {
          emoji: '🛠',
          tag: '实用工具',
          color: '#00897b',
          title: '在线工具箱发布',
          desc: 'JSON 格式化、文本处理、图片转换、加解密……18+ 工具，全部浏览器端运行。',
          author: 'YC',
          date: '2024',
          link: '/tools/',
        },
      ],
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
      // 给 iconfont 图标补上 emoji 降级
      const map = {
        'icon-github': '🐙',
        'icon-youjian': '📧',
        'icon-erji': '🎵',
      }
      return icons.map(i => ({ ...i, emoji: map[i.iconClass] || '🔗' }))
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
    statList() {
      return [
        { display: this.stats.articles, suffix: '+', label: '技术文章' },
        { display: this.stats.categories, suffix: '', label: '知识分类' },
        { display: this.stats.tools, suffix: '+', label: '在线工具' },
        { display: this.stats.days, suffix: '+', label: '建站天数' },
      ]
    },
  },
  mounted() {
    this.breakWrapper()
    this.startTyping()
    this.observeReveal()
  },
  beforeDestroy() {
    if (this.typeTimer) clearTimeout(this.typeTimer)
  },
  methods: {
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
    breakWrapper() {
      // 直接操作 style 属性挣脱 max-width/padding（JS 比 CSS class 更可靠，不受 cascade 影响）
      const wrapper = this.$el.parentElement
      if (wrapper) {
        const s = wrapper.style
        s.maxWidth = 'none'
        s.padding = '0'
        s.margin = '0'
        s.background = 'transparent'
        s.boxShadow = 'none'
        s.borderRadius = '0'
        s.marginBottom = '0'
      }
      if (wrapper && wrapper.parentElement) {
        const page = wrapper.parentElement
        page.style.paddingTop = '3.6rem'
        page.style.paddingBottom = '0'
        // 隐藏占位元素和页脚多余间距
        const placeholder = wrapper.querySelector(':scope > .placeholder')
        if (placeholder) placeholder.style.display = 'none'
        const pageNav = wrapper.querySelector(':scope > .page-nav')
        if (pageNav) pageNav.style.display = 'none'
        const pageEdit = wrapper.querySelector(':scope > .page-edit')
        if (pageEdit) pageEdit.style.display = 'none'
      }
    },
  },
}
</script>

<!-- 非 scoped：用于"挣脱"主题 Page.vue 的 wrapper 限制，让首页全屏 -->
<style>
/* ===== 首页全屏：通过 JS 注入 class 后精准覆盖（兼容所有浏览器） ===== */
.theme-vdoing-wrapper.hp-fullwidth {
  max-width: none !important;
  padding: 0 !important;
  margin: 0 !important;
  background: transparent !important;
  box-shadow: none !important;
  border-radius: 0 !important;
  margin-bottom: 0 !important;
}
/* 首页 page 容器也去除约束 */
.page.hp-fullwidth-page {
  padding-top: 3.6rem !important;
  padding-bottom: 0 !important;
}
/* 隐藏 Page.vue 的占位、编辑、翻页等无关元素 */
.theme-vdoing-wrapper.hp-fullwidth > .placeholder,
.theme-vdoing-wrapper.hp-fullwidth > .page-edit,
.theme-vdoing-wrapper.hp-fullwidth > .page-nav,
.theme-vdoing-wrapper.hp-fullwidth > .content-wrapper {
  display: none !important;
}
/* footer 贴齐 */
.page.hp-fullwidth-page + .footer,
.page.hp-fullwidth-page .footer {
  margin-top: 0 !important;
}

/* ===== 以下为 :has() 降级方案（现代浏览器直接匹配，无需等 JS） ===== */
.page > .theme-vdoing-wrapper:has(.hp-wrapper),
.page > .theme-vdoing-wrapper.has-homepage {
  max-width: none !important;
  padding: 0 !important;
  margin: 0 !important;
  background: transparent !important;
  box-shadow: none !important;
  border-radius: 0 !important;
}
.theme-vdoing-wrapper:has(> .hp-wrapper),
.theme-vdoing-wrapper:has(> .theme-vdoing-content.hp-wrapper) {
  max-width: none !important;
  padding: 0 !important;
  background: transparent !important;
  box-shadow: none !important;
}
.theme-vdoing-wrapper:has(.hp-wrapper) > .placeholder,
.theme-vdoing-wrapper:has(.hp-wrapper) > .page-edit,
.theme-vdoing-wrapper:has(.hp-wrapper) > .page-nav,
.theme-vdoing-wrapper:has(.hp-wrapper) > .content-wrapper {
  display: none !important;
}
.page:has(.hp-wrapper) {
  padding-top: 3.6rem !important;
  padding-bottom: 0 !important;
}
.page:has(.hp-wrapper) + .footer,
.page:has(.hp-wrapper) .footer { margin-top: 0 !important; }
</style>

<style scoped>
/* ============== 基础变量 ============== */
.hp-wrapper {
  --hp-brand: #4a90d9;
  --hp-brand-2: #7b5fd9;
  --hp-brand-3: #11a8cd;
  --hp-text: #2c3e50;
  --hp-text-light: #6a737d;
  --hp-bg: #fafbfc;
  --hp-card: #ffffff;
  --hp-border: #e8ecf1;
  margin: 0;
  padding: 0;
  color: var(--hp-text);
  background: var(--hp-bg);
  overflow-x: hidden;
}
.hp-wrapper *,
.hp-wrapper *::before,
.hp-wrapper *::after { box-sizing: border-box; }

/* ============== Reveal Animation ============== */
.hp-reveal {
  opacity: 0;
  transform: translateY(28px);
  transition: opacity 0.7s ease, transform 0.7s cubic-bezier(0.16, 1, 0.3, 1);
}
.hp-reveal.hp-in {
  opacity: 1;
  transform: translateY(0);
}

/* ==================== 屏 1：英雄区 ==================== */
.hp-hero {
  position: relative;
  min-height: 660px;
  padding: 80px 24px 60px;
  overflow: hidden;
  background: linear-gradient(135deg, #f5f7fb 0%, #eef2f9 50%, #ebe9fc 100%);
}
.hp-hero-bg {
  position: absolute; inset: 0;
  pointer-events: none;
  overflow: hidden;
}
.hp-blob {
  position: absolute;
  border-radius: 50%;
  filter: blur(60px);
  opacity: 0.5;
  animation: hpFloat 14s ease-in-out infinite;
}
.hp-blob-1 { width: 420px; height: 420px; top: -100px; left: -80px;  background: #93c5fd; }
.hp-blob-2 { width: 360px; height: 360px; top: 100px;  right: -60px; background: #c4b5fd; animation-delay: -4s; }
.hp-blob-3 { width: 320px; height: 320px; bottom: -80px; left: 40%;  background: #a5f3fc; animation-delay: -8s; }
.hp-grid-bg {
  position: absolute; inset: 0;
  background-image:
    linear-gradient(rgba(74, 144, 217, 0.06) 1px, transparent 1px),
    linear-gradient(90deg, rgba(74, 144, 217, 0.06) 1px, transparent 1px);
  background-size: 48px 48px;
  mask-image: radial-gradient(ellipse at center, black 30%, transparent 75%);
  -webkit-mask-image: radial-gradient(ellipse at center, black 30%, transparent 75%);
}
@keyframes hpFloat {
  0%, 100% { transform: translate(0, 0) scale(1); }
  33% { transform: translate(30px, -20px) scale(1.05); }
  66% { transform: translate(-20px, 30px) scale(0.95); }
}

.hp-hero-inner {
  position: relative;
  max-width: 1180px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: 1.1fr 1fr;
  gap: 60px;
  align-items: center;
}

/* 左侧 */
.hp-hero-badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 6px 14px;
  background: rgba(255, 255, 255, 0.7);
  backdrop-filter: blur(8px);
  border: 1px solid rgba(74, 144, 217, 0.18);
  border-radius: 999px;
  font-size: 13px;
  color: var(--hp-text-light);
  margin-bottom: 24px;
}
.hp-badge-dot {
  width: 8px; height: 8px;
  background: #22c55e;
  border-radius: 50%;
  box-shadow: 0 0 0 4px rgba(34, 197, 94, 0.2);
  animation: hpPulse 2s ease-in-out infinite;
}
@keyframes hpPulse {
  0%, 100% { box-shadow: 0 0 0 4px rgba(34, 197, 94, 0.2); }
  50% { box-shadow: 0 0 0 8px rgba(34, 197, 94, 0.05); }
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
  background: linear-gradient(135deg, #1a202c 0%, #4a5568 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
.hp-title-line-2 {
  display: block;
  background: linear-gradient(135deg, var(--hp-brand) 0%, var(--hp-brand-2) 60%, #ec4899 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.hp-hero-sub {
  font-size: 22px;
  font-weight: 500;
  color: var(--hp-text);
  margin: 0 0 16px;
  min-height: 32px;
  display: flex;
  align-items: center;
}
.hp-typewriter { color: var(--hp-brand-2); }
.hp-cursor {
  display: inline-block;
  color: var(--hp-brand);
  animation: hpBlink 1s steps(1) infinite;
  margin-left: 2px;
  font-weight: 300;
}
@keyframes hpBlink { 50% { opacity: 0; } }

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
.hp-btn {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 13px 26px;
  border-radius: 12px;
  font-size: 15px;
  font-weight: 600;
  text-decoration: none !important;
  border: none;
  cursor: pointer;
  transition: all 0.25s ease;
  letter-spacing: 0.2px;
}
.hp-btn-primary {
  background: linear-gradient(135deg, var(--hp-brand) 0%, var(--hp-brand-2) 100%);
  color: #fff !important;
  box-shadow: 0 6px 18px rgba(74, 144, 217, 0.35);
}
.hp-btn-primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 10px 24px rgba(74, 144, 217, 0.45);
}
.hp-btn-ghost {
  background: rgba(255, 255, 255, 0.8);
  color: var(--hp-text) !important;
  border: 1px solid var(--hp-border);
  backdrop-filter: blur(6px);
}
.hp-btn-ghost:hover {
  background: #fff;
  border-color: var(--hp-brand);
  color: var(--hp-brand) !important;
  transform: translateY(-2px);
}
.hp-btn-lg {
  padding: 16px 32px;
  font-size: 16px;
  border-radius: 14px;
}

/* 右侧终端 */
.hp-hero-right {
  position: relative;
  display: flex;
  justify-content: center;
}
.hp-terminal {
  width: 100%;
  max-width: 480px;
  background: #1e1e2e;
  border-radius: 14px;
  overflow: hidden;
  box-shadow:
    0 20px 50px rgba(31, 38, 135, 0.25),
    0 8px 16px rgba(0, 0, 0, 0.08);
  font-family: 'SF Mono', Menlo, Consolas, 'Liberation Mono', monospace;
  transform: rotate(-1deg);
  transition: transform 0.4s ease;
}
.hp-terminal:hover { transform: rotate(0deg) scale(1.02); }

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
.hp-cursor-term {
  color: #cdd6f4;
  animation: hpBlink 1s steps(1) infinite;
  margin-left: 4px;
}

/* 浮动小标签 */
.hp-float-tag {
  position: absolute;
  padding: 8px 14px;
  background: #fff;
  border-radius: 10px;
  font-size: 13px;
  font-weight: 600;
  font-family: 'SF Mono', Menlo, Consolas, monospace;
  box-shadow: 0 8px 20px rgba(31, 38, 135, 0.15);
  animation: hpFloatTag 4s ease-in-out infinite;
}
.hp-tag-1 { top: 10%; left: -20px; color: var(--hp-brand-2); animation-delay: 0s; }
.hp-tag-2 { top: 50%; right: -30px; color: #ef6c00; animation-delay: -1.3s; }
.hp-tag-3 { bottom: 8%; left: 8%;  color: #00897b; animation-delay: -2.6s; }
@keyframes hpFloatTag {
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(-12px); }
}

/* 数据带 */
.hp-hero-stats {
  position: relative;
  max-width: 1180px;
  margin: 64px auto 0;
  padding: 28px 24px;
  background: rgba(255, 255, 255, 0.7);
  backdrop-filter: blur(12px);
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
  background: linear-gradient(135deg, var(--hp-brand) 0%, var(--hp-brand-2) 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
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

/* ==================== 通用 Section 头 ==================== */
.hp-section {
  padding: 90px 24px;
  max-width: 1180px;
  margin: 0 auto;
}
.hp-section-head {
  text-align: center;
  margin-bottom: 56px;
}
.hp-section-eyebrow {
  display: inline-block;
  font-size: 12px;
  font-weight: 700;
  letter-spacing: 3px;
  color: var(--hp-brand-2);
  background: rgba(123, 95, 217, 0.1);
  padding: 5px 14px;
  border-radius: 999px;
  margin-bottom: 16px;
}
.hp-eyebrow-light {
  color: #fff;
  background: rgba(255, 255, 255, 0.18);
}
.hp-section-title {
  font-size: 42px;
  font-weight: 800;
  margin: 0 0 14px;
  letter-spacing: -1px;
  color: var(--hp-text);
}
.hp-section-sub {
  font-size: 16px;
  color: var(--hp-text-light);
  margin: 0;
}

/* ==================== 屏 2：知识体系 ==================== */
.hp-knowledge { background: #fff; }
.hp-kgroup { margin-bottom: 52px; }
.hp-kgroup:last-child { margin-bottom: 0; }

.hp-kgroup-head {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 22px;
}
.hp-kgroup-icon {
  font-size: 22px;
}
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
  background: color-mix(in srgb, var(--c) 10%, transparent);
  color: var(--c);
  border-radius: 999px;
  font-weight: 500;
  /* Fallback for browsers without color-mix */
  background: rgba(74, 144, 217, 0.1);
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

/* ==================== 屏 3：工具箱 ==================== */
.hp-tools {
  max-width: none;
  background: linear-gradient(135deg, #4a90d9 0%, #7b5fd9 50%, #ec4899 100%);
  color: #fff;
  padding: 100px 24px;
  position: relative;
  overflow: hidden;
}
.hp-tools::before {
  content: '';
  position: absolute; inset: 0;
  background-image:
    radial-gradient(circle at 20% 30%, rgba(255,255,255,0.15) 0%, transparent 40%),
    radial-gradient(circle at 80% 70%, rgba(255,255,255,0.1) 0%, transparent 40%);
  pointer-events: none;
}
.hp-tools-inner {
  position: relative;
  max-width: 1180px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 70px;
  align-items: center;
}
.hp-tools-title {
  font-size: 40px;
  font-weight: 800;
  margin: 12px 0 18px;
  color: #fff;
  letter-spacing: -1px;
}
.hp-tools-sub {
  font-size: 16px;
  line-height: 1.85;
  color: rgba(255, 255, 255, 0.85);
  margin: 0 0 32px;
}

.hp-tools-features {
  display: flex;
  flex-direction: column;
  gap: 16px;
  margin-bottom: 36px;
}
.hp-tf {
  display: flex;
  gap: 14px;
  align-items: flex-start;
  padding: 14px 18px;
  background: rgba(255, 255, 255, 0.12);
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 12px;
  backdrop-filter: blur(6px);
  transition: all 0.25s ease;
}
.hp-tf:hover {
  background: rgba(255, 255, 255, 0.18);
  transform: translateX(4px);
}
.hp-tf-icon {
  font-size: 24px;
  flex-shrink: 0;
}
.hp-tf-text {
  display: flex;
  flex-direction: column;
}
.hp-tf-text strong {
  font-size: 15px;
  font-weight: 600;
  color: #fff;
  margin-bottom: 2px;
}
.hp-tf-text span {
  font-size: 13px;
  color: rgba(255, 255, 255, 0.75);
}

.hp-tools .hp-btn-primary {
  background: #fff;
  color: var(--hp-brand-2) !important;
  box-shadow: 0 8px 20px rgba(0, 0, 0, 0.15);
}
.hp-tools .hp-btn-primary:hover {
  background: #f9fafb;
  box-shadow: 0 12px 28px rgba(0, 0, 0, 0.2);
}

/* 工具云 */
.hp-tools-cloud {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 14px;
  position: relative;
}
.hp-tcloud {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
  padding: 18px 8px;
  background: rgba(255, 255, 255, 0.15);
  border: 1px solid rgba(255, 255, 255, 0.25);
  border-radius: 14px;
  backdrop-filter: blur(8px);
  transition: all 0.3s ease;
  animation: hpTcFloat 4s ease-in-out infinite;
  animation-delay: var(--d);
}
.hp-tcloud:hover {
  background: rgba(255, 255, 255, 0.25);
  transform: translateY(-4px) scale(1.05);
}
.hp-tcloud-icon { font-size: 28px; }
.hp-tcloud-name { font-size: 12.5px; color: #fff; font-weight: 500; }
.hp-tcloud-1 { transform: translateY(0); }
.hp-tcloud-2 { transform: translateY(-10px); }
.hp-tcloud-3 { transform: translateY(10px); }
@keyframes hpTcFloat {
  0%, 100% { transform: translateY(0) scale(1); }
  50% { transform: translateY(-6px) scale(1.02); }
}

/* ==================== 屏 4：精选推荐 ==================== */
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
  background: linear-gradient(135deg, var(--c), color-mix(in srgb, var(--c) 60%, #fff));
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

/* ==================== 屏 4.5：博主信息 + 二维码（原右侧栏） ==================== */
.hp-bloginfo {
  background: #fff;
  padding: 60px 24px 0;
}
.hp-bi-grid {
  max-width: 1180px;
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

/* ==================== 屏 5：站点哲学 ==================== */
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

/* ==================== 响应式 ==================== */
@media (max-width: 960px) {
  .hp-hero-inner { grid-template-columns: 1fr; gap: 40px; }
  .hp-hero-right { display: none; }
  .hp-hero-title { font-size: 48px; }
  .hp-tools-inner { grid-template-columns: 1fr; gap: 40px; }
  .hp-kgrid { grid-template-columns: repeat(2, 1fr); }
  .hp-fgrid { grid-template-columns: repeat(2, 1fr); }
  .hp-mf-pillars { grid-template-columns: 1fr; }
}
@media (max-width: 960px) {
  .hp-bi-grid { grid-template-columns: 1fr 1fr; }
}
@media (max-width: 768px) {
  .hp-bi-grid { grid-template-columns: 1fr; }
  .hp-hero { padding: 60px 18px 40px; min-height: auto; }
  .hp-hero-title { font-size: 40px; letter-spacing: -1px; }
  .hp-hero-sub { font-size: 18px; }
  .hp-section { padding: 60px 18px; }
  .hp-section-title { font-size: 32px; }
  .hp-tools-title { font-size: 30px; }
  .hp-mf-title { font-size: 28px; }
  .hp-hero-stats {
    grid-template-columns: repeat(2, 1fr);
    gap: 18px;
    padding: 20px 18px;
  }
  .hp-stat-num { font-size: 30px; }
  .hp-kgrid { grid-template-columns: 1fr; }
  .hp-fgrid { grid-template-columns: 1fr; }
  .hp-tools-cloud { grid-template-columns: repeat(3, 1fr); }
}
@media (max-width: 480px) {
  .hp-hero-title { font-size: 34px; }
  .hp-hero-actions { flex-direction: column; align-items: stretch; }
  .hp-btn { justify-content: center; }
  .hp-blob { opacity: 0.3; }
  .hp-grid-bg { opacity: 0.5; }
}

/* ==================== 暗色模式 ==================== */
.theme--dark .hp-wrapper {
  --hp-text: #e0e0e0;
  --hp-text-light: #9aa0a6;
  --hp-bg: #181825;
  --hp-card: #1e1e2e;
  --hp-border: #2a2a3a;
}
.theme--dark .hp-hero {
  background: linear-gradient(135deg, #11111b 0%, #1a1a2e 50%, #1f1a3a 100%);
}
.theme--dark .hp-blob { opacity: 0.25; }
.theme--dark .hp-grid-bg {
  background-image:
    linear-gradient(rgba(123, 95, 217, 0.08) 1px, transparent 1px),
    linear-gradient(90deg, rgba(123, 95, 217, 0.08) 1px, transparent 1px);
}
.theme--dark .hp-hero-badge {
  background: rgba(30, 30, 46, 0.7);
  border-color: rgba(123, 95, 217, 0.3);
  color: #aaa;
}
.theme--dark .hp-title-line-1 {
  background: linear-gradient(135deg, #f0f0f0 0%, #b0b0b0 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
.theme--dark .hp-hero-sub { color: #ddd; }
.theme--dark .hp-btn-ghost {
  background: rgba(30, 30, 46, 0.8);
  color: #ddd !important;
  border-color: #2a2a3a;
}
.theme--dark .hp-btn-ghost:hover { background: #1e1e2e; }
.theme--dark .hp-hero-stats {
  background: rgba(30, 30, 46, 0.6);
  border-color: rgba(123, 95, 217, 0.2);
}
.theme--dark .hp-knowledge,
.theme--dark .hp-manifesto {
  background: #181825;
}
.theme--dark .hp-section-title,
.theme--dark .hp-mf-title,
.theme--dark .hp-kcard-title,
.theme--dark .hp-fcard-title,
.theme--dark .hp-mf-pillar h4 { color: #e0e0e0; }
.theme--dark .hp-kcard,
.theme--dark .hp-fcard,
.theme--dark .hp-mf-pillar {
  background: #1e1e2e;
  border-color: #2a2a3a;
}
.theme--dark .hp-kcard-arrow {
  background: #2a2a3a;
  color: #888;
}
.theme--dark .hp-fcard-meta { border-top-color: #2a2a3a; }
.theme--dark .hp-featured { background: #11111b; }
.theme--dark .hp-bloginfo { background: #181825; }
.theme--dark .hp-bi-card { background: #1e1e2e; border-color: #2a2a3a; }
.theme--dark .hp-bi-si { background: #2a2a3a; color: #888; }
.theme--dark .hp-bi-si:hover { background: var(--hp-brand); color: #fff; }
.theme--dark .hp-bi-tag { background: rgba(123, 95, 217, 0.15); }
</style>
