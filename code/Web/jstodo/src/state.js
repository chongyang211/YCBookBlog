import { load, save } from './store.js';

// ⭐ 启动时从 store 恢复
const initial = load();
let todos = initial.todos;
let nextId = initial.nextId;

const subscribers = new Set();   // ⭐ 用 Set 防止重复订阅

/** 通知所有订阅者：数据变了 */
function notify() {
  // ⭐ 每次变化自动保存
  save({ todos, nextId });
  for (const fn of subscribers) {
    try { fn(todos); }
    catch (err) { console.error('[state] subscriber error:', err); }
    // ⭐ 一个订阅者抛错不能影响其他订阅者
  }
}

/** 拷贝返回，防止外部直接 push 突变内部数组 */
export function getTodos() {
  return todos.slice();
}

export function addTodo(text) {
  text = text.trim();
  if (!text) return null;                    // 空字符串拒绝
  const todo = { id: nextId++, text, done: false };
  todos.push(todo);
  notify();
  return todo;
}

export function removeTodo(id) {
  const idx = todos.findIndex(t => t.id === id);
  if (idx === -1) return false;
  todos.splice(idx, 1);
  notify();
  return true;
}

export function toggleTodo(id) {
  const t = todos.find(t => t.id === id);
  if (!t) return false;
  t.done = !t.done;
  notify();
  return true;
}

export function subscribe(fn) {
  subscribers.add(fn);
  // ⭐ 返回取消订阅函数（这是订阅模式的标准写法）
  return () => subscribers.delete(fn);
}
