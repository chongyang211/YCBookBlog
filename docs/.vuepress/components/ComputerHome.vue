<template>
  <div class="ch-wrapper">
    <!-- Hero -->
    <div class="ch-hero">
      <h1 class="ch-title">深入计算机底层</h1>
      <p class="ch-subtitle">从晶体管的物理开关，到浏览器的 HTTP 请求。65 篇硬核长文，四个模块，一把你什么时候都能用的底牌。</p>
      <div class="ch-stats">
        <span class="ch-stat">{{ modules.length }} 个模块</span>
        <span class="ch-stat-sep">·</span>
        <span class="ch-stat">{{ totalArticles }} 篇文章</span>
        <span class="ch-stat-sep">·</span>
        <span class="ch-stat">持续更新中</span>
      </div>
    </div>

    <!-- Pain Points -->
    <div class="ch-section">
      <h2 class="ch-section-title">为什么你需要读</h2>
      <div class="ch-pain-table">
        <div class="ch-pain-row" v-for="(item, i) in painPoints" :key="i">
          <div class="ch-pain-cell ch-pain-problem">{{ item.problem }}</div>
          <div class="ch-pain-cell ch-pain-answer">{{ item.answer }}</div>
        </div>
      </div>
    </div>

    <!-- Module Cards -->
    <div class="ch-section">
      <h2 class="ch-section-title">四个模块</h2>
      <div class="ch-cards">
        <a v-for="m in modules" :key="m.id" :href="m.link" class="ch-card" :style="{ borderTopColor: m.color }">
          <div class="ch-card-emoji">{{ m.emoji }}</div>
          <div class="ch-card-body">
            <h3 class="ch-card-title">{{ m.name }}</h3>
            <p class="ch-card-meta">{{ m.count }} 篇 · {{ m.start }}</p>
            <p class="ch-card-desc">{{ m.desc }}</p>
          </div>
        </a>
      </div>
    </div>

    <!-- What You'll Gain -->
    <div class="ch-section">
      <h2 class="ch-section-title">你会收获什么</h2>
      <div class="ch-gains">
        <div class="ch-gain" v-for="g in gains" :key="g.title">
          <div class="ch-gain-emoji">{{ g.emoji }}</div>
          <div>
            <h4 class="ch-gain-title">{{ g.title }}</h4>
            <p class="ch-gain-desc">{{ g.desc }}</p>
          </div>
        </div>
      </div>
    </div>

    <!-- Reading Guide -->
    <div class="ch-section">
      <h2 class="ch-section-title">怎么读</h2>
      <div class="ch-guide-table">
        <div class="ch-guide-row ch-guide-header">
          <div class="ch-guide-cell">目标</div>
          <div class="ch-guide-cell">路径</div>
          <div class="ch-guide-cell">耗时</div>
        </div>
        <div class="ch-guide-row">
          <div class="ch-guide-cell">查问题</div>
          <div class="ch-guide-cell">上面的对照表 → 点进对应篇章 → 看速通卡</div>
          <div class="ch-guide-cell">5 分钟</div>
        </div>
        <div class="ch-guide-row">
          <div class="ch-guide-cell">系统学</div>
          <div class="ch-guide-cell">组成原理 → 操作系统 → 网络协议 → 数据库，按序</div>
          <div class="ch-guide-cell">每天 1 篇</div>
        </div>
        <div class="ch-guide-row">
          <div class="ch-guide-cell">面试突击</div>
          <div class="ch-guide-cell">每篇只看首段故事 + 末段"一句话拎走"</div>
          <div class="ch-guide-cell">每篇 3 分钟</div>
        </div>
      </div>
    </div>

    <!-- Footer -->
    <div class="ch-footer">
      <p>计算机不会背叛你。你理解它多深，它就回馈你多少。</p>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      painPoints: [
        { problem: '接口 RT 飙到 2s，CPU 才 5%，不知道为什么慢', answer: '硬盘 I/O 阻塞——CPU 在等磁盘。组成原理·§01' },
        { problem: '多线程计数器跑完值总是少——AtomicLong 也救不了', answer: 'MESI 协议让缓存行在 8 个核之间弹跳。组成原理·§03' },
        { problem: '同一个循环换个嵌套顺序性能差 15 倍', answer: '缓存行预取——空间局部性。组成原理·§04' },
        { problem: '财务对账差 0.01 元，查了三天', answer: '0.1 在二进制里是无限循环小数。组成原理·§10' },
        { problem: '凌晨三点告警"服务挂了"——第一反应翻代码', answer: '90% 是网络层问题，ping → telnet → curl。网络协议·§15' },
        { problem: '加了 Redis 反而慢了', answer: '你不是少了缓存，是多了一次网络 I/O。网络协议·§10' },
      ],
      modules: [
        {
          id: 'arch', emoji: '⚙️', color: '#4a90d9',
          name: '计算机组成原理', count: 14,
          start: '冯·诺依曼架构 · 五大部件 · 三级缓存 · MESI',
          desc: '从凌晨三点的磁盘告警出发，把 CPU 拆开给你看。你写下的每一行代码，在硬件上到底经过了哪些部件。',
          link: '/pages/a043ba/'
        },
        {
          id: 'network', emoji: '🌐', color: '#e67e22',
          name: '网络协议原理', count: 19,
          start: 'HTTP 全链路 · TCP/UDP · TLS 1.3 · QUIC · WebSocket',
          desc: '从一条打不开的新闻出发，逐层爬完七层协议栈。抓包、排障、加密——网络的每一块拼图。',
          link: '/pages/c0cb52/'
        },
        {
          id: 'os', emoji: '🖥️', color: '#27ae60',
          name: '操作系统原理', count: 10,
          start: '进程/线程 · 虚拟内存 · 文件系统 · 容器',
          desc: 'molloc(1GB)为什么瞬间返回？Docker 怎么"骗"你的进程？操作系统是最大的中间件。',
          link: '/pages/a5534e/'
        },
        {
          id: 'db', emoji: '🗄️', color: '#8e44ad',
          name: '数据库系统原理', count: 10,
          start: 'B+ 树 · MVCC · redo log · 分库分表',
          desc: 'WHERE id=100 为什么不扫全表？事务 ACID 是怎么落到磁盘上的？数据库的底层没有魔法。',
          link: '/pages/7fe482/'
        }
      ],
      gains: [
        { emoji: '🗺️', title: '一张速度地图', desc: '寄存器(0.3ns) → 内存(100ns) → SSD(100μs) → 网络(100ms)，差距 3 亿倍。从此你看到"循环里读文件"会生理不适。' },
        { emoji: '🔧', title: '一条排障铁律', desc: '自底向上：先 ping、再 telnet、最后 curl。10 分钟定位 90% 的"服务挂了"——不需要猜。' },
        { emoji: '⚡', title: '一把性能直觉', desc: '遍历数组 i/j 和 j/i 差 15 倍？CAS 高竞争比 synchronized 慢？这些都是硬件在告诉你"你写错了"。' },
      ]
    }
  },
  computed: {
    totalArticles() { return this.modules.reduce((s, m) => s + m.count, 0) }
  }
}
</script>

