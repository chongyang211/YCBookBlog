<template>
  <div class="jt-panel">
    <div class="jt-toolbar">
      <div class="jt-field" style="flex: 1; max-width: 400px;">
        <label>路径搜索</label>
        <input class="jt-input" v-model="treeSearch" placeholder="例如：user.name 或 items[0]" style="flex: 1;" />
      </div>
      <div class="jt-actions">
        <button class="jt-btn" @click="buildTree">全部展开</button>
        <button class="jt-btn" @click="buildTree">全部折叠</button>
        <button class="jt-btn jt-btn-primary" @click="buildTree">渲染树</button>
      </div>
    </div>
    <div class="jt-grid-2">
      <div class="jt-io">
        <div class="jt-io-head">
          <span>JSON 源</span>
          <span class="jt-io-cnt">{{ treeIn.length }} 字符</span>
        </div>
        <textarea v-model="treeIn" placeholder="输入 JSON 后点击「渲染树」..." spellcheck="false"></textarea>
      </div>
      <div class="jt-io">
        <div class="jt-io-head">
          <span>树视图</span>
          <span class="jt-io-cnt" v-if="treeRoot">{{ treeStats }}</span>
          <button class="jt-mini" v-if="treeSelectedPath" @click="copy(treeSelectedPath, 'tp')">
            {{ copyHint==='tp' ? '已复制' : '复制路径' }}
          </button>
        </div>
        <div class="jt-tree-box">
          <p v-if="treeErr" class="jt-err">⚠ {{ treeErr }}</p>
          <div v-else-if="!treeRoot" class="jt-empty">点击「渲染树」查看结构</div>
          <ul v-else class="jt-tree">
            <JtNode :node="treeRoot" :search="treeSearch" @select="onTreeSelect" />
          </ul>
        </div>
      </div>
    </div>
    <p v-if="treeSelectedPath" class="jt-tree-path">📍 选中路径：<code>{{ treeSelectedPath }}</code></p>
  </div>
</template>

<script>
import { buildNode, copyMixin } from './utils.js'
import JtNode from './JtNode.vue'

export default {
  name: 'TreePanel',
  mixins: [copyMixin],
  components: { JtNode },
  data() {
    return {
      treeIn: '',
      treeErr: '',
      treeRoot: null,
      treeSearch: '',
      treeSelectedPath: '',
    }
  },
  computed: {
    treeStats() {
      if (!this.treeRoot) return ''
      const count = this.treeRoot.children ? this.treeRoot.children.length : 0
      return `${this.treeRoot.type} · 顶层 ${count} 项`
    },
  },
  methods: {
    buildTree() {
      this.treeErr = ''
      this.treeSelectedPath = ''
      if (!this.treeIn.trim()) { this.treeRoot = null; return }
      try {
        this.treeRoot = buildNode(JSON.parse(this.treeIn), '$', '$', 0)
      } catch (e) {
        this.treeErr = '解析失败：' + e.message
        this.treeRoot = null
      }
    },
    onTreeSelect(path) { this.treeSelectedPath = path },
  },
}
</script>
