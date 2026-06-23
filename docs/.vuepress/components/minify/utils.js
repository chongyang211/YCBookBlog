/* ============================================================
 * MinifyToolbox · 工具函数集合
 *  - 全部在浏览器端运行，零依赖
 *  - 对外暴露：minifyJs / beautifyJs / minifyCss / beautifyCss /
 *             minifyHtml / beautifyHtml / formatJson / formatSql /
 *             formatXml / processLines / copyMixin / downloadText /
 *             byteLen
 * ============================================================ */

/* ---------- 通用 ---------- */

export function byteLen(s) {
  if (s == null) return 0
  // 使用 TextEncoder 计算实际 UTF-8 字节
  return new TextEncoder().encode(String(s)).length
}

export function downloadText(filename, text, mime = 'text/plain;charset=utf-8') {
  const blob = new Blob([text], { type: mime })
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = filename
  document.body.appendChild(a)
  a.click()
  document.body.removeChild(a)
  setTimeout(() => URL.revokeObjectURL(url), 1000)
}

export const copyMixin = {
  data() {
    return { _mt_copied: false }
  },
  methods: {
    async mtCopy(text) {
      if (!text) return false
      try {
        if (navigator.clipboard && window.isSecureContext) {
          await navigator.clipboard.writeText(text)
        } else {
          const ta = document.createElement('textarea')
          ta.value = text
          ta.style.position = 'fixed'
          ta.style.opacity = '0'
          document.body.appendChild(ta)
          ta.select()
          document.execCommand('copy')
          document.body.removeChild(ta)
        }
        return true
      } catch (e) {
        return false
      }
    },
  },
}

/* ============================================================
 * JavaScript
 * ============================================================ */

/**
 * 安全地去除注释而不破坏字符串 / 正则字面量
 * 状态机：normal / single-string / double-string / template-string / regex / line-comment / block-comment
 */
function stripJsCommentsSafe(src) {
  let out = ''
  let i = 0
  const n = src.length
  let state = 'n' // n=normal s=' d=" t=` r=/ L=// B=/*
  let prevNonWs = '' // 用于判断 / 是除号还是正则起始
  const regexAllowedPrev = new Set([
    '', '(', '[', '{', ',', ';', ':', '!', '&', '|',
    '?', '+', '-', '*', '/', '%', '=', '<', '>', '~', '^',
    'r', // return / typeof / delete / in / of 等关键词结尾粗略判定
  ])

  while (i < n) {
    const c = src[i]
    const nx = src[i + 1]

    if (state === 'L') {
      if (c === '\n') { out += '\n'; state = 'n' }
      i++
      continue
    }
    if (state === 'B') {
      if (c === '*' && nx === '/') { state = 'n'; i += 2 } else { i++ }
      continue
    }
    if (state === 's' || state === 'd' || state === 't') {
      out += c
      if (c === '\\' && nx !== undefined) { out += nx; i += 2; continue }
      if ((state === 's' && c === "'") ||
          (state === 'd' && c === '"') ||
          (state === 't' && c === '`')) {
        state = 'n'
      }
      i++
      continue
    }
    if (state === 'r') {
      out += c
      if (c === '\\' && nx !== undefined) { out += nx; i += 2; continue }
      if (c === '[') state = 'rc'
      else if (c === '/') state = 'n'
      i++
      continue
    }
    if (state === 'rc') {
      out += c
      if (c === '\\' && nx !== undefined) { out += nx; i += 2; continue }
      if (c === ']') state = 'r'
      i++
      continue
    }

    // normal
    if (c === '/' && nx === '/') { state = 'L'; i += 2; continue }
    if (c === '/' && nx === '*') { state = 'B'; i += 2; continue }
    if (c === "'") { state = 's'; out += c; i++; continue }
    if (c === '"') { state = 'd'; out += c; i++; continue }
    if (c === '`') { state = 't'; out += c; i++; continue }
    if (c === '/') {
      // 判定除号 vs 正则
      if (regexAllowedPrev.has(prevNonWs) || /[a-z]/.test(prevNonWs) === false) {
        // 粗略判定：前一个非空白字符若为标识符/数字结尾，则视为除号
        if (/[\w)\]]/.test(prevNonWs)) {
          out += c
        } else {
          state = 'r'
          out += c
        }
      } else {
        out += c
      }
      i++
      if (!/\s/.test(c)) prevNonWs = c
      continue
    }

    out += c
    if (!/\s/.test(c)) prevNonWs = c
    i++
  }
  return out
}

