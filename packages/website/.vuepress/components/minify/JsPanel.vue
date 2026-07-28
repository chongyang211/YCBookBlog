<template>
  <div class="mt-panel">
    <div class="mt-toolbar">
      <span class="mt-toolbar-label">操作</span>
      <button class="mt-btn mt-btn-primary" @click="doMinify">压缩</button>
      <button class="mt-btn" @click="doBeautify">美化</button>
      <button class="mt-btn" @click="loadSample">示例</button>
      <button class="mt-btn" @click="swap">↑↓ 交换</button>
      <button class="mt-btn" @click="clear">清空</button>
      <button class="mt-btn" @click="copyOut">复制结果</button>
      <button class="mt-btn" @click="download">下载 .js</button>

      <span class="mt-toolbar-label" style="margin-left: 8px;">缩进</span>
      <input v-model.number="indent" type="number" min="0" max="8" class="mt-input" />

      <label class="mt-form-label">
        <input v-model="keepNewlines" type="checkbox" class="mt-checkbox" />保留换行
      </label>
    </div>

    <div v-if="error" class="mt-msg mt-msg-err">{{ error }}</div>

    <div class="mt-cols">
      <div class="mt-col">
        <div class="mt-col-head">
          <span>输入 · JavaScript</span>
          <span class="mt-col-head-tag">{{ inputBytes }} B · {{ inputLines }} 行</span>
        </div>
        <textarea v-model="input" class="mt-textarea" placeholder="粘贴 JS 代码..."></textarea>
      </div>
      <div class="mt-col">
        <div class="mt-col-head">
          <span>输出</span>
          <span class="mt-col-head-tag">{{ outputBytes }} B · {{ outputLines }} 行</span>
        </div>
        <textarea v-model="output" class="mt-textarea" readonly></textarea>
      </div>
    </div>

    <div v-if="output" class="mt-stats">
      <span class="mt-stat"><strong>{{ inputBytes }}</strong>B 原始</span>
      <span class="mt-stat"><strong>{{ outputBytes }}</strong>B 处理后</span>
      <span class="mt-stat mt-stat-saved"><strong>{{ savedPct }}%</strong>体积变化</span>
    </div>
  </div>
</template>

<script>
import { minifyJs, beautifyJs, byteLen, downloadText, copyMixin } from './utils'

const SAMPLE = `// 示例代码：计算斐波那契
function fib(n) {
  /* 递归实现 */
  if (n < 2) return n;
  return fib(n - 1) + fib(n - 2);
}

const result = [];
for (let i = 0; i < 10; i++) {
  result.push(fib(i));
}
console.log("Fibonacci:", result.join(', '));
`

export default {
  name: 'JsPanel',
  mixins: [copyMixin],
  data() {
    return {
      input: '', output: '', error: '',
      indent: 2, keepNewlines: false,
    }
  },
  computed: {
    inputBytes() { return byteLen(this.input) },
    outputBytes() { return byteLen(this.output) },
    inputLines() { return this.input ? this.input.split('\n').length : 0 },
    outputLines() { return this.output ? this.output.split('\n').length : 0 },
    savedPct() {
      if (!this.inputBytes) return 0
      return ((this.outputBytes - this.inputBytes) / this.inputBytes * 100).toFixed(1)
    },
  },
  methods: {
    doMinify() {
      this.error = ''
      try {
        this.output = minifyJs(this.input, { aggressive: true, keepNewlines: this.keepNewlines })
      } catch (e) { this.error = '压缩失败：' + e.message }
    },
    doBeautify() {
      this.error = ''
      try {
        this.output = beautifyJs(this.input, this.indent)
      } catch (e) { this.error = '美化失败：' + e.message }
    },
    loadSample() { this.input = SAMPLE; this.output = ''; this.error = '' },
    swap() { [this.input, this.output] = [this.output, this.input] },
    clear() { this.input = ''; this.output = ''; this.error = '' },
    async copyOut() { if (await this.mtCopy(this.output)) this.error = '' },
    download() { if (this.output) downloadText('minified.js', this.output, 'application/javascript') },
  },
}
</script>
