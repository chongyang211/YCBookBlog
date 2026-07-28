<template>
  <div class="dv-panel">
    <div class="dv-toolbar">
      <span class="dv-toolbar-label">类型</span>
      <select v-model="type" class="dv-select">
        <option value="uuid">UUID v4</option>
        <option value="nano">NanoID (21 位)</option>
        <option value="short">短 ID</option>
      </select>

      <span class="dv-toolbar-label" v-if="type === 'short'">长度</span>
      <input v-if="type === 'short'" v-model.number="shortLen" type="number" min="4" max="32" class="dv-input" style="width:80px;" />

      <span class="dv-toolbar-label">数量</span>
      <input v-model.number="count" type="number" min="1" max="500" class="dv-input" style="width:90px;" />

      <label style="font-size:13px;color:#555;display:inline-flex;align-items:center;gap:4px;" v-if="type === 'uuid'">
        <input type="checkbox" v-model="upper" /> 大写
      </label>
      <label style="font-size:13px;color:#555;display:inline-flex;align-items:center;gap:4px;" v-if="type === 'uuid'">
        <input type="checkbox" v-model="noDash" /> 去掉连字符
      </label>

      <button class="dv-btn dv-btn-primary" @click="generate">生成</button>
      <button class="dv-btn" @click="copyAll" :disabled="!list.length">复制全部</button>
      <button class="dv-btn" @click="download" :disabled="!list.length">下载 TXT</button>
      <button class="dv-btn" @click="list = []" :disabled="!list.length">清空</button>
    </div>

    <div v-if="msg" class="dv-msg" :class="msgCls">{{ msg }}</div>

    <div class="dv-result-list" v-if="list.length">
      <div v-for="(item, i) in list" :key="i" class="dv-result-item">
        <span style="color:#aaa;font-size:11px;width:28px;">{{ i + 1 }}</span>
        <code>{{ item }}</code>
        <button class="dv-mini-btn" @click="copyOne(item)">复制</button>
      </div>
    </div>
  </div>
</template>

<script>
import { uuidV4, nanoId, shortId, copyText, downloadText } from './utils.js';

export default {
  name: 'UuidPanel',
  data() {
    return {
      type: 'uuid',
      count: 5,
      shortLen: 8,
      upper: false,
      noDash: false,
      list: [],
      msg: '',
      msgCls: 'dv-msg-info'
    };
  },
  mounted() { this.generate(); },
  methods: {
    generate() {
      const n = Math.min(500, Math.max(1, this.count | 0 || 1));
      const out = [];
      for (let i = 0; i < n; i++) {
        let s;
        if (this.type === 'uuid') {
          s = uuidV4();
          if (this.noDash) s = s.replace(/-/g, '');
          if (this.upper) s = s.toUpperCase();
        } else if (this.type === 'nano') {
          s = nanoId(21);
        } else {
          s = shortId(Math.min(32, Math.max(4, this.shortLen | 0)));
        }
        out.push(s);
      }
      this.list = out;
      this.show(`已生成 ${n} 个`, true);
    },
    async copyOne(v) {
      const ok = await copyText(v);
      this.show(ok ? '已复制' : '复制失败', ok);
    },
    async copyAll() {
      const ok = await copyText(this.list.join('\n'));
      this.show(ok ? '已复制全部' : '复制失败', ok);
    },
    download() { downloadText(this.list.join('\n'), `${this.type}.txt`); },
    show(t, ok) { this.msg = t; this.msgCls = ok ? 'dv-msg-ok' : 'dv-msg-err'; }
  }
};
</script>
