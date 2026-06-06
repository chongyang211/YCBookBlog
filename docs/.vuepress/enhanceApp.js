export default ({
  Vue,
  options,
  router,
  siteData
}) => {
  // 限制复制最多100字
  if (typeof document !== 'undefined') {
    document.addEventListener('copy', (e) => {
      const selection = window.getSelection()
      if (!selection) return
      const text = selection.toString()
      if (text.length > 100) {
        // 截取前100字 + 省略提示
        const truncated = text.slice(0, 100) + '...'
        // 阻止默认复制，替换为截断内容
        e.preventDefault()
        e.clipboardData.setData('text/plain', truncated)
      }
    })
  }
}
