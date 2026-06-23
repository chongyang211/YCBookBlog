/**
 * 首页静态数据（与视图解耦，便于维护）
 * 仅做数据导出，不包含任何 DOM / Vue API 调用
 */

// 打字机切换的句子
export const typePhrases = [
  '原理 > 工具 > 调包',
  '把复杂讲简单，把简单做透彻',
  '每一行代码都有它的成本',
  'Read the f**king source code',
]

// 顶部数据带
export const stats = {
  articles: 1000,
  categories: 12,
  tools: 18,
  days: 1800,
}

// 工具箱速达 · 工具云
export const toolCloud = [
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
]

// 知识体系 · 4 大分组 × 12 分类
export const groups = [
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
]

// 编辑精选
export const featured = [
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
]

// iconfont -> emoji 兜底映射（暗色 / 字体未加载时使用）
export const socialIconFallback = {
  'icon-github': '🐙',
  'icon-youjian': '📧',
  'icon-erji': '🎵',
}
