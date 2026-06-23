<template>
  <div class="jt-panel">
    <div class="jt-io jt-io-full">
      <div class="jt-io-head">
        <span>JSON 源</span>
        <span class="jt-io-cnt">{{ statsIn.length }} 字符</span>
        <button class="jt-mini jt-btn-primary" @click="doStats">立即统计</button>
      </div>
      <textarea v-model="statsIn" placeholder="输入 JSON 后点击「立即统计」..." spellcheck="false"></textarea>
    </div>
    <div v-if="statsResult && !statsErr" class="jt-stats-grid">
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.size }}</div>
        <div class="jt-stat-lbl">总大小</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.lines }}</div>
        <div class="jt-stat-lbl">总行数</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.topType }}</div>
        <div class="jt-stat-lbl">顶级类型</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.totalKeys }}</div>
        <div class="jt-stat-lbl">键总数</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.maxDepth }}</div>
        <div class="jt-stat-lbl">最大嵌套</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.arrayCount }}</div>
        <div class="jt-stat-lbl">数组个数</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.objectCount }}</div>
        <div class="jt-stat-lbl">对象个数</div>
      </div>
      <div class="jt-stat-card">
        <div class="jt-stat-val">{{ statsResult.leafCount }}</div>
        <div class="jt-stat-lbl">叶子节点</div>
      </div>
    </div>
    <div v-if="statsResult && !statsErr" class="jt-stats-detail">
      <div class="jt-detail-block">
        <div class="jt-detail-head">类型分布</div>
        <div class="jt-type-bars">
          <div v-for="(v, k) in statsResult.types" :key="k" class="jt-type-bar">
            <span class="jt-type-name">{{ k }}</span>
            <div class="jt-type-track">
              <div class="jt-type-fill" :style="{ width: (v / statsResult.leafCount * 100) + '%' }"></div>
            </div>
            <span class="jt-type-count">{{ v }}</span>
          </div>
        </div>
      </div>
      <div class="jt-detail-block" v-if="statsResult.longestPath">
        <div class="jt-detail-head">最长路径</div>
        <code class="jt-longest-path">{{ statsResult.longestPath }}</code>
      </div>
    </div>
    <p v-if="statsErr" class="jt-err">⚠ {{ statsErr }}</p>
  </div>
</template>

<script>
export default {
  name: 'StatsPanel',
  data() {
    return { statsIn: '', statsResult: null, statsErr: '' }
  },
  methods: {
    doStats() {
      this.statsErr = ''
      if (!this.statsIn.trim()) { this.statsResult = null; return }
      try {
        const obj = JSON.parse(this.statsIn)
        const bytes = new TextEncoder().encode(this.statsIn).length
        const sz = bytes < 1024
          ? bytes + ' B'
          : bytes < 1048576
            ? (bytes / 1024).toFixed(1) + ' KB'
            : (bytes / 1048576).toFixed(1) + ' MB'
        const lines = this.statsIn.split('\n').length
        let totalKeys = 0, arrayCount = 0, objectCount = 0, leafCount = 0
        let maxDepth = 0, longestPath = ''
        const types = { string: 0, number: 0, boolean: 0, null: 0 }
        const walk = (v, depth, path) => {
          if (depth > maxDepth) { maxDepth = depth; longestPath = path }
          if (Array.isArray(v)) {
            arrayCount++
            v.forEach((it, i) => walk(it, depth + 1, `${path}[${i}]`))
          } else if (v !== null && typeof v === 'object') {
            objectCount++
            const ks = Object.keys(v)
            totalKeys += ks.length
            ks.forEach(k => walk(v[k], depth + 1, path ? `${path}.${k}` : k))
          } else {
            leafCount++
            const t = v === null ? 'null' : typeof v
            types[t] = (types[t] || 0) + 1
          }
        }
        walk(obj, 1, '$')
        this.statsResult = {
          size: sz,
          lines,
          topType: Array.isArray(obj) ? 'Array' : obj === null ? 'null' : typeof obj,
          totalKeys, maxDepth, arrayCount, objectCount, leafCount,
          types, longestPath,
        }
      } catch (e) {
        this.statsErr = '解析失败：' + e.message
        this.statsResult = null
      }
    },
  },
}
</script>
