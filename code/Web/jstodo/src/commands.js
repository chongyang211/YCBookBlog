import { addTodo, removeTodo, toggleTodo, getTodos } from './state.js';

/** 抽象基类 */
class Command {
  execute() { throw new Error('Command.execute() must be overridden'); }
  undo()    { throw new Error('Command.undo() must be overridden'); }
}

/** 添加命令：execute = state.addTodo / undo = state.removeTodo */
export class AddCommand extends Command {
  constructor(text) {
    super();
    this.text = text;
    this.createdId = null;     // execute 时记录 id，undo 时用
  }
  execute() {
    const todo = addTodo(this.text);
    if (todo) { this.createdId = todo.id; return true; }
    return false;
  }
  undo() {
    if (this.createdId != null) removeTodo(this.createdId);
  }
}

/** 删除命令：需要保存被删的 todo 完整数据，否则 undo 时数据丢了 */
export class DeleteCommand extends Command {
  constructor(id) {
    super();
    this.id = id;
    this.snapshot = null;     // 删之前先快照
  }
  execute() {
    this.snapshot = getTodos().find(t => t.id === this.id);
    if (!this.snapshot) return false;
    removeTodo(this.id);
    return true;
  }
  undo() {
    if (!this.snapshot) return;
    // ⭐ 注意：addTodo 会分配新 id，原 id 找不回来——这是已知局限
    // 真正容器设计实战会让 state 提供 "按指定 id 恢复" 的 API
    addTodo(this.snapshot.text);
  }
}

/** 切换完成状态：undo = 再 toggle 一次 */
export class ToggleCommand extends Command {
  constructor(id) { super(); this.id = id; }
  execute() { return toggleTodo(this.id); }
  undo()    { toggleTodo(this.id); }
}

// ⭐⭐⭐ 调度器：双栈实现撤销重做
const undoStack = [];
const redoStack = [];

export function execute(cmd) {
  if (cmd.execute() === false) return;   // 命令被拒（如空字符串），不入栈
  undoStack.push(cmd);
  redoStack.length = 0;                  // ⭐ 新操作发生，清空 redo 栈
}

export function undo() {
  const cmd = undoStack.pop();
  if (!cmd) return;
  cmd.undo();
  redoStack.push(cmd);
}

export function redo() {
  const cmd = redoStack.pop();
  if (!cmd) return;
  cmd.execute();
  undoStack.push(cmd);
}
