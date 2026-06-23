<template>
  <div class="dv-panel">
    <div class="dv-toolbar">
      <span class="dv-toolbar-label">输入来源</span>
      <select v-model.number="srcBase" class="dv-select">
        <option v-for="b in commonBases" :key="b.v" :value="b.v">{{ b.label }}</option>
      </select>
      <input v-model="srcValue" class="dv-input" style="flex:1;min-width:160px;font-family:monospace;" :placeholder="srcBase === 16 ? 'ff' : '255'" />
      <button class="dv-btn dv-btn-primary" @click="convert">转换</button>
      <button class="dv-btn" @click="clear">清空</button>
    </div>

    <div v-if="msg" class="dv-msg" :class="msgCls">{{ msg }}</div>

    <div>
      <div v-for="b in commonBases" :key="b.v" class="dv-base-grid">
        <label>{{ b.label }}</label>
        <input
          class="dv-input"
          :value="outputs[b.v] || ''"
          @input="onInput(b.v, $event.target.value)"
        />
        <span class="dv-tag">base {{ b.v }}</span>
      </div>

      <div class="dv-base-grid">
        <label>自定义进制</label>
        <input class="dv-input" :value="outputs.custom || ''" @input="onCustom($event.target.value)" />
        <input v-model.number="customBase" type="number" min="2" max="36" class="dv-input" style="text-align:center;" />
      </div>
    </div>

    <div class="dv-toolbar" style="margin-top:8px;">
      <span class="dv-toolbar-label">ASCII / Unicode</span>
      <button class="dv-btn" @click="textToHex">文本 → 十六进制</button>
      <button class="dv-btn" @click="hexToText">十六进制 → 文本</button>
      <button class="dv-btn" @click="textToBin">文本 → 二进制</button>
    </div>
    <textarea v-model="textBuf" class="dv-textarea" rows="3" placeholder="文本或十六进制 (空格分隔)"></textarea>
  </div>
</template>

<script>
import { convertBase, copyText } from './utils.js';

export default {
  name: 'BasePanel',
  data() {
    return {
      srcBase: 10,
      srcValue: '255',
      customBase: 7,
      outputs: {},
      textBuf: '',
      msg: '', msgCls: 'dv-msg-info',
      commonBases: [
        { v: 2,  label: '二进制' },
        { v: 8,  label: '八进制' },
        { v: 10, label: '十进制' },
        { v: 16, label: '十六进制' }
      ]
    };
  },
  mounted() { this.convert(); },
  methods: {
    convert() {
      try {
        const targets = [2, 8, 10, 16];
        const next = {};
        for (const b of targets) next[b] = convertBase(this.srcValue, this.srcBase, b);
        next.custom = convertBase(this.srcValue, this.srcBase, this.customBase || 10);
        this.outputs = next;
        this.msg = '';
      } catch (e) { this.show(e.message, 'err'); }
    },
    onInput(base, value) {
      this.srcBase = base; this.srcValue = value; this.convert();
    },
    onCustom(value) {
      this.srcBase = this.customBase || 10;
      this.srcValue = value; this.convert();
    },
    clear() { this.srcValue = ''; this.outputs = {}; this.msg = ''; this.textBuf = ''; },
    textToHex() {
      const enc = new TextEncoder().encode(this.textBuf);
      this.textBuf = Array.from(enc, b => b.toString(16).padStart(2, '0')).join(' ');
      this.show('已转换为十六进制', 'ok');
    },
    hexToText() {
      try {
        const hex = this.textBuf.replace(/[^0-9a-fA-F]/g, '');
        if (hex.length % 2) throw new Error('十六进制位数必须为偶数');
        const bytes = new Uint8Array(hex.length / 2);
        for (let i = 0; i < bytes.length; i++) bytes[i] = parseInt(hex.substr(i * 2, 2), 16);
        this.textBuf = new TextDecoder().decode(bytes);
        this.show('已转换为文本', 'ok');
      } catch (e) { this.show(e.message, 'err'); }
    },
    textToBin() {
      const enc = new TextEncoder().encode(this.textBuf);
      this.textBuf = Array.from(enc, b => b.toString(2).padStart(8, '0')).join(' ');
      this.show('已转换为二进制', 'ok');
    },
    show(t, lv) { this.msg = t; this.msgCls = 'dv-msg-' + lv; }
  }
};
</script>
