<template>
  <div class="tg-wrapper">
    <!-- Hero -->
    <div class="tg-hero">
      <h1 class="tg-hero-title">
        <span class="tg-hero-emoji">🛠</span>
        在线工具箱
      </h1>
      <p class="tg-hero-desc">
        浏览器端运行 · 数据不上传 · 开箱即用 · 无需注册
      </p>
      <div class="tg-hero-stats">
        <span class="tg-stat-item">
          <strong>{{ readyCount }}</strong> 个工具已上线
        </span>
        <span class="tg-stat-sep">·</span>
        <span class="tg-stat-item">
          <strong>{{ planCount }}</strong> 个工具开发中
        </span>
        <span class="tg-stat-sep">·</span>
        <span class="tg-stat-item">
          <strong>{{ categories.length }}</strong> 大分类
        </span>
      </div>
    </div>

    <!-- Search -->
    <div class="tg-search">
      <span class="tg-search-icon">
        <svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2">
          <circle cx="11" cy="11" r="8"/>
          <line x1="21" y1="21" x2="16.65" y2="16.65"/>
        </svg>
      </span>
      <input
        v-model="keyword"
        class="tg-search-input"
        placeholder="搜索工具，如 JSON · 二维码 · MD5 · Cron..."
      />
      <button v-if="keyword" class="tg-search-clear" @click="keyword = ''">✕</button>
    </div>

    <!-- Filter Tabs -->
    <div class="tg-filter">
      <button
        class="tg-filter-tab"
        :class="{ active: filter === 'all' }"
        @click="filter = 'all'"
      >
        全部 <span class="tg-filter-cnt">{{ categories.length }}</span>
      </button>
      <button
        class="tg-filter-tab"
        :class="{ active: filter === 'ready' }"
        @click="filter = 'ready'"
      >
        已上线 <span class="tg-filter-cnt">{{ readyCats.length }}</span>
      </button>
      <button
        class="tg-filter-tab"
        :class="{ active: filter === 'planning' }"
        @click="filter = 'planning'"
      >
        开发中 <span class="tg-filter-cnt">{{ planningCats.length }}</span>
      </button>
    </div>

    <!-- Cards Grid -->
    <div class="tg-grid" v-if="filteredCats.length > 0">
      <a
        v-for="cat in filteredCats"
        :key="cat.id"
        class="tg-card"
        :class="{ 'tg-card-planning': cat.status === 'planning' }"
        :href="cat.link"
        :style="cardStyle(cat)"
      >
        <div class="tg-card-badge" v-if="cat.status === 'planning'">规划中</div>
        <div class="tg-card-badge tg-card-badge-hot" v-else-if="cat.hot">HOT</div>
        <div class="tg-card-icon">
          <span>{{ cat.icon }}</span>
        </div>
        <div class="tg-card-body">
          <h3 class="tg-card-title">{{ cat.title }}</h3>
          <p class="tg-card-desc">{{ cat.desc }}</p>
          <div class="tg-card-tools">
            <span
              v-for="(t, idx) in displayTools(cat)"
              :key="idx"
              class="tg-card-tool"
            >{{ t }}</span>
            <span v-if="cat.tools.length > 4" class="tg-card-tool tg-card-tool-more">
              +{{ cat.tools.length - 4 }}
            </span>
          </div>
        </div>
        <div class="tg-card-footer">
          <span class="tg-card-count">{{ cat.tools.length }} 个工具</span>
          <span class="tg-card-arrow">→</span>
        </div>
      </a>
    </div>

    <div v-else class="tg-empty">
      <p>没有找到匹配的工具</p>
      <button class="tg-empty-btn" @click="keyword = ''; filter = 'all'">重置筛选</button>
    </div>

    <!-- Footer -->
    <div class="tg-footer">
      <p>
        💡 所有工具完全在浏览器中运行，输入的数据不会上传到任何服务器，
        请放心使用。
      </p>
      <p class="tg-footer-tip">
        🚀 工具持续开发中，
        <a href="https://github.com/yangchong211/YCBookBlog/issues" target="_blank" rel="noopener">
          欢迎提需求 / Issue
        </a>
      </p>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ToolsGrid',
  data() {
    return {
      keyword: '',
      filter: 'all',
      categories: [
        {
          id: 'json',
          title: 'JSON 工具',
          desc: '格式化、压缩、校验、统计、转换',
          icon: '{ }',
          link: '/tools/json/',
          status: 'ready',
          hot: true,
          color1: '#4a90d9',
          color2: '#7b5fd9',
          tools: ['格式化', '压缩', '校验', '统计', '转 YAML/XML'],
        },
        {
          id: 'text',
          title: '文本工具',
          desc: '编码转换、正则测试、文本对比、字数统计',
          icon: '📝',
          link: '/tools/text/',
          status: 'ready',
          hot: true,
          color1: '#52b788',
          color2: '#4a90d9',
          tools: ['编码转换', '正则测试', '文本对比', '字数统计', '大小写'],
        },
        {
          id: 'image',
          title: '图片处理',
          desc: '批量格式转换 WebP/JPG/PNG/AVIF，浏览器端处理',
          icon: '🖼',
          link: '/tools/image/',
          status: 'ready',
          hot: true,
          color1: '#f5a623',
          color2: '#e74c3c',
          tools: ['格式互转', '批量转换', '在线预览', 'ZIP 打包'],
        },
        {
          id: 'doc',
          title: '文档转化',
          desc: 'Markdown ↔ HTML ↔ PDF，文档格式互转',
          icon: '📄',
          link: '/tools/doc/',
          status: 'planning',
          color1: '#9b59b6',
          color2: '#4a90d9',
          tools: ['MD → HTML', 'HTML → MD', 'MD → PDF', 'PDF 拆分', 'Word → MD'],
        },
        {
          id: 'minify',
          title: '代码压缩',
          desc: 'JS / CSS / HTML / SQL 压缩与美化',
          icon: '📦',
          link: '/tools/minify/',
          status: 'planning',
          color1: '#e67e22',
          color2: '#e74c3c',
          tools: ['JS 压缩', 'CSS 压缩', 'HTML 美化', 'SQL 格式化', '代码混淆'],
        },
        {
          id: 'crypto',
          title: '加解密',
          desc: 'AES / RSA / Hash / JWT / HMAC',
          icon: '🔐',
          link: '/tools/crypto/',
          status: 'planning',
          hot: true,
          color1: '#c0392b',
          color2: '#7b5fd9',
          tools: ['AES 加解密', 'RSA', 'MD5/SHA', 'JWT 解码', 'HMAC'],
        },
        {
          id: 'time',
          title: '时间日期',
          desc: '时间戳、时区、Cron 表达式、日期计算',
          icon: '⏰',
          link: '/tools/time/',
          status: 'planning',
          hot: true,
          color1: '#16a085',
          color2: '#4a90d9',
          tools: ['时间戳转换', '时区对比', 'Cron 解析', '日期计算', '倒计时'],
        },
        {
          id: 'network',
          title: '网络工具',
          desc: 'URL / UA / IP / HTTP / CIDR 子网计算',
          icon: '🌐',
          link: '/tools/network/',
          status: 'planning',
          color1: '#2980b9',
          color2: '#16a085',
          tools: ['URL 解析', 'User-Agent', '状态码查询', 'MIME 类型', 'CIDR 计算'],
        },
        {
          id: 'color',
          title: '颜色设计',
          desc: '取色板、渐变、调色板、对比度检测',
          icon: '🎨',
          link: '/tools/color/',
          status: 'planning',
          color1: '#e91e63',
          color2: '#f5a623',
          tools: ['取色板', '渐变生成', '调色板', '图片取色', '对比度'],
        },
        {
          id: 'qrcode',
          title: '二维码',
          desc: '二维码生成 / 识别 / 条形码',
          icon: '📱',
          link: '/tools/qrcode/',
          status: 'planning',
          color1: '#34495e',
          color2: '#16a085',
          tools: ['二维码生成', '扫码识别', '彩色二维码', '加 Logo', '条形码'],
        },
        {
          id: 'dev',
          title: '开发实用',
          desc: 'UUID、随机密码、进制转换、单位换算',
          icon: '⚙️',
          link: '/tools/dev/',
          status: 'planning',
          hot: true,
          color1: '#7b5fd9',
          color2: '#4a90d9',
          tools: ['UUID 生成', '随机密码', '进制转换', '单位换算', 'CSV ↔ JSON'],
        },
      ],
    }
  },
  computed: {
    readyCats() {
      return this.categories.filter(c => c.status === 'ready')
    },
    planningCats() {
      return this.categories.filter(c => c.status === 'planning')
    },
    readyCount() {
      return this.readyCats.reduce((sum, c) => sum + c.tools.length, 0)
    },
    planCount() {
      return this.planningCats.reduce((sum, c) => sum + c.tools.length, 0)
    },
    filteredCats() {
      let list = this.categories
      if (this.filter === 'ready') list = this.readyCats
      else if (this.filter === 'planning') list = this.planningCats

      const kw = this.keyword.trim().toLowerCase()
      if (!kw) return list

      return list.filter(c => {
        if (c.title.toLowerCase().includes(kw)) return true
        if (c.desc.toLowerCase().includes(kw)) return true
        if (c.id.toLowerCase().includes(kw)) return true
        if (c.tools.some(t => t.toLowerCase().includes(kw))) return true
        return false
      })
    },
  },
  methods: {
    cardStyle(cat) {
      return {
        '--c1': cat.color1,
        '--c2': cat.color2,
      }
    },
    displayTools(cat) {
      return cat.tools.slice(0, 4)
    },
  },
}
</script>

