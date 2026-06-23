<template>
  <div class="dt-panel">
    <div class="dt-toolbar">
      <div class="dt-field" style="flex:1; min-width:0;">
        <span style="color:#888; font-size:12px;">
          支持粘贴富文本网页 HTML，自动转 Markdown。&lt;script&gt;/&lt;style&gt; 会被剔除。
        </span>
      </div>
      <div class="dt-actions">
        <button class="dt-btn" @click="loadSample">示例</button>
        <button class="dt-btn" @click="clearAll">清空</button>
        <button class="dt-btn dt-btn-primary" :disabled="!md" @click="exportMd">导出 .md</button>
      </div>
    </div>

    <div class="dt-grid-2">
      <div class="dt-io">
        <div class="dt-io-head">
          <span>HTML 输入</span>
          <span class="dt-io-cnt">{{ input.length }} 字符</span>
          <button class="dt-mini" @click="copy(input, 'in')">{{ copyHint==='in' ? '已复制' : '复制' }}</button>
        </div>
        <textarea
          v-model="input"
          placeholder='<h1>标题</h1>&#10;<p>段落，<strong>粗体</strong>，<a href="https://example.com">链接</a>。</p>'
          spellcheck="false"
        ></textarea>
      </div>

      <div class="dt-io">
        <div class="dt-io-head">
          <span>Markdown 输出</span>
          <span class="dt-io-cnt">{{ md.length }} 字符</span>
          <button class="dt-mini" @click="copy(md, 'out')" v-if="md">{{ copyHint==='out' ? '已复制' : '复制' }}</button>
        </div>
        <textarea :value="md" readonly spellcheck="false"></textarea>
      </div>
    </div>
  </div>
</template>

<script>
import { htmlToMd, copyMixin } from './utils.js'

export default {
  name: 'HtmlToMdPanel',
  mixins: [copyMixin],
  data() { return { input: '' } },
  computed: {
    md() {
      try { return htmlToMd(this.input) } catch (e) { return '转换失败：' + e.message }
    },
  },
  methods: {
    loadSample() {
      this.input = `<h1>示例文章</h1>
<p>这是一段 <strong>粗体文字</strong> 与 <em>斜体</em>，还有 <code>inline code</code>。</p>
<h2>列表</h2>
<ul>
  <li>第一项</li>
  <li>第二项 <a href="https://example.com">链接</a></li>
</ul>
<pre><code class="lang-js">const x = 1
console.log(x)
</code></pre>
<blockquote><p>这是引用</p></blockquote>
<table>
  <thead><tr><th>列1</th><th>列2</th></tr></thead>
  <tbody><tr><td>a</td><td>b</td></tr><tr><td>c</td><td>d</td></tr></tbody>
</table>`
    },
    clearAll() { this.input = '' },
    exportMd() { this.download(this.md, 'document.md', 'text/markdown') },
  },
}
</script>
