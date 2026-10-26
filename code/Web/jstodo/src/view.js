import { execute, DeleteCommand, ToggleCommand } from './commands.js';

const ul = document.querySelector('#todo-list');
const countEl = document.querySelector('#todo-count');

// ⭐ 一次性在 ul 上绑事件——后续不管 li 怎么增删，监听器永远在
ul.addEventListener('click', (e) => {
  const li = e.target.closest('li[data-id]');
  if (!li) return;
  const id = Number(li.dataset.id);

  // 用 dataset.action 分发动作
  if (e.target.dataset.action === 'delete') {
    execute(new DeleteCommand(id));
  }
});

// checkbox 的状态变化用 change 事件更准确（不是点击坐标，而是状态切换）
ul.addEventListener('change', (e) => {
  if (e.target.dataset.action !== 'toggle') return;
  const li = e.target.closest('li[data-id]');
  if (!li) return;
  execute(new ToggleCommand(Number(li.dataset.id)));
});

/**
 * 把单个 todo 变成 <li> 节点
 * 重点：用 createElement + textContent，绝不用 innerHTML 拼字符串
 */
function createTodoItem(todo) {
  const li = document.createElement('li');
  li.dataset.id = todo.id;          // ⭐ 把 id 写到 data-id 属性，事件委托时取
  if (todo.done) li.classList.add('done');

  // checkbox
  const checkbox = document.createElement('input');
  checkbox.type = 'checkbox';
  checkbox.checked = todo.done;
  checkbox.dataset.action = 'toggle';   // ⭐ 标记动作类型，事件委托时分发
  li.appendChild(checkbox);

  // 文本（textContent 自动转义，安全）
  const span = document.createElement('span');
  span.textContent = todo.text;
  li.appendChild(span);

  // 删除按钮
  const btn = document.createElement('button');
  btn.className = 'delete';
  btn.textContent = '✕';
  btn.dataset.action = 'delete';
  li.appendChild(btn);

  return li;
  // ⭐ 注意：这里没有 addEventListener！
}

// ⭐ id → li 节点的复用缓存（增量 diff）
const itemCache = new Map();

export function render(todos) {
  const t0 = performance.now();
  const seen = new Set();
  const frag = document.createDocumentFragment();

  for (const todo of todos) {
    seen.add(todo.id);
    let li = itemCache.get(todo.id);
    if (!li) {
      li = createTodoItem(todo);            // 新增节点
      itemCache.set(todo.id, li);
    } else {
      // ⭐ 节点已存在，只更新变化的字段
      li.classList.toggle('done', todo.done);
      const checkbox = li.querySelector('input[type=checkbox]');
      if (checkbox.checked !== todo.done) checkbox.checked = todo.done;
      // text 一般不变，这里就不更新；如果做编辑功能再加
    }
    frag.appendChild(li);   // ⭐ appendChild 已存在的节点 = 移动它的位置
  }

  // ⭐ 清掉缓存里已删除的 id
  for (const id of itemCache.keys()) {
    if (!seen.has(id)) itemCache.delete(id);
  }

  ul.innerHTML = '';
  ul.appendChild(frag);
  countEl.textContent = `${todos.filter(t => !t.done).length} 项未完成`;
  console.log(`[render diff] ${todos.length} items in ${(performance.now() - t0).toFixed(2)}ms`);
}
