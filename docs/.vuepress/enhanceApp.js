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
        data: { 
          show: false, copied: false, showQr: false
        },
        methods: {
          toggle() { this.show = !this.show; this.showQr = false },
          copyLink() {
            const fn = navigator.clipboard 
              ? () => navigator.clipboard.writeText(location.href)
              : () => new Promise(r => {
                  const t = document.createElement('textarea')
                  t.value = location.href; document.body.appendChild(t)
                  t.select(); document.execCommand('copy')
                  document.body.removeChild(t); r()
                })
            fn().then(() => { this.copied = true; this.show = false; setTimeout(() => this.copied = false, 2000) })
          },
          showWechatQr() { this.showQr = true },
          hideQr() { this.showQr = false },
          shareQQ() {
            open(`https://connect.qq.com/widget/shareqq/index.html?url=${encodeURIComponent(location.href)}&title=${encodeURIComponent(document.title)}&desc=${encodeURIComponent(document.title)}`)
            this.show = false
          },
          shareQzone() {
            open(`https://sns.qzone.qq.com/cgi-bin/qzshare/cgi_qzshare_onekey?url=${encodeURIComponent(location.href)}&title=${encodeURIComponent(document.title)}`)
            this.show = false
          },
          shareWeibo() {
            open(`https://service.weibo.com/share/share.php?url=${encodeURIComponent(location.href)}&title=${encodeURIComponent(document.title)}`)
            this.show = false
          },
          closePanel(e) {
            if (!this.$el.contains(e.target)) { this.show = false; this.showQr = false }
          }
        },
        mounted() { document.addEventListener('click', this.closePanel) },
        destroyed() { document.removeEventListener('click', this.closePanel) },
        template: `
<div class="share-float-wrap" style="position:fixed;bottom:120px;right:20px;z-index:999;display:flex;flex-direction:column;align-items:center;gap:8px;font-family:-apple-system,BlinkMacSystemFont,sans-serif">

  <!-- QR Code Overlay -->
  <div v-if="showQr" @click.stop="hideQr" style="position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:9999;display:flex;align-items:center;justify-content:center">
    <div @click.stop="" style="background:#fff;border-radius:16px;padding:24px;text-align:center;max-width:320px;box-shadow:0 8px 32px rgba(0,0,0,.3)">
      <div style="font-size:16px;font-weight:bold;margin-bottom:12px;color:#333">微信扫码分享</div>
      <img :src="'https://api.qrserver.com/v1/create-qr-code/?size=200x200&data='+encodeURIComponent(location.href)" 
           style="width:200px;height:200px;border-radius:8px;border:1px solid #eee" alt="QR Code"/>
      <div style="font-size:12px;color:#999;margin-top:8px">打开微信扫一扫，分享给好友或朋友圈</div>
      <button @click="hideQr" style="margin-top:12px;padding:8px 24px;border:none;border-radius:8px;background:#07c160;color:#fff;font-size:14px;cursor:pointer">关闭</button>
    </div>
  </div>

  <!-- Share Panel -->
  <div v-if="show" class="share-panel" style="background:var(--background,#fff);border:1px solid var(--borderColor,#eaecef);border-radius:16px;padding:12px 8px;box-shadow:0 8px 24px rgba(0,0,0,.15);display:flex;flex-direction:column;gap:2px;min-width:160px">
    
    <div @click="showWechatQr" style="display:flex;align-items:center;gap:10px;padding:12px 16px;border-radius:10px;cursor:pointer;font-size:14px;color:#07c160;transition:background .2s">
      <span style="font-size:18px">💬</span> 微信 / 朋友圈
    </div>

    <div @click="shareQQ" style="display:flex;align-items:center;gap:10px;padding:12px 16px;border-radius:10px;cursor:pointer;font-size:14px;color:#12b7f5;transition:background .2s">
      <span style="font-size:18px">🐧</span> QQ好友
    </div>

    <div @click="shareQzone" style="display:flex;align-items:center;gap:10px;padding:12px 16px;border-radius:10px;cursor:pointer;font-size:14px;color:#f5c842;transition:background .2s">
      <span style="font-size:18px">🌐</span> QQ空间
    </div>

    <div @click="shareWeibo" style="display:flex;align-items:center;gap:10px;padding:12px 16px;border-radius:10px;cursor:pointer;font-size:14px;color:#e6162d;transition:background .2s">
      <span style="font-size:18px">📢</span> 新浪微博
    </div>

    <div style="border-top:1px solid var(--borderColor,#eee);margin:4px 0"></div>

    <div @click="copyLink" style="display:flex;align-items:center;gap:10px;padding:12px 16px;border-radius:10px;cursor:pointer;font-size:14px;color:var(--textColor,#333);transition:background .2s">
      <span style="font-size:18px">🔗</span> 复制链接
    </div>

  </div>

  <!-- Floating Button -->
  <button @click="toggle" 
    style="width:48px;height:48px;border-radius:50%;border:none;background:linear-gradient(135deg,#3eaf7c,#2d8cf0);color:#fff;cursor:pointer;display:flex;align-items:center;justify-content:center;box-shadow:0 6px 20px rgba(62,175,124,.4);outline:none;font-size:20px;transition:transform .2s"
    :title="copied?'已复制':'分享本文'"
    onmouseover="this.style.transform='scale(1.1)'" onmouseout="this.style.transform='scale(1)'">
    <span v-if="!copied">↗</span><span v-else style="font-size:22px">✓</span>
  </button>

  <span v-if="copied" style="font-size:12px;color:#4caf50;padding:4px 12px;border-radius:8px;background:var(--background,#fff);box-shadow:0 2px 8px rgba(0,0,0,.1);white-space:nowrap">链接已复制 ✓</span>

</div>`
      }).$mount('#share-float-app')
    })
  }
}
