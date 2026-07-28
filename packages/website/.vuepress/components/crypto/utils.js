/* ============================================================
 * CryptoToolbox · 工具函数
 *  - 全部基于浏览器原生 Web Crypto API (SubtleCrypto)
 *  - MD5 自实现（SubtleCrypto 不支持 MD5）
 *  - 所有运算在本地完成，密钥与明文不上传服务器
 * ============================================================ */

/* ---------- 编解码工具 ---------- */

const te = new TextEncoder()
const td = new TextDecoder()

export function strToBytes(s) { return te.encode(s) }
export function bytesToStr(b) { return td.decode(b) }

export function bytesToHex(buf) {
  const arr = buf instanceof ArrayBuffer ? new Uint8Array(buf) : buf
  let out = ''
  for (let i = 0; i < arr.length; i++) {
    out += arr[i].toString(16).padStart(2, '0')
  }
  return out
}

export function hexToBytes(hex) {
  hex = hex.replace(/\s+/g, '')
  if (hex.length % 2 !== 0) throw new Error('hex 长度必须为偶数')
  if (!/^[0-9a-f]*$/i.test(hex)) throw new Error('hex 含非法字符')
  const out = new Uint8Array(hex.length / 2)
  for (let i = 0; i < out.length; i++) {
    out[i] = parseInt(hex.substr(i * 2, 2), 16)
  }
  return out
}

export function bytesToBase64(buf) {
  const arr = buf instanceof ArrayBuffer ? new Uint8Array(buf) : buf
  let bin = ''
  for (let i = 0; i < arr.length; i++) bin += String.fromCharCode(arr[i])
  return btoa(bin)
}

export function base64ToBytes(b64) {
  // 兼容 URL-safe
  const s = b64.replace(/-/g, '+').replace(/_/g, '/').replace(/\s+/g, '')
  const pad = s.length % 4 === 0 ? s : s + '='.repeat(4 - (s.length % 4))
  const bin = atob(pad)
  const out = new Uint8Array(bin.length)
  for (let i = 0; i < bin.length; i++) out[i] = bin.charCodeAt(i)
  return out
}

/* ---------- Base64 编/解码（UTF-8 安全） ---------- */

