<template>
  <div class="dv-panel">
    <div class="dv-toolbar">
      <span class="dv-toolbar-label">类别</span>
      <select v-model="group" class="dv-select" @change="onGroupChange">
        <option v-for="(g, key) in groups" :key="key" :value="key">{{ g.name }}</option>
      </select>

      <span class="dv-toolbar-label">基准</span>
      <select v-model="baseUnit" class="dv-select">
        <option v-for="u in groups[group].units" :key="u.id" :value="u.id">{{ u.label }}</option>
      </select>

      <input v-model="baseValue" class="dv-input" style="width:160px;font-family:monospace;text-align:right;" />

      <button class="dv-btn dv-btn-primary" @click="compute">换算</button>
    </div>

    <div v-if="msg" class="dv-msg" :class="msgCls">{{ msg }}</div>

    <div>
      <div v-for="u in groups[group].units" :key="u.id" class="dv-unit-row">
        <label>{{ u.label }}</label>
        <input
          class="dv-input"
          :value="results[u.id]"
          @input="onUnitInput(u.id, $event.target.value)"
        />
        <span class="dv-tag">{{ u.id }}</span>
      </div>
    </div>
  </div>
</template>

<script>
import { UNIT_GROUPS, convertUnit } from './utils.js';

export default {
  name: 'UnitPanel',
  data() {
    return {
      groups: UNIT_GROUPS,
      group: 'length',
      baseUnit: 'm',
      baseValue: '1',
      results: {},
      msg: '', msgCls: 'dv-msg-info'
    };
  },
  watch: {
    group() { this.compute(); },
    baseUnit() { this.compute(); },
    baseValue() { this.compute(); }
  },
  mounted() { this.compute(); },
  methods: {
    onGroupChange() {
      this.baseUnit = this.groups[this.group].units[0].id;
      this.baseValue = '1';
    },
    compute() {
      try {
        const out = {};
        const v = Number(this.baseValue);
        if (!Number.isFinite(v)) { this.results = {}; return; }
        for (const u of this.groups[this.group].units) {
          const r = convertUnit(this.group, this.baseUnit, u.id, v);
          out[u.id] = this.format(r);
        }
        this.results = out;
        this.msg = '';
      } catch (e) { this.show(e.message, 'err'); }
    },
    onUnitInput(id, value) {
      this.baseUnit = id;
      this.baseValue = value;
    },
    format(n) {
      if (!Number.isFinite(n)) return '';
      if (n === 0) return '0';
      const abs = Math.abs(n);
      if (abs >= 1e15 || abs < 1e-6) return n.toExponential(6);
      // 自动 6 位有效数字
      return parseFloat(n.toPrecision(10)).toString();
    },
    show(t, lv) { this.msg = t; this.msgCls = 'dv-msg-' + lv; }
  }
};
</script>
