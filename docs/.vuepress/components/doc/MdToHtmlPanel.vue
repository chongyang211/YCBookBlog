<template>
  <div class="dt-panel">
    <div class="dt-toolbar">
      <div class="dt-field">
        <label>视图</label>
        <select v-model="viewMode">
          <option value="split">分屏预览</option>
          <option value="html">查看 HTML 源码</option>
        </select>
      </div>
      <div class="dt-actions">
        <button class="dt-btn" @click="loadSample">示例</button>
        <button class="dt-btn" @click="clearAll">清空</button>
        <button class="dt-btn dt-btn-primary" :disabled="!html" @click="exportHtml">导出 .html</button>
      </div>
    </div>

    <div class="dt-grid-2">
      <div class="dt-io">
        <div class="dt-io-head">
          <span>Markdown 输入</span>
          <span class="dt-io-cnt">{{ input.length }} 字符</span>
          <button class="dt-mini" @click="copy(input, 'in')">{{ copyHint==='in' ? '已复制' : '复制' }}</button>
        </div>
        <textarea
          v-model="input"
          placeholder="# 标题&#10;&#10;**粗体** 与 *斜体*，行内 `code` 与代码块：&#10;&#10;```js&#10;console.log('hi')&#10;```"
          spellcheck="false"
        ></textarea>
      </div>

      <div class="dt-io">
        <div class="dt-io-head">
          <span>{{ viewMode === 'html' ? 'HTML 源码' : '预览' }}</span>
          <span class="dt-io-cnt">{{ html.length }} 字符</span>
          <button class="dt-mini" @click="copy(html, 'out')" v-if="html">{{ copyHint==='out' ? '已复制' : '复制 HTML' }}</button>
        </div>
        <textarea
          v-if="viewMode === 'html'"
          :value="html"
          readonly
          spellcheck="false"
        ></textarea>
        <div v-else class="dt-preview" v-html="html"></div>
      </div>
    </div>
  </div>
</template>

<script>
import { mdToHtml, copyMixin, escapeHtml } from './utils.js'

export default {
  name: 'MdToHtmlPanel',
  mixins: [copyMixin],
  data() {
    return {
      input: '',
      viewMode: 'split',
    }
  },
  computed: {
    html() {
      try { return mdToHtml(this.input) } catch (e) { return `<p style="color:#c0392b">解析失败：${escapeHtml(e.message)}</p>` }
    },
  },
  methods: {
    loadSample() {
      this.input = `# 项目名称

> 一句话项目介绍

## 特性
- **快**：基于 \`Vue\` 实现
- *易用*：开箱即用
- ~~过时~~ 现代化设计

## 代码示例
\`\`\`js
function hello(name) {
  return 'Hello, ' + name
}
\`\`\`

## 对比表
| 项目 | A | B |
| --- | :---: | ---: |
| 速度 | 快 | 慢 |
| 体积 | 小 | 大 |

> 引用区块：> 开头

---

更多见 [官网](https://example.com)
`
    },
    clearAll() { this.input = '' },
    exportHtml() {
      if (!this.html) return
      const full = `<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8"/>
<title>Exported Document</title>
<style>
body { max-width: 760px; margin: 40px auto; padding: 0 16px; font-family: -apple-system, 'PingFang SC', sans-serif; line-height: 1.7; color: #333; }
pre { background: #2d2d2d; color: #f0f0f0; padding: 12px; border-radius: 6px; overflow-x: auto; }
code { background: #f3f3f3; color: #d63384; padding: 1px 6px; border-radius: 4px; font-family: 'JetBrains Mono', Menlo, monospace; font-size: 0.9em; }
pre code { background: transparent; color: inherit; padding: 0; }
blockquote { margin: 0.6em 0; padding: 6px 14px; border-left: 4px solid #4a90d9; background: #f7faff; color: #555; }
table { border-collapse: collapse; }
th, td { border: 1px solid #e0e0e0; padding: 6px 10px; }
th { background: #f7f7f9; }
img { max-width: 100%; }
</style>
</head>
<body>
${this.html}
</body>
</html>`
      this.download(full, 'document.html', 'text/html')
    },
  },
}
</script>
