<template>
  <li class="jt-node" v-show="matched || (node.children && node.children.length)">
    <div class="jt-node-row" :class="{ 'jt-node-hl': search && matched && search.length > 0 }">
      <span class="jt-tri" v-if="node.children && node.children.length" @click="toggle">
        {{ open ? '▼' : '▶' }}
      </span>
      <span class="jt-tri jt-tri-leaf" v-else>•</span>
      <span class="jt-key" @click="handleSelect">{{ node.key }}</span>
      <span class="jt-colon" v-if="!node.children || !node.children.length">:</span>
      <span v-if="!node.children || !node.children.length" :class="['jt-val', typeBadge]">{{ node.preview }}</span>
      <span v-else class="jt-summary">{{ node.summary }}</span>
      <span class="jt-type">{{ node.type }}</span>
    </div>
    <ul v-if="node.children && node.children.length && open" class="jt-children">
      <JtNode
        v-for="(c, i) in node.children"
        :key="i"
        :node="c"
        :search="search"
        @select="onChildSelect"
      />
    </ul>
  </li>
</template>

<script>
export default {
  name: 'JtNode',
  props: {
    node: { type: Object, required: true },
    search: { type: String, default: '' },
  },
  data() {
    return { open: this.node.depth < 2 }
  },
  computed: {
    matched() {
      if (!this.search) return true
      return this.node.path.toLowerCase().includes(this.search.toLowerCase())
    },
    typeBadge() {
      const map = {
        object: 'jt-t-obj',
        array: 'jt-t-arr',
        string: 'jt-t-str',
        number: 'jt-t-num',
        boolean: 'jt-t-bool',
        null: 'jt-t-null',
      }
      return map[this.node.type] || 'jt-t-other'
    },
  },
  methods: {
    toggle() {
      if (this.node.children && this.node.children.length) this.open = !this.open
    },
    handleSelect() { this.$emit('select', this.node.path) },
    onChildSelect(p) { this.$emit('select', p) },
  },
}
</script>
