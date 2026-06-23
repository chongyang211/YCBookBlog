import { resolve } from 'path'
import * as os from 'os'
import { defineConfig4CustomTheme, UserPlugins } from 'vuepress/config'
import { VdoingThemeConfig } from 'vuepress-theme-vdoing/types'
import dayjs from 'dayjs'
import baiduCode from './config/baiduCode' // 百度统计hm码
import htmlModules from './config/htmlModules' // 自定义插入的html块

const DOMAIN_NAME = 'yccoding.com' // 域名 (不带https)
const WEB_SITE = `https://${DOMAIN_NAME}` // 网址

// ===== 快速构建模式 =====
// 用法：FAST_BUILD=1 npm run build   （仅用于百度广告验证/紧急小改动等场景）
// 该模式会跳过大目录的 md 编译，构建时间从数十分钟降到几分钟
const FAST_BUILD = process.env.FAST_BUILD === '1' || process.env.FAST_BUILD === 'true'

// 快速模式下排除的大目录（保留 工具/计算机/编程/算法/技术/更多/00.目录 等核心内容 + 首页）
const FAST_EXCLUDE_DIRS = [
  '10.专栏',
  '11.CodeX',
  '12.Apps',
  '20.ScriptHub',
]

if (FAST_BUILD) {
  // 醒目提示
  console.log('\n\x1b[33m========================================\x1b[0m')
  console.log('\x1b[33m  ⚡ FAST_BUILD 模式启用\x1b[0m')
  console.log('\x1b[33m  已跳过以下目录的编译：\x1b[0m')
  FAST_EXCLUDE_DIRS.forEach(d => console.log(`\x1b[33m    - ${d}\x1b[0m`))
  console.log('\x1b[33m========================================\x1b[0m\n')
}