<style scoped>
.ch-wrapper { max-width: 860px; margin: 0 auto; padding: 0 1rem; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; }

/* Hero */
.ch-hero { text-align: center; padding: 2.5rem 0 1.5rem; }
.ch-title { font-size: 2rem; font-weight: 800; margin: 0 0 .5rem; background: linear-gradient(135deg, #4a90d9, #8e44ad); -webkit-background-clip: text; -webkit-text-fill-color: transparent; background-clip: text; }
.ch-subtitle { max-width: 600px; margin: 0 auto 1.2rem; color: #666; font-size: .95rem; line-height: 1.6; }
.ch-stats { display: flex; justify-content: center; align-items: center; gap: .5rem; color: #999; font-size: .85rem; }
.ch-stat-sep { color: #ddd; }

/* Section */
.ch-section { margin: 2rem 0; }
.ch-section-title { font-size: 1.15rem; font-weight: 700; margin: 0 0 1rem; padding-bottom: .5rem; border-bottom: 2px solid #f0f0f0; }

/* Pain table */
.ch-pain-table { border: 1px solid #eee; border-radius: 8px; overflow: hidden; }
.ch-pain-row { display: flex; border-bottom: 1px solid #f5f5f5; }
.ch-pain-row:last-child { border-bottom: none; }
.ch-pain-cell { padding: .7rem .9rem; font-size: .88rem; line-height: 1.5; }
.ch-pain-problem { flex: 1; color: #e74c3c; font-weight: 500; background: #fdf2f2; }
.ch-pain-answer { flex: 1.2; color: #555; background: #f9fdf5; }

/* Cards */
.ch-cards { display: grid; grid-template-columns: 1fr 1fr; gap: 1rem; }
@media (max-width: 600px) { .ch-cards { grid-template-columns: 1fr; } }
.ch-card { display: flex; gap: .8rem; padding: 1rem; border-radius: 10px; border: 1px solid #eee; border-top: 3px solid; text-decoration: none; color: inherit; transition: box-shadow .2s; background: #fff; }
.ch-card:hover { box-shadow: 0 4px 16px rgba(0,0,0,.08); }
.ch-card-emoji { font-size: 1.6rem; flex-shrink: 0; }
.ch-card-title { font-size: 1rem; font-weight: 700; margin: 0 0 .3rem; }
.ch-card-meta { font-size: .75rem; color: #999; margin: 0 0 .4rem; }
.ch-card-desc { font-size: .83rem; color: #666; margin: 0; line-height: 1.5; }

/* Gains */
.ch-gains { display: flex; flex-direction: column; gap: .8rem; }
.ch-gain { display: flex; gap: .8rem; padding: .8rem; border-radius: 8px; background: #fafafa; }
.ch-gain-emoji { font-size: 1.4rem; flex-shrink: 0; }
.ch-gain-title { font-size: .9rem; font-weight: 700; margin: 0 0 .2rem; }
.ch-gain-desc { font-size: .83rem; color: #666; margin: 0; line-height: 1.5; }

/* Guide table */
.ch-guide-table { border: 1px solid #eee; border-radius: 8px; overflow: hidden; }
.ch-guide-row { display: flex; border-bottom: 1px solid #f5f5f5; }
.ch-guide-row:last-child { border-bottom: none; }
.ch-guide-header { background: #fafafa; font-weight: 700; font-size: .85rem; }
.ch-guide-cell { flex: 1; padding: .6rem .8rem; font-size: .85rem; }
.ch-guide-header .ch-guide-cell { font-weight: 700; }

/* Footer */
.ch-footer { text-align: center; padding: 1.5rem 0; color: #999; font-size: .85rem; }
</style>
