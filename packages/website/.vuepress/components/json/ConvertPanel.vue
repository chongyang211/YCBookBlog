<template>
  <div class="jt-panel">
    <div class="jt-toolbar">
      <div class="jt-field">
        <label>转换为</label>
        <select v-model="convTarget">
          <option value="yaml">YAML</option>
          <option value="xml">XML</option>
          <option value="csv">CSV（数组对象）</option>
          <option value="query">Query String</option>
          <option value="ts">TypeScript Interface</option>
          <option value="go">Go Struct</option>
          <option value="java">Java Bean</option>
        </select>
      </div>
      <div class="jt-actions">
        <button class="jt-btn jt-btn-primary" @click="doConvert">▷ 转换</button>
        <button class="jt-btn" @click="copy(convOut, 'cv')" v-if="convOut">{{ copyHint==='cv' ? '已复制' : '复制结果' }}</button>
      </div>
    </div>
    <div class="jt-grid-2">
      <div class="jt-io">
        <div class="jt-io-head">
          <span>JSON 源</span>
          <span class="jt-io-cnt">{{ convIn.length }} 字符</span>
        </div>
        <textarea v-model="convIn" placeholder="输入 JSON..." spellcheck="false"></textarea>
      </div>
      <div class="jt-io">
        <div class="jt-io-head">
          <span>{{ convLabel }}</span>
          <span class="jt-io-cnt">{{ convOut.length }} 字符</span>
        </div>
        <textarea v-model="convOut" :placeholder="`${convLabel} 结果...`" spellcheck="false"></textarea>
      </div>
    </div>
    <p v-if="convErr" class="jt-err">⚠ {{ convErr }}</p>
  </div>
</template>

<script>
import { toYAML, toXML, toCSV, toQuery, toTS, toGo, toJava, copyMixin } from './utils.js'

export default {
  name: 'ConvertPanel',
  mixins: [copyMixin],
  data() {
    return { convIn: '', convOut: '', convErr: '', convTarget: 'yaml' }
  },
  computed: {
    convLabel() {
      const map = {
        yaml: 'YAML', xml: 'XML', csv: 'CSV', query: 'Query String',
        ts: 'TypeScript', go: 'Go', java: 'Java',
      }
      return map[this.convTarget] || '结果'
    },
  },
  methods: {
    doConvert() {
      this.convErr = ''
      this.convOut = ''
      if (!this.convIn.trim()) return
      try {
        const obj = JSON.parse(this.convIn)
        switch (this.convTarget) {
          case 'yaml':  this.convOut = toYAML(obj); break
          case 'xml':   this.convOut = '<?xml version="1.0" encoding="UTF-8"?>\n' + toXML(obj, 'root'); break
          case 'csv':   this.convOut = toCSV(obj); break
          case 'query': this.convOut = toQuery(obj); break
          case 'ts':    this.convOut = toTS(obj, 'Root'); break
          case 'go':    this.convOut = toGo(obj, 'Root'); break
          case 'java':  this.convOut = toJava(obj, 'Root'); break
        }
      } catch (e) {
        this.convErr = '转换失败：' + e.message
      }
    },
  },
}
</script>
