import { defineUserConfig } from 'vuepress'
import { viteBundler } from '@vuepress/bundler-vite'
import { hopeTheme } from 'vuepress-theme-hope'

export default defineUserConfig({
  bundler: viteBundler(),

  lang: 'zh-CN',
  title: "Evan's blog",
  description: 'web前端技术博客,专注web前端学习与总结。JavaScript,js,ES6,TypeScript,vue,React,python,css3,html5,Node,git,github等技术文章。',

  head: [
    ['link', { rel: 'icon', href: '/img/favicon.ico' }],
    ['meta', { name: 'keywords', content: '前端博客,个人技术博客,前端,前端开发,前端框架,web前端,前端面试题,技术文档,学习,面试,JavaScript,js,ES6,TypeScript,vue,python,css3,html5,Node,git,github,markdown' }],
    ['meta', { name: 'baidu-site-verification', content: '7F55weZDDc' }],
    ['meta', { name: 'theme-color', content: '#11a8cd' }],
  ],

  markdown: {
    headers: {
      level: [2, 3, 4, 5, 6],
    },
  },

  theme: hopeTheme({
    hostname: 'https://xugaoyi.com',

    author: {
      name: 'xugaoyi',
      url: 'https://github.com/xugaoyi',
    },

    logo: '/img/logo.png',
    repo: 'xugaoyi/vuepress-theme-vdoing',

    docsDir: 'docs',
    editLink: true,
    editLinkPattern: ':repo/edit/:branch/:path',

    lastUpdated: true,

    // 导航栏
    navbar: [
      { text: '首页', link: '/' },
      {
        text: '前端',
        children: [
          {
            text: '前端文章',
            children: [
              { text: 'JavaScript', link: '/pages/8143cc480faf9a11/' },
            ],
          },
          {
            text: '学习笔记',
            children: [
              { text: '《JavaScript教程》', link: '/note/javascript/' },
              { text: '《JavaScript高级程序设计》', link: '/note/js/' },
              { text: '《ES6 教程》', link: '/note/es6/' },
              { text: '《Vue》', link: '/note/vue/' },
              { text: '《React》', link: '/note/react/' },
              { text: '《TypeScript 从零实现 axios》', link: '/note/typescript-axios/' },
              { text: '《Git》', link: '/note/git/' },
              { text: 'TypeScript', link: '/pages/51afd6/' },
              { text: 'JS设计模式总结', link: '/pages/4643cd/' },
            ],
          },
        ],
      },
      {
        text: '页面',
        children: [
          { text: 'HTML', link: '/pages/8309a5b876fc95e3/' },
          { text: 'CSS', link: '/pages/0a83b083bdf257cb/' },
        ],
      },
      {
        text: '技术',
        children: [
          { text: '技术文档', link: '/pages/9a7ee40fc232253e/' },
          { text: 'GitHub技巧', link: '/pages/4c778760be26d8b3/' },
          { text: 'Nodejs', link: '/pages/117708e0af7f0bd9/' },
          { text: '博客搭建', link: '/pages/41f87d890d0a02af/' },
        ],
      },
      {
        text: '更多',
        children: [
          { text: '学习', link: '/pages/f2a556/' },
          { text: '面试', link: '/pages/aea6571b7a8bae86/' },
          { text: '心情杂货', link: '/pages/2d615df9a36a98ed/' },
          { text: '实用技巧', link: '/pages/baaa02/' },
          { text: '友情链接', link: '/friends/' },
        ],
      },
      { text: '关于', link: '/about/' },
      { text: '收藏', link: '/pages/beb6c0bd8a66cea6/' },
      {
        text: '索引',
        children: [
          { text: '分类', link: '/categories/' },
          { text: '标签', link: '/tags/' },
          { text: '归档', link: '/archives/' },
        ],
      },
    ],

    // 侧边栏
    sidebar: 'structure',

    // 博客配置
    blog: {
      name: 'Evan Xu',
      avatar: 'https://jsd.cdn.zzko.cn/gh/xugaoyi/image_store/blog/20200103123203.jpg',
      description: '前端界的小学生',
      intro: '/about/',
      medias: {
        Email: 'mailto:894072666@qq.com',
        GitHub: 'https://github.com/xugaoyi',
      },
    },

    // 页脚
    footer: 'Evan Xu | <a href="https://github.com/xugaoyi/vuepress-theme-vdoing/blob/master/LICENSE" target="_blank">MIT License</a>',
    copyright: 'Copyright © 2019-present Evan Xu',
    displayFooter: true,

    // 插件配置
    plugins: {
      // 搜索
      search: true,
      searchMaxSuggestions: 10,

      // 评论 (Gitalk)
      comment: {
        provider: 'Giscus',
      },

      // 站点地图
      sitemap: {
        hostname: 'https://xugaoyi.com',
      },

      // 代码复制
      copyCode: {},

      // 图片放大
      mediumZoom: true,

      // SEO
      seo: {
        hostname: 'https://xugaoyi.com',
        author: {
          name: 'xugaoyi',
        },
      },

      // 百度统计
      components: {
        rootComponents: {
          baiduTongji: '7F55weZDDc',
        },
      },
    },
  }),
})
