const KEY = 'jstodo:v1';   // ⭐ 带版本号，将来 schema 升级好做迁移

/**
 * 从 localStorage 读取 todos
 * 任何异常都返回空数组，绝不让上层崩溃
 */
export function load() {
  try {
    const raw = localStorage.getItem(KEY);
    if (!raw) return { todos: [], nextId: 1 };

    const data = JSON.parse(raw);

    // ⭐ schema 校验：必须是数组才接受
    if (!Array.isArray(data.todos)) {
      console.warn('[store] 数据格式异常，使用默认值');
      return { todos: [], nextId: 1 };
    }
    return {
      todos: data.todos,
      nextId: typeof data.nextId === 'number' ? data.nextId : data.todos.length + 1,
    };
  } catch (err) {
    console.error('[store] 读取失败:', err);
    return { todos: [], nextId: 1 };   // ⭐ 兜底返回空，绝不抛出去
  }
}

export function save(state) {
  try {
    localStorage.setItem(KEY, JSON.stringify(state));
  } catch (err) {
    // 可能是隐私模式 / 配额超限
    console.error('[store] 写入失败:', err);
  }
}
