// ── jschart · 响应式数据看板（毕业设计）──────────────────
// 组装：Pipeline 三源合一 → effect 自动重绘图表 → 插件 → Devtools
import { effect } from './reactivity/effect.js';
import { queueRender } from './reactivity/scheduler.js';
import { LineChart } from './chart/LineChart.js';
import { BarChart }  from './chart/BarChart.js';
import { PieChart }  from './chart/PieChart.js';
import { Pipeline }  from './data/Pipeline.js';
import { EventEmitter } from './events/EventEmitter.js';
import { ExportPngPlugin } from './plugins/ExportPng.js';
import { ExportCsvPlugin } from './plugins/ExportCsv.js';
import { Inspector } from './devtools/Inspector.js';
import { render } from './template/compile.js';

console.log('[jschart] hello, 毕业设计启动！');

const app = document.querySelector('#app');

/**
 * 插件宿主：把一个图表包成 Plugin 协议需要的接口
 * （toolbar / canvas / getData / on('beforeDestroy') / use）
 */
class ChartHost extends EventEmitter {
  constructor(chart) {
    super();
    this.chart = chart;
    this.toolbar = document.createElement('div');
    this.toolbar.className = 'toolbar';
    chart.container.appendChild(this.toolbar);
  }
  get canvas() { return this.chart.canvas; }
  getData() {
    return (this.chart.lastData ?? []).map((p) => [p.x ?? p.ts, p.y ?? p.value]);
  }
  use(PluginClass) {
    const plugin = new PluginClass();
    plugin.install(this);
    console.log(`[Chart] 插件 ${PluginClass.name} 注册，id=${plugin.id.toString()}`);
    return plugin;
  }
  destroy() {
    this.emit('beforeDestroy');
    this.chart.destroy?.();
  }
}

function makeCard(title) {
  const card = document.createElement('div');
  card.className = 'card';
  const h = document.createElement('h2');
  h.textContent = title;
  card.appendChild(h);
  app.appendChild(card);
  return card;
}

// ── ① CPU 折线图：Pipeline 三源合一 + effect 自动重绘 ──
const lineCard = makeCard('CPU 使用率（IDB 缓存 → HTTP 补齐 → 实时刷新）');
const pipeline = new Pipeline('cpu');
const lineChart = new LineChart(lineCard);
const host = new ChartHost(lineChart);
host.use(ExportPngPlugin);
host.use(ExportCsvPlugin);

// ⭐ 灵魂操作：effect 包住 render，且 render 由 rAF 调度（一帧一次）
effect(
  () => {
    lineChart.render(
      pipeline.state.series.map((p) => ({ x: p.ts, y: p.value })),
    );
  },
  { scheduler: queueRender },
);

pipeline.start({ from: 1720000000, to: 1720000300 });

// ── ② 内存柱状图（静态 mock）──
const barCard = makeCard('各机器内存占用');
new BarChart(barCard).render([
  { x: 1, y: 30 }, { x: 2, y: 50 }, { x: 3, y: 45 }, { x: 4, y: 80 },
]);

// ── ③ 机器分布饼图（静态 mock）──
const pieCard = makeCard('集群角色分布');
new PieChart(pieCard).render([
  { label: '前端', value: 35 },
  { label: '后端', value: 28 },
  { label: '运维', value: 12 },
  { label: '产品', value: 25 },
]);

// ── ④ 状态栏：模板引擎 + effect 自动刷新 ──
const status = document.querySelector('#status');
effect(() => {
  status.innerHTML = render(
    '✅ 数据流水线：{{ status }} · 数据点：{{ n }} · 更新于 {{ time }}',
    {
      status: pipeline.state.status,
      n: pipeline.state.series.length,
      time: new Date().toLocaleTimeString(),
    },
  );
});

// ── ⑤ Devtools：FPS / effect 计数 / 内存（自举）──
const inspector = new Inspector();
inspector.mount(document.body);
