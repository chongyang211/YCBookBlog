/**
 * 把模板字符串编译成函数
 * 输入：'<h1>{{ user.name }}</h1>'
 * 输出：(scope) => `<h1>${scope.user.name}</h1>`
 */
export function compile(template) {
  // 1. 扫描所有 {{ expr }} → 替换成 ${(expr)}
  const code = template.replace(/\{\{\s*([\s\S]+?)\s*\}\}/g, (_, expr) => {
    return '${(' + expr.trim() + ')}';
  });
  // 2. 用 new Function 把模板字符串变成函数
  // scope 用 with 块——能直接写 user.name 不写 scope.user.name
  // ⚠️ 严格模式不支持 with，所以这里拼 sloppy 函数体
  const body = `with (scope) { return \`${code}\`; }`;
  // eslint-disable-next-line no-new-func
  return new Function('scope', body);
}

/** 渲染（template 必须来自开发者代码，绝不允许直接喂用户输入！）*/
export function render(template, scope) {
  return compile(template)(scope);
}

// 简易 escape（真实项目用 DOMPurify）
export function escape(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#039;');
}
