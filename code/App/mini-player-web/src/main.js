// 应用入口。
// 对应书中 §02 阶段① MVP 骨架：Pinia + Router + 全局样式。
import { createApp } from 'vue';
import { createPinia } from 'pinia';
import App from './App.vue';
import router from './router';
import { applyInitialTheme } from './utils/theme';
import './style.css';
// 主题初始化放在 createApp 之前：避免首屏闪白
applyInitialTheme();
const app = createApp(App);
app.use(createPinia());
app.use(router);
app.mount('#app');
