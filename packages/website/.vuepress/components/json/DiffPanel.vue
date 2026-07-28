<template>
  <div class="jt-panel">
    <div class="jt-toolbar">
      <div class="jt-check-field">
        <label><input type="checkbox" v-model="diffIgnoreOrder" /> 忽略数组顺序</label>
        <label><input type="checkbox" v-model="diffIgnoreCase" /> 忽略大小写（字符串）</label>
      </div>
      <div class="jt-actions">
        <button class="jt-btn jt-btn-primary" @click="doDiff">▷ 开始对比</button>
        <button class="jt-btn" @click="clearAll">清空</button>
      </div>
    </div>
    <div class="jt-grid-2">
      <div class="jt-io">
        <div class="jt-io-head">
          <span>JSON A（旧）</span>
          <span class="jt-io-cnt">{{ diffA.length }} 字符</span>
        </div>
        <textarea v-model="diffA" placeholder="左侧旧 JSON..." spellcheck="false"></textarea>
      </div>
      <div class="jt-io">
        <div class="jt-io-head">
          <span>JSON B（新）</span>
          <span class="jt-io-cnt">{{ diffB.length }} 字符</span>
        </div>
        <textarea v-model="diffB" placeholder="右侧新 JSON..." spellcheck="false"></textarea>
      </div>
    </div>
    <div v-if="diffResult" class="jt-diff-result">
      <div class="jt-diff-summary">
        <span class="jt-diff-pill jt-diff-add">+ 新增 {{ diffResult.added.length }}</span>
        <span class="jt-diff-pill jt-diff-del">− 删除 {{ diffResult.removed.length }}</span>
        <span class="jt-diff-pill jt-diff-chg">~ 修改 {{ diffResult.changed.length }}</span>
        <span class="jt-diff-pill jt-diff-same" v-if="diffResult.equal">✓ 完全相同</span>
      </div>
      <div class="jt-diff-list" v-if="!diffResult.equal">
        <div v-for="(c, i) in diffResult.changed" :key="'c'+i" class="jt-diff-item jt-diff-chg-row">
          <span class="jt-diff-tag">~</span>
          <code class="jt-diff-path">{{ c.path }}</code>
          <span class="jt-diff-arrow">
            <span class="jt-diff-old">{{ c.oldVal }}</span>
            <span class="jt-diff-sep">→</span>
            <span class="jt-diff-new">{{ c.newVal }}</span>
          </span>
        </div>
        <div v-for="(p, i) in diffResult.added" :key="'a'+i" class="jt-diff-item jt-diff-add-row">
          <span class="jt-diff-tag">+</span>
          <code class="jt-diff-path">{{ p.path }}</code>
          <span class="jt-diff-new">{{ p.val }}</span>
        </div>
        <div v-for="(p, i) in diffResult.removed" :key="'r'+i" class="jt-diff-item jt-diff-del-row">
          <span class="jt-diff-tag">−</span>
          <code class="jt-diff-path">{{ p.path }}</code>
          <span class="jt-diff-old">{{ p.val }}</span>
        </div>
      </div>
    </div>
    <p v-if="diffErr" class="jt-err">⚠ {{ diffErr }}</p>
  </div>
</template>

<script>
import { jsonDiff } from './utils.js'

export default {
  name: 'DiffPanel',
  data() {
    return {
      diffA: '', diffB: '',
      diffIgnoreOrder: false,
      diffIgnoreCase: false,
      diffResult: null,
      diffErr: '',
    }
  },
  methods: {
    doDiff() {
      this.diffErr = ''
      try {
        const a = JSON.parse(this.diffA)
        const b = JSON.parse(this.diffB)
        this.diffResult = jsonDiff(a, b, {
          ignoreOrder: this.diffIgnoreOrder,
          ignoreCase: this.diffIgnoreCase,
        })
      } catch (e) {
        this.diffErr = '解析失败：' + e.message
        this.diffResult = null
      }
    },
    clearAll() {
      this.diffA = ''
      this.diffB = ''
      this.diffResult = null
      this.diffErr = ''
    },
  },
}
</script>
