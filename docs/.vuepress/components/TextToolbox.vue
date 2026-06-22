<template>
  <div class="tt-wrapper">
    <!-- Header -->
    <div class="tt-header">
      <h2 class="tt-title">文本处理工具箱 · 浏览器端</h2>
      <p class="tt-desc">编码转换 · 正则测试 · 文本对比 · 字数统计 · 大小写转换，所有计算在本地完成，无需上传</p>
    </div>

    <!-- Tabs -->
    <div class="tt-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="tt-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span class="tt-tab-icon" v-html="tab.icon"></span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <!-- ============== 编码转换 ============== -->
    <div v-show="active === 'encode'" class="tt-panel">
      <div class="tt-toolbar">
        <div class="tt-field">
          <label>编码类型</label>
          <select v-model="encodeType">
            <option value="url">URL 编码</option>
            <option value="base64">Base64</option>
            <option value="unicode">Unicode ↔ 中文</option>
            <option value="hex">十六进制</option>
            <option value="html">HTML 实体</option>
            <option value="utf8">UTF-8 字节</option>
          </select>
        </div>
        <div class="tt-actions">
          <button class="tt-btn tt-btn-primary" @click="doEncode">↓ 编码</button>
          <button class="tt-btn tt-btn-primary" @click="doDecode">↑ 解码</button>
          <button class="tt-btn" @click="swapEncode">⇅ 交换</button>
          <button class="tt-btn" @click="clearEncode">清空</button>
        </div>
      </div>
      <div class="tt-grid-2">
        <div class="tt-io">
          <div class="tt-io-head">
            <span>原文</span>
            <span class="tt-io-cnt">{{ encodeIn.length }} 字符</span>
            <button class="tt-mini" @click="copy(encodeIn, 'in')">{{ copyHint==='in' ? '已复制' : '复制' }}</button>
          </div>
          <textarea v-model="encodeIn" placeholder="输入要编码或解码的文本..."></textarea>
        </div>
        <div class="tt-io">
          <div class="tt-io-head">
            <span>结果</span>
            <span class="tt-io-cnt">{{ encodeOut.length }} 字符</span>
            <button class="tt-mini" @click="copy(encodeOut, 'out')">{{ copyHint==='out' ? '已复制' : '复制' }}</button>
          </div>
          <textarea v-model="encodeOut" placeholder="结果输出..."></textarea>
        </div>
      </div>
      <p v-if="encodeErr" class="tt-err">⚠ {{ encodeErr }}</p>
    </div>

    <!-- ============== 正则测试 ============== -->
    <div v-show="active === 'regex'" class="tt-panel">
      <div class="tt-regex-bar">
        <span class="tt-regex-slash">/</span>
        <input class="tt-regex-input" v-model="regexPattern" placeholder="正则表达式，例如 \w+@\w+\.\w+" />
        <span class="tt-regex-slash">/</span>
        <input class="tt-regex-flags" v-model="regexFlags" placeholder="gmi" />
      </div>
      <div class="tt-regex-quick">
        <span>常用：</span>
        <button v-for="p in regexPresets" :key="p.label" class="tt-tag" @click="applyPreset(p)">{{ p.label }}</button>
      </div>
      <div class="tt-grid-2">
        <div class="tt-io">
          <div class="tt-io-head">
            <span>测试文本</span>
            <span class="tt-io-cnt">{{ regexMatches.length }} 匹配</span>
          </div>
          <textarea v-model="regexText" placeholder="在这里粘贴要测试的文本..."></textarea>
        </div>
        <div class="tt-io">
          <div class="tt-io-head">
            <span>高亮结果</span>
            <button class="tt-mini" @click="regexShow = regexShow === 'highlight' ? 'list' : 'highlight'">
              {{ regexShow === 'highlight' ? '查看列表' : '查看高亮' }}
            </button>
          </div>
          <div class="tt-regex-result" v-if="regexShow === 'highlight'" v-html="regexHighlighted"></div>
          <div class="tt-regex-result tt-regex-list" v-else>
            <div v-if="regexMatches.length === 0" class="tt-empty">无匹配</div>
            <div v-for="(m, i) in regexMatches" :key="i" class="tt-match-item">
              <span class="tt-match-idx">#{{ i+1 }}</span>
              <span class="tt-match-text">{{ m.value }}</span>
              <span class="tt-match-pos">@{{ m.index }}</span>
            </div>
          </div>
        </div>
      </div>
      <div class="tt-regex-replace">
        <input class="tt-input" v-model="regexReplace" placeholder="替换为（支持 $1 $2 反向引用）" />
        <button class="tt-btn tt-btn-primary" @click="doRegexReplace">执行替换</button>
        <button class="tt-mini" v-if="regexReplaced" @click="copy(regexReplaced, 'rep')">{{ copyHint==='rep' ? '已复制' : '复制结果' }}</button>
      </div>
      <textarea v-if="regexReplaced" class="tt-replace-out" v-model="regexReplaced" readonly></textarea>
      <p v-if="regexErr" class="tt-err">⚠ {{ regexErr }}</p>
    </div>

    <!-- ============== 文本对比 ============== -->
    <div v-show="active === 'diff'" class="tt-panel">
      <div class="tt-toolbar">
        <div class="tt-field">
          <label>对比模式</label>
          <select v-model="diffMode">
            <option value="line">按行对比</option>
            <option value="word">按词对比</option>
            <option value="char">按字符对比</option>
          </select>
        </div>
        <div class="tt-field tt-check-field">
          <label>
            <input type="checkbox" v-model="diffIgnoreSpace" /> 忽略空白
          </label>
          <label>
            <input type="checkbox" v-model="diffIgnoreCase" /> 忽略大小写
          </label>
        </div>
        <div class="tt-actions">
          <button class="tt-btn tt-btn-primary" @click="doDiff">开始对比</button>
          <button class="tt-btn" @click="clearDiff">清空</button>
        </div>
      </div>
      <div class="tt-grid-2">
        <div class="tt-io">
          <div class="tt-io-head"><span>文本 A（原文）</span><span class="tt-io-cnt">{{ diffA.length }} 字符</span></div>
          <textarea v-model="diffA" placeholder="粘贴第一段文本..."></textarea>
        </div>
        <div class="tt-io">
          <div class="tt-io-head"><span>文本 B（新文）</span><span class="tt-io-cnt">{{ diffB.length }} 字符</span></div>
          <textarea v-model="diffB" placeholder="粘贴第二段文本..."></textarea>
        </div>
      </div>
      <div v-if="diffResult.length > 0" class="tt-diff-summary">
        <span class="tt-diff-add">+ {{ diffStats.add }} 新增</span>
        <span class="tt-diff-del">- {{ diffStats.del }} 删除</span>
        <span class="tt-diff-eq">= {{ diffStats.eq }} 相同</span>
      </div>
      <div v-if="diffResult.length > 0" class="tt-diff-output">
        <div
          v-for="(d, i) in diffResult"
          :key="i"
          class="tt-diff-line"
          :class="'tt-diff-' + d.type"
        >
          <span class="tt-diff-mark">{{ d.type === 'add' ? '+' : d.type === 'del' ? '-' : ' ' }}</span>
          <span class="tt-diff-content">{{ d.value }}</span>
        </div>
      </div>
    </div>

    <!-- ============== 字数统计 ============== -->
    <div v-show="active === 'count'" class="tt-panel">
      <div class="tt-io tt-io-full">
        <div class="tt-io-head">
          <span>输入文本</span>
          <button class="tt-mini" @click="countText = ''">清空</button>
          <button class="tt-mini" @click="copy(countText, 'cnt')">{{ copyHint==='cnt' ? '已复制' : '复制' }}</button>
        </div>
        <textarea v-model="countText" placeholder="在此输入或粘贴文本，统计实时更新..." class="tt-textarea-large"></textarea>
      </div>
      <div class="tt-stat-grid">
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.chars }}</div>
          <div class="tt-stat-lbl">字符总数</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.charsNoSpace }}</div>
          <div class="tt-stat-lbl">不含空白</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.cn }}</div>
          <div class="tt-stat-lbl">中文字符</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.en }}</div>
          <div class="tt-stat-lbl">英文单词</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.digits }}</div>
          <div class="tt-stat-lbl">数字字符</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.lines }}</div>
          <div class="tt-stat-lbl">行数</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.paragraphs }}</div>
          <div class="tt-stat-lbl">段落数</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ formatBytes(countStats.bytes) }}</div>
          <div class="tt-stat-lbl">UTF-8 字节</div>
        </div>
        <div class="tt-stat-card">
          <div class="tt-stat-val">{{ countStats.readTime }}</div>
          <div class="tt-stat-lbl">阅读时长（分钟）</div>
        </div>
      </div>
    </div>

    <!-- ============== 大小写转换 ============== -->
    <div v-show="active === 'case'" class="tt-panel">
      <div class="tt-case-bar">
        <button class="tt-case-btn" v-for="c in caseOps" :key="c.key" @click="applyCase(c.key)">
          {{ c.label }}
        </button>
      </div>
      <div class="tt-grid-2">
        <div class="tt-io">
          <div class="tt-io-head">
            <span>原文</span>
            <span class="tt-io-cnt">{{ caseIn.length }} 字符</span>
          </div>
          <textarea v-model="caseIn" placeholder="输入要转换的文本..."></textarea>
        </div>
        <div class="tt-io">
          <div class="tt-io-head">
            <span>结果</span>
            <span class="tt-io-cnt">{{ caseOut.length }} 字符</span>
            <button class="tt-mini" @click="copy(caseOut, 'case')">{{ copyHint==='case' ? '已复制' : '复制' }}</button>
          </div>
          <textarea v-model="caseOut" placeholder="结果输出..."></textarea>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'TextToolbox',
  data() {
    return {
      active: 'encode',
      copyHint: '',
      tabs: [
        { key: 'encode', label: '编码转换', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><polyline points="16 18 22 12 16 6"/><polyline points="8 6 2 12 8 18"/></svg>' },
        { key: 'regex',  label: '正则测试', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M17 3v6M14 6h6M21 13.5a1.5 1.5 0 1 1-3 0 1.5 1.5 0 0 1 3 0zM3 3h8v8H3z"/></svg>' },
        { key: 'diff',   label: '文本对比', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M9 3H4v18h5M15 3h5v18h-5M12 6v12"/></svg>' },
        { key: 'count',  label: '字数统计', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><line x1="4" y1="6" x2="20" y2="6"/><line x1="4" y1="12" x2="20" y2="12"/><line x1="4" y1="18" x2="14" y2="18"/></svg>' },
        { key: 'case',   label: '大小写', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M4 20l4-12 4 12M5 16h6M14 20V8M14 8a4 4 0 0 1 4 4 4 4 0 0 1-4 4"/></svg>' },
      ],

      // ===== 编码转换 =====
      encodeType: 'url',
      encodeIn: '',
      encodeOut: '',
      encodeErr: '',

      // ===== 正则 =====
      regexPattern: '\\w+@\\w+\\.\\w+',
      regexFlags: 'g',
      regexText: '联系邮箱: hello@example.com, support@yc-blog.dev\n反馈邮箱: feedback@yangchong.com',
      regexReplace: '',
      regexReplaced: '',
      regexErr: '',
      regexShow: 'highlight',
      regexPresets: [
        { label: '邮箱',   pattern: '[\\w.+-]+@[\\w-]+\\.[\\w.-]+', flags: 'g' },
        { label: '手机号', pattern: '1[3-9]\\d{9}',                   flags: 'g' },
        { label: 'URL',    pattern: 'https?:\\/\\/[^\\s]+',          flags: 'g' },
        { label: 'IP',     pattern: '\\b(?:\\d{1,3}\\.){3}\\d{1,3}\\b', flags: 'g' },
        { label: '日期',   pattern: '\\d{4}-\\d{2}-\\d{2}',           flags: 'g' },
        { label: '中文',   pattern: '[\\u4e00-\\u9fa5]+',              flags: 'g' },
        { label: '数字',   pattern: '-?\\d+(?:\\.\\d+)?',              flags: 'g' },
      ],

      // ===== 对比 =====
      diffMode: 'line',
      diffIgnoreSpace: false,
      diffIgnoreCase: false,
      diffA: '',
      diffB: '',
      diffResult: [],

      // ===== 字数统计 =====
      countText: '',

      // ===== 大小写 =====
      caseIn: '',
      caseOut: '',
      caseOps: [
        { key: 'upper',   label: '全大写  ABC' },
        { key: 'lower',   label: '全小写  abc' },
        { key: 'title',   label: '首字母大写  Abc' },
        { key: 'sentence',label: '句首大写  Hello world.' },
        { key: 'camel',   label: '驼峰  helloWorld' },
        { key: 'pascal',  label: '大驼峰  HelloWorld' },
        { key: 'snake',   label: '下划线  hello_world' },
        { key: 'kebab',   label: '中划线  hello-world' },
        { key: 'constant',label: '常量  HELLO_WORLD' },
        { key: 'reverse', label: '反转  cba' },
      ],
    }
  },

  computed: {
    // ===== 正则匹配 =====
    regex() {
      if (!this.regexPattern) return null
      try {
        const re = new RegExp(this.regexPattern, this.regexFlags || '')
        this.regexErr = ''
        return re
      } catch (e) {
        this.regexErr = '正则语法错误：' + e.message
        return null
      }
    },
    regexMatches() {
      if (!this.regex || !this.regexText) return []
      const matches = []
      const re = this.regex
      if (re.global) {
        let m
        re.lastIndex = 0
        while ((m = re.exec(this.regexText)) !== null) {
          matches.push({ value: m[0], index: m.index, groups: m.slice(1) })
          if (m.index === re.lastIndex) re.lastIndex++
        }
      } else {
        const m = re.exec(this.regexText)
        if (m) matches.push({ value: m[0], index: m.index, groups: m.slice(1) })
      }
      return matches
    },
    regexHighlighted() {
      if (!this.regex || !this.regexText) return this.escapeHtml(this.regexText)
      if (this.regexMatches.length === 0) return this.escapeHtml(this.regexText)
      let html = ''
      let last = 0
      for (const m of this.regexMatches) {
        html += this.escapeHtml(this.regexText.slice(last, m.index))
        html += `<mark class="tt-hl">${this.escapeHtml(m.value)}</mark>`
        last = m.index + m.value.length
      }
      html += this.escapeHtml(this.regexText.slice(last))
      return html.replace(/\n/g, '<br/>')
    },

    // ===== 字数统计 =====
    countStats() {
      const txt = this.countText || ''
      const chars = txt.length
      const charsNoSpace = txt.replace(/\s+/g, '').length
      const cn = (txt.match(/[\u4e00-\u9fa5]/g) || []).length
      const en = (txt.match(/[a-zA-Z]+/g) || []).length
      const digits = (txt.match(/\d/g) || []).length
      const lines = txt === '' ? 0 : txt.split(/\r?\n/).length
      const paragraphs = txt.trim() === '' ? 0 : txt.trim().split(/\n\s*\n/).length
      let bytes = 0
      try { bytes = new TextEncoder().encode(txt).length } catch (e) { bytes = txt.length }
      // 中文 300 字/分钟、英文 200 词/分钟，混合估算
      const minutes = cn / 300 + en / 200
      const readTime = minutes < 1 ? '< 1' : Math.ceil(minutes).toString()
      return { chars, charsNoSpace, cn, en, digits, lines, paragraphs, bytes, readTime }
    },

    // ===== Diff 统计 =====
    diffStats() {
      const stats = { add: 0, del: 0, eq: 0 }
      for (const d of this.diffResult) stats[d.type]++
      return stats
    },
  },

  methods: {
    // ===== 工具 =====
    copy(txt, key) {
      if (!txt) return
      const ok = (() => {
        try {
          if (navigator.clipboard && window.isSecureContext) {
            navigator.clipboard.writeText(txt)
            return true
          }
        } catch (e) {}
        const ta = document.createElement('textarea')
        ta.value = txt
        ta.style.position = 'fixed'
        ta.style.opacity = '0'
        document.body.appendChild(ta)
        ta.select()
        try { document.execCommand('copy') } catch (e) {}
        document.body.removeChild(ta)
        return true
      })()
      if (ok) {
        this.copyHint = key
        setTimeout(() => { if (this.copyHint === key) this.copyHint = '' }, 1200)
      }
    },
    escapeHtml(s) {
      return String(s)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#39;')
    },
    formatBytes(b) {
      if (b < 1024) return b + ' B'
      if (b < 1024 * 1024) return (b / 1024).toFixed(2) + ' KB'
      return (b / 1024 / 1024).toFixed(2) + ' MB'
    },

    // ===== 编码转换 =====
    doEncode() {
      this.encodeErr = ''
      try {
        const v = this.encodeIn
        if (!v) { this.encodeOut = ''; return }
        switch (this.encodeType) {
          case 'url':     this.encodeOut = encodeURIComponent(v); break
          case 'base64':  this.encodeOut = this.b64Encode(v); break
          case 'unicode': this.encodeOut = v.split('').map(c => '\\u' + c.charCodeAt(0).toString(16).padStart(4, '0')).join(''); break
          case 'hex':     this.encodeOut = Array.from(new TextEncoder().encode(v)).map(b => b.toString(16).padStart(2, '0')).join(' '); break
          case 'html':    this.encodeOut = v.replace(/[\u00A0-\u9999<>&'"]/g, c => '&#' + c.charCodeAt(0) + ';'); break
          case 'utf8':    this.encodeOut = Array.from(new TextEncoder().encode(v)).join(' '); break
        }
      } catch (e) { this.encodeErr = '编码失败：' + e.message }
    },
    doDecode() {
      this.encodeErr = ''
      try {
        const v = this.encodeIn
        if (!v) { this.encodeOut = ''; return }
        switch (this.encodeType) {
          case 'url':     this.encodeOut = decodeURIComponent(v); break
          case 'base64':  this.encodeOut = this.b64Decode(v); break
          case 'unicode': this.encodeOut = v.replace(/\\u([\da-fA-F]{4})/g, (_, n) => String.fromCharCode(parseInt(n, 16))); break
          case 'hex': {
            const bytes = v.trim().split(/\s+/).map(h => parseInt(h, 16))
            this.encodeOut = new TextDecoder().decode(new Uint8Array(bytes)); break
          }
          case 'html':    this.encodeOut = v.replace(/&#(\d+);/g, (_, n) => String.fromCharCode(parseInt(n, 10)))
                                            .replace(/&amp;/g, '&').replace(/&lt;/g, '<').replace(/&gt;/g, '>')
                                            .replace(/&quot;/g, '"').replace(/&#39;/g, "'"); break
          case 'utf8': {
            const bytes = v.trim().split(/\s+/).map(Number)
            this.encodeOut = new TextDecoder().decode(new Uint8Array(bytes)); break
          }
        }
      } catch (e) { this.encodeErr = '解码失败：' + e.message + '（请检查输入格式）' }
    },
    b64Encode(str) {
      // 支持 Unicode（中文）
      return btoa(unescape(encodeURIComponent(str)))
    },
    b64Decode(str) {
      return decodeURIComponent(escape(atob(str)))
    },
    swapEncode() {
      const t = this.encodeIn; this.encodeIn = this.encodeOut; this.encodeOut = t
    },
    clearEncode() { this.encodeIn = ''; this.encodeOut = ''; this.encodeErr = '' },

    // ===== 正则 =====
    applyPreset(p) { this.regexPattern = p.pattern; this.regexFlags = p.flags },
    doRegexReplace() {
      if (!this.regex) return
      try {
        this.regexReplaced = this.regexText.replace(this.regex, this.regexReplace)
      } catch (e) { this.regexErr = '替换失败：' + e.message }
    },

    // ===== 对比 =====
    doDiff() {
      let a = this.diffA, b = this.diffB
      if (this.diffIgnoreCase) { a = a.toLowerCase(); b = b.toLowerCase() }
      if (this.diffIgnoreSpace) { a = a.replace(/\s+/g, ' ').trim(); b = b.replace(/\s+/g, ' ').trim() }

      let arrA, arrB
      if (this.diffMode === 'line')      { arrA = a.split(/\r?\n/); arrB = b.split(/\r?\n/) }
      else if (this.diffMode === 'word') { arrA = a.split(/\s+/);   arrB = b.split(/\s+/) }
      else                                { arrA = a.split('');      arrB = b.split('') }

      this.diffResult = this.lcsDiff(arrA, arrB)
    },
    clearDiff() { this.diffA = ''; this.diffB = ''; this.diffResult = [] },
    // 经典 LCS 动态规划 diff
    lcsDiff(a, b) {
      const m = a.length, n = b.length
      // 大规模文本保护：超过 3000 项截断到行级粗对比
      if (m * n > 9000000) {
        return [{ type: 'eq', value: '⚠ 文本过大，已跳过细粒度对比。请拆分后再试。' }]
      }
      const dp = Array.from({ length: m + 1 }, () => new Uint32Array(n + 1))
      for (let i = 1; i <= m; i++) {
        for (let j = 1; j <= n; j++) {
          dp[i][j] = a[i - 1] === b[j - 1] ? dp[i - 1][j - 1] + 1 : Math.max(dp[i - 1][j], dp[i][j - 1])
        }
      }
      const out = []
      let i = m, j = n
      while (i > 0 && j > 0) {
        if (a[i - 1] === b[j - 1]) { out.unshift({ type: 'eq',  value: a[i - 1] }); i--; j-- }
        else if (dp[i - 1][j] >= dp[i][j - 1]) { out.unshift({ type: 'del', value: a[i - 1] }); i-- }
        else { out.unshift({ type: 'add', value: b[j - 1] }); j-- }
      }
      while (i > 0) { out.unshift({ type: 'del', value: a[i - 1] }); i-- }
      while (j > 0) { out.unshift({ type: 'add', value: b[j - 1] }); j-- }
      return out
    },

    // ===== 大小写 =====
    applyCase(op) {
      const v = this.caseIn
      if (!v) { this.caseOut = ''; return }
      const words = (s) => s.replace(/[_\-\s]+/g, ' ').replace(/([a-z])([A-Z])/g, '$1 $2').trim().split(/\s+/)
      switch (op) {
        case 'upper':    this.caseOut = v.toUpperCase(); break
        case 'lower':    this.caseOut = v.toLowerCase(); break
        case 'title':    this.caseOut = v.replace(/\w\S*/g, w => w[0].toUpperCase() + w.slice(1).toLowerCase()); break
        case 'sentence': this.caseOut = v.toLowerCase().replace(/(^|[.!?]\s+)([a-z])/g, (_, p, c) => p + c.toUpperCase()); break
        case 'camel':    this.caseOut = words(v).map((w, i) => i === 0 ? w.toLowerCase() : w[0].toUpperCase() + w.slice(1).toLowerCase()).join(''); break
        case 'pascal':   this.caseOut = words(v).map(w => w[0].toUpperCase() + w.slice(1).toLowerCase()).join(''); break
        case 'snake':    this.caseOut = words(v).map(w => w.toLowerCase()).join('_'); break
        case 'kebab':    this.caseOut = words(v).map(w => w.toLowerCase()).join('-'); break
        case 'constant': this.caseOut = words(v).map(w => w.toUpperCase()).join('_'); break
        case 'reverse':  this.caseOut = Array.from(v).reverse().join(''); break
      }
    },
  }
}
</script>

<style scoped>
.tt-wrapper {
  max-width: 1100px;
  margin: 0 auto;
  padding: 20px 0 40px;
  font-family: -apple-system, BlinkMacSystemFont, 'PingFang SC', 'Microsoft YaHei', sans-serif;
}

/* Header */
.tt-header {
  text-align: center;
  margin-bottom: 20px;
}
.tt-title {
  font-size: 26px;
  font-weight: 700;
  margin: 0 0 8px;
  background: linear-gradient(90deg, #4a90d9, #7b5fd9);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}
.tt-desc {
  font-size: 13px;
  color: #888;
  margin: 0;
}

/* Tabs */
.tt-tabs {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
  margin-bottom: 18px;
  border-bottom: 1px solid #e8e8e8;
  padding-bottom: 0;
}
.tt-tab {
  padding: 10px 16px;
  font-size: 14px;
  background: transparent;
  border: none;
  border-bottom: 2px solid transparent;
  cursor: pointer;
  color: #666;
  display: inline-flex;
  align-items: center;
  gap: 6px;
  transition: all 0.15s;
  margin-bottom: -1px;
}
.tt-tab:hover { color: #4a90d9; }
.tt-tab.active {
  color: #4a90d9;
  border-bottom-color: #4a90d9;
  font-weight: 600;
}
.tt-tab-icon {
  display: inline-flex;
  align-items: center;
}

/* Panel */
.tt-panel {
  padding-top: 4px;
}

/* Toolbar */
.tt-toolbar {
  display: flex;
  flex-wrap: wrap;
  gap: 14px;
  align-items: center;
  padding: 12px 16px;
  background: #f7f7f9;
  border-radius: 8px;
  margin-bottom: 14px;
}
.tt-field {
  display: flex;
  align-items: center;
  gap: 8px;
}
.tt-field label {
  font-size: 13px;
  color: #555;
}
.tt-field select,
.tt-input {
  padding: 6px 10px;
  font-size: 13px;
  border: 1px solid #d0d0d0;
  border-radius: 5px;
  background: #fff;
  outline: none;
  transition: border-color 0.15s;
}
.tt-field select:focus,
.tt-input:focus {
  border-color: #4a90d9;
}
.tt-check-field label {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  cursor: pointer;
  margin-right: 10px;
}
.tt-check-field input { margin: 0; }
.tt-actions {
  margin-left: auto;
  display: flex;
  gap: 8px;
}

/* Buttons */
.tt-btn {
  padding: 6px 14px;
  font-size: 13px;
  border-radius: 5px;
  border: 1px solid #d0d0d0;
  background: #fff;
  color: #333;
  cursor: pointer;
  transition: all 0.15s;
}
.tt-btn:hover {
  background: #eef5ff;
  border-color: #4a90d9;
}
.tt-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}
.tt-btn-primary {
  background: #4a90d9;
  color: #fff;
  border-color: #4a90d9;
}
.tt-btn-primary:hover {
  background: #3a80c9;
  color: #fff;
}
.tt-mini {
  padding: 2px 8px;
  font-size: 11px;
  border-radius: 4px;
  border: 1px solid #d0d0d0;
  background: #fff;
  color: #666;
  cursor: pointer;
  transition: all 0.15s;
}
.tt-mini:hover {
  background: #eef5ff;
  color: #4a90d9;
  border-color: #4a90d9;
}

/* IO Grid */
.tt-grid-2 {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
}
.tt-io {
  display: flex;
  flex-direction: column;
  border: 1px solid #e0e0e0;
  border-radius: 8px;
  overflow: hidden;
  background: #fff;
}
.tt-io-head {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 12px;
  background: #f7f7f9;
  border-bottom: 1px solid #e8e8e8;
  font-size: 12px;
  color: #555;
}
.tt-io-cnt {
  margin-left: auto;
  margin-right: 4px;
  color: #999;
  font-size: 11px;
}
.tt-io textarea {
  width: 100%;
  min-height: 220px;
  border: none;
  outline: none;
  resize: vertical;
  padding: 10px 12px;
  font-family: 'JetBrains Mono', Menlo, Consolas, monospace;
  font-size: 13px;
  line-height: 1.6;
  color: #333;
  background: #fff;
  box-sizing: border-box;
}
.tt-textarea-large { min-height: 280px; }
.tt-io-full {
  margin-bottom: 16px;
}

/* Error */
.tt-err {
  margin-top: 10px;
  padding: 8px 12px;
  background: #fff4f4;
  border-left: 3px solid #e74c3c;
  color: #c0392b;
  font-size: 13px;
  border-radius: 4px;
}

/* ===== Regex ===== */
.tt-regex-bar {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 10px;
  background: #f7f7f9;
  padding: 8px 12px;
  border-radius: 8px;
  border: 1px solid #e0e0e0;
}
.tt-regex-slash {
  font-size: 18px;
  font-weight: 700;
  color: #4a90d9;
  font-family: monospace;
}
.tt-regex-input {
  flex: 1;
  border: none;
  outline: none;
  font-family: 'JetBrains Mono', monospace;
  font-size: 14px;
  background: transparent;
  padding: 4px 6px;
  color: #333;
}
.tt-regex-flags {
  width: 60px;
  border: none;
  outline: none;
  font-family: 'JetBrains Mono', monospace;
  font-size: 14px;
  background: transparent;
  padding: 4px 6px;
  color: #e67e22;
}
.tt-regex-quick {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 6px;
  margin-bottom: 12px;
  font-size: 12px;
  color: #888;
}
.tt-tag {
  padding: 3px 10px;
  font-size: 12px;
  border-radius: 12px;
  border: 1px solid #d0d0d0;
  background: #fff;
  cursor: pointer;
  color: #555;
  transition: all 0.15s;
}
.tt-tag:hover {
  background: #4a90d9;
  color: #fff;
  border-color: #4a90d9;
}
.tt-regex-result {
  flex: 1;
  padding: 10px 12px;
  min-height: 220px;
  max-height: 320px;
  overflow: auto;
  font-family: 'JetBrains Mono', monospace;
  font-size: 13px;
  line-height: 1.6;
  white-space: pre-wrap;
  word-break: break-all;
  background: #fff;
  color: #333;
}
.tt-regex-result.tt-regex-list { white-space: normal; }
.tt-hl {
  background: #fff3a0;
  color: #b76b00;
  padding: 1px 3px;
  border-radius: 3px;
  font-weight: 600;
}
.tt-match-item {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 6px 0;
  border-bottom: 1px dashed #ececec;
}
.tt-match-idx {
  width: 36px;
  font-size: 11px;
  color: #999;
  flex-shrink: 0;
}
.tt-match-text {
  flex: 1;
  color: #b76b00;
  background: #fff3a0;
  padding: 2px 6px;
  border-radius: 3px;
  word-break: break-all;
}
.tt-match-pos {
  font-size: 11px;
  color: #999;
}
.tt-empty {
  color: #aaa;
  text-align: center;
  padding: 20px;
}
.tt-regex-replace {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-top: 12px;
}
.tt-regex-replace .tt-input { flex: 1; }
.tt-replace-out {
  margin-top: 10px;
  width: 100%;
  min-height: 90px;
  padding: 10px 12px;
  border: 1px solid #e0e0e0;
  border-radius: 6px;
  font-family: 'JetBrains Mono', monospace;
  font-size: 13px;
  background: #fafafa;
  resize: vertical;
  outline: none;
  box-sizing: border-box;
}

/* ===== Diff ===== */
.tt-diff-summary {
  display: flex;
  gap: 16px;
  margin: 14px 0 8px;
  padding: 8px 12px;
  background: #f7f7f9;
  border-radius: 6px;
  font-size: 13px;
  font-weight: 600;
}
.tt-diff-add { color: #2ecc71; }
.tt-diff-del { color: #e74c3c; }
.tt-diff-eq  { color: #888; }
.tt-diff-output {
  border: 1px solid #e0e0e0;
  border-radius: 8px;
  background: #fff;
  font-family: 'JetBrains Mono', monospace;
  font-size: 13px;
  line-height: 1.7;
  max-height: 450px;
  overflow: auto;
}
.tt-diff-line {
  display: flex;
  padding: 2px 12px;
  white-space: pre-wrap;
  word-break: break-all;
}
.tt-diff-line.tt-diff-add {
  background: #e6ffed;
  color: #22863a;
}
.tt-diff-line.tt-diff-del {
  background: #ffeef0;
  color: #b31d28;
}
.tt-diff-line.tt-diff-eq { color: #555; }
.tt-diff-mark {
  width: 18px;
  text-align: center;
  flex-shrink: 0;
  font-weight: 700;
}
.tt-diff-content {
  flex: 1;
  word-break: break-all;
}

/* ===== Count ===== */
.tt-stat-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 12px;
}
.tt-stat-card {
  padding: 16px 12px;
  text-align: center;
  background: linear-gradient(135deg, #f7f9fc, #eef3fa);
  border: 1px solid #e3eaf2;
  border-radius: 8px;
  transition: transform 0.15s;
}
.tt-stat-card:hover {
  transform: translateY(-2px);
}
.tt-stat-val {
  font-size: 24px;
  font-weight: 700;
  color: #4a90d9;
  line-height: 1.2;
}
.tt-stat-lbl {
  font-size: 12px;
  color: #888;
  margin-top: 4px;
}

/* ===== Case ===== */
.tt-case-bar {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-bottom: 14px;
  padding: 12px;
  background: #f7f7f9;
  border-radius: 8px;
}
.tt-case-btn {
  padding: 6px 14px;
  font-size: 13px;
  border-radius: 5px;
  border: 1px solid #d0d0d0;
  background: #fff;
  cursor: pointer;
  color: #333;
  transition: all 0.15s;
}
.tt-case-btn:hover {
  background: #4a90d9;
  color: #fff;
  border-color: #4a90d9;
}

/* ===== Dark mode ===== */
.theme--dark .tt-tabs { border-bottom-color: #3a3a4a; }
.theme--dark .tt-tab { color: #aaa; }
.theme--dark .tt-tab.active { color: #5a9ae0; border-bottom-color: #5a9ae0; }
.theme--dark .tt-toolbar,
.theme--dark .tt-case-bar,
.theme--dark .tt-regex-bar,
.theme--dark .tt-regex-quick,
.theme--dark .tt-io-head,
.theme--dark .tt-diff-summary { background: #181825; border-color: #3a3a4a; color: #ccc; }
.theme--dark .tt-io,
.theme--dark .tt-diff-output { background: #1e1e2e; border-color: #3a3a4a; }
.theme--dark .tt-io textarea,
.theme--dark .tt-replace-out,
.theme--dark .tt-regex-result { background: #1e1e2e; color: #ddd; }
.theme--dark .tt-replace-out { background: #181825; }
.theme--dark .tt-field select,
.theme--dark .tt-input,
.theme--dark .tt-btn,
.theme--dark .tt-case-btn,
.theme--dark .tt-mini,
.theme--dark .tt-tag { background: #2a2a3a; border-color: #3a3a4a; color: #ddd; }
.theme--dark .tt-btn-primary { background: #4a90d9; color: #fff; border-color: #4a90d9; }
.theme--dark .tt-stat-card {
  background: linear-gradient(135deg, #1e1e2e, #25253a);
  border-color: #3a3a4a;
}
.theme--dark .tt-stat-val { color: #5a9ae0; }
.theme--dark .tt-diff-line.tt-diff-add { background: #143620; color: #7ee2a8; }
.theme--dark .tt-diff-line.tt-diff-del { background: #3a1418; color: #ff8b94; }
.theme--dark .tt-diff-line.tt-diff-eq  { color: #aaa; }
.theme--dark .tt-hl { background: #5a4a00; color: #ffe680; }
.theme--dark .tt-err { background: #3a1418; color: #ff8b94; }

/* ===== Responsive ===== */
@media (max-width: 768px) {
  .tt-grid-2 {
    grid-template-columns: 1fr;
  }
  .tt-toolbar {
    flex-direction: column;
    align-items: stretch;
  }
  .tt-actions {
    margin-left: 0;
    justify-content: flex-end;
  }
  .tt-tab {
    padding: 8px 10px;
    font-size: 13px;
  }
}
</style>
