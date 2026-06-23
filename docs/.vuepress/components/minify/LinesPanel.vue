<template>
  <div class="mt-panel">
    <div class="mt-toolbar">
      <button class="mt-btn mt-btn-primary" @click="run">执行</button>
      <button class="mt-btn" @click="loadSample">示例</button>
      <button class="mt-btn" @click="swap">↑↓ 交换</button>
      <button class="mt-btn" @click="clear">清空</button>
      <button class="mt-btn" @click="copyOut">复制结果</button>
      <button class="mt-btn" @click="download">下载 .txt</button>
    </div>

    <div class="mt-toolbar" style="flex-direction: column; align-items: flex-start;">
      <div class="mt-form-row">
        <label class="mt-form-label">
          <input v-model="opts.trim" type="checkbox" class="mt-checkbox" />去除每行首尾空白
        </label>
        <label class="mt-form-label">
          <input v-model="opts.removeEmpty" type="checkbox" class="mt-checkbox" />删除空行
        </label>
        <label class="mt-form-label">
          <input v-model="opts.dedup" type="checkbox" class="mt-checkbox" />去重
        </label>
        <label class="mt-form-label">
          <input v-model="opts.reverse" type="checkbox" class="mt-checkbox" />倒序
        </label>
      </div>
      <div class="mt-form-row">
        <label class="mt-form-label">
          排序
          <select v-model="opts.sort" class="mt-input" style="width: 100px;">
            <option value="none">不变</option>
            <option value="asc">升序</option>
            <option value="desc">降序</option>
            <option value="shuffle">随机</option>
          </select>
        </label>
        <label class="mt-form-label">
          前缀
          <input v-model="opts.prefix" type="text" class="mt-input" style="width: 120px;" placeholder="如 - " />
        </label>
        <label class="mt-form-label">
          后缀
          <input v-model="opts.suffix" type="text" class="mt-input" style="width: 120px;" placeholder="如 ;" />
        </label>
      </div>
      <div class="mt-form-row">
        <label class="mt-form-label">
          <input v-model="opts.numbering" type="checkbox" class="mt-checkbox" />行号
        </label>
        <label class="mt-form-label">
          起始
          <input v-model.number="opts.startNo" type="number" class="mt-input" />
        </label>
        <label class="mt-form-label">
          补零位数
          <input v-model.number="opts.numWidth" type="number" min="0" max="6" class="mt-input" />
        </label>
        <label class="mt-form-label">
          分隔
          <input v-model="opts.numSep" type="text" class="mt-input" style="width: 80px;" />
        </label>
      </div>
    </div>

    <div class="mt-cols">
      <div class="mt-col">
        <div class="mt-col-head">
          <span>输入</span>
          <span class="mt-col-head-tag">{{ inputLines }} 行</span>
        </div>
        <textarea v-model="input" class="mt-textarea" placeholder="每行一条..."></textarea>
      </div>
      <div class="mt-col">
        <div class="mt-col-head">
          <span>输出</span>
          <span class="mt-col-head-tag">{{ outputLines }} 行</span>
        </div>
        <textarea v-model="output" class="mt-textarea" readonly></textarea>
      </div>
    </div>
  </div>
</template>

<script>
import { processLines, downloadText, copyMixin } from './utils'

const SAMPLE = `apple
banana
cherry
apple
  banana

durian
elderberry
cherry`

export default {
  name: 'LinesPanel',
  mixins: [copyMixin],
  data() {
    return {
      input: '',
      output: '',
      opts: {
        trim: true, removeEmpty: true, dedup: true, reverse: false,
        sort: 'asc',
        prefix: '', suffix: '',
        numbering: false, startNo: 1, numWidth: 0, numSep: '. ',
      },
    }
  },
  computed: {
    inputLines() { return this.input ? this.input.split('\n').length : 0 },
    outputLines() { return this.output ? this.output.split('\n').length : 0 },
  },
  methods: {
    run() { this.output = processLines(this.input, this.opts) },
    loadSample() { this.input = SAMPLE; this.output = '' },
    swap() { [this.input, this.output] = [this.output, this.input] },
    clear() { this.input = ''; this.output = '' },
    async copyOut() { await this.mtCopy(this.output) },
    download() { if (this.output) downloadText('lines.txt', this.output) },
  },
}
</script>
