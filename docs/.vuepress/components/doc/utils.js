// ============================================================
// DocToolbox 纯函数库
//   - Markdown → HTML：mdToHtml
//   - HTML → Markdown：htmlToMd（基于 DOMParser）
//   - 模板：buildTemplate
//   - 通用：copyText / downloadFile / loadScript / copyMixin
// 全部浏览器端运行，无外部依赖；PDF 处理使用 pdf-lib CDN 按需懒加载（在 Panel 中处理）
// ============================================================

// ---------- 公共工具 ----------
export function escapeHtml(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;')
}

export function copyText(text) {
  if (typeof navigator !== 'undefined' && navigator.clipboard) {
    return navigator.clipboard.writeText(text).then(() => true).catch(() => fallbackCopy(text))
  }
  return Promise.resolve(fallbackCopy(text))
}
function fallbackCopy(text) {
  try {
    const ta = document.createElement('textarea')
    ta.value = text; ta.style.position = 'fixed'; ta.style.left = '-9999px'
    document.body.appendChild(ta); ta.select()
    const ok = document.execCommand('copy')
    document.body.removeChild(ta)
    return ok
  } catch (_) { return false }
}

export function downloadFile(data, filename, mime = 'text/plain') {
  const blob = data instanceof Blob ? data : new Blob([data], { type: mime + ';charset=utf-8' })
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url; a.download = filename
  document.body.appendChild(a); a.click(); document.body.removeChild(a)
  setTimeout(() => URL.revokeObjectURL(url), 1000)
}

// 同源/受信源脚本懒加载（仅允许加载预定义白名单 CDN，避免 XSS/任意 src）
const SCRIPT_WHITELIST = {
  'pdf-lib': 'https://cdn.jsdelivr.net/npm/pdf-lib@1.17.1/dist/pdf-lib.min.js',
}
const scriptCache = {}
export function loadScript(key) {
  const src = SCRIPT_WHITELIST[key]
  if (!src) return Promise.reject(new Error('script not in whitelist: ' + key))
  if (scriptCache[key]) return scriptCache[key]
  scriptCache[key] = new Promise((resolve, reject) => {
    const s = document.createElement('script')
    s.src = src; s.async = true
    s.onload = () => resolve()
    s.onerror = () => { delete scriptCache[key]; reject(new Error('failed to load ' + src)) }
    document.head.appendChild(s)
  })
  return scriptCache[key]
}

export const copyMixin = {
  data() { return { copyHint: '' } },
  methods: {
    copy(text, key) {
      if (!text) return
      copyText(text).then(ok => {
        if (ok) {
          this.copyHint = key
          setTimeout(() => { if (this.copyHint === key) this.copyHint = '' }, 1500)
        }
      })
    },
    download(content, filename, mime) { downloadFile(content, filename, mime) },
  },
}