export function minifyJs(src, opts = {}) {
  const { aggressive = true, keepNewlines = false } = opts
  if (!src) return ''

  let code = stripJsCommentsSafe(src)

  if (aggressive) {
    // 折叠空白（但保留字符串/模板字面量内的）：再做一次状态机
    code = collapseJsWhitespace(code, keepNewlines)
  }
  // 末尾换行
  return code.trim()
}

function collapseJsWhitespace(src, keepNewlines) {
  let out = ''
  let i = 0
  const n = src.length
  let state = 'n'
  while (i < n) {
    const c = src[i]
    const nx = src[i + 1]
    if (state === 'n') {
      if (c === "'") { out += c; state = 's'; i++; continue }
      if (c === '"') { out += c; state = 'd'; i++; continue }
      if (c === '`') { out += c; state = 't'; i++; continue }
      if (/\s/.test(c)) {
        // 折叠连续空白为单空格；若空白前后均为标识符/数字相关，保留 1 空格；否则丢弃
        let j = i
        let hasNl = false
        while (j < n && /\s/.test(src[j])) { if (src[j] === '\n') hasNl = true; j++ }
        const prev = out[out.length - 1] || ''
        const next = src[j] || ''
        const needSpace = /[\w$]/.test(prev) && /[\w$]/.test(next)
        if (keepNewlines && hasNl) out += '\n'
        else if (needSpace) out += ' '
        i = j
        continue
      }
      out += c
      i++
      continue
    }
    // 字符串/模板内：原样保留
    out += c
    if (c === '\\' && nx !== undefined) { out += nx; i += 2; continue }
    if ((state === 's' && c === "'") ||
        (state === 'd' && c === '"') ||
        (state === 't' && c === '`')) {
      state = 'n'
    }
    i++
  }
  return out
}

/* JS 美化（简单的缩进恢复，适合压缩后的代码） */
export function beautifyJs(src, indent = 2) {
  if (!src) return ''
  const pad = ' '.repeat(indent)
  let out = ''
  let depth = 0
  let i = 0
  const n = src.length
  let state = 'n'
  let lineStart = true

  const addNl = () => {
    if (out[out.length - 1] !== '\n') out += '\n'
    lineStart = true
  }
  const writeIndent = () => {
    out += pad.repeat(Math.max(depth, 0))
    lineStart = false
  }

  while (i < n) {
    const c = src[i]
    const nx = src[i + 1]
    if (state === 'n') {
      if (c === '/' && nx === '/') {
        while (i < n && src[i] !== '\n') { out += src[i]; i++ }
        continue
      }
      if (c === '/' && nx === '*') {
        while (i < n && !(src[i] === '*' && src[i + 1] === '/')) { out += src[i]; i++ }
        if (i < n) { out += '*/'; i += 2 }
        continue
      }
      if (c === "'" || c === '"' || c === '`') {
        if (lineStart) writeIndent()
        out += c
        state = c
        i++
        continue
      }
      if (c === '{') {
        if (lineStart) writeIndent()
        out += '{'
        depth++
        addNl()
        i++
        continue
      }
      if (c === '}') {
        addNl()
        depth--
        writeIndent()
        out += '}'
        // 处理 } else / } catch / } finally / } while
        let j = i + 1
        while (j < n && (src[j] === ' ' || src[j] === '\t')) j++
        if (src[j] === ';' || src[j] === ',') {
          out += src[j]
          i = j + 1
          addNl()
          continue
        }
        i++
        continue
      }
      if (c === ';') {
        if (lineStart) writeIndent()
        out += ';'
        addNl()
        i++
        continue
      }
      if (c === '\n') {
        addNl()
        i++
        continue
      }
      if (lineStart && /\s/.test(c)) { i++; continue }
      if (lineStart) writeIndent()
      out += c
      i++
      continue
    }
    // 字符串
    out += c
    if (c === '\\' && nx !== undefined) { out += nx; i += 2; continue }
    if (c === state) state = 'n'
    i++
  }
  return out.trim()
}