export function base64Encode(text, urlSafe = false) {
  const b = bytesToBase64(te.encode(text))
  return urlSafe ? b.replace(/\+/g, '-').replace(/\//g, '_').replace(/=+$/, '') : b
}

export function base64Decode(b64) {
  return td.decode(base64ToBytes(b64))
}

/* ============================================================
 * Hash
 * ============================================================ */

async function subtleHash(algo, data) {
  const buf = await crypto.subtle.digest(algo, te.encode(data))
  return bytesToHex(buf)
}

export const sha1   = (s) => subtleHash('SHA-1', s)
export const sha256 = (s) => subtleHash('SHA-256', s)
export const sha384 = (s) => subtleHash('SHA-384', s)
export const sha512 = (s) => subtleHash('SHA-512', s)

/* ----------- MD5 自实现 ----------- */
/* 参考 RFC 1321，纯 JS 实现，供本地校验/对比用 */
function md5cycle(x, k) {
  let a = x[0], b = x[1], c = x[2], d = x[3]
  a = ff(a, b, c, d, k[0],  7,  -680876936)
  d = ff(d, a, b, c, k[1],  12, -389564586)
  c = ff(c, d, a, b, k[2],  17, 606105819)
  b = ff(b, c, d, a, k[3],  22, -1044525330)
  a = ff(a, b, c, d, k[4],  7,  -176418897)
  d = ff(d, a, b, c, k[5],  12, 1200080426)
  c = ff(c, d, a, b, k[6],  17, -1473231341)
  b = ff(b, c, d, a, k[7],  22, -45705983)
  a = ff(a, b, c, d, k[8],  7,  1770035416)
  d = ff(d, a, b, c, k[9],  12, -1958414417)
  c = ff(c, d, a, b, k[10], 17, -42063)
  b = ff(b, c, d, a, k[11], 22, -1990404162)
  a = ff(a, b, c, d, k[12], 7,  1804603682)
  d = ff(d, a, b, c, k[13], 12, -40341101)
  c = ff(c, d, a, b, k[14], 17, -1502002290)
  b = ff(b, c, d, a, k[15], 22, 1236535329)

  a = gg(a, b, c, d, k[1],  5,  -165796510)
  d = gg(d, a, b, c, k[6],  9,  -1069501632)
  c = gg(c, d, a, b, k[11], 14, 643717713)
  b = gg(b, c, d, a, k[0],  20, -373897302)
  a = gg(a, b, c, d, k[5],  5,  -701558691)
  d = gg(d, a, b, c, k[10], 9,  38016083)
  c = gg(c, d, a, b, k[15], 14, -660478335)
  b = gg(b, c, d, a, k[4],  20, -405537848)
  a = gg(a, b, c, d, k[9],  5,  568446438)
  d = gg(d, a, b, c, k[14], 9,  -1019803690)
  c = gg(c, d, a, b, k[3],  14, -187363961)
  b = gg(b, c, d, a, k[8],  20, 1163531501)
  a = gg(a, b, c, d, k[13], 5,  -1444681467)
  d = gg(d, a, b, c, k[2],  9,  -51403784)
  c = gg(c, d, a, b, k[7],  14, 1735328473)
  b = gg(b, c, d, a, k[12], 20, -1926607734)

  a = hh(a, b, c, d, k[5],  4,  -378558)
  d = hh(d, a, b, c, k[8],  11, -2022574463)
  c = hh(c, d, a, b, k[11], 16, 1839030562)
  b = hh(b, c, d, a, k[14], 23, -35309556)
  a = hh(a, b, c, d, k[1],  4,  -1530992060)
  d = hh(d, a, b, c, k[4],  11, 1272893353)
  c = hh(c, d, a, b, k[7],  16, -155497632)
  b = hh(b, c, d, a, k[10], 23, -1094730640)
  a = hh(a, b, c, d, k[13], 4,  681279174)
  d = hh(d, a, b, c, k[0],  11, -358537222)
  c = hh(c, d, a, b, k[3],  16, -722521979)
  b = hh(b, c, d, a, k[6],  23, 76029189)
  a = hh(a, b, c, d, k[9],  4,  -640364487)
  d = hh(d, a, b, c, k[12], 11, -421815835)
  c = hh(c, d, a, b, k[15], 16, 530742520)
  b = hh(b, c, d, a, k[2],  23, -995338651)

  a = ii(a, b, c, d, k[0],  6,  -198630844)
  d = ii(d, a, b, c, k[7],  10, 1126891415)
  c = ii(c, d, a, b, k[14], 15, -1416354905)
  b = ii(b, c, d, a, k[5],  21, -57434055)
  a = ii(a, b, c, d, k[12], 6,  1700485571)
  d = ii(d, a, b, c, k[3],  10, -1894986606)
  c = ii(c, d, a, b, k[10], 15, -1051523)
  b = ii(b, c, d, a, k[1],  21, -2054922799)
  a = ii(a, b, c, d, k[8],  6,  1873313359)
  d = ii(d, a, b, c, k[15], 10, -30611744)
  c = ii(c, d, a, b, k[6],  15, -1560198380)
  b = ii(b, c, d, a, k[13], 21, 1309151649)
  a = ii(a, b, c, d, k[4],  6,  -145523070)
  d = ii(d, a, b, c, k[11], 10, -1120210379)
  c = ii(c, d, a, b, k[2],  15, 718787259)
  b = ii(b, c, d, a, k[9],  21, -343485551)

  x[0] = add32(a, x[0])
  x[1] = add32(b, x[1])
  x[2] = add32(c, x[2])
  x[3] = add32(d, x[3])
}
function cmn(q, a, b, x, s, t) { a = add32(add32(a, q), add32(x, t)); return add32((a << s) | (a >>> (32 - s)), b) }
function ff(a, b, c, d, x, s, t) { return cmn((b & c) | ((~b) & d), a, b, x, s, t) }
function gg(a, b, c, d, x, s, t) { return cmn((b & d) | (c & (~d)), a, b, x, s, t) }
function hh(a, b, c, d, x, s, t) { return cmn(b ^ c ^ d, a, b, x, s, t) }
function ii(a, b, c, d, x, s, t) { return cmn(c ^ (b | (~d)), a, b, x, s, t) }
function add32(a, b) { return (a + b) & 0xFFFFFFFF }

function md51(bytes) {
  const n = bytes.length
  const state = [1732584193, -271733879, -1732584194, 271733878]
  let i
  for (i = 64; i <= n; i += 64) md5cycle(state, md5blk(bytes.subarray(i - 64, i)))
  const tail = new Uint8Array(64)
  const left = n % 64
  tail.set(bytes.subarray(n - left))
  tail[left] = 0x80
  if (left >= 56) {
    md5cycle(state, md5blk(tail))
    tail.fill(0)
  }
  const bits = n * 8
  // low 32 bits
  tail[56] = bits & 0xff
  tail[57] = (bits >>> 8) & 0xff
  tail[58] = (bits >>> 16) & 0xff
  tail[59] = (bits >>> 24) & 0xff
  md5cycle(state, md5blk(tail))
  return state
}
function md5blk(b) {
  const out = new Array(16)
  for (let i = 0; i < 16; i++) {
    out[i] = b[i * 4] | (b[i * 4 + 1] << 8) | (b[i * 4 + 2] << 16) | (b[i * 4 + 3] << 24)
  }
  return out
}
function rhex(n) {
  let s = '', j
  for (j = 0; j < 4; j++) s += ((n >> (j * 8 + 4)) & 0x0F).toString(16) + ((n >> (j * 8)) & 0x0F).toString(16)
  return s
}
export function md5(text) {
  const state = md51(te.encode(String(text)))
  return rhex(state[0]) + rhex(state[1]) + rhex(state[2]) + rhex(state[3])
}

/* ============================================================
 * HMAC
 * ============================================================ */

const HASH_NAME = { 'SHA-1': 'SHA-1', 'SHA-256': 'SHA-256', 'SHA-384': 'SHA-384', 'SHA-512': 'SHA-512' }

export async function hmac(algo, key, msg) {
  const hash = HASH_NAME[algo]
  if (!hash) throw new Error('不支持的算法：' + algo)
  const k = await crypto.subtle.importKey(
    'raw', te.encode(key), { name: 'HMAC', hash }, false, ['sign']
  )
  const sig = await crypto.subtle.sign('HMAC', k, te.encode(msg))
  return { hex: bytesToHex(sig), base64: bytesToBase64(sig) }
}

/* ============================================================
 * AES (GCM / CBC) · 密钥派生：PBKDF2 from 密码
 * ============================================================ */

async function deriveAesKey(password, salt, length = 256, mode = 'AES-GCM') {
  const baseKey = await crypto.subtle.importKey(
    'raw', te.encode(password), 'PBKDF2', false, ['deriveKey']
  )
  return crypto.subtle.deriveKey(
    { name: 'PBKDF2', salt, iterations: 100000, hash: 'SHA-256' },
    baseKey,
    { name: mode, length },
    false,
    ['encrypt', 'decrypt']
  )
}

/**
 * encryptAes
 *  output 结构（base64）= [salt(16) | iv(12 or 16) | ciphertext]
 *  这样解密时只需密码即可，无需单独保存盐和 IV
 */
export async function encryptAes(plaintext, password, mode = 'AES-GCM', length = 256) {
  if (!password) throw new Error('密码不能为空')
  const salt = crypto.getRandomValues(new Uint8Array(16))
  const ivLen = mode === 'AES-GCM' ? 12 : 16
  const iv = crypto.getRandomValues(new Uint8Array(ivLen))
  const key = await deriveAesKey(password, salt, length, mode)
  const ct = await crypto.subtle.encrypt({ name: mode, iv }, key, te.encode(plaintext))
  const ctArr = new Uint8Array(ct)
  const out = new Uint8Array(salt.length + iv.length + ctArr.length)
  out.set(salt, 0)
  out.set(iv, salt.length)
  out.set(ctArr, salt.length + iv.length)
  return bytesToBase64(out)
}

export async function decryptAes(b64, password, mode = 'AES-GCM', length = 256) {
  if (!password) throw new Error('密码不能为空')
  const all = base64ToBytes(b64)
  const ivLen = mode === 'AES-GCM' ? 12 : 16
  if (all.length < 16 + ivLen + 1) throw new Error('密文长度异常')
  const salt = all.slice(0, 16)
  const iv = all.slice(16, 16 + ivLen)
  const ct = all.slice(16 + ivLen)
  const key = await deriveAesKey(password, salt, length, mode)
  const pt = await crypto.subtle.decrypt({ name: mode, iv }, key, ct)
  return td.decode(pt)
}

/* ============================================================
 * JWT 解码（仅解析展示，不验签）
 * ============================================================ */

export function decodeJwt(token) {
  if (!token || typeof token !== 'string') throw new Error('JWT 为空')
  const parts = token.trim().split('.')
  if (parts.length !== 3) throw new Error('JWT 必须由 3 段以 . 分隔')

  const decodeSeg = (seg) => {
    const json = base64Decode(seg)
    try { return JSON.parse(json) }
    catch (e) { throw new Error('Base64Url JSON 解码失败：' + e.message) }
  }

  const header = decodeSeg(parts[0])
  const payload = decodeSeg(parts[1])
  const signature = parts[2]

  const meta = {}
  const now = Math.floor(Date.now() / 1000)
  if (payload.iat) meta.iat = new Date(payload.iat * 1000).toLocaleString()
  if (payload.nbf) meta.nbf = new Date(payload.nbf * 1000).toLocaleString()
  if (payload.exp) {
    meta.exp = new Date(payload.exp * 1000).toLocaleString()
    meta.expired = payload.exp < now
    meta.expiresIn = payload.exp - now
  }
  return { header, payload, signature, meta }
}

/* ============================================================
 * RSA · 密钥生成 / 加密 / 解密
 * ============================================================ */

export async function generateRsaKeyPair(modulusLength = 2048) {
  const pair = await crypto.subtle.generateKey(
    {
      name: 'RSA-OAEP',
      modulusLength,
      publicExponent: new Uint8Array([1, 0, 1]),
      hash: 'SHA-256',
    },
    true,
    ['encrypt', 'decrypt']
  )
  const pub = await crypto.subtle.exportKey('spki', pair.publicKey)
  const pri = await crypto.subtle.exportKey('pkcs8', pair.privateKey)
  return {
    publicKey: toPem(pub, 'PUBLIC KEY'),
    privateKey: toPem(pri, 'PRIVATE KEY'),
  }
}

function toPem(buf, label) {
  const b64 = bytesToBase64(buf)
  const lines = b64.match(/.{1,64}/g) || ['']
  return `-----BEGIN ${label}-----\n${lines.join('\n')}\n-----END ${label}-----`
}

function fromPem(pem) {
  const m = pem.match(/-----BEGIN [^-]+-----([\s\S]+?)-----END [^-]+-----/)
  if (!m) throw new Error('PEM 格式无效')
  return base64ToBytes(m[1].replace(/\s+/g, ''))
}

export async function rsaEncrypt(plaintext, publicKeyPem) {
  const der = fromPem(publicKeyPem)
  const key = await crypto.subtle.importKey(
    'spki', der, { name: 'RSA-OAEP', hash: 'SHA-256' }, false, ['encrypt']
  )
  const ct = await crypto.subtle.encrypt({ name: 'RSA-OAEP' }, key, te.encode(plaintext))
  return bytesToBase64(ct)
}

export async function rsaDecrypt(ciphertextB64, privateKeyPem) {
  const der = fromPem(privateKeyPem)
  const key = await crypto.subtle.importKey(
    'pkcs8', der, { name: 'RSA-OAEP', hash: 'SHA-256' }, false, ['decrypt']
  )
  const pt = await crypto.subtle.decrypt({ name: 'RSA-OAEP' }, key, base64ToBytes(ciphertextB64))
  return td.decode(pt)
}

/* ============================================================
 * 通用 mixin
 * ============================================================ */

export const copyMixin = {
  methods: {
    async ctCopy(text) {
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

/* 密码强度评估（0-4） */
export function passwordStrength(pwd) {
  if (!pwd) return { score: 0, label: '空', color: '#ccc' }
  let score = 0
  if (pwd.length >= 8) score++
  if (pwd.length >= 12) score++
  if (/[a-z]/.test(pwd) && /[A-Z]/.test(pwd)) score++
  if (/[0-9]/.test(pwd)) score++
  if (/[^a-zA-Z0-9]/.test(pwd)) score++
  const labels = ['很弱', '弱', '一般', '强', '很强', '极强']
  const colors = ['#e74c3c', '#e67e22', '#f5a623', '#27ae60', '#16a085', '#7b5fd9']
  return { score, label: labels[score], color: colors[score] }
}
