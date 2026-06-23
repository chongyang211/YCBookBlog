<template>
  <div class="dv-panel">
    <div class="dv-toolbar">
      <span class="dv-toolbar-label">长度</span>
      <input v-model.number="length" type="range" min="4" max="64" class="dv-input" style="flex:1;max-width:220px;" />
      <span style="font-family:monospace;color:#6c5ce7;font-weight:600;width:36px;">{{ length }}</span>

      <span class="dv-toolbar-label">数量</span>
      <input v-model.number="count" type="number" min="1" max="50" class="dv-input" style="width:80px;" />

      <button class="dv-btn dv-btn-primary" @click="generate">生成</button>
      <button class="dv-btn" @click="copyAll" :disabled="!list.length">复制全部</button>
    </div>

    <div class="dv-checkbox-group">
      <label><input type="checkbox" v-model="lower" /> 小写字母 a-z</label>
      <label><input type="checkbox" v-model="upper" /> 大写字母 A-Z</label>
      <label><input type="checkbox" v-model="number" /> 数字 0-9</label>
      <label><input type="checkbox" v-model="symbol" /> 符号 !@#$…</label>
      <label><input type="checkbox" v-model="excludeAmbiguous" /> 排除易混字符 (O0Il1|)</label>
    </div>

    <div v-if="msg" class="dv-msg" :class="msgCls">{{ msg }}</div>

    <div class="dv-result-list" v-if="list.length">
      <div v-for="(item, i) in list" :key="i">
        <div class="dv-result-item">
          <code>{{ item }}</code>
          <button class="dv-mini-btn" @click="copyOne(item)">复制</button>
        </div>
        <div class="dv-strength" :class="strengthOf(item).cls">
          <span style="width:42px;color:#888;">强度</span>
          <div class="dv-strength-bar"><div :style="{width: strengthOf(item).pct + '%'}"></div></div>
          <span style="width:40px;text-align:right;">{{ strengthOf(item).label }}</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { genPassword, passwordStrength, copyText } from './utils.js';

export default {
  name: 'PasswordPanel',
  data() {
    return {
      length: 16, count: 5,
      lower: true, upper: true, number: true, symbol: true, excludeAmbiguous: false,
      list: [], msg: '', msgCls: 'dv-msg-info'
    };
  },
  mounted() { this.generate(); },
  methods: {
    generate() {
      try {
        const n = Math.min(50, Math.max(1, this.count | 0 || 1));
        const out = [];
        for (let i = 0; i < n; i++) {
          out.push(genPassword({
            length: this.length, lower: this.lower, upper: this.upper,
            number: this.number, symbol: this.symbol, excludeAmbiguous: this.excludeAmbiguous
          }));
        }
        this.list = out;
        this.show(`已生成 ${n} 个`, 'ok');
      } catch (e) { this.show(e.message, 'err'); }
    },
    strengthOf(pw) { return passwordStrength(pw); },
    async copyOne(v) {
      const ok = await copyText(v);
      this.show(ok ? '已复制' : '复制失败', ok ? 'ok' : 'err');
    },
    async copyAll() {
      const ok = await copyText(this.list.join('\n'));
      this.show(ok ? '已复制全部' : '复制失败', ok ? 'ok' : 'err');
    },
    show(t, lv) { this.msg = t; this.msgCls = 'dv-msg-' + lv; }
  }
};
</script>