/* ============================================================
 * CSS
 * ============================================================ */

export function minifyCss(src) {
  if (!src) return ''
  let code = src
  // 删除注释 /* ... */
  code = code.replace(/\/\*[\s\S]*?\*\//g, '')
  // 折叠空白
  code = code.replace(/\s+/g, ' ')
  // 删除选择器/属性周围的空白
  code = code.replace(/\s*([{}:;,>+~])\s*/g, '$1')
  // 删除最后一个分号
  code = code.replace(/;}/g, '}')
  // 0 单位
  code = code.replace(/(^|[\s:])0(?:px|em|rem|%|pt|vh|vw|ex|ch)\b/gi, '$10')
  // 颜色 #ffffff -> #fff
  code = code.replace(/#([0-9a-f])\1([0-9a-f])\2([0-9a-f])\3\b/gi, '#$1$2$3')
  return code.trim()
}

export function beautifyCss(src, indent = 2) {
  if (!src) return ''
  const pad = ' '.repeat(indent)
  let out = ''
  let depth = 0
  let i = 0
  // 先粗压缩去掉注释外多余空白
  const compact = src
    .replace(/\/\*[\s\S]*?\*\//g, '')
    .replace(/\s+/g, ' ')
    .replace(/\s*([{};,])\s*/g, '$1')

  while (i < compact.length) {
    const c = compact[i]
    if (c === '{') {
      out += ' {\n'
      depth++
      out += pad.repeat(depth)
    } else if (c === '}') {
      depth = Math.max(depth - 1, 0)
      out = out.replace(/[ \t]+$/, '')
      if (out[out.length - 1] !== '\n') out += '\n'
      out += pad.repeat(depth) + '}\n'
      if (depth === 0) out += '\n'
    } else if (c === ';') {
      out += ';\n' + pad.repeat(depth)
    } else if (c === ',' && depth === 0) {
      out += ',\n'
    } else {
      out += c
    }
    i++
  }
  return out.replace(/\n{3,}/g, '\n\n').trim() + '\n'
}

/* ============================================================
 * HTML
 * ============================================================ */

export function minifyHtml(src) {
  if (!src) return ''
  let code = src

  // 提取 pre/script/style/textarea/code 内容用占位符替换，最后还原
  const guards = []
  const re = /<(pre|script|style|textarea|code)\b[^>]*>[\s\S]*?<\/\1>/gi
  code = code.replace(re, (m) => {
    guards.push(m)
    return `\u0001${guards.length - 1}\u0001`
  })

  // 删除 HTML 注释 <!-- --> （保留 <!--[if --><![endif]-->）
  code = code.replace(/<!--(?!\[if )[\s\S]*?-->/g, '')

  // 折叠空白
  code = code.replace(/\s+/g, ' ')
  // 标签间空白
  code = code.replace(/>\s+</g, '><')
  // 标签内属性周围空白
  code = code.replace(/\s+>/g, '>')
  code = code.replace(/\s+\/>/g, '/>')

  // 还原占位符
  code = code.replace(/\u0001(\d+)\u0001/g, (_, idx) => guards[+idx])

  return code.trim()
}

export function beautifyHtml(src, indent = 2) {
  if (!src) return ''
  const pad = ' '.repeat(indent)
  const voidTags = new Set([
    'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
    'link', 'meta', 'param', 'source', 'track', 'wbr',
  ])
  const preserveTags = new Set(['pre', 'script', 'style', 'textarea', 'code'])

  // 先压缩
  const compact = minifyHtml(src)
  // 切 token
  const tokens = compact.split(/(<[^>]+>)/g).filter(Boolean)

  let depth = 0
  let out = ''
  let inPreserve = 0

  for (let i = 0; i < tokens.length; i++) {
    const t = tokens[i]
    if (t.startsWith('<')) {
      const close = t.startsWith('</')
      const m = t.match(/^<\/?([a-zA-Z0-9-]+)/)
      const name = m ? m[1].toLowerCase() : ''
      const selfClose = t.endsWith('/>') || voidTags.has(name)

      if (preserveTags.has(name)) {
        if (close) {
          inPreserve = Math.max(inPreserve - 1, 0)
          out += t
        } else {
          if (out && out[out.length - 1] !== '\n') out += '\n'
          out += pad.repeat(depth) + t
          inPreserve++
        }
        continue
      }
      if (inPreserve > 0) { out += t; continue }

      if (close) {
        depth = Math.max(depth - 1, 0)
        if (out && out[out.length - 1] !== '\n') out += '\n'
        out += pad.repeat(depth) + t + '\n'
      } else if (selfClose) {
        out += pad.repeat(depth) + t + '\n'
      } else {
        out += pad.repeat(depth) + t
        // 若下一个 token 是文本，则不立即换行
        const next = tokens[i + 1] || ''
        if (next.startsWith('<') || !next.trim()) {
          out += '\n'
        }
        depth++
      }
    } else {
      if (inPreserve > 0) { out += t; continue }
      const text = t.trim()
      if (!text) continue
      // 上一个非换行字符如果是 >，紧贴文本，否则缩进
      if (out.endsWith('\n')) out += pad.repeat(depth) + text + '\n'
      else { out += text; out += '\n' }
      // 上一个 open tag 的 depth 已 ++ 过，关闭时会 --
      // 需要在文本后让下一个 close 缩进
    }
  }
  return out.replace(/\n{3,}/g, '\n\n').trim() + '\n'
}

/* ============================================================
 * JSON / SQL / XML
 * ============================================================ */

export function formatJson(src, indent = 2) {
  if (!src.trim()) return ''
  const obj = JSON.parse(src)
  return JSON.stringify(obj, null, indent)
}

export function minifyJson(src) {
  if (!src.trim()) return ''
  return JSON.stringify(JSON.parse(src))
}

const SQL_KEYWORDS = [
  'SELECT', 'FROM', 'WHERE', 'AND', 'OR', 'NOT', 'IN', 'IS', 'NULL',
  'INSERT INTO', 'VALUES', 'UPDATE', 'SET', 'DELETE FROM',
  'CREATE TABLE', 'DROP TABLE', 'ALTER TABLE', 'ADD', 'COLUMN',
  'INNER JOIN', 'LEFT JOIN', 'RIGHT JOIN', 'FULL JOIN', 'JOIN', 'ON',
  'GROUP BY', 'ORDER BY', 'HAVING', 'LIMIT', 'OFFSET',
  'UNION ALL', 'UNION', 'AS', 'DISTINCT', 'CASE', 'WHEN', 'THEN', 'ELSE', 'END',
  'BEGIN', 'COMMIT', 'ROLLBACK', 'TRANSACTION',
  'LIKE', 'BETWEEN', 'EXISTS', 'INDEX', 'PRIMARY KEY', 'FOREIGN KEY', 'REFERENCES',
  'INT', 'VARCHAR', 'TEXT', 'DATETIME', 'TIMESTAMP', 'BOOLEAN', 'FLOAT', 'DOUBLE',
]

export function formatSql(src, opts = {}) {
  if (!src) return ''
  const { upper = true, indent = 2 } = opts
  const pad = ' '.repeat(indent)

  let s = src.replace(/\s+/g, ' ').trim()

  // 关键字大写/小写
  SQL_KEYWORDS.forEach(kw => {
    const re = new RegExp(`\\b${kw.replace(/\s+/g, '\\s+')}\\b`, 'gi')
    s = s.replace(re, upper ? kw : kw.toLowerCase())
  })

  // 主关键字换行
  const breaks = [
    'SELECT', 'FROM', 'WHERE', 'GROUP BY', 'ORDER BY', 'HAVING',
    'LIMIT', 'OFFSET', 'UNION ALL', 'UNION',
    'INNER JOIN', 'LEFT JOIN', 'RIGHT JOIN', 'FULL JOIN', 'JOIN',
    'INSERT INTO', 'VALUES', 'UPDATE', 'SET', 'DELETE FROM',
  ]
  breaks.forEach(kw => {
    const target = upper ? kw : kw.toLowerCase()
    s = s.split(target).join('\n' + target)
  })

  // AND / OR 换行 + 缩进
  s = s.replace(/\s+(AND|OR|and|or)\s+/g, '\n' + pad + '$1 ')

  // SELECT 后的字段每个换行（仅当字段多）
  s = s.replace(/(SELECT|select)\s+([^]*?)\s+(FROM|from)/, (_, sel, cols, frm) => {
    const list = cols.split(',').map(x => x.trim()).filter(Boolean)
    if (list.length <= 2) return `${sel} ${list.join(', ')}\n${frm}`
    return `${sel}\n${pad}${list.join(',\n' + pad)}\n${frm}`
  })

  return s.replace(/^\n+/, '').replace(/\n{2,}/g, '\n').trim() + ';'.repeat(src.trim().endsWith(';') ? 1 : 0)
}

export function formatXml(src, indent = 2) {
  if (!src) return ''
  const pad = ' '.repeat(indent)
  // 先把所有标签间多余空白去掉
  const compact = src.replace(/>\s+</g, '><').trim()
  let depth = 0
  let out = ''
  const re = /<[^>]+>|[^<]+/g
  let m
  while ((m = re.exec(compact)) !== null) {
    const t = m[0]
    if (t.startsWith('</')) {
      depth = Math.max(depth - 1, 0)
      out += pad.repeat(depth) + t + '\n'
    } else if (t.startsWith('<') && t.endsWith('/>')) {
      out += pad.repeat(depth) + t + '\n'
    } else if (t.startsWith('<!') || t.startsWith('<?')) {
      out += pad.repeat(depth) + t + '\n'
    } else if (t.startsWith('<')) {
      out += pad.repeat(depth) + t + '\n'
      depth++
    } else {
      const text = t.trim()
      if (text) out += pad.repeat(depth) + text + '\n'
    }
  }
  return out.trim()
}

/* ============================================================
 * 行处理
 * ============================================================ */

export function processLines(src, opts = {}) {
  if (!src) return ''
  const {
    trim = false, removeEmpty = false, dedup = false,
    sort = 'none',           // none / asc / desc / shuffle
    reverse = false,
    prefix = '', suffix = '',
    numbering = false, startNo = 1, numWidth = 0, numSep = '. ',
  } = opts
  let lines = src.split(/\r?\n/)
  if (trim) lines = lines.map(l => l.trim())
  if (removeEmpty) lines = lines.filter(l => l.length > 0)
  if (dedup) {
    const seen = new Set()
    lines = lines.filter(l => (seen.has(l) ? false : (seen.add(l), true)))
  }
  if (sort === 'asc') lines = lines.slice().sort((a, b) => a.localeCompare(b))
  else if (sort === 'desc') lines = lines.slice().sort((a, b) => b.localeCompare(a))
  else if (sort === 'shuffle') {
    lines = lines.slice()
    for (let i = lines.length - 1; i > 0; i--) {
      const j = Math.floor(Math.random() * (i + 1))
      ;[lines[i], lines[j]] = [lines[j], lines[i]]
    }
  }
  if (reverse) lines = lines.slice().reverse()
  if (prefix || suffix) lines = lines.map(l => prefix + l + suffix)
  if (numbering) {
    lines = lines.map((l, i) => {
      const no = String(startNo + i)
      const padNo = numWidth > 0 ? no.padStart(numWidth, '0') : no
      return padNo + numSep + l
    })
  }
  return lines.join('\n')
}
