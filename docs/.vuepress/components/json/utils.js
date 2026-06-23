// ============================================================
// JsonToolbox 纯函数库
//   - 解析：looseParse / locateError
//   - 格式化：sortDeep
//   - 转换：toYAML / toXML / toCSV / toTS / toGo / toJava / toQuery
//   - 树：buildNode
//   - 对比：jsonDiff
//   - 通用：copyText / downloadText
// 所有函数无副作用、可单独测试，浏览器端运行。
// ============================================================

// 宽松 JSON 解析（兼容尾逗号、单行/块注释、单引号字符串）
export function looseParse(text) {
  let out = ''
  let i = 0, inStr = false, strCh = ''
  while (i < text.length) {
    const c = text[i], n = text[i + 1]
    if (inStr) {
      out += c
      if (c === '\\' && i + 1 < text.length) { out += text[i + 1]; i += 2; continue }
      if (c === strCh) inStr = false
      i++; continue
    }
    if (c === '"' || c === "'") { inStr = true; strCh = c; out += c === "'" ? '"' : c; i++; continue }
    if (c === '/' && n === '/') { while (i < text.length && text[i] !== '\n') i++; continue }
    if (c === '/' && n === '*') { i += 2; while (i < text.length - 1 && !(text[i] === '*' && text[i + 1] === '/')) i++; i += 2; continue }
    out += c; i++
  }
  out = out.replace(/,(\s*[}\]])/g, '$1')
  return JSON.parse(out)
}

// 解析错误的行/列定位 + 上下文片段
export function locateError(text, e) {
  const msg = String(e && e.message || e)
  let pos = -1, line = 0, col = 0
  let m = msg.match(/position\s+(\d+)/i)
  if (m) pos = parseInt(m[1], 10)
  if (pos >= 0) {
    const before = text.slice(0, pos)
    line = before.split('\n').length
    col = pos - before.lastIndexOf('\n')
  } else {
    m = msg.match(/line\s+(\d+).*?column\s+(\d+)/i)
    if (m) { line = parseInt(m[1], 10); col = parseInt(m[2], 10) }
  }
  let snippet = ''
  if (line > 0) {
    const lines = text.split('\n')
    const start = Math.max(0, line - 3)
    const end = Math.min(lines.length, line + 2)
    for (let i = start; i < end; i++) {
      const ln = i + 1
      const isErr = ln === line
      const escaped = lines[i].replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
      snippet += `<span class="${isErr ? 'jt-snip-err' : 'jt-snip-ok'}">${String(ln).padStart(4)} | ${escaped}</span>\n`
      if (isErr && col > 0) snippet += `<span class="jt-snip-caret">     | ${' '.repeat(col - 1)}^</span>\n`
    }
  }
  return { line, col, message: msg, snippet }
}

// 深度排序对象 key
export function sortDeep(o) {
  if (Array.isArray(o)) return o.map(v => sortDeep(v))
  if (o && typeof o === 'object') {
    const s = {}
    Object.keys(o).sort().forEach(k => { s[k] = sortDeep(o[k]) })
    return s
  }
  return o
}

// ===== 转换 =====
export function toYAML(obj, indent = 0) {
  const pad = '  '.repeat(indent)
  if (obj === null) return 'null'
  if (typeof obj !== 'object') {
    if (typeof obj === 'string') {
      if (/^[\w\-./]+$/.test(obj) && obj.length < 80) return obj
      return JSON.stringify(obj)
    }
    return String(obj)
  }
  if (Array.isArray(obj)) {
    if (obj.length === 0) return '[]'
    return obj.map(v => {
      if (v !== null && typeof v === 'object') {
        const sub = toYAML(v, indent + 1)
        return `${pad}-\n${sub}`
      }
      return `${pad}- ${toYAML(v, indent + 1)}`
    }).join('\n')
  }
  const keys = Object.keys(obj)
  if (keys.length === 0) return '{}'
  return keys.map(k => {
    const v = obj[k]
    if (v !== null && typeof v === 'object' && (Array.isArray(v) ? v.length : Object.keys(v).length)) {
      return `${pad}${k}:\n${toYAML(v, indent + 1)}`
    }
    return `${pad}${k}: ${toYAML(v, indent + 1)}`
  }).join('\n')
}

export function toXML(obj, name = 'root', indent = 0) {
  const pad = '  '.repeat(indent)
  if (obj === null || obj === undefined) return `${pad}<${name}/>`
  if (typeof obj !== 'object') {
    const v = String(obj).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
    return `${pad}<${name}>${v}</${name}>`
  }
  if (Array.isArray(obj)) return obj.map(v => toXML(v, name, indent)).join('\n')
  const inner = Object.keys(obj).map(k => toXML(obj[k], k.replace(/[^\w-]/g, '_'), indent + 1)).join('\n')
  return `${pad}<${name}>\n${inner}\n${pad}</${name}>`
}

