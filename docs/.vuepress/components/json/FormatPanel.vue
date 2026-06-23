<template>
  <div class="jt-panel">
    <div class="jt-toolbar">
      <div class="jt-field">
        <label>缩进</label>
        <select v-model="fmtIndent">
          <option value="2">2 空格</option>
          <option value="4">4 空格</option>
          <option value="tab">Tab</option>
          <option value="0">压缩</option>
        </select>
      </div>
      <div class="jt-check-field">
        <label><input type="checkbox" v-model="fmtSortKeys" /> 按键排序</label>
        <label><input type="checkbox" v-model="fmtUnicode" /> Unicode 转义</label>
      </div>
      <div class="jt-actions">
        <button class="jt-btn jt-btn-primary" @click="doFormat">▷ 格式化</button>
        <button class="jt-btn" @click="doEscape">↗ 字符串转义</button>
        <button class="jt-btn" @click="doUnescape">↙ 反转义</button>
        <button class="jt-btn" @click="loadSample">示例</button>
        <button class="jt-btn" @click="clearAll">清空</button>
      </div>
    </div>
    <div class="jt-grid-2">
      <div class="jt-io">
        <div class="jt-io-head">
          <span>输入 JSON</span>
          <span class="jt-io-cnt">{{ fmtIn.length }} 字符</span>
          <button class="jt-mini" @click="copy(fmtIn, 'fin')">{{ copyHint==='fin' ? '已复制' : '复制' }}</button>
        </div>
        <textarea v-model="fmtIn" placeholder='粘贴 JSON 到这里，例如：&#10;{&#10;  "name": "编程进阶",&#10;  "stack": ["C/C++", "Go", "Web"]&#10;}' spellcheck="false"></textarea>
      </div>
      <div class="jt-io">
        <div class="jt-io-head">
          <span :style="{ color: fmtErr ? '#e74c3c' : '#27ae60' }">
            {{ fmtErr ? '❌ 错误' : '✅ 输出' }}
          </span>
          <span class="jt-io-cnt">{{ fmtOut.length }} 字符</span>
          <button class="jt-mini" @click="copy(fmtOut, 'fout')" v-if="fmtOut">{{ copyHint==='fout' ? '已复制' : '复制' }}</button>
          <button class="jt-mini" @click="download(fmtOut, 'output.json')" v-if="fmtOut">下载</button>
        </div>
        <textarea v-model="fmtOut" :class="{ 'jt-err-area': fmtErr }" placeholder="格式化后的 JSON..." spellcheck="false"></textarea>
      </div>
    </div>
    <p v-if="fmtErr" class="jt-err">⚠ {{ fmtErr }}</p>
  </div>
</template>

<script>
import { sortDeep, copyMixin } from './utils.js'

export default {
  name: 'FormatPanel',
  mixins: [copyMixin],
  data() {
    return {
      fmtIn: '', fmtOut: '', fmtErr: '',
      fmtIndent: '2', fmtSortKeys: false, fmtUnicode: false,
    }
  },
  methods: {
    doFormat() {
      this.fmtErr = ''
      if (!this.fmtIn.trim()) { this.fmtOut = ''; return }
      try {
        let obj = JSON.parse(this.fmtIn)
        if (this.fmtSortKeys) obj = sortDeep(obj)
        const indent = this.fmtIndent === 'tab' ? '\t' : this.fmtIndent === '0' ? 0 : parseInt(this.fmtIndent, 10)
        let out = JSON.stringify(obj, null, indent)
        if (this.fmtUnicode) out = out.replace(/[\u0080-\uffff]/g, c => '\\u' + c.charCodeAt(0).toString(16).padStart(4, '0'))
        this.fmtOut = out
      } catch (e) {
        this.fmtErr = 'JSON 解析失败：' + e.message
        this.fmtOut = ''
      }
    },
    doEscape() {
      this.fmtErr = ''
      this.fmtOut = JSON.stringify(this.fmtIn).slice(1, -1)
    },
    doUnescape() {
      this.fmtErr = ''
      try { this.fmtOut = JSON.parse('"' + this.fmtIn.replace(/"/g, '\\"') + '"') }
      catch (e) { this.fmtErr = '反转义失败：' + e.message; this.fmtOut = '' }
    },
    loadSample() {
      this.fmtIn = JSON.stringify({
        mission: '让复杂的知识可被检索',
        belief: ['原理', '工具', '调包'],
        stack: { backend: ['Go', 'Java', 'C/C++'], web: ['Vue', 'React'] },
        active: true, version: 1.2, repo: null,
      }, null, 2)
    },
    clearAll() { this.fmtIn = this.fmtOut = this.fmtErr = '' },
  },
}
</script>
