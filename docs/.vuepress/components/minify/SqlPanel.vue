<template>
  <div class="mt-panel">
    <div class="mt-toolbar">
      <span class="mt-toolbar-label">操作</span>
      <button class="mt-btn mt-btn-primary" @click="doFormat">格式化</button>
      <button class="mt-btn" @click="loadSample">示例</button>
      <button class="mt-btn" @click="swap">↑↓ 交换</button>
      <button class="mt-btn" @click="clear">清空</button>
      <button class="mt-btn" @click="copyOut">复制结果</button>
      <button class="mt-btn" @click="download">下载 .sql</button>

      <label class="mt-form-label">
        <input v-model="upper" type="checkbox" class="mt-checkbox" />关键字大写
      </label>
      <span class="mt-toolbar-label">缩进</span>
      <input v-model.number="indent" type="number" min="0" max="8" class="mt-input" />
    </div>

    <div v-if="error" class="mt-msg mt-msg-err">{{ error }}</div>

    <div class="mt-cols">
      <div class="mt-col">
        <div class="mt-col-head">
          <span>输入 · SQL</span>
          <span class="mt-col-head-tag">{{ inputBytes }} B · {{ inputLines }} 行</span>
        </div>
        <textarea v-model="input" class="mt-textarea" placeholder="select * from users where ..."></textarea>
      </div>
      <div class="mt-col">
        <div class="mt-col-head">
          <span>输出</span>
          <span class="mt-col-head-tag">{{ outputBytes }} B · {{ outputLines }} 行</span>
        </div>
        <textarea v-model="output" class="mt-textarea" readonly></textarea>
      </div>
    </div>
  </div>
</template>

<script>
import { formatSql, byteLen, downloadText, copyMixin } from './utils'

const SAMPLE = `select u.id, u.name, u.email, o.total from users u inner join orders o on u.id = o.user_id where u.status = 1 and o.created_at > '2024-01-01' and o.total > 100 order by o.total desc limit 20`

export default {
  name: 'SqlPanel',
  mixins: [copyMixin],
  data() {
    return { input: '', output: '', error: '', upper: true, indent: 2 }
  },
  computed: {
    inputBytes() { return byteLen(this.input) },
    outputBytes() { return byteLen(this.output) },
    inputLines() { return this.input ? this.input.split('\n').length : 0 },
    outputLines() { return this.output ? this.output.split('\n').length : 0 },
  },
  methods: {
    doFormat() {
      this.error = ''
      try { this.output = formatSql(this.input, { upper: this.upper, indent: this.indent }) }
      catch (e) { this.error = '格式化失败：' + e.message }
    },
    loadSample() { this.input = SAMPLE; this.output = ''; this.error = '' },
    swap() { [this.input, this.output] = [this.output, this.input] },
    clear() { this.input = ''; this.output = ''; this.error = '' },
    async copyOut() { await this.mtCopy(this.output) },
    download() { if (this.output) downloadText('query.sql', this.output, 'text/plain') },
  },
}
</script>
