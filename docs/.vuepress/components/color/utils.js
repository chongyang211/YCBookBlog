/* ============================================================
 * ColorToolbox · 颜色工具函数
 *  - 格式互转 / 调色板 / 对比度 / 图片取色（k-means 简化版）
 *  - 全部本地运算
 * ============================================================ */

export const copyMixin = {
  methods: {
    async clCopy(text) {
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

const clamp = (v, lo, hi) => Math.min(Math.max(v, lo), hi)

/* ---------- 格式转换 ---------- */

export function rgbToHex(r, g, b) {
  const h = (n) => clamp(Math.round(n), 0, 255).toString(16).padStart(2, '0')
  return '#' + h(r) + h(g) + h(b)
}

export function hexToRgb(hex) {
  if (!hex) return null
  let s = hex.trim().replace('#', '')
  if (s.length === 3) s = s.split('').map(c => c + c).join('')
  if (s.length === 4) s = s.split('').map(c => c + c).join('')
  if (!/^[0-9a-fA-F]{6,8}$/.test(s)) return null
  return {
    r: parseInt(s.slice(0, 2), 16),
    g: parseInt(s.slice(2, 4), 16),
    b: parseInt(s.slice(4, 6), 16),
    a: s.length === 8 ? parseInt(s.slice(6, 8), 16) / 255 : 1,
  }
}

export function rgbToHsl(r, g, b) {
  r /= 255; g /= 255; b /= 255
  const max = Math.max(r, g, b), min = Math.min(r, g, b)
  let h = 0, s = 0
  const l = (max + min) / 2
  if (max !== min) {
    const d = max - min
    s = l > 0.5 ? d / (2 - max - min) : d / (max + min)
    switch (max) {
      case r: h = (g - b) / d + (g < b ? 6 : 0); break
      case g: h = (b - r) / d + 2; break
      case b: h = (r - g) / d + 4; break
    }
    h /= 6
  }
  return { h: Math.round(h * 360), s: Math.round(s * 100), l: Math.round(l * 100) }
}

export function hslToRgb(h, s, l) {
  h = ((h % 360) + 360) % 360
  s = clamp(s, 0, 100) / 100
  l = clamp(l, 0, 100) / 100
  const c = (1 - Math.abs(2 * l - 1)) * s
  const x = c * (1 - Math.abs(((h / 60) % 2) - 1))
  const m = l - c / 2
  let r1, g1, b1
  if (h < 60)      { r1 = c; g1 = x; b1 = 0 }
  else if (h < 120) { r1 = x; g1 = c; b1 = 0 }
  else if (h < 180) { r1 = 0; g1 = c; b1 = x }
  else if (h < 240) { r1 = 0; g1 = x; b1 = c }
  else if (h < 300) { r1 = x; g1 = 0; b1 = c }
  else              { r1 = c; g1 = 0; b1 = x }
  return {
    r: Math.round((r1 + m) * 255),
    g: Math.round((g1 + m) * 255),
    b: Math.round((b1 + m) * 255),
  }
}

export function rgbToHsv(r, g, b) {
  r /= 255; g /= 255; b /= 255
  const max = Math.max(r, g, b), min = Math.min(r, g, b), d = max - min
  let h = 0
  const v = max
  const s = max === 0 ? 0 : d / max
  if (d !== 0) {
    switch (max) {
      case r: h = (g - b) / d + (g < b ? 6 : 0); break
      case g: h = (b - r) / d + 2; break
      case b: h = (r - g) / d + 4; break
    }
    h /= 6
  }
  return { h: Math.round(h * 360), s: Math.round(s * 100), v: Math.round(v * 100) }
}

/* 统一解析任意 CSS 颜色（hex / rgb() / hsl() / 颜色名）成 {r,g,b,a} */
export function parseColor(input) {
  if (!input) return null
  const s = input.trim()
  // hex
  const hex = hexToRgb(s)
  if (hex) return hex
  // rgb(a)?
  const rgb = s.match(/^rgba?\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*(?:,\s*([\d.]+))?\s*\)$/i)
  if (rgb) return { r: +rgb[1], g: +rgb[2], b: +rgb[3], a: rgb[4] != null ? +rgb[4] : 1 }
  // hsl(a)?
  const hsl = s.match(/^hsla?\s*\(\s*(\d+)\s*,\s*(\d+)%\s*,\s*(\d+)%\s*(?:,\s*([\d.]+))?\s*\)$/i)
  if (hsl) {
    const rgb2 = hslToRgb(+hsl[1], +hsl[2], +hsl[3])
    return { ...rgb2, a: hsl[4] != null ? +hsl[4] : 1 }
  }
  // 颜色名走 canvas
  try {
    const ctx = document.createElement('canvas').getContext('2d')
    ctx.fillStyle = '#000'
    ctx.fillStyle = s
    if (ctx.fillStyle === '#000' && !/^#?000+/i.test(s) && s.toLowerCase() !== 'black') return null
    const m = ctx.fillStyle.match(/#([0-9a-f]{6})/i)
    if (m) return hexToRgb(m[0])
  } catch (e) { /* noop */ }
  return null
}

export function formatColor(rgb, format) {
  if (!rgb) return ''
  const { r, g, b, a = 1 } = rgb
  if (format === 'hex') return rgbToHex(r, g, b)
  if (format === 'hexa') return rgbToHex(r, g, b) + Math.round(a * 255).toString(16).padStart(2, '0')
  if (format === 'rgb') return `rgb(${r}, ${g}, ${b})`
  if (format === 'rgba') return `rgba(${r}, ${g}, ${b}, ${a})`
  if (format === 'hsl') {
    const { h, s, l } = rgbToHsl(r, g, b)
    return `hsl(${h}, ${s}%, ${l}%)`
  }
  if (format === 'hsla') {
    const { h, s, l } = rgbToHsl(r, g, b)
    return `hsla(${h}, ${s}%, ${l}%, ${a})`
  }
  if (format === 'hsv') {
    const { h, s, v } = rgbToHsv(r, g, b)
    return `hsv(${h}, ${s}%, ${v}%)`
  }
  return ''
}

/* ---------- 调色板生成 ---------- */

function rotateHue(rgb, delta) {
  const { h, s, l } = rgbToHsl(rgb.r, rgb.g, rgb.b)
  return hslToRgb(h + delta, s, l)
}

export function paletteSchemes(rgb) {
  return {
    complement: [rgb, rotateHue(rgb, 180)],
    analogous: [rotateHue(rgb, -30), rgb, rotateHue(rgb, 30)],
    triad: [rgb, rotateHue(rgb, 120), rotateHue(rgb, 240)],
    tetrad: [rgb, rotateHue(rgb, 90), rotateHue(rgb, 180), rotateHue(rgb, 270)],
    splitComplement: [rgb, rotateHue(rgb, 150), rotateHue(rgb, 210)],
  }
}

export function shades(rgb, count = 9) {
  const out = []
  const { h, s } = rgbToHsl(rgb.r, rgb.g, rgb.b)
  for (let i = 0; i < count; i++) {
    const l = 95 - (90 / (count - 1)) * i
    out.push(hslToRgb(h, s, l))
  }
  return out
}

/* ---------- 对比度（WCAG 2.1） ---------- */

function lum(c) {
  const a = [c.r, c.g, c.b].map(v => {
    v /= 255
    return v <= 0.03928 ? v / 12.92 : Math.pow((v + 0.055) / 1.055, 2.4)
  })
  return a[0] * 0.2126 + a[1] * 0.7152 + a[2] * 0.0722
}

export function contrastRatio(fg, bg) {
  const l1 = lum(fg), l2 = lum(bg)
  const a = Math.max(l1, l2), b = Math.min(l1, l2)
  return (a + 0.05) / (b + 0.05)
}

export function evalContrast(ratio) {
  return {
    aaNormal: ratio >= 4.5,
    aaLarge: ratio >= 3,
    aaaNormal: ratio >= 7,
    aaaLarge: ratio >= 4.5,
  }
}

/* ---------- 图片取色：K-means 简化版（量化 + 桶排序） ---------- */

export async function extractColorsFromImage(file, k = 8) {
  return new Promise((resolve, reject) => {
    const reader = new FileReader()
    reader.onload = () => {
      const img = new Image()
      img.onload = () => {
        const canvas = document.createElement('canvas')
        const scale = Math.min(200 / img.width, 200 / img.height, 1)
        canvas.width = Math.max(1, Math.floor(img.width * scale))
        canvas.height = Math.max(1, Math.floor(img.height * scale))
        const ctx = canvas.getContext('2d')
        ctx.drawImage(img, 0, 0, canvas.width, canvas.height)
        const data = ctx.getImageData(0, 0, canvas.width, canvas.height).data
        const counts = new Map()
        // 量化到 0~31，每通道 5 位，共 32768 个桶
        for (let i = 0; i < data.length; i += 4) {
          const a = data[i + 3]
          if (a < 128) continue
          const r = data[i] >> 3
          const g = data[i + 1] >> 3
          const b = data[i + 2] >> 3
          const key = (r << 10) | (g << 5) | b
          counts.set(key, (counts.get(key) || 0) + 1)
        }
        const sorted = Array.from(counts.entries()).sort((a, b) => b[1] - a[1])
        const out = []
        for (const [key, count] of sorted) {
          const r = ((key >> 10) & 31) << 3
          const g = ((key >> 5) & 31) << 3
          const b = (key & 31) << 3
          // 跳过过于接近已有色（简单去重）
          if (out.some(c => Math.abs(c.r - r) < 24 && Math.abs(c.g - g) < 24 && Math.abs(c.b - b) < 24)) continue
          out.push({ r, g, b, count })
          if (out.length >= k) break
        }
        resolve(out)
      }
      img.onerror = () => reject(new Error('图片加载失败'))
      img.src = reader.result
    }
    reader.onerror = () => reject(new Error('文件读取失败'))
    reader.readAsDataURL(file)
  })
}
