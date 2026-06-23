<template>
  <div class="dv-panel">
    <div class="dv-toolbar">
      <span class="dv-toolbar-label">方向</span>
      <select v-model="dir" class="dv-select">
        <option value="c2j">CSV → JSON</option>
        <option value="j2c">JSON → CSV</option>
      </select>

      <span class="dv-toolbar-label">分隔符</span>
      <select v-model="delimiter" class="dv-select">
        <option value=",">, 逗号</option>
        <option value=";">; 分号</option>
        <option value="\t">⇥ Tab</option>
        <option value="|">| 竖线</option>
      </select>

      <label style="font-size:13px;color:#555;display:inline-flex;align-items:center;gap:4px;" v-if="dir === 'c2j'">
        <input type="checkbox" v-model="headerFirstRow" /> 首行为表头
      </label>
      <label style="font-size:13px;color:#555;display:inline-flex;align-items:center;gap:4px;" v-if="dir === 'c2j'">
        <input type="checkbox" v-model="pretty" /> 美化输出
      </label>

      <button class="dv-btn dv-btn-primary" @click="convert">转换</button>
      <button class="dv-btn" @click="swap" :disabled="!output">⇅ 互换</button>
      <button class="dv-btn" @click="copy" :disabled="!output">复制结果</button>
      <button class="dv-btn" @click="download" :disabled="!output">下载</button>
      <button class="dv-btn" @click="loadSample">示例</button>
    </div>

    <div v-if="msg" class="dv-msg" :class="msgCls">{{ msg }}</div>

    <div class="dv-grid-2">
      <div>
        <div class="dv-label">输入 ({{ dir === 'c2j' ? 'CSV' : 'JSON' }})</div>
        <textarea v-model="input" class="dv-textarea" rows="14" :placeholder="placeholderIn"></textarea>
      </div>
      <div>
        <div class="dv-label">输出 ({{ dir === 'c2j' ? 'JSON' : 'CSV' }})</div>
        <textarea :value="output" class="dv-textarea" rows="14" readonly></textarea>
      </div>
    </div>

    <div v-if="tableData.length" style="overflow-x:auto;">
      <div class="dv-label">表格预览（前 20 行）</div>
      <table class="dv-table">
        <thead><tr><th v-for="(h, i) in tableHeader" :key="i">{{ h }}</th></tr></thead>
        <tbody>
          <tr v-for="(row, i) in tableData.slice(0, 20)" :key="i">
            <td v-for="(h, j) in tableHeader" :key="j">{{ row[h] }}</td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script>
import { csvToJson, jsonToCsv, copyText, downloadText } from './utils.js';

export default {
  name: 'CsvPanel',
  data() {
    return {
      dir: 'c2j',
      delimiter: ',',
      headerFirstRow: true,
      pretty: true,
      input: '',
      output: '',
      msg: '', msgCls: 'dv-msg-info',
      tableData: [],
      tableHeader: []
    };
  },
  computed: {
    placeholderIn() {
      return this.dir === 'c2j'
        ? 'name,age,city\n张三,25,北京\n李四,30,上海'
        : '[\n  {"name":"张三","age":25,"city":"北京"},\n  {"name":"李四","age":30,"city":"上海"}\n]';
    },
    delim() { return this.delimiter === '\\t' ? '\t' : this.delimiter; }
  },
  methods: {
    convert() {
      try {
        if (this.dir === 'c2j') {
          const data = csvToJson(this.input, { delimiter: this.delim, headerFirstRow: this.headerFirstRow });
          this.output = this.pretty ? JSON.stringify(data, null, 2) : JSON.stringify(data);
          if (this.headerFirstRow && Array.isArray(data) && data.length && typeof data[0] === 'object' && !Array.isArray(data[0])) {
            this.tableHeader = Object.keys(data[0]);
            this.tableData = data;
          } else { this.tableHeader = []; this.tableData = []; }
        } else {
          const data = JSON.parse(this.input);
          this.output = jsonToCsv(data, { delimiter: this.delim });
          if (Array.isArray(data) && data.length && typeof data[0] === 'object' && !Array.isArray(data[0])) {
            this.tableHeader = Object.keys(data[0]);
            this.tableData = data;
          } else { this.tableHeader = []; this.tableData = []; }
        }
        this.show('转换成功', 'ok');
      } catch (e) { this.show(e.message, 'err'); this.output = ''; }
    },
    swap() {
      if (!this.output) return;
      this.input = this.output;
      this.dir = this.dir === 'c2j' ? 'j2c' : 'c2j';
      this.convert();
    },
    async copy() {
      const ok = await copyText(this.output);
      this.show(ok ? '已复制' : '复制失败', ok ? 'ok' : 'err');
    },
    download() {
      const name = this.dir === 'c2j' ? 'data.json' : 'data.csv';
      const mime = this.dir === 'c2j' ? 'application/json' : 'text/csv';
      downloadText(this.output, name, mime + ';charset=utf-8');
    },
    loadSample() {
      this.input = this.dir === 'c2j'
        ? 'name,age,city,job\n张三,25,北京,工程师\n李四,30,上海,设计师\n王五,28,深圳,产品经理\n"赵, 六",35,广州,"测试\n工程师"'
        : '[\n  {"name":"张三","age":25,"city":"北京"},\n  {"name":"李四","age":30,"city":"上海"},\n  {"name":"王五","age":28,"city":"深圳"}\n]';
      this.convert();
    },
    show(t, lv) { this.msg = t; this.msgCls = 'dv-msg-' + lv; }
  }
};
</script>