<style scoped>
.tg-wrapper {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px 0 40px;
  font-family: -apple-system, BlinkMacSystemFont, 'PingFang SC', 'Microsoft YaHei', sans-serif;
}

/* Hero */
.tg-hero {
  text-align: center;
  padding: 30px 20px 20px;
  margin-bottom: 24px;
}
.tg-hero-title {
  font-size: 36px;
  font-weight: 700;
  margin: 0 0 10px;
  background: linear-gradient(90deg, #4a90d9, #7b5fd9);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  display: inline-flex;
  align-items: center;
  gap: 12px;
}
.tg-hero-emoji {
  -webkit-text-fill-color: initial;
  font-size: 38px;
}
.tg-hero-desc {
  font-size: 15px;
  color: #777;
  margin: 0 0 16px;
}
.tg-hero-stats {
  display: inline-flex;
  align-items: center;
  gap: 10px;
  font-size: 13px;
  color: #666;
}
.tg-stat-item strong {
  color: #4a90d9;
  font-size: 16px;
  font-weight: 700;
  margin-right: 3px;
}
.tg-stat-sep {
  color: #ccc;
}

/* Search */
.tg-search {
  position: relative;
  max-width: 560px;
  margin: 0 auto 18px;
}
.tg-search-icon {
  position: absolute;
  left: 16px;
  top: 50%;
  transform: translateY(-50%);
  color: #aaa;
  display: flex;
}
.tg-search-input {
  width: 100%;
  padding: 12px 40px 12px 44px;
  font-size: 14px;
  border: 1px solid #e0e0e0;
  border-radius: 24px;
  background: #f9f9fb;
  outline: none;
  transition: all 0.2s;
  box-sizing: border-box;
  color: #333;
}
.tg-search-input:focus {
  border-color: #4a90d9;
  background: #fff;
  box-shadow: 0 0 0 4px rgba(74, 144, 217, 0.1);
}
.tg-search-clear {
  position: absolute;
  right: 12px;
  top: 50%;
  transform: translateY(-50%);
  background: #ddd;
  color: #fff;
  border: none;
  border-radius: 50%;
  width: 20px;
  height: 20px;
  font-size: 11px;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  line-height: 1;
}
.tg-search-clear:hover { background: #aaa; }

/* Filter */
.tg-filter {
  display: flex;
  justify-content: center;
  gap: 8px;
  margin-bottom: 24px;
}
.tg-filter-tab {
  padding: 6px 16px;
  font-size: 13px;
  border-radius: 16px;
  border: 1px solid #e0e0e0;
  background: #fff;
  color: #666;
  cursor: pointer;
  transition: all 0.15s;
  display: inline-flex;
  align-items: center;
  gap: 6px;
}
.tg-filter-tab:hover {
  border-color: #4a90d9;
  color: #4a90d9;
}
.tg-filter-tab.active {
  background: linear-gradient(90deg, #4a90d9, #7b5fd9);
  color: #fff;
  border-color: transparent;
}
.tg-filter-cnt {
  background: rgba(255, 255, 255, 0.25);
  border-radius: 8px;
  padding: 1px 7px;
  font-size: 11px;
  font-weight: 600;
}
.tg-filter-tab:not(.active) .tg-filter-cnt {
  background: #f0f0f0;
  color: #999;
}

/* Grid */
.tg-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
  gap: 18px;
}

/* Card */
.tg-card {
  position: relative;
  display: flex;
  flex-direction: column;
  background: #fff;
  border-radius: 14px;
  padding: 22px 20px 18px;
  border: 1px solid #ececec;
  text-decoration: none !important;
  color: inherit;
  transition: all 0.25s cubic-bezier(0.4, 0, 0.2, 1);
  cursor: pointer;
  overflow: hidden;
  --c1: #4a90d9;
  --c2: #7b5fd9;
}
.tg-card::before {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  height: 3px;
  background: linear-gradient(90deg, var(--c1), var(--c2));
  opacity: 0;
  transition: opacity 0.25s;
}
.tg-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 12px 32px rgba(74, 144, 217, 0.15);
  border-color: transparent;
}
.tg-card:hover::before {
  opacity: 1;
}

.tg-card-planning {
  opacity: 0.7;
}
.tg-card-planning:hover {
  opacity: 1;
}

/* Badges */
.tg-card-badge {
  position: absolute;
  top: 14px;
  right: 14px;
  padding: 2px 8px;
  font-size: 10px;
  font-weight: 700;
  border-radius: 10px;
  background: #f0f0f0;
  color: #999;
  letter-spacing: 0.5px;
}
.tg-card-badge-hot {
  background: linear-gradient(90deg, #e74c3c, #f5a623);
  color: #fff;
}

/* Icon */
.tg-card-icon {
  width: 52px;
  height: 52px;
  border-radius: 12px;
  background: linear-gradient(135deg, var(--c1), var(--c2));
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 26px;
  margin-bottom: 14px;
  color: #fff;
  font-weight: 700;
  font-family: 'JetBrains Mono', monospace;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
}

.tg-card-body {
  flex: 1;
}
.tg-card-title {
  font-size: 17px;
  font-weight: 700;
  margin: 0 0 6px;
  color: #222;
}
.tg-card-desc {
  font-size: 12.5px;
  color: #888;
  margin: 0 0 12px;
  line-height: 1.5;
  min-height: 36px;
}

.tg-card-tools {
  display: flex;
  flex-wrap: wrap;
  gap: 4px;
  margin-bottom: 14px;
}
.tg-card-tool {
  padding: 2px 8px;
  font-size: 11px;
  border-radius: 4px;
  background: #f5f7fa;
  color: #666;
  white-space: nowrap;
}
.tg-card-tool-more {
  background: linear-gradient(90deg, var(--c1), var(--c2));
  color: #fff;
  font-weight: 600;
}

.tg-card-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding-top: 12px;
  border-top: 1px dashed #f0f0f0;
}
.tg-card-count {
  font-size: 12px;
  color: #999;
}
.tg-card-arrow {
  font-size: 18px;
  color: var(--c1);
  transition: transform 0.25s;
}
.tg-card:hover .tg-card-arrow {
  transform: translateX(4px);
}

/* Empty */
.tg-empty {
  text-align: center;
  padding: 60px 20px;
  color: #aaa;
}
.tg-empty-btn {
  margin-top: 14px;
  padding: 6px 16px;
  border-radius: 16px;
  border: 1px solid #ddd;
  background: #fff;
  color: #666;
  cursor: pointer;
}
.tg-empty-btn:hover {
  border-color: #4a90d9;
  color: #4a90d9;
}

/* Footer */
.tg-footer {
  margin-top: 40px;
  padding: 20px;
  text-align: center;
  background: linear-gradient(135deg, #f9fafe, #f5f7fc);
  border-radius: 12px;
  font-size: 13px;
  color: #777;
}
.tg-footer p { margin: 4px 0; }
.tg-footer a {
  color: #4a90d9;
  text-decoration: none;
  font-weight: 600;
}
.tg-footer a:hover { text-decoration: underline; }
.tg-footer-tip { font-size: 12px; }

/* Dark Mode */
.theme--dark .tg-hero-desc { color: #888; }
.theme--dark .tg-hero-stats { color: #aaa; }
.theme--dark .tg-stat-sep { color: #444; }
.theme--dark .tg-search-input {
  background: #1e1e2e;
  border-color: #3a3a4a;
  color: #ddd;
}
.theme--dark .tg-search-input:focus {
  background: #181825;
  border-color: #5a9ae0;
}
.theme--dark .tg-filter-tab {
  background: #1e1e2e;
  border-color: #3a3a4a;
  color: #aaa;
}
.theme--dark .tg-filter-tab:not(.active) .tg-filter-cnt {
  background: #2a2a3a;
  color: #888;
}
.theme--dark .tg-card {
  background: #1e1e2e;
  border-color: #3a3a4a;
}
.theme--dark .tg-card:hover {
  box-shadow: 0 12px 32px rgba(0, 0, 0, 0.5);
}
.theme--dark .tg-card-title { color: #ddd; }
.theme--dark .tg-card-desc { color: #888; }
.theme--dark .tg-card-tool {
  background: #2a2a3a;
  color: #aaa;
}
.theme--dark .tg-card-footer {
  border-top-color: #2a2a3a;
}
.theme--dark .tg-card-badge {
  background: #2a2a3a;
  color: #777;
}
.theme--dark .tg-footer {
  background: linear-gradient(135deg, #181825, #1e1e2e);
  color: #888;
}

/* Responsive */
@media (max-width: 640px) {
  .tg-hero-title { font-size: 28px; }
  .tg-hero-emoji { font-size: 30px; }
  .tg-hero-stats {
    flex-wrap: wrap;
    justify-content: center;
  }
  .tg-grid {
    grid-template-columns: 1fr;
    gap: 14px;
  }
  .tg-filter {
    flex-wrap: wrap;
  }
}
</style>
