/* ============================================================
 * NetworkToolbox · 工具函数
 *  - URL 解析 / UA 解析 / HTTP 状态码 / MIME / CIDR
 *  - 零依赖，全部本地运算
 * ============================================================ */

export const copyMixin = {
  methods: {
    async ntCopy(text) {
      if (!text) return false
      try {
        if (navigator.clipboard && window.isSecureContext) {
          await navigator.clipboard.writeText(text)
        } else {
          const ta = document.createElement('textarea')
          ta.value = text
          document.body.appendChild(ta)
          ta.select()
          document.execCommand('copy')
          document.body.removeChild(ta)
        }
        return true
      } catch (e) { return false }
    },
  },
}

/* ============================================================
 * URL 解析
 * ============================================================ */

export function parseUrl(input) {
  if (!input) return null
  let s = input.trim()
  // 自动补 scheme（仅用于解析展示，不外发请求）
  if (!/^[a-z][a-z0-9+\-.]*:\/\//i.test(s) && !s.startsWith('//')) {
    s = 'http://' + s
  }
  let u
  try { u = new URL(s) }
  catch (e) { throw new Error('URL 格式无效：' + e.message) }
  const params = []
  for (const [k, v] of u.searchParams) params.push({ key: k, value: v })
  return {
    href: u.href,
    protocol: u.protocol.replace(':', ''),
    username: u.username,
    password: u.password,
    host: u.host,
    hostname: u.hostname,
    port: u.port || (u.protocol === 'https:' ? '443' : u.protocol === 'http:' ? '80' : ''),
    pathname: u.pathname,
    search: u.search,
    hash: u.hash,
    origin: u.origin,
    params,
  }
}

export function buildUrl(base, params) {
  try {
    const u = new URL(base)
    u.search = ''
    for (const p of params) {
      if (p.key) u.searchParams.append(p.key, p.value || '')
    }
    return u.href
  } catch (e) { return '' }
}

export function encodeUrl(s) { return encodeURIComponent(s) }
export function decodeUrl(s) {
  try { return decodeURIComponent(s) }
  catch (e) { throw new Error('解码失败：' + e.message) }
}

/* QueryString ↔ JSON */
export function qsToJson(s) {
  if (!s) return {}
  const str = s.startsWith('?') ? s.slice(1) : s
  const out = {}
  for (const pair of str.split('&')) {
    if (!pair) continue
    const idx = pair.indexOf('=')
    const k = idx < 0 ? pair : pair.slice(0, idx)
    const v = idx < 0 ? '' : pair.slice(idx + 1)
    try {
      const dk = decodeURIComponent(k.replace(/\+/g, ' '))
      const dv = decodeURIComponent(v.replace(/\+/g, ' '))
      if (out[dk] !== undefined) {
        if (!Array.isArray(out[dk])) out[dk] = [out[dk]]
        out[dk].push(dv)
      } else { out[dk] = dv }
    } catch (e) { out[k] = v }
  }
  return out
}

export function jsonToQs(obj) {
  const parts = []
  for (const [k, v] of Object.entries(obj || {})) {
    if (Array.isArray(v)) {
      for (const it of v) parts.push(encodeURIComponent(k) + '=' + encodeURIComponent(it == null ? '' : it))
    } else {
      parts.push(encodeURIComponent(k) + '=' + encodeURIComponent(v == null ? '' : v))
    }
  }
  return parts.join('&')
}

/* ============================================================
 * User-Agent 解析（轻量）
 * ============================================================ */