export function toCSV(obj) {
  if (!Array.isArray(obj)) throw new Error('CSV 转换要求顶级为数组')
  if (obj.length === 0) return ''
  const headers = Array.from(new Set(obj.flatMap(o => o && typeof o === 'object' ? Object.keys(o) : [])))
  const esc = v => {
    if (v === null || v === undefined) return ''
    const s = typeof v === 'object' ? JSON.stringify(v) : String(v)
    return /[",\n]/.test(s) ? `"${s.replace(/"/g, '""')}"` : s
  }
  const lines = [headers.join(',')]
  obj.forEach(row => lines.push(headers.map(h => esc(row && row[h])).join(',')))
  return lines.join('\n')
}

export function toTS(obj, name = 'Root') {
  const out = []
  const cache = new Map()
  const tsType = (v) => {
    if (v === null) return 'any'
    if (Array.isArray(v)) return v.length === 0 ? 'any[]' : `${tsType(v[0])}[]`
    if (typeof v === 'object') {
      const sig = JSON.stringify(Object.keys(v).sort())
      let n = cache.get(sig)
      if (!n) { n = name + (cache.size === 0 ? '' : cache.size + 1); cache.set(sig, n); emit(v, n) }
      return n
    }
    return typeof v
  }
  const emit = (o, n) => {
    const lines = [`interface ${n} {`]
    Object.keys(o).forEach(k => {
      const key = /^[a-zA-Z_$][\w$]*$/.test(k) ? k : `"${k}"`
      lines.push(`  ${key}: ${tsType(o[k])};`)
    })
    lines.push('}')
    out.push(lines.join('\n'))
  }
  if (obj !== null && typeof obj === 'object' && !Array.isArray(obj)) emit(obj, name)
  else out.push(`type ${name} = ${tsType(obj)};`)
  return out.reverse().join('\n\n')
}

export function toGo(obj, name = 'Root') {
  const out = []
  const cache = new Map()
  const cap = s => s ? s[0].toUpperCase() + s.slice(1) : s
  const goType = (v) => {
    if (v === null) return 'interface{}'
    if (Array.isArray(v)) return v.length === 0 ? '[]interface{}' : `[]${goType(v[0])}`
    if (typeof v === 'object') {
      const sig = JSON.stringify(Object.keys(v).sort())
      let n = cache.get(sig)
      if (!n) { n = name + (cache.size === 0 ? '' : cache.size + 1); cache.set(sig, n); emit(v, n) }
      return n
    }
    if (typeof v === 'number') return Number.isInteger(v) ? 'int' : 'float64'
    if (typeof v === 'boolean') return 'bool'
    return 'string'
  }
  const emit = (o, n) => {
    const lines = [`type ${n} struct {`]
    Object.keys(o).forEach(k => lines.push(`    ${cap(k.replace(/[^\w]/g, '_'))} ${goType(o[k])} \`json:"${k}"\``))
    lines.push('}')
    out.push(lines.join('\n'))
  }
  if (obj !== null && typeof obj === 'object' && !Array.isArray(obj)) emit(obj, name)
  else out.push(`type ${name} ${goType(obj)}`)
  return out.reverse().join('\n\n')
}

export function toJava(obj, name = 'Root') {
  const cap = s => s ? s[0].toUpperCase() + s.slice(1) : s
  const out = []
  const cache = new Map()
  const javaType = (v) => {
    if (v === null) return 'Object'
    if (Array.isArray(v)) return v.length === 0 ? 'List<Object>' : `List<${javaType(v[0])}>`
    if (typeof v === 'object') {
      const sig = JSON.stringify(Object.keys(v).sort())
      let n = cache.get(sig)
      if (!n) { n = name + (cache.size === 0 ? '' : cache.size + 1); cache.set(sig, n); emit(v, n) }
      return n
    }
    if (typeof v === 'number') return Number.isInteger(v) ? 'Integer' : 'Double'
    if (typeof v === 'boolean') return 'Boolean'
    return 'String'
  }
  const emit = (o, n) => {
    const fields = Object.keys(o)
    const lines = [`public class ${n} {`]
    fields.forEach(k => lines.push(`    private ${javaType(o[k])} ${k.replace(/[^\w]/g, '_')};`))
    lines.push('')
    fields.forEach(k => {
      const key = k.replace(/[^\w]/g, '_')
      const t = javaType(o[k])
      lines.push(`    public ${t} get${cap(key)}() { return ${key}; }`)
      lines.push(`    public void set${cap(key)}(${t} ${key}) { this.${key} = ${key}; }`)
    })
    lines.push('}')
    out.push(lines.join('\n'))
  }
  if (obj !== null && typeof obj === 'object' && !Array.isArray(obj)) emit(obj, name)
  else out.push(`// 顶级非对象，无法生成 Bean，类型：${javaType(obj)}`)
  return out.reverse().join('\n\n')
}

export function toQuery(obj) {
  const parts = []
  const walk = (v, prefix) => {
    if (v === null || v === undefined) return
    if (Array.isArray(v)) v.forEach((it, i) => walk(it, `${prefix}[${i}]`))
    else if (typeof v === 'object') Object.keys(v).forEach(k => walk(v[k], prefix ? `${prefix}.${k}` : k))
    else parts.push(`${encodeURIComponent(prefix)}=${encodeURIComponent(v)}`)
  }
  walk(obj, '')
  return parts.join('&')
}

// 构建树节点
export function buildNode(value, key = '$', path = '$', depth = 0) {
  const type = value === null ? 'null' : Array.isArray(value) ? 'array' : typeof value
  const node = { key: String(key), path, type, depth, preview: '', summary: '', children: null }
  if (type === 'object') {
    const keys = Object.keys(value)
    node.summary = `{ ${keys.length} 键 }`
    node.children = keys.map(k => {
      const nextPath = /^[a-zA-Z_$][\w$]*$/.test(k) ? `${path}.${k}` : `${path}["${k}"]`
      return buildNode(value[k], k, nextPath, depth + 1)
    })
  } else if (type === 'array') {
    node.summary = `[ ${value.length} 项 ]`
    node.children = value.map((v, i) => buildNode(v, `[${i}]`, `${path}[${i}]`, depth + 1))
  } else if (type === 'string') {
    node.preview = '"' + (value.length > 60 ? value.slice(0, 60) + '…' : value) + '"'
  } else {
    node.preview = String(value)
  }
  return node
}

// 语义级 Diff
export function jsonDiff(a, b, opts) {
  const added = [], removed = [], changed = []
  const isObj = x => x !== null && typeof x === 'object'
  const norm = v => opts.ignoreCase && typeof v === 'string' ? v.toLowerCase() : v
  const arrEq = (x, y) => {
    if (x.length !== y.length) return false
    if (opts.ignoreOrder) {
      const sx = [...x].map(v => JSON.stringify(v)).sort()
      const sy = [...y].map(v => JSON.stringify(v)).sort()
      return sx.every((v, i) => v === sy[i])
    }
    return false
  }
  const walk = (x, y, path) => {
    if (Array.isArray(x) && Array.isArray(y)) {
      if (arrEq(x, y)) return
      const max = Math.max(x.length, y.length)
      for (let i = 0; i < max; i++) {
        const p = `${path}[${i}]`
        if (i >= x.length) added.push({ path: p, val: JSON.stringify(y[i]) })
        else if (i >= y.length) removed.push({ path: p, val: JSON.stringify(x[i]) })
        else walk(x[i], y[i], p)
      }
      return
    }
    if (isObj(x) && isObj(y)) {
      const keys = new Set([...Object.keys(x), ...Object.keys(y)])
      keys.forEach(k => {
        const p = path === '$' ? `$.${k}` : `${path}.${k}`
        if (!(k in x)) added.push({ path: p, val: JSON.stringify(y[k]) })
        else if (!(k in y)) removed.push({ path: p, val: JSON.stringify(x[k]) })
        else walk(x[k], y[k], p)
      })
      return
    }
    if (norm(x) !== norm(y)) changed.push({ path, oldVal: JSON.stringify(x), newVal: JSON.stringify(y) })
  }
  walk(a, b, '$')
  return { added, removed, changed, equal: added.length === 0 && removed.length === 0 && changed.length === 0 }
}

// ===== 通用 =====
export function copyText(text) {
  if (!text) return Promise.resolve()
  if (navigator.clipboard && navigator.clipboard.writeText) return navigator.clipboard.writeText(text)
  return new Promise((res, rej) => {
    const ta = document.createElement('textarea')
    ta.value = text; document.body.appendChild(ta); ta.select()
    try { document.execCommand('copy'); res() } catch (e) { rej(e) } finally { document.body.removeChild(ta) }
  })
}

export function downloadText(text, name, mime = 'application/json;charset=utf-8') {
  const blob = new Blob([text], { type: mime })
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a'); a.href = url; a.download = name; a.click()
  setTimeout(() => URL.revokeObjectURL(url), 1000)
}

// 复制 mixin：被各 Panel 复用，提供 this.copy(text, key) + 视觉反馈 this.copyHint
export const copyMixin = {
  data() { return { copyHint: '' } },
  methods: {
    copy(text, key) {
      if (!text) return
      copyText(text).then(() => {
        this.copyHint = key
        setTimeout(() => { this.copyHint = '' }, 1400)
      })
    },
    download: downloadText,
  },
}
