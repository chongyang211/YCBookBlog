<template>
  <div class="cl-panel">
    <div class="cl-toolbar">
      <span class="cl-toolbar-label">类型</span>
      <select v-model="type" class="cl-input" style="width: 120px;">
        <option value="linear">线性</option>
        <option value="radial">径向</option>
        <option value="conic">圆锥</option>
      </select>
      <template v-if="type === 'linear'">
        <span class="cl-toolbar-label">角度</span>
        <input v-model.number="angle" type="range" min="0" max="360" class="cl-slider" style="width: 200px;" />
        <span class="cl-slider-val" style="width: 50px;">{{ angle }}°</span>
      </template>
      <button class="cl-btn cl-btn-primary" @click="addStop">+ 添加色标</button>
      <button class="cl-btn" @click="randomize">🎲 随机</button>
    </div>

    <div class="cl-gradient-preview" :style="{ background: gradientCss }"></div>

    <div class="cl-stops">
      <div v-for="(s, i) in stops" :key="i" class="cl-stop-row">
        <input type="color" class="cl-stop-color" v-model="s.color" />
        <input v-model.number="s.pos" type="range" min="0" max="100" class="cl-slider" />
        <span class="cl-slider-val">{{ s.pos }}%</span>
        <button class="cl-btn" :disabled="stops.length <= 2" @click="removeStop(i)">×</button>
      </div>
    </div>

    <div class="cl-picker-format">
      <div class="cl-picker-format-label">CSS</div>
      <div class="cl-picker-format-val">
        <span>background: {{ gradientCss }};</span>
        <button class="cl-picker-format-copy" @click="clCopy('background: ' + gradientCss + ';')">复制</button>
      </div>
    </div>
  </div>
</template>

<script>
import { copyMixin } from './utils'

export default {
  name: 'GradientPanel',
  mixins: [copyMixin],
  data() {
    return {
      type: 'linear', angle: 135,
      stops: [
        { color: '#e91e63', pos: 0 },
        { color: '#f5a623', pos: 100 },
      ],
    }
  },
  computed: {
    stopsStr() {
      return this.stops
        .slice().sort((a, b) => a.pos - b.pos)
        .map(s => `${s.color} ${s.pos}%`).join(', ')
    },
    gradientCss() {
      if (this.type === 'linear') return `linear-gradient(${this.angle}deg, ${this.stopsStr})`
      if (this.type === 'radial') return `radial-gradient(circle, ${this.stopsStr})`
      return `conic-gradient(from ${this.angle}deg, ${this.stopsStr})`
    },
  },
  methods: {
    addStop() {
      const mid = Math.round((this.stops[this.stops.length - 1].pos + this.stops[0].pos) / 2)
      this.stops.push({ color: '#7b5fd9', pos: mid })
    },
    removeStop(i) { if (this.stops.length > 2) this.stops.splice(i, 1) },
    randomize() {
      const rand = () => '#' + Array.from({ length: 6 }, () => '0123456789abcdef'[Math.floor(Math.random() * 16)]).join('')
      this.stops = [
        { color: rand(), pos: 0 },
        { color: rand(), pos: 50 },
        { color: rand(), pos: 100 },
      ]
      this.angle = Math.floor(Math.random() * 360)
    },
  },
}
</script>
