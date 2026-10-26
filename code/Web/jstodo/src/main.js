import { getTodos, subscribe } from './state.js';
import { render } from './view.js';
import { execute, AddCommand, undo, redo } from './commands.js';

console.log('[main] hello jstodo');

// ⭐ 关键：state 一变就重渲
subscribe(render);
// 启动时先渲一次（从 localStorage 恢复的数据）
render(getTodos());

// ── 添加 todo：表单 submit ──────────────────────────
const form = document.querySelector('#todo-form');
const input = document.querySelector('#todo-input');

form.addEventListener('submit', (e) => {
  e.preventDefault();              // ⭐ 阻止表单默认提交（否则页面刷新）
  const text = input.value.trim();
  if (!text) return;
  // ⭐ 不直接调 addTodo，而是包装成命令
  execute(new AddCommand(text));
  input.value = '';
  // ⭐ 这里不需要手动调 render——state.addTodo 触发订阅，view.render 自动跑
});

// ── Ctrl+Z / Ctrl+Y 全局快捷键 ─────────────────────
document.addEventListener('keydown', (e) => {
  // 在其它输入框里时，让浏览器原生的输入法撤销正常工作
  if (e.target.tagName === 'INPUT' && e.target !== input) return;

  const isMod = e.ctrlKey || e.metaKey;          // Ctrl (Win/Linux) or Cmd (Mac)
  if (isMod && e.key === 'z' && !e.shiftKey) { e.preventDefault(); undo(); }
  if (isMod && (e.key === 'y' || (e.key === 'z' && e.shiftKey))) {
    e.preventDefault(); redo();
  }
});