export function parseUa(ua) {
  if (!ua) return null
  const out = { browser: '', browserVer: '', os: '', osVer: '', device: 'Desktop', engine: '' }

  // OS
  if (/Windows NT 10/.test(ua)) { out.os = 'Windows'; out.osVer = '10/11' }
  else if (/Windows NT 6\.3/.test(ua)) { out.os = 'Windows'; out.osVer = '8.1' }
  else if (/Windows NT 6\.1/.test(ua)) { out.os = 'Windows'; out.osVer = '7' }
  else if (/Mac OS X (\d+[._]\d+(?:[._]\d+)?)/.test(ua)) {
    out.os = 'macOS'
    out.osVer = RegExp.$1.replace(/_/g, '.')
  } else if (/Android (\d+(?:\.\d+)?)/.test(ua)) {
    out.os = 'Android'; out.osVer = RegExp.$1; out.device = 'Mobile'
  } else if (/iPhone OS (\d+[_]\d+(?:[_]\d+)?)/.test(ua)) {
    out.os = 'iOS'; out.osVer = RegExp.$1.replace(/_/g, '.'); out.device = /iPad/.test(ua) ? 'Tablet' : 'Mobile'
  } else if (/Linux/.test(ua)) { out.os = 'Linux' }

  // Device
  if (/iPad/.test(ua)) out.device = 'Tablet'
  else if (/Mobile|Android|iPhone/.test(ua)) out.device = 'Mobile'

  // Browser
  if (/Edg\/(\d+(?:\.\d+)*)/.test(ua)) { out.browser = 'Edge'; out.browserVer = RegExp.$1 }
  else if (/OPR\/(\d+(?:\.\d+)*)|Opera\/(\d+(?:\.\d+)*)/.test(ua)) {
    out.browser = 'Opera'; out.browserVer = RegExp.$1 || RegExp.$2
  } else if (/Firefox\/(\d+(?:\.\d+)*)/.test(ua)) { out.browser = 'Firefox'; out.browserVer = RegExp.$1 }
  else if (/Chrome\/(\d+(?:\.\d+)*)/.test(ua)) { out.browser = 'Chrome'; out.browserVer = RegExp.$1 }
  else if (/Version\/(\d+(?:\.\d+)*).+Safari/.test(ua)) { out.browser = 'Safari'; out.browserVer = RegExp.$1 }
  else if (/MSIE (\d+(?:\.\d+)*)/.test(ua) || /Trident.+rv:(\d+(?:\.\d+)*)/.test(ua)) {
    out.browser = 'IE'; out.browserVer = RegExp.$1
  }

  // Engine
  if (/Gecko\//.test(ua)) out.engine = 'Gecko'
  else if (/WebKit\//.test(ua)) out.engine = 'WebKit'
  else if (/Trident/.test(ua)) out.engine = 'Trident'
  else if (/Blink/.test(ua)) out.engine = 'Blink'

  // 微信 / QQ 内嵌
  if (/MicroMessenger/.test(ua)) out.browser = '微信内置 (' + (out.browser || 'WebView') + ')'
  if (/\bQQ\//.test(ua)) out.browser = 'QQ 内置 (' + (out.browser || 'WebView') + ')'
  if (/\bDingTalk/.test(ua)) out.browser = '钉钉 (' + (out.browser || 'WebView') + ')'

  return out
}

/* ============================================================
 * HTTP 状态码
 * ============================================================ */

export const HTTP_STATUS = [
  { code: 100, name: 'Continue', desc: '继续。客户端应继续其请求' },
  { code: 101, name: 'Switching Protocols', desc: '服务器根据客户端的请求切换协议' },
  { code: 200, name: 'OK', desc: '请求成功' },
  { code: 201, name: 'Created', desc: '已创建。成功请求并创建了新资源' },
  { code: 202, name: 'Accepted', desc: '已接受。已经接受请求，但未处理完成' },
  { code: 204, name: 'No Content', desc: '无内容。服务器成功处理但未返回内容' },
  { code: 206, name: 'Partial Content', desc: '部分内容。服务器成功处理了部分 GET 请求' },
  { code: 301, name: 'Moved Permanently', desc: '永久重定向。资源已被永久移动到新 URI' },
  { code: 302, name: 'Found', desc: '临时重定向。资源临时位于不同的 URI' },
  { code: 304, name: 'Not Modified', desc: '未修改。资源未修改，可使用缓存' },
  { code: 307, name: 'Temporary Redirect', desc: '临时重定向。使用相同方法重新请求' },
  { code: 308, name: 'Permanent Redirect', desc: '永久重定向。使用相同方法重新请求' },
  { code: 400, name: 'Bad Request', desc: '客户端请求语法错误' },
  { code: 401, name: 'Unauthorized', desc: '请求未授权，需要身份认证' },
  { code: 403, name: 'Forbidden', desc: '服务器理解请求但拒绝执行' },
  { code: 404, name: 'Not Found', desc: '服务器找不到请求的资源' },
  { code: 405, name: 'Method Not Allowed', desc: '请求方法不允许' },
  { code: 408, name: 'Request Timeout', desc: '请求超时' },
  { code: 409, name: 'Conflict', desc: '请求冲突，无法完成' },
  { code: 410, name: 'Gone', desc: '资源已被永久删除' },
  { code: 413, name: 'Payload Too Large', desc: '请求体过大' },
  { code: 414, name: 'URI Too Long', desc: 'URI 过长' },
  { code: 415, name: 'Unsupported Media Type', desc: '不支持的媒体类型' },
  { code: 422, name: 'Unprocessable Entity', desc: '语义错误，请求无法被处理' },
  { code: 429, name: 'Too Many Requests', desc: '请求过多，触发限流' },
  { code: 500, name: 'Internal Server Error', desc: '服务器内部错误' },
  { code: 501, name: 'Not Implemented', desc: '服务器不支持该请求的方法' },
  { code: 502, name: 'Bad Gateway', desc: '网关错误。上游服务器返回无效响应' },
  { code: 503, name: 'Service Unavailable', desc: '服务不可用，可能正在维护' },
  { code: 504, name: 'Gateway Timeout', desc: '网关超时' },
  { code: 505, name: 'HTTP Version Not Supported', desc: '不支持的 HTTP 版本' },
]

/* ============================================================
 * MIME 类型
 * ============================================================ */

export const MIME_TYPES = [
  // Text
  { ext: 'txt', type: 'text/plain' },
  { ext: 'html', type: 'text/html' },
  { ext: 'htm', type: 'text/html' },
  { ext: 'css', type: 'text/css' },
  { ext: 'csv', type: 'text/csv' },
  { ext: 'md', type: 'text/markdown' },
  // App
  { ext: 'js', type: 'application/javascript' },
  { ext: 'mjs', type: 'application/javascript' },
  { ext: 'json', type: 'application/json' },
  { ext: 'xml', type: 'application/xml' },
  { ext: 'pdf', type: 'application/pdf' },
  { ext: 'zip', type: 'application/zip' },
  { ext: 'gz', type: 'application/gzip' },
  { ext: 'tar', type: 'application/x-tar' },
  { ext: '7z', type: 'application/x-7z-compressed' },
  { ext: 'rar', type: 'application/vnd.rar' },
  { ext: 'doc', type: 'application/msword' },
  { ext: 'docx', type: 'application/vnd.openxmlformats-officedocument.wordprocessingml.document' },
  { ext: 'xls', type: 'application/vnd.ms-excel' },
  { ext: 'xlsx', type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet' },
  { ext: 'ppt', type: 'application/vnd.ms-powerpoint' },
  { ext: 'pptx', type: 'application/vnd.openxmlformats-officedocument.presentationml.presentation' },
  { ext: 'wasm', type: 'application/wasm' },
  // Image
  { ext: 'png', type: 'image/png' },
  { ext: 'jpg', type: 'image/jpeg' },
  { ext: 'jpeg', type: 'image/jpeg' },
  { ext: 'gif', type: 'image/gif' },
  { ext: 'webp', type: 'image/webp' },
  { ext: 'svg', type: 'image/svg+xml' },
  { ext: 'ico', type: 'image/x-icon' },
  { ext: 'bmp', type: 'image/bmp' },
  { ext: 'avif', type: 'image/avif' },
  // Audio
  { ext: 'mp3', type: 'audio/mpeg' },
  { ext: 'wav', type: 'audio/wav' },
  { ext: 'ogg', type: 'audio/ogg' },
  { ext: 'aac', type: 'audio/aac' },
  // Video
  { ext: 'mp4', type: 'video/mp4' },
  { ext: 'webm', type: 'video/webm' },
  { ext: 'mov', type: 'video/quicktime' },
  { ext: 'avi', type: 'video/x-msvideo' },
  // Font
  { ext: 'woff', type: 'font/woff' },
  { ext: 'woff2', type: 'font/woff2' },
  { ext: 'ttf', type: 'font/ttf' },
  { ext: 'otf', type: 'font/otf' },
  { ext: 'eot', type: 'application/vnd.ms-fontobject' },
]

/* ============================================================
 * CIDR 子网计算
 * ============================================================ */

function ipToInt(ip) {
  const m = ip.trim().split('.')
  if (m.length !== 4) throw new Error('IP 格式无效')
  let n = 0
  for (let i = 0; i < 4; i++) {
    const b = parseInt(m[i], 10)
    if (!Number.isFinite(b) || b < 0 || b > 255) throw new Error('IP 字节越界：' + m[i])
    n = n * 256 + b
  }
  return n
}

function intToIp(n) {
  return [
    (n >>> 24) & 0xFF,
    (n >>> 16) & 0xFF,
    (n >>> 8) & 0xFF,
    n & 0xFF,
  ].join('.')
}

export function parseCidr(input) {
  if (!input) throw new Error('请输入 CIDR')
  const s = input.trim()
  let ip, prefix
  if (s.includes('/')) {
    ;[ip, prefix] = s.split('/')
    prefix = parseInt(prefix, 10)
  } else {
    ip = s; prefix = 32
  }
  if (!Number.isFinite(prefix) || prefix < 0 || prefix > 32) throw new Error('前缀长度需在 0-32 之间')
  const ipInt = ipToInt(ip)
  // 用 BigInt 避免 32 位左移溢出
  const mask = prefix === 0 ? 0 : ((0xFFFFFFFF << (32 - prefix)) >>> 0)
  const network = (ipInt & mask) >>> 0
  const broadcast = (network | (~mask >>> 0)) >>> 0
  const total = prefix === 32 ? 1 : prefix === 31 ? 2 : 2 ** (32 - prefix)
  const usable = prefix >= 31 ? total : Math.max(total - 2, 0)
  const firstHost = prefix >= 31 ? network : (network + 1) >>> 0
  const lastHost = prefix >= 31 ? broadcast : (broadcast - 1) >>> 0

  // 二进制
  const bin = (n) => intToIp(n).split('.').map(x => parseInt(x, 10).toString(2).padStart(8, '0')).join('.')

  // 类别
  let cls = ''
  const a = (ipInt >>> 24) & 0xFF
  if (a < 128) cls = 'A'
  else if (a < 192) cls = 'B'
  else if (a < 224) cls = 'C'
  else if (a < 240) cls = 'D（组播）'
  else cls = 'E（保留）'

  // 私有 IP
  const isPrivate = (a === 10) ||
    (a === 172 && ((ipInt >>> 16) & 0xFF) >= 16 && ((ipInt >>> 16) & 0xFF) <= 31) ||
    (a === 192 && ((ipInt >>> 16) & 0xFF) === 168) ||
    (a === 127)

  return {
    cidr: `${intToIp(ipInt)}/${prefix}`,
    ip: intToIp(ipInt),
    network: intToIp(network),
    broadcast: intToIp(broadcast),
    mask: intToIp(mask),
    wildcard: intToIp((~mask >>> 0)),
    prefix,
    total, usable,
    firstHost: intToIp(firstHost),
    lastHost: intToIp(lastHost),
    binaryMask: bin(mask),
    binaryIp: bin(ipInt),
    class: cls,
    isPrivate,
  }
}
