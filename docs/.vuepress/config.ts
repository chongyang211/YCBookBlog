import { resolve } from 'path'
import * as os from 'os'
import { defineConfig4CustomTheme, UserPlugins } from 'vuepress/config'
import { VdoingThemeConfig } from 'vuepress-theme-vdoing/types'
import dayjs from 'dayjs'
import baiduCode from './config/baiduCode' // 百度统计hm码
import htmlModules from './config/htmlModules' // 自定义插入的html块

const DOMAIN_NAME = 'yccoding.com' // 域名 (不带https)
const WEB_SITE = `https://${DOMAIN_NAME}` // 网址

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
    // 导航配置
    nav: [
      { text: '首页', link: '/' },
      {
        text: '工具箱',
        link: '/pages/tools/',
        items: [
          { text: '在线工具',  link: '/pages/tools/' },
          { text: 'JSON工具',  link: '/tools/json/' },
          { text: '文本工具',  link: '/tools/text/' },
          { text: '图片处理',  link: '/tools/image/' },
          { text: '文档转化',  link: '/tools/doc/' },
          { text: '代码压缩',  link: '/tools/minify/' },
          { text: '加解密',    link: '/tools/crypto/' },
          { text: '时间日期',  link: '/tools/time/' },
          { text: '网络工具',  link: '/tools/network/' },
          { text: '颜色设计',  link: '/tools/color/' },
          { text: '二维码',    link: '/tools/qrcode/' },
          { text: '开发实用',  link: '/tools/dev/' },
        ],
      },
      {
        text: '计算机',
        link: '/pages/computer/',
        items: [
          { text: '计算机的原理', link: '/pages/arch/' },
          { text: '操作系统原理', link: '/pages/os/' },
          { text: '网络协议原理', link: '/pages/network/' },
          { text: '数据库的原理', link: '/pages/database/' },
        ],
      },
      {
        text: '内功',
        link: '/pages/27cbb6/',
        items: [
          { text: '序卷导读', link: '/pages/27cbb6/' },
          { text: '数据本质', link: '/pages/27269c/' },
          { text: '运行模型', link: '/pages/74953e/' },
          { text: '并发设计', link: '/pages/0554f8/' },
          { text: '内存真相', link: '/pages/a606b7/' },
          { text: '交互系统', link: '/pages/2f009c/' },
        ],
      },
      {
        text: '编程',
        link: '/programming/',
        items: [
          { text: '面向对象', link: '/pages/object/' },
          { text: '设计原则', link: '/pages/principles/' },
          { text: '设计模式', link: '/pages/patterns/' },
          { text: '系统架构', link: '/pages/architecture/' },
          { text: '技能之旅', link: '/pages/growth/' },
        ],
      },
      {
        text: '真经',
        link: '/column/',
        items: [
          { text: '体系建设', link: '/pages/system/' },
          { text: '代码品质', link: '/pages/quality/' },
          { text: '方案设计', link: '/pages/solution/' },
          { text: '稳定可靠', link: '/pages/stability/' },
          { text: '工程运维', link: '/pages/devops/' },
          { text: '性能优化', link: '/pages/perf/' },
        ],
      },
      {
        text: '算法',
        link: '/algorithm/',
        items: [
          { text: '数据结构导论', link: '/pages/algobase/' },
          { text: '线性结构详解', link: '/pages/linear/' },
          { text: '树哈希结构论', link: '/pages/tree/' },
          { text: '容器设计实战', link: '/pages/industrial/' },
          { text: '经典算法思想', link: '/pages/thinking/' },
          { text: '工程案例剖析', link: '/pages/practice/' },
          { text: '算法题库精练', link: '/pages/problems/' },
        ],
      },
      {
        text: 'Code',
        link: '/codex/',
        items: [
          { text: 'C语言入门', link: '/pages/c/' },
          { text: 'C综合案例', link: '/pages/ccase/' },
          { text: 'C专栏博客', link: '/pages/cblog/' },
          { text: 'C标准集库', link: '/pages/clib/' },
          { text: 'C++入门教程', link: '/pages/cpp/' },
          { text: 'C++综合案例', link: '/pages/cppcase/' },
          { text: 'C++专栏博客', link: '/pages/cppblog/' },
          { text: 'C++编程技巧', link: '/pages/cppsignal/' },
          { text: 'Java入门教程', link: '/pages/java/' },
          { text: 'Java综合案例', link: '/pages/javacase/' },
          { text: 'Java专栏博客', link: '/pages/javablog/' },
          { text: 'Go入门教程', link: '/pages/go/' },
          { text: 'Go综合案例', link: '/pages/gocase/' },
          { text: 'Go专栏博客', link: '/pages/goblog/' },
          { text: 'Go开发技巧', link: '/pages/gotips/' },
          { text: 'JavaScript入门', link: '/pages/js/' },
          { text: 'JavaScript案例', link: '/pages/jscases/' },
          { text: 'JavaScript高级', link: '/pages/jsadv/' },
        ],
      },
      {
        text: 'Apps',
        link: '/app/',
        items: [
          { text: 'Kotlin精通', link: '/pages/6314d5/' },
          { text: 'Android库解读', link: '/pages/android/' },
          { text: 'Android专栏', link: '/pages/androidblog/' },
          { text: 'iOS ObjC入门', link: '/pages/ios/' },
          { text: 'iOS Swift入门', link: '/pages/swift/' },
          { text: 'iOS入门精通', link: '/pages/iosapp/' },
          { text: 'Web之Html手册', link: '/pages/html/' },
          { text: 'Web之TypeScript', link: '/pages/ts/' },
          { text: 'Web之Vue高级进阶', link: '/pages/vue/' },
          { text: 'Linux之QML入门', link: '/pages/qml/' },
          { text: 'Linux之QT核心库', link: '/pages/qt/' },
        ],
      },
      {
        text: 'ScriptHub',
        link: '/scripthub/',
        items: [
          { text: 'Python教程', link: '/pages/python/' },
          { text: 'Shell&Bash教程', link: '/pages/shell/' },
          { text: '工具脚本', link: '/pages/scripts/' },
          { text: '自动化脚本', link: '/pages/auto/' },
        ],
      },
      {
        text: '技术',
        link: '/technology/',
        items: [
          { text: '质量保障', link: '/pages/biz/' },
          { text: '产品思考', link: '/pages/product/' },
          { text: '软实力', link: '/pages/soft/' },
          { text: '开发流程', link: '/pages/devflow/' },
          { text: 'Git应用', link: '/pages/git/' },
          { text: '技术模版', link: '/pages/template/' },
          { text: '技术规范', link: '/pages/spec/' },
          { text: 'Markdown', link: '/pages/md/' },
          { text: 'Mermaid', link: '/pages/mermaid/' },
          { text: '开源协议', link: '/pages/license/' },
        ],
      },
      {
        text: '书籍',
        link: '/pages/maoism/',
        items: [
          { text: '毛选解读', link: '/pages/maoism/' },
          { text: '自我精进', link: '/pages/nobody/' },
        ],
      },
      {
        text: '更多',
        link: '/more/',
        items: [
          { text: '关于我', link: '/pages/about/' },
          { text: '自我精进', link: '/pages/improve/' },
          { text: '职场管理', link: '/pages/work/' },
          { text: '职场面试', link: '/pages/interview/' },
          { text: '心情杂货', link: '/pages/mood/' },
          { text: '友情链接', link: '/pages/about/' },
        ],
      },

    ],
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
      link: 'https://yccoding.com/', // 可选的
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
          link: 'https://yccoding.com/',
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
        '杨充 | <a href="https://yccoding.com/vuepress-theme-vdoing/blob/master/LICENSE" target="_blank">MIT License</a> | <a href="https://beian.miit.gov.cn/" target="_blank">鄂ICP备2024073355号-1</a> | <img src="/img/beian.png" style="width: 15px; margin-bottom: -3px;" /> <a href="https://beian.mps.gov.cn/#/query/webSearch?code=45142202000030" rel="noreferrer" target="_blank">鄂ICP备2024073355号</a>', // 博客版权信息、备案信息等，支持a标签或换行标签</br>
    },

    // 扩展自动生成frontmatter。（当md文件的frontmatter不存在相应的字段时将自动添加。不会覆盖已有的数据。）
    extendFrontmatter: {
      author: {
        name: '杨充',
        link: 'https://yccoding.com/'
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
    // Google AdSense（异步加载，不阻塞首屏）
    ['script', { async: true, src: 'https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=ca-pub-2772800276709949', crossorigin: 'anonymous' }],
  ],


  // 插件配置
  plugins: <UserPlugins>[
    [
      'vuepress-plugin-mermaidjs', // mermaid 流程图渲染（GitHub 风格 ```mermaid 代码块）
      {
        // mermaid 配置项参考：https://mermaid.js.org/config/schema-docs/config.html
        theme: 'default',
        themeVariables: {
          fontFamily: 'inherit',
        },
        flowchart: {
          htmlLabels: true,
          curve: 'basis',
        },
        securityLevel: 'loose',
      },
    ],

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
    // 强制 mermaid 解析到根目录的版本，否则 vuepress-plugin-mermaidjs 会用自带的 8.x
    config.resolve.alias.set('mermaid', resolve(__dirname, '../../node_modules/mermaid'))

    // mermaid 9.x 的 dist 使用了 ES2020 语法（??、?.），webpack 4 的 acorn 6 解析不了
    // 需要用 babel-loader 转换它（默认 babel 规则 exclude 了 node_modules，这里单独配置）
    config.module
      .rule('mermaid-babel')
      .test(/\.m?js$/)
      .include
        .add(resolve(__dirname, '../../node_modules/mermaid'))
        .end()
      .use('babel-loader')
        .loader('babel-loader')
        .options({
          babelrc: false,
          configFile: false,
          presets: [
            [resolve(__dirname, '../../node_modules/@babel/preset-env'), { targets: { esmodules: true } }],
          ],
          cacheDirectory: true,
        })

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

  // 文件匹配模式（默认值）
  patterns: ['**/*.md', '**/*.vue'],
})
