<template>
  <client-only>
    <div class="dv-wrapper">
      <div class="dv-header">
        <h2 class="dv-title">⚙️ 开发实用工具箱</h2>
        <p class="dv-desc">UUID / 密码 / 进制 / 单位 / CSV 互转，全部本地运算，安全可靠</p>
      </div>

      <div class="dv-tabs">
        <button
          v-for="t in tabs"
          :key="t.id"
          class="dv-tab"
          :class="{ active: active === t.id }"
          @click="active = t.id"
        >{{ t.icon }} {{ t.label }}</button>
      </div>

      <keep-alive>
        <component :is="currentPanel" />
      </keep-alive>
    </div>
  </client-only>
</template>

<script>
import './dev/styles.css';
import UuidPanel from './dev/UuidPanel.vue';
import PasswordPanel from './dev/PasswordPanel.vue';
import BasePanel from './dev/BasePanel.vue';
import UnitPanel from './dev/UnitPanel.vue';
import CsvPanel from './dev/CsvPanel.vue';

export default {
  name: 'DevToolbox',
  components: { UuidPanel, PasswordPanel, BasePanel, UnitPanel, CsvPanel },
  data() {
    return {
      active: 'uuid',
      tabs: [
        { id: 'uuid',     label: 'UUID / 短 ID', icon: '🆔' },
        { id: 'password', label: '随机密码',     icon: '🔑' },
        { id: 'base',     label: '进制转换',     icon: '🔢' },
        { id: 'unit',     label: '单位换算',     icon: '📏' },
        { id: 'csv',      label: 'CSV ↔ JSON',  icon: '📊' }
      ]
    };
  },
  computed: {
    currentPanel() {
      return {
        uuid: 'UuidPanel',
        password: 'PasswordPanel',
        base: 'BasePanel',
        unit: 'UnitPanel',
        csv: 'CsvPanel'
      }[this.active];
    }
  }
};
</script>
