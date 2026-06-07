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
      if (!selection || selection.toString().length <= 100) return
      e.preventDefault()
      e.clipboardData.setData('text/plain', selection.toString().slice(0, 100) + '...')
    })
  }

  // 注入分享悬浮按钮
  if (typeof window !== 'undefined') {
    router.afterEach(() => {
      if (document.getElementById('share-float-app')) return
      const app = document.createElement('div')
      app.id = 'share-float-app'
      document.body.appendChild(app)
      new Vue({
        data: { show: false, copied: false },
        methods: {
          toggle() { this.show = !this.show },
          copyLink() {
            const url = location.href
            const fn = navigator.clipboard 
              ? () => navigator.clipboard.writeText(url)
              : () => new Promise(r => {
                  const t = document.createElement('textarea')
                  t.value = url; document.body.appendChild(t)
                  t.select(); document.execCommand('copy')
                  document.body.removeChild(t); r()
                })
            fn().then(() => { this.copied = true; this.show = false; setTimeout(() => this.copied = false, 2000) })
          },
          shareWeibo() {
            const u = encodeURIComponent(location.href)
            open(`https://service.weibo.com/share/share.php?url=${u}&title=${encodeURIComponent(document.title)}`)
            this.show = false
          },
          shareTwitter() {
            open(`https://twitter.com/intent/tweet?url=${encodeURIComponent(location.href)}&text=${encodeURIComponent(document.title)}`)
            this.show = false
          },
          closePanel(e) {
            if (!this.$el.contains(e.target)) this.show = false
          }
        },
        mounted() { document.addEventListener('click', this.closePanel) },
        destroyed() { document.removeEventListener('click', this.closePanel) },
        template: `
          <div class="share-float-wrap" style="position:fixed;bottom:120px;right:20px;z-index:999;display:flex;flex-direction:column;align-items:center;gap:8px">
            <div v-if="show" class="share-panel" style="background:var(--background,#fff);border:1px solid var(--borderColor,#eaecef);border-radius:12px;padding:8px;box-shadow:0 4px 16px rgba(0,0,0,.12);display:flex;flex-direction:column;gap:4px">
              <div @click="copyLink" style="display:flex;align-items:center;gap:10px;padding:10px 14px;border-radius:8px;cursor:pointer;font-size:14px;color:var(--textColor,#2c3e50)">📋 复制链接</div>
              <div @click="shareWeibo" style="display:flex;align-items:center;gap:10px;padding:10px 14px;border-radius:8px;cursor:pointer;font-size:14px;color:var(--textColor,#2c3e50)">📢 分享微博</div>
              <div @click="shareTwitter" style="display:flex;align-items:center;gap:10px;padding:10px 14px;border-radius:8px;cursor:pointer;font-size:14px;color:var(--textColor,#2c3e50)">🐦 Twitter</div>
            </div>
            <button @click="toggle" style="width:44px;height:44px;border-radius:50%;border:none;background:var(--accentColor,#3eaf7c);color:#fff;cursor:pointer;display:flex;align-items:center;justify-content:center;box-shadow:0 4px 12px rgba(62,175,124,.35);outline:none" :title="copied?'已复制':'分享本文'">
              <span v-if="!copied">📤</span><span v-else>✓</span>
            </button>
            <span v-if="copied" style="font-size:12px;color:#4caf50;padding:4px 10px;border-radius:6px;background:var(--background,#fff);box-shadow:0 2px 8px rgba(0,0,0,.1)">已复制</span>
          </div>
        `
      }).$mount('#share-float-app')
    })
  }
}
