/* ============================================================
 * TimeToolbox · 工具函数
 *  - 时间戳 / 时区 / Cron / 日期计算
 *  - 零依赖，全部浏览器端
 * ============================================================ */

export const copyMixin = {
  methods: {
    async tmCopy(text) {
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

/* ---------- 时间戳格式化 ---------- */

const pad = (n, w = 2) => String(n).padStart(w, '0')

export function formatDate(d, tz) {
  if (!(d instanceof Date) || isNaN(d.getTime())) return ''
  if (tz) {
    try {
      const opts = {
        timeZone: tz,
        year: 'numeric', month: '2-digit', day: '2-digit',
        hour: '2-digit', minute: '2-digit', second: '2-digit',
        hour12: false,
      }
      const parts = new Intl.DateTimeFormat('en-CA', opts).formatToParts(d)
      const get = (t) => parts.find(p => p.type === t).value
      return `${get('year')}-${get('month')}-${get('day')} ${get('hour')}:${get('minute')}:${get('second')}`
    } catch (e) {
      // fall through
    }
  }
  return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`
}

export function formatIso(d) {
  return d instanceof Date && !isNaN(d) ? d.toISOString() : ''
}

export function parseTimestamp(input) {
  if (input == null) return null
  const s = String(input).trim()
  if (!s) return null
  if (!/^-?\d+(\.\d+)?$/.test(s)) return null
  const n = Number(s)
  // 自动识别秒 / 毫秒：长度 ≤ 10 当秒
  const ms = s.replace('-', '').split('.')[0].length <= 10 ? n * 1000 : n
  const d = new Date(ms)
  return isNaN(d.getTime()) ? null : d
}

export function parseDateInput(s) {
  if (!s) return null
  // 支持 "2024-01-01", "2024-01-01 12:00:00", ISO 字符串
  let str = s.trim()
  if (/^\d{4}-\d{2}-\d{2}$/.test(str)) str += 'T00:00:00'
  if (/^\d{4}-\d{2}-\d{2} \d{2}:\d{2}(:\d{2})?$/.test(str)) {
    str = str.replace(' ', 'T')
  }
  const d = new Date(str)
  return isNaN(d.getTime()) ? null : d
}

export const COMMON_TIMEZONES = [
  { city: '北京 / 上海', tz: 'Asia/Shanghai' },
  { city: '东京', tz: 'Asia/Tokyo' },
  { city: '首尔', tz: 'Asia/Seoul' },
  { city: '新加坡', tz: 'Asia/Singapore' },
  { city: '迪拜', tz: 'Asia/Dubai' },
  { city: '莫斯科', tz: 'Europe/Moscow' },
  { city: '伦敦', tz: 'Europe/London' },
  { city: '巴黎', tz: 'Europe/Paris' },
  { city: '柏林', tz: 'Europe/Berlin' },
  { city: '纽约', tz: 'America/New_York' },
  { city: '洛杉矶', tz: 'America/Los_Angeles' },
  { city: '芝加哥', tz: 'America/Chicago' },
  { city: '圣保罗', tz: 'America/Sao_Paulo' },
  { city: '悉尼', tz: 'Australia/Sydney' },
  { city: 'UTC', tz: 'UTC' },
]

/* 获取某时区当前 UTC 偏移（分钟） */
export function getTzOffsetMinutes(tz, date = new Date()) {
  try {
    const dtf = new Intl.DateTimeFormat('en-US', {
      timeZone: tz, hour12: false,
      year: 'numeric', month: '2-digit', day: '2-digit',
      hour: '2-digit', minute: '2-digit', second: '2-digit',
    })
    const parts = dtf.formatToParts(date)
    const get = (t) => parseInt(parts.find(p => p.type === t).value, 10)
    const y = get('year'), mo = get('month'), d = get('day')
    const h = get('hour'), mi = get('minute'), s = get('second')
    const asUTC = Date.UTC(y, mo - 1, d, h, mi, s)
    return Math.round((asUTC - date.getTime()) / 60000)
  } catch (e) { return 0 }
}

export function isNightInTz(tz, date = new Date()) {
  try {
    const h = parseInt(
      new Intl.DateTimeFormat('en-US', { timeZone: tz, hour12: false, hour: '2-digit' })
        .formatToParts(date).find(p => p.type === 'hour').value, 10)
    return h < 6 || h >= 19
  } catch (e) { return false }
}

/* ============================================================
 * Cron 解析器
 *  支持 5 段 (min hour dom mon dow)
 *  字段语法：* / number / a-b / a-b/n / *|n
 * ============================================================ */

const CRON_RANGES = [
  { name: '分', min: 0, max: 59 },
  { name: '时', min: 0, max: 23 },
  { name: '日', min: 1, max: 31 },
  { name: '月', min: 1, max: 12 },
  { name: '周', min: 0, max: 6 }, // 0=周日 ... 6=周六（兼容 7=周日）
]

function expandCronField(token, range) {
  const out = new Set()
  const parts = token.split(',')
  for (const part of parts) {
    let step = 1
    let body = part
    const slashIdx = part.indexOf('/')
    if (slashIdx >= 0) {
      step = parseInt(part.slice(slashIdx + 1), 10)
      body = part.slice(0, slashIdx)
      if (!Number.isFinite(step) || step <= 0) throw new Error('步长非法: ' + part)
    }
    let lo, hi
    if (body === '*') { lo = range.min; hi = range.max }
    else if (body.includes('-')) {
      const [a, b] = body.split('-').map(x => parseInt(x, 10))
      if (!Number.isFinite(a) || !Number.isFinite(b)) throw new Error('范围非法: ' + body)
      lo = a; hi = b
    } else {
      const n = parseInt(body, 10)
      if (!Number.isFinite(n)) throw new Error('数值非法: ' + body)
      lo = n; hi = n
    }
    if (range === CRON_RANGES[4] && lo === 7) lo = 0
    if (range === CRON_RANGES[4] && hi === 7) hi = 0
    if (lo > hi) throw new Error('范围越界: ' + body)
    if (lo < range.min || hi > range.max) throw new Error(`${range.name}超出范围 ${range.min}-${range.max}: ${body}`)
    for (let v = lo; v <= hi; v += step) out.add(v)
  }
  return Array.from(out).sort((a, b) => a - b)
}

export function parseCron(expr) {
  if (!expr) throw new Error('表达式为空')
  const tokens = expr.trim().split(/\s+/)
  if (tokens.length !== 5) throw new Error('需要 5 段，当前 ' + tokens.length + ' 段')
  const fields = tokens.map((t, i) => expandCronField(t, CRON_RANGES[i]))
  return {
    minute: fields[0], hour: fields[1], dom: fields[2], month: fields[3], dow: fields[4],
    raw: tokens,
  }
}

export function describeCron(expr) {
  try {
    const tokens = expr.trim().split(/\s+/)
    if (tokens.length !== 5) return '需 5 段表达式'
    const [m, h, dom, mon, dow] = tokens
    const part = (t, name) => t === '*' ? `每${name}` : `${name}${t}`
    const desc = `${part(m, '分')} ${part(h, '时')} ${dom !== '*' ? '日' + dom : '每日'}${mon !== '*' ? ' 月' + mon : ''}${dow !== '*' ? ' 周' + dow : ''}`
    return desc.trim()
  } catch (e) { return '' }
}

/* 计算未来 n 次执行时间，从指定时间起。最多扫描 4 年防止死循环 */
export function nextCronTimes(expr, count = 5, from = new Date()) {
  const c = parseCron(expr)
  const out = []
  const cur = new Date(from)
  cur.setSeconds(0, 0)
  cur.setMinutes(cur.getMinutes() + 1)

  const limit = new Date(from)
  limit.setFullYear(limit.getFullYear() + 4)

  while (out.length < count && cur < limit) {
    if (!c.month.includes(cur.getMonth() + 1)) {
      cur.setDate(1)
      cur.setHours(0, 0, 0, 0)
      cur.setMonth(cur.getMonth() + 1)
      continue
    }
    const domMatch = c.dom.includes(cur.getDate())
    const dowMatch = c.dow.includes(cur.getDay())
    // 标准 cron：dom 与 dow 中若任一字段非 *（即包含限定），两者用 OR；否则两者都是 * → AND（每天）
    const domWild = c.raw[2] === '*'
    const dowWild = c.raw[4] === '*'
    const dayMatch = (domWild && dowWild)
      ? true
      : (domWild ? dowMatch : (dowWild ? domMatch : (domMatch || dowMatch)))
    if (!dayMatch) {
      cur.setHours(0, 0, 0, 0)
      cur.setDate(cur.getDate() + 1)
      continue
    }
    if (!c.hour.includes(cur.getHours())) {
      cur.setMinutes(0)
      cur.setHours(cur.getHours() + 1)
      continue
    }
    if (!c.minute.includes(cur.getMinutes())) {
      cur.setMinutes(cur.getMinutes() + 1)
      continue
    }
    out.push(new Date(cur))
    cur.setMinutes(cur.getMinutes() + 1)
  }
  return out
}

/* ============================================================
 * 日期计算
 * ============================================================ */

export function diffDates(a, b) {
  const ms = b.getTime() - a.getTime()
  const abs = Math.abs(ms)
  const days = Math.floor(abs / 86400000)
  const hours = Math.floor(abs / 3600000) % 24
  const minutes = Math.floor(abs / 60000) % 60
  const seconds = Math.floor(abs / 1000) % 60
  // 工作日（周一~五）
  let workDays = 0
  const sign = ms < 0 ? -1 : 1
  const start = new Date(Math.min(a, b))
  const end = new Date(Math.max(a, b))
  const cur = new Date(start)
  cur.setHours(0, 0, 0, 0)
  const endN = new Date(end)
  endN.setHours(0, 0, 0, 0)
  while (cur < endN) {
    const dow = cur.getDay()
    if (dow !== 0 && dow !== 6) workDays++
    cur.setDate(cur.getDate() + 1)
  }
  return { ms, days, hours, minutes, seconds, workDays, sign }
}

export function addDate(base, { years = 0, months = 0, days = 0, hours = 0, minutes = 0, seconds = 0 }) {
  const d = new Date(base)
  d.setFullYear(d.getFullYear() + (+years || 0))
  d.setMonth(d.getMonth() + (+months || 0))
  d.setDate(d.getDate() + (+days || 0))
  d.setHours(d.getHours() + (+hours || 0))
  d.setMinutes(d.getMinutes() + (+minutes || 0))
  d.setSeconds(d.getSeconds() + (+seconds || 0))
  return d
}

/* ============================================================
 * 倒计时
 * ============================================================ */

export function countdownTo(target, now = new Date()) {
  const ms = target.getTime() - now.getTime()
  const abs = Math.max(ms, 0)
  return {
    finished: ms <= 0,
    totalMs: ms,
    days: Math.floor(abs / 86400000),
    hours: Math.floor(abs / 3600000) % 24,
    minutes: Math.floor(abs / 60000) % 60,
    seconds: Math.floor(abs / 1000) % 60,
  }
}
