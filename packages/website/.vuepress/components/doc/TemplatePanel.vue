<template>
  <div class="dt-panel">
    <div class="dt-tpl-layout">
      <!-- 左侧：模板选择 + 表单 -->
      <div class="dt-tpl-side">
        <div class="dt-tpl-tabs">
          <span
            v-for="t in templates"
            :key="t.key"
            class="dt-tpl-chip"
            :class="{ active: current.key === t.key }"
            @click="select(t)"
          >{{ t.name }}</span>
        </div>
        <div class="dt-tpl-form">
          <div v-for="f in current.fields" :key="f.key">
            <label>{{ f.label }}</label>
            <textarea
              v-if="f.type === 'textarea'"
              v-model="form[f.key]"
              :placeholder="f.placeholder || ''"
            ></textarea>
            <input
              v-else
              v-model="form[f.key]"
              :placeholder="f.placeholder || ''"
            />
          </div>
        </div>
      </div>

      <!-- 右侧：输出 -->
      <div class="dt-io">
        <div class="dt-io-head">
          <span>生成的 Markdown</span>
          <span class="dt-io-cnt">{{ output.length }} 字符</span>
          <button class="dt-mini" @click="copy(output, 'out')">{{ copyHint==='out' ? '已复制' : '复制' }}</button>
          <button class="dt-mini" @click="exportMd">下载 .md</button>
        </div>
        <textarea :value="output" readonly spellcheck="false" style="min-height:480px;"></textarea>
      </div>
    </div>
  </div>
</template>

<script>
import { TEMPLATES, copyMixin } from './utils.js'

export default {
  name: 'TemplatePanel',
  mixins: [copyMixin],
  data() {
    return {
      templates: TEMPLATES,
      current: TEMPLATES[0],
      form: {},
    }
  },
  computed: {
    output() {
      try { return this.current.build(this.form || {}) } catch (e) { return '生成失败：' + e.message }
    },
  },
  created() { this.select(this.templates[0]) },
  methods: {
    select(t) {
      this.current = t
      const next = {}
      t.fields.forEach(f => { next[f.key] = '' })
      this.form = next
    },
    exportMd() {
      this.download(this.output, `${this.current.key}.md`, 'text/markdown')
    },
  },
}
</script>