// ============================================================
// Markdown → HTML（mini 实现，覆盖 90% 常见语法）
//   - ATX 标题 #~######
//   - 围栏代码块 ```lang
//   - 引用 >
//   - 无序列表 - + *
//   - 有序列表 1.
//   - 表格 | a | b |
//   - 水平线 ---
//   - 内联：**bold** *em* `code` [text](url) ![alt](url) ~~del~~
//   - 自动 URL
// ============================================================
export function mdToHtml(md) {
  if (!md) return ''
  const lines = md.replace(/\r\n?/g, '\n').split('\n')
  let html = ''
  let i = 0
  while (i < lines.length) {
    const line = lines[i]

    // 围栏代码块
    const fence = line.match(/^```(\w*)\s*$/)
    if (fence) {
      const lang = fence[1] || ''
      let code = ''
      i++
      while (i < lines.length && !/^```\s*$/.test(lines[i])) { code += lines[i] + '\n'; i++ }
      i++ // skip closing ```
      html += `<pre class="dt-pre"><code${lang ? ` class="lang-${escapeHtml(lang)}"` : ''}>${escapeHtml(code.replace(/\n$/, ''))}</code></pre>\n`
      continue
    }

    // 水平线
    if (/^\s*([-*_])(\s*\1){2,}\s*$/.test(line)) { html += '<hr/>\n'; i++; continue }

    // ATX 标题
    const h = line.match(/^(#{1,6})\s+(.*?)\s*#*\s*$/)
    if (h) { html += `<h${h[1].length}>${inline(h[2])}</h${h[1].length}>\n`; i++; continue }

    // 引用块
    if (/^>\s?/.test(line)) {
      let body = ''
      while (i < lines.length && /^>\s?/.test(lines[i])) {
        body += lines[i].replace(/^>\s?/, '') + '\n'
        i++
      }
      html += `<blockquote>${mdToHtml(body)}</blockquote>\n`
      continue
    }

    // 表格
    if (/^\s*\|.*\|\s*$/.test(line) && i + 1 < lines.length && /^\s*\|?\s*:?-+:?(\s*\|\s*:?-+:?)+\s*\|?\s*$/.test(lines[i + 1])) {
      const headCells = splitRow(line)
      const aligns = splitRow(lines[i + 1]).map(c => {
        const left = c.startsWith(':'), right = c.endsWith(':')
        if (left && right) return 'center'
        if (right) return 'right'
        return 'left'
      })
      i += 2
      const bodyRows = []
      while (i < lines.length && /^\s*\|.*\|\s*$/.test(lines[i])) { bodyRows.push(splitRow(lines[i])); i++ }
      html += '<table class="dt-table"><thead><tr>'
      headCells.forEach((c, idx) => { html += `<th style="text-align:${aligns[idx] || 'left'}">${inline(c)}</th>` })
      html += '</tr></thead><tbody>'
      bodyRows.forEach(row => {
        html += '<tr>'
        row.forEach((c, idx) => { html += `<td style="text-align:${aligns[idx] || 'left'}">${inline(c)}</td>` })
        html += '</tr>'
      })
      html += '</tbody></table>\n'
      continue
    }

    // 列表
    if (/^\s*([-+*]|\d+\.)\s+/.test(line)) {
      const ordered = /^\s*\d+\.\s+/.test(line)
      const items = []
      while (i < lines.length && /^\s*([-+*]|\d+\.)\s+/.test(lines[i])) {
        let body = lines[i].replace(/^\s*([-+*]|\d+\.)\s+/, '')
        i++
        // 续行（缩进 ≥2 视为同 item）
        while (i < lines.length && /^\s{2,}\S/.test(lines[i])) { body += '\n' + lines[i].replace(/^\s{2}/, ''); i++ }
        items.push(body)
      }
      html += ordered ? '<ol>' : '<ul>'
      items.forEach(it => { html += `<li>${inline(it)}</li>` })
      html += ordered ? '</ol>\n' : '</ul>\n'
      continue
    }

    // 空行
    if (!line.trim()) { i++; continue }

    // 段落（连续非空行合并）
    let para = line
    i++
    while (i < lines.length && lines[i].trim() && !/^(#{1,6}\s|```|>|\s*[-+*]\s|\s*\d+\.\s|\s*\|.*\|)/.test(lines[i])) {
      para += '\n' + lines[i]; i++
    }
    html += `<p>${inline(para)}</p>\n`
  }
  return html
}

function splitRow(line) {
  let s = line.trim()
  if (s.startsWith('|')) s = s.slice(1)
  if (s.endsWith('|')) s = s.slice(0, -1)
  return s.split('|').map(c => c.trim())
}

function inline(text) {
  // 先把行内代码挑出占位，避免内部被加粗/链接误处理
  const codes = []
  text = text.replace(/`([^`]+)`/g, (_, c) => {
    codes.push(c); return `\u0000CODE${codes.length - 1}\u0000`
  })
  // 转义保护未处理时易混的 HTML 字符
  let out = escapeHtml(text)
  // 图片 ![alt](url)
  out = out.replace(/!\[([^\]]*)\]\(([^)\s]+)(?:\s+&quot;([^)]*)&quot;)?\)/g,
    (_, alt, url, title) => `<img src="${url}" alt="${alt}"${title ? ` title="${title}"` : ''}/>`)
  // 链接 [text](url)
  out = out.replace(/\[([^\]]+)\]\(([^)\s]+)(?:\s+&quot;([^)]*)&quot;)?\)/g,
    (_, label, url, title) => `<a href="${url}" target="_blank" rel="noopener"${title ? ` title="${title}"` : ''}>${label}</a>`)
  // 粗体、斜体、删除线
  out = out.replace(/\*\*([^*\n]+)\*\*/g, '<strong>$1</strong>')
  out = out.replace(/(^|[\s(])\*([^*\n]+)\*/g, '$1<em>$2</em>')
  out = out.replace(/~~([^~\n]+)~~/g, '<del>$1</del>')
  // 裸 URL
  out = out.replace(/(^|\s)(https?:\/\/[^\s<]+)/g, '$1<a href="$2" target="_blank" rel="noopener">$2</a>')
  // 换行 → <br/>
  out = out.replace(/\n/g, '<br/>')
  // 还原行内代码
  out = out.replace(/\u0000CODE(\d+)\u0000/g, (_, n) => `<code class="dt-code">${escapeHtml(codes[+n])}</code>`)
  return out
}

// ============================================================
// HTML → Markdown
//   基于 DOMParser 递归节点，支持常见标签
// ============================================================
export function htmlToMd(html) {
  if (!html || typeof DOMParser === 'undefined') return ''
  const doc = new DOMParser().parseFromString(`<div id="root">${html}</div>`, 'text/html')
  const root = doc.getElementById('root')
  return walk(root).replace(/\n{3,}/g, '\n\n').trim() + '\n'
}

function walk(node) {
  if (!node) return ''
  if (node.nodeType === 3 /* text */) return node.nodeValue.replace(/\s+/g, ' ')
  if (node.nodeType !== 1) return ''
  const tag = node.tagName.toLowerCase()
  const inner = () => Array.from(node.childNodes).map(walk).join('')

  switch (tag) {
    case 'h1': case 'h2': case 'h3': case 'h4': case 'h5': case 'h6':
      return '\n\n' + '#'.repeat(+tag[1]) + ' ' + inner().trim() + '\n\n'
    case 'p': return '\n\n' + inner().trim() + '\n\n'
    case 'br': return '  \n'
    case 'hr': return '\n\n---\n\n'
    case 'strong': case 'b': return '**' + inner() + '**'
    case 'em': case 'i': return '*' + inner() + '*'
    case 'del': case 's': case 'strike': return '~~' + inner() + '~~'
    case 'code':
      if (node.parentNode && node.parentNode.tagName === 'PRE') return inner()
      return '`' + inner() + '`'
    case 'pre': {
      const codeEl = node.querySelector('code')
      const lang = codeEl && codeEl.className.match(/lang-(\w+)/)
      const text = codeEl ? codeEl.textContent : node.textContent
      return '\n\n```' + (lang ? lang[1] : '') + '\n' + text.replace(/\n$/, '') + '\n```\n\n'
    }
    case 'blockquote':
      return '\n\n' + inner().trim().split('\n').map(l => '> ' + l).join('\n') + '\n\n'
    case 'a': {
      const href = node.getAttribute('href') || ''
      const text = inner() || href
      return `[${text}](${href})`
    }
    case 'img': {
      const src = node.getAttribute('src') || ''
      const alt = node.getAttribute('alt') || ''
      return `![${alt}](${src})`
    }
    case 'ul': case 'ol': {
      const items = Array.from(node.children).filter(c => c.tagName === 'LI')
      const ordered = tag === 'ol'
      const out = items.map((li, idx) => {
        const prefix = ordered ? `${idx + 1}. ` : '- '
        const body = walk(li).replace(/^\n+|\n+$/g, '')
        return prefix + body.replace(/\n/g, '\n  ')
      }).join('\n')
      return '\n\n' + out + '\n\n'
    }
    case 'li': return inner().trim()
    case 'table': {
      const rows = Array.from(node.querySelectorAll('tr'))
      if (!rows.length) return ''
      const grid = rows.map(tr => Array.from(tr.children).map(td => walk(td).trim().replace(/\|/g, '\\|')))
      const headLen = grid[0].length
      let out = '\n\n| ' + grid[0].join(' | ') + ' |\n'
      out += '| ' + Array(headLen).fill('---').join(' | ') + ' |\n'
      for (let r = 1; r < grid.length; r++) out += '| ' + grid[r].join(' | ') + ' |\n'
      return out + '\n'
    }
    case 'script': case 'style': case 'noscript': return ''
    default: return inner()
  }
}

// ============================================================
// 文档模板
// ============================================================
export const TEMPLATES = [
  {
    key: 'tech-design',
    name: '技术方案',
    fields: [
      { key: 'title', label: '方案标题', placeholder: '订单中心重构方案' },
      { key: 'author', label: '作者', placeholder: '张三' },
      { key: 'background', label: '背景', type: 'textarea', placeholder: '为什么要做这件事…' },
      { key: 'goal', label: '目标', type: 'textarea', placeholder: '可衡量的目标，如 P99 < 200ms' },
      { key: 'solution', label: '方案设计', type: 'textarea', placeholder: '核心思路、架构图…' },
      { key: 'risk', label: '风险与回滚', type: 'textarea', placeholder: '可能的风险与回滚策略' },
    ],
    build: f => `# ${f.title || '技术方案'}

> 作者：${f.author || '佚名'} · 日期：${today()}

## 一、背景
${f.background || '_待补充_'}

## 二、目标
${f.goal || '_待补充_'}

## 三、方案设计
${f.solution || '_待补充_'}

## 四、影响面 & 改动点
- 待补充

## 五、风险与回滚
${f.risk || '_待补充_'}

## 六、排期 & 分工
| 阶段 | 内容 | 负责人 | 截止 |
| --- | --- | --- | --- |
|  |  |  |  |
`,
  },
  {
    key: 'api-doc',
    name: '接口文档',
    fields: [
      { key: 'name', label: '接口名称', placeholder: '创建订单' },
      { key: 'method', label: '请求方法', placeholder: 'POST' },
      { key: 'path', label: '路径', placeholder: '/api/v1/orders' },
      { key: 'desc', label: '描述', type: 'textarea', placeholder: '接口用途说明' },
      { key: 'reqExample', label: '请求示例 (JSON)', type: 'textarea', placeholder: '{ "uid": 123 }' },
      { key: 'respExample', label: '响应示例 (JSON)', type: 'textarea', placeholder: '{ "code": 0 }' },
    ],
    build: f => `# ${f.name || '接口名称'}

\`${(f.method || 'POST').toUpperCase()} ${f.path || '/api/xxx'}\`

## 描述
${f.desc || '_待补充_'}

## 请求参数
| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
|  |  |  |  |

## 请求示例
\`\`\`json
${f.reqExample || '{\n}'}
\`\`\`

## 响应字段
| 字段 | 类型 | 说明 |
| --- | --- | --- |
|  |  |  |

## 响应示例
\`\`\`json
${f.respExample || '{\n  "code": 0,\n  "data": {}\n}'}
\`\`\`

## 错误码
| 错误码 | 含义 |
| --- | --- |
| 0 | 成功 |
`,
  },
  {
    key: 'readme',
    name: 'README',
    fields: [
      { key: 'project', label: '项目名', placeholder: 'my-awesome-project' },
      { key: 'desc', label: '一句话描述', placeholder: '简洁有力的项目介绍' },
      { key: 'install', label: '安装命令', placeholder: 'npm install my-awesome-project' },
      { key: 'usage', label: '使用示例', type: 'textarea', placeholder: '简单的代码示例' },
    ],
    build: f => `# ${f.project || 'project-name'}

${f.desc || '一句话描述项目'}

## 安装
\`\`\`bash
${f.install || 'npm install'}
\`\`\`

## 快速开始
${f.usage ? '```\n' + f.usage + '\n```' : '```\n// 你的示例代码\n```'}

## 功能特性
- 特性 1
- 特性 2
- 特性 3

## 贡献
欢迎 PR 与 Issue。

## 许可
MIT
`,
  },
  {
    key: 'weekly',
    name: '周报',
    fields: [
      { key: 'name', label: '姓名', placeholder: '张三' },
      { key: 'week', label: '周次', placeholder: today() },
      { key: 'done', label: '本周完成', type: 'textarea', placeholder: '- 项目 A 上线\n- ...' },
      { key: 'next', label: '下周计划', type: 'textarea', placeholder: '- 项目 B 启动\n- ...' },
      { key: 'risk', label: '风险/需协调', type: 'textarea', placeholder: '需要 XX 支持' },
    ],
    build: f => `# 周报 · ${f.week || today()}

> ${f.name || '佚名'}

## 本周完成
${f.done || '- _待补充_'}

## 下周计划
${f.next || '- _待补充_'}

## 风险 / 需协调
${f.risk || '无'}
`,
  },
]

function today() {
  const d = new Date()
  const p = n => String(n).padStart(2, '0')
  return `${d.getFullYear()}-${p(d.getMonth() + 1)}-${p(d.getDate())}`
}