export default defineConfig4CustomTheme<VdoingThemeConfig>({
  theme: 'vdoing', // 使用npm主题包
  // theme: resolve(__dirname, '../../vdoing'), // 使用本地主题包

  locales: {
    '/': {
      lang: 'zh-CN',
      title: "编程进阶网",
      description: '编程进阶，你值得拥有！',
    }
  },
  // base: '/', // 默认'/'。如果你想将你的网站部署到如 https://foo.github.io/bar/，那么 base 应该被设置成 "/bar/",（否则页面将失去样式等文件）

  // 主题配置
  themeConfig: {
    search: false, // 隐藏导航栏搜索框
    // 导航配置（FAST_BUILD 模式下会过滤掉指向被排除目录的入口，避免大量死链 warning）
    nav: ([
      { text: '首页', link: '/' },
      {
        text: '工具',
        link: '/tools/',
        items: [
          { text: 'JSON工具',  link: '/tools/json/' },
          { text: '文本工具',  link: '/tools/text/' },
          { text: '图片处理',  link: '/tools/image/' },
          { text: '文档转化',  link: '/tools/doc/' },
          { text: '代码压缩',  link: '/tools/minify/' },
        ],
      },
      {
        text: '计算机',
        link: '/computer/',
        items: [
          { text: '计算机原理', link: '/pages/a4c9df/' },
          { text: '操作系统', link: '/pages/ed6717/' },
          { text: '网络协议', link: '/pages/a9fec0/' },
          { text: '数据库原理', link: '/pages/c81a8d/' },
        ],
      },
      {
        text: '编程',
        link: '/programming/',
        items: [
          { text: '面向对象', link: '/pages/452dc5/' },
          { text: '设计原则', link: '/pages/759f12/' },
          { text: '设计模式', link: '/pages/6f8962/' },
          { text: '系统架构', link: '/pages/c288a4/' },
        ],
      },
      {
        text: '专栏',
        link: '/column/',
        items: [
          { text: '性能优化', link: '/pages/974deb/' },
          { text: '编程原理', link: '/pages/6adec7/' },
          { text: '方案设计', link: '/pages/a2fa2a/' },
          { text: '稳定可靠', link: '/pages/334660/' },
          { text: '工程运维', link: '/pages/6302f1/' },
        ],
      },
      {
        text: '算法',
        link: '/algorithm/',
        items: [
          { text: '基础认知', link: '/pages/245c07/' },
          { text: '线性结构', link: '/pages/7631f3/' },
          { text: '树与哈希', link: '/pages/d1e7ae/' },
          { text: '工业级实现', link: '/pages/430428/' },
          { text: '算法思想', link: '/pages/2e8a53/' },
          { text: '实战与综合', link: '/pages/af1a01/' },
          { text: '算法题考核', link: '/pages/f7f3e6/' },
        ],
      },
      {
        text: 'CodeX',
        link: '/codex/',
        items: [
          { text: 'C语言入门', link: '/pages/28ffbd/' },
          { text: 'C综合案例', link: '/pages/4cfc16/' },
          { text: 'C专栏博客', link: '/pages/c49a16/' },
          { text: 'C标准集库', link: '/pages/33bb66/' },
          { text: 'C++入门教程', link: '/pages/a4e0ab/' },
          { text: 'C++综合案例', link: '/pages/20d11f/' },
          { text: 'C++专栏博客', link: '/pages/a764fa/' },
          { text: 'C++开发技巧', link: '/pages/dcb4af/' },
          { text: 'Java入门教程', link: '/pages/fd2602/' },
          { text: 'Java综合案例', link: '/pages/6cc1aa/' },
          { text: 'Java专栏博客', link: '/pages/ca4c47/' },
          { text: 'Go入门教程', link: '/pages/05649e/' },
          { text: 'Go综合案例', link: '/pages/7e0920/' },
          { text: 'Go专栏博客', link: '/pages/930ccc/' },
          { text: 'Go开发技巧', link: '/pages/2f4f90/' },
          { text: 'JavaScript入门', link: '/pages/812fea/' },
          { text: 'JavaScript高级', link: '/pages/436764/' },
        ],
      },
      {
        text: 'Apps',
        link: '/app/',
        items: [
          { text: 'Android库解读', link: '/pages/502baa/' },
          { text: 'Android专栏', link: '/pages/65f3a0/' },
          { text: 'Android智能硬件', link: '/pages/a22b52/' },
          { text: 'iOS ObjC入门', link: '/pages/bb502c/' },
          { text: 'iOS Swift入门', link: '/pages/643728/' },
          { text: 'iOS入门精通', link: '/pages/78b9d9/' },
          { text: 'Web之Html手册', link: '/pages/cfa178/' },
          { text: 'Web之TypeScript', link: '/pages/85bb9e/' },
          { text: 'Web之Vue高级进阶', link: '/pages/cc1d0b/' },
          { text: 'Linux之QML入门', link: '/pages/99d701/' },
          { text: 'Linux之QT核心库', link: '/pages/7e9682/' },
          { text: 'Linux实践开发', link: '/pages/c61feb/' },
        ],
      },
      {
        text: 'Script',
        link: '/scripthub/',
        items: [
          { text: 'Python教程', link: '/pages/e7844c/' },
          { text: 'Shell&Bash教程', link: '/pages/cf8eba/' },
          { text: '工具脚本', link: '/pages/811c1e/' },
          { text: '自动化脚本', link: '/pages/029a17/' },
        ],
      },
      {
        text: '技术',
        link: '/technology/',
        items: [
          { text: '质量保障', link: '/pages/9a7ee40fc232253e/' },
          { text: '产品思考', link: '/pages/4c778760be26d8b3/' },
          { text: '软实力', link: '/pages/117708e0af7f0bd9/' },
          { text: '开发流程', link: '/pages/41f87d890d0a02af/' },
          { text: 'Git应用', link: '/pages/36ec62/' },
          { text: '技术模版', link: '/pages/4c795f/' },
          { text: '技术规范', link: '/pages/326cba/' },
          { text: 'Markdown', link: '/pages/440ec8/' },
          { text: 'Mermaid', link: '/pages/02ce97/' },
          { text: '开源协议', link: '/pages/b396e9/' },
        ],
      },
      {
        text: '更多',
        link: '/more/',
        items: [
          { text: '关于我', link: '/pages/d14ddf/' },
          { text: '自我精进', link: '/pages/0f148b/' },
          { text: '职场管理', link: '/pages/672218/' },
          { text: '职场面试', link: '/pages/71252c/' },
          { text: '心情杂货', link: '/pages/0d2b93/' },
          { text: '友情链接', link: '/friends/' },
        ],
      },

    ] as any[]).filter((item: any) => {
      if (!FAST_BUILD) return true
      // 快速模式：剔除指向已排除目录的一级菜单
      const fastExcludeNavText = ['专栏', 'CodeX', 'Apps', 'Script']
      return !fastExcludeNavText.includes(item.text)
    }),
    sidebarDepth: 2, // 侧边栏显示深度，默认1，最大2（显示到h3标题）
    logo: '/img/logo.png', // 导航栏logo
    // repo: '杨充/vuepress-theme-vdoing', // 导航栏右侧生成Github链接
    searchMaxSuggestions: 10, // 搜索结果显示最大数
    lastUpdated: '上次更新', // 开启更新时间，并配置前缀文字   string | boolean (取值为git提交时间)
    docsDir: 'docs', // 编辑的文件夹
    // docsBranch: 'master', // 编辑的文件所在分支，默认master。 注意：如果你的分支是main则修改为main
    editLinks: true, // 启用编辑
    editLinkText: '编辑',

    //*** 以下是Vdoing主题相关配置，文档：https://doc.杨充.com/pages/a20ce8/ ***//

    // category: false, // 是否打开分类功能，默认true
    // tag: false, // 是否打开标签功能，默认true
    // archive: false, // 是否打开归档功能，默认true
    // categoryText: '随笔', // 碎片化文章（_posts文件夹的文章）预设生成的分类值，默认'随笔'

    // pageStyle: 'line', // 页面风格，可选值：'card'卡片 | 'line' 线（未设置bodyBgImg时才生效）， 默认'card'。 说明：card时背景显示灰色衬托出卡片样式，line时背景显示纯色，并且部分模块带线条边框

    // bodyBgImg: [
    //   'https://jsd.cdn.zzko.cn/gh/杨充/image_store/blog/20200507175828.jpeg',
    //   'https://jsd.cdn.zzko.cn/gh/杨充/image_store/blog/20200507175845.jpeg',
    //   'https://jsd.cdn.zzko.cn/gh/杨充/image_store/blog/20200507175846.jpeg'
    // ], // body背景大图，默认无。 单张图片 String | 多张图片 Array, 多张图片时隔bodyBgImgInterval切换一张。
    // bodyBgImgOpacity: 0.5, // body背景图透明度，选值 0.1~1.0, 默认0.5
    // bodyBgImgInterval: 15, // body多张背景图时的切换间隔, 默认15，单位s
    // titleBadge: false, // 文章标题前的图标是否显示，默认true
    // titleBadgeIcons: [ // 文章标题前图标的地址，默认主题内置图标
    //   '图标地址1',
    //   '图标地址2'
    // ],
    // contentBgStyle: 1, // 文章内容块的背景风格，默认无. 1 方格 | 2 横线 | 3 竖线 | 4 左斜线 | 5 右斜线 | 6 点状

    // updateBar: { // 最近更新栏
    //   showToArticle: true, // 显示到文章页底部，默认true
    //   moreArticle: '/archives' // “更多文章”跳转的页面，默认'/archives'
    // },
    // rightMenuBar: false, // 是否显示右侧文章大纲栏，默认true (屏宽小于1300px下无论如何都不显示)
    // sidebarOpen: false, // 初始状态是否打开左侧边栏，默认true
    // pageButton: false, // 是否显示快捷翻页按钮，默认true

    // 默认外观模式（用户未在页面手动修改过模式时才生效，否则以用户设置的模式为准），可选：'auto' | 'light' | 'dark' | 'read'，默认'auto'。
    // defaultMode: 'auto',

    // 侧边栏  'structuring' | { mode: 'structuring', collapsable: Boolean} | 'auto' | <自定义>    温馨提示：目录页数据依赖于结构化的侧边栏数据，如果你不设置为'structuring',将无法使用目录页
    sidebar: 'structuring',

    // 文章默认的作者信息，(可在md文件中单独配置此信息) string | {name: string, link?: string}
    author: {
      name: '杨充', // 必需
      link: 'https://github.com/yangchong211', // 可选的
    },

    // 博主信息 (显示在首页侧边栏)
    blogger: {
      avatar: '/img/logo.png',
      name: '杨充',
      slogan: '专注编程 · 终身学习者',
    },

    // 社交图标 (显示于博主信息栏和页脚栏。内置图标：https://doc.杨充.com/pages/a20ce8/#social)
    social: {
      // iconfontCssFile: '//at.alicdn.com/t/xxx.css', // 可选，阿里图标库在线css文件地址，对于主题没有的图标可自己添加。阿里图片库：https://www.iconfont.cn/
      icons: [
        {
          iconClass: 'icon-github',
          title: 'GitHub',
          link: 'https://github.com/yangchong211',
        },
        {
          iconClass: 'icon-youjian',
          title: '发邮件',
          link: 'mailto:yangchong211@163.com',
        },
        {
          iconClass: 'icon-erji',
          title: '听音乐',
          link: 'https://music.163.com/#/playlist?id=755597173',
        },
      ],
    },

    // 页脚信息
    footer: {
      createYear: 2019, // 博客创建年份
      copyrightInfo:
        '杨充 | <a href="https://github.com/杨充/vuepress-theme-vdoing/blob/master/LICENSE" target="_blank">MIT License</a> | <a href="https://beian.miit.gov.cn/" target="_blank">鄂ICP备2024073355号-1</a> | <img src="/img/beian.png" style="width: 15px; margin-bottom: -3px;" /> <a href="https://beian.mps.gov.cn/#/query/webSearch?code=45142202000030" rel="noreferrer" target="_blank">鄂ICP备2024073355号</a>', // 博客版权信息、备案信息等，支持a标签或换行标签</br>
    },

    // 扩展自动生成frontmatter。（当md文件的frontmatter不存在相应的字段时将自动添加。不会覆盖已有的数据。）
    extendFrontmatter: {
      author: {
        name: '杨充',
        link: 'https://github.com/yangchong211'
      }
    },

    // 自定义hmtl(广告)模块
    htmlModules
  },

  // 注入到页面<head>中的标签，格式[tagName, { attrName: attrValue }, innerHTML?]
  head: [
    ['link', { rel: 'icon', href: '/img/favicon.ico' }], //favicons，资源放在public文件夹
    [
      'meta',
      {
        name: 'keywords',
        content: '前端博客,个人技术博客,前端,前端开发,前端框架,web前端,前端面试题,技术文档,学习,面试,JavaScript,js,ES6,TypeScript,vue,python,css3,html5,Node,git,github,markdown',
      },
    ],
    ['meta', { name: 'baidu-site-verification', content: '7F55weZDDc' }], // 百度统计的站长验证
    ['meta', { name: 'baidu_union_verify', content: '42d5e897c251a660a5bedd8f01cbefa0' }], // 百度联盟验证
    ['meta', { name: 'theme-color', content: '#11a8cd' }], // 移动浏览器主题颜色
  ],


  // 插件配置
  plugins: <UserPlugins>[
    [
      "sitemap", // 网站地图
      {
        hostname: WEB_SITE,
      },
    ],

    'vuepress-plugin-baidu-autopush', // 百度自动推送

    [
      'vuepress-plugin-baidu-tongji', // 百度统计
      {
        hm: baiduCode,
      },
    ],

    // 全文搜索。 ⚠️注意：此插件会在打开网站时多加载部分js文件用于搜索，导致初次访问网站变慢。如在意初次访问速度的话可以不使用此插件！（推荐：vuepress-plugin-thirdparty-search）
    // 'fulltext-search',

    // 可以添加第三方搜索链接的搜索框（继承原官方搜索框的配置参数）
    [
      'thirdparty-search',
      {
        thirdparty: [
          {
            title: '在MDN中搜索',
            frontUrl: 'https://developer.mozilla.org/zh-CN/search?q=', // 搜索链接的前面部分
            behindUrl: '', // 搜索链接的后面部分，可选，默认 ''
          },
          {
            title: '在Runoob中搜索',
            frontUrl: 'https://www.runoob.com/?s=',
          },
          {
            title: '在Vue API中搜索',
            frontUrl: 'https://cn.vuejs.org/v2/api/#',
          },
          {
            title: '在Bing中搜索',
            frontUrl: 'https://cn.bing.com/search?q=',
          },
          {
            title: '通过百度搜索本站的',
            frontUrl: `https://www.baidu.com/s?wd=site%3A${DOMAIN_NAME}%20`,
          },
        ],
      }
    ],

    [
      'one-click-copy', // 代码块复制按钮
      {
        copySelector: ['div[class*="language-"] pre', 'div[class*="aside-code"] aside'], // String or Array
        copyMessage: '复制成功，最多100字', // default is 'Copy successfully and then paste it for use.'
        duration: 1000, // prompt message display time.
        showInMobile: false, // whether to display on the mobile side, default: false.
      },
    ],

    [
      'demo-block', // demo演示模块 https://github.com/xiguaxigua/vuepress-plugin-demo-block
      {
        settings: {
          // jsLib: ['http://xxx'], // 在线示例(jsfiddle, codepen)中的js依赖
          // cssLib: ['http://xxx'], // 在线示例中的css依赖
          // vue: 'https://jsd.cdn.zzko.cn/npm/vue/dist/vue.min.js', // 在线示例中的vue依赖
          jsfiddle: false, // 是否显示 jsfiddle 链接
          codepen: true, // 是否显示 codepen 链接
          horizontal: false, // 是否展示为横向样式
        },
      },
    ],
    [
      'vuepress-plugin-zooming', // 放大图片
      {
        selector: '.theme-vdoing-content img:not(.no-zoom)', // 排除class是no-zoom的图片
        options: {
          bgColor: 'rgba(0,0,0,0.6)',
        },
      },
    ],
    [
      'vuepress-plugin-comment', // 评论
      {
        choosen: 'gitalk',
        options: {
          clientID: 'a6e1355287947096b88b',
          clientSecret: 'f0e77d070fabfcd5af95bebb82b2d574d7248d71',
          repo: 'blog-gitalk-comment', // GitHub 仓库
          owner: '杨充', // GitHub仓库所有者
          admin: ['杨充'], // 对仓库有写权限的人
          // distractionFreeMode: true,
          pagerDirection: 'last', // 'first'正序 | 'last'倒序
          id: '<%- (frontmatter.permalink || frontmatter.to.path).slice(-16) %>', //  页面的唯一标识,长度不能超过50
          title: '「评论」<%- frontmatter.title %>', // GitHub issue 的标题
          labels: ['Gitalk', 'Comment'], // GitHub issue 的标签
          body:
            '页面：<%- window.location.origin + (frontmatter.to.path || window.location.pathname) %>', // GitHub issue 的内容
        },
      },
    ],
    [
      '@vuepress/last-updated', // "上次更新"时间格式
      {
        transformer: (timestamp, lang) => {
          return dayjs(timestamp).format('YYYY/MM/DD, HH:mm:ss')
        },
      },
    ],
  ],

  markdown: {
    lineNumbers: false, // 关闭行号加速渲染
    extractHeaders: ['h2', 'h3'], // 仅提取 h2/h3，减少处理开销
  },

  // 只编译到 ES2015+，大幅减少 bundle 体积
  evergreen: true,

  // 禁用 SSR：1000+ 页博客的 SSR 包会超出 JSON.stringify 上限
  chainWebpack(config, isServer) {
    if (isServer) {
      config.plugins.delete('vue-server-renderer-server-plugin')
    }
    // 生产构建：关闭 SourceMap 省内存
    config.devtool(false)
    // 并行线程数：使用 CPU 核心数（预留 1-2 核给系统）
    const cpuCount = os.cpus().length
    config.parallelism(Math.max(2, cpuCount - 1))
    // 开启文件系统缓存，增量构建时复用上次结果
    config.cache({
      type: 'filesystem',
      buildDependencies: {
        config: [__filename],
      },
    })
  },

  // 监听文件变化并重新构建
  extraWatchFiles: [
    '.vuepress/config.ts',
    '.vuepress/config/htmlModules.ts',
  ],

  // 文件匹配模式：FAST_BUILD 模式下排除大目录，加速构建
  // 默认值：['**/*.md', '**/*.vue']
  patterns: FAST_BUILD
    ? [
        '**/*.md',
        '**/*.vue',
        ...FAST_EXCLUDE_DIRS.map(d => `!${d}/**`),
      ]
    : ['**/*.md', '**/*.vue'],
})
