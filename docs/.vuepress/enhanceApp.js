export default ({
  Vue,
  options,
  router,
  siteData
}) => {
  // ========== 安全防护区 ==========

  // 1. 控制台版权警告（震慑爬虫和F12抓取）
  if (typeof console !== 'undefined') {
    console.log(
      '%c⚠️ 安全警告 %c请勿在此页面执行任何脚本。%c 如发现漏洞欢迎联系 yangchong211@163.com',
      'color:#e74c3c;font-size:20px;font-weight:bold',
      'color:#333;font-size:14px',
      'color:#999;font-size:12px'
    )
  }

  // 2. 复制限制：超过100字自动截断并附加来源
  if (typeof document !== 'undefined') {
    document.addEventListener('copy', (e) => {
      const selection = window.getSelection()
      if (!selection || selection.toString().length <= 100) return
      const clipped = selection.toString().slice(0, 100) + 
        `...\n\n— 原文来自 编程进阶网 yccoding.com —`
      e.preventDefault()
      e.clipboardData.setData('text/plain', clipped)
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
        computed: {
          shareTitle() {
            // 优先取 h1，其次 document.title，最后用域名
            const h1 = document.querySelector('h1')
            if (h1) return h1.textContent.trim()
            const t = document.title
            return t.includes('|') ? t.split('|')[0].trim() : t
          },
          shareDesc() {
            // 取 meta description，其次取第一段文字
            const meta = document.querySelector('meta[name="description"]')
            if (meta) return meta.content.slice(0, 100)
            const p = document.querySelector('.theme-default-content p')
            return p ? p.textContent.slice(0, 100) : '编程进阶，你值得拥有！'
          },
          shareUrl() {
            return location.href
          },
          sharePic() {
            // 取第一张图片作为分享缩略图
            const img = document.querySelector('.theme-default-content img')
            return img ? img.src : ''
          }
        },
        methods: {
          toggle() { this.show = !this.show; this.showQr = false },
          copyLink() {
            const text = `${this.shareTitle}\n${this.shareUrl}`
            const fn = navigator.clipboard 
              ? () => navigator.clipboard.writeText(text)
              : () => new Promise(r => {
                  const t = document.createElement('textarea')
                  t.value = text; document.body.appendChild(t)
                  t.select(); document.execCommand('copy')
                  document.body.removeChild(t); r()
                })
            fn().then(() => { this.copied = true; this.show = false; setTimeout(() => this.copied = false, 2000) })
          },
          showWechatQr() { this.showQr = true },
          hideQr() { this.showQr = false },
          shareQQ() {
            const u = encodeURIComponent(this.shareUrl)
            const t = encodeURIComponent(this.shareTitle)
            const d = encodeURIComponent(this.shareDesc)
            const p = this.sharePic ? `&pics=${encodeURIComponent(this.sharePic)}` : ''
            open(`https://connect.qq.com/widget/shareqq/index.html?url=${u}&title=${t}&desc=${d}&summary=${d}${p}`)
            this.show = false
          },
          shareQzone() {
            const u = encodeURIComponent(this.shareUrl)
            const t = encodeURIComponent(this.shareTitle)
            const d = encodeURIComponent(this.shareDesc)
            const p = this.sharePic ? `&pics=${encodeURIComponent(this.sharePic)}` : ''
            open(`https://sns.qzone.qq.com/cgi-bin/qzshare/cgi_qzshare_onekey?url=${u}&title=${t}&desc=${d}&summary=${d}${p}`)
            this.show = false
          },
          shareWeibo() {
            const u = encodeURIComponent(this.shareUrl)
            const t = encodeURIComponent(this.shareTitle)
            const d = encodeURIComponent(this.shareDesc)
            const p = this.sharePic ? `&pic=${encodeURIComponent(this.sharePic)}` : ''
            open(`https://service.weibo.com/share/share.php?url=${u}&title=${t}&summary=${d}${p}`)
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
    <div @click.stop="" style="background:#fff;border-radius:16px;padding:24px;text-align:center;max-width:340px;box-shadow:0 8px 32px rgba(0,0,0,.3)">
      <div style="font-size:16px;font-weight:bold;margin-bottom:8px;color:#333">📱 微信扫码分享</div>
      
      <!-- Share Preview -->
      <div style="background:#f5f5f5;border-radius:8px;padding:12px;margin-bottom:12px;text-align:left">
        <div style="font-size:14px;font-weight:bold;color:#333;margin-bottom:4px;line-height:1.4">{{ shareTitle }}</div>
        <div style="font-size:12px;color:#666;line-height:1.4">{{ shareDesc }}</div>
      </div>
      
      <img :src="'https://api.qrserver.com/v1/create-qr-code/?size=200x200&data='+encodeURIComponent(shareUrl)" 
           style="width:200px;height:200px;border-radius:8px;border:1px solid #eee" alt="QR Code"/>
      <div style="font-size:12px;color:#999;margin-top:8px">打开微信扫一扫，分享给好友或朋友圈</div>
      <button @click="hideQr" style="margin-top:12px;padding:8px 32px;border:none;border-radius:8px;background:#07c160;color:#fff;font-size:14px;cursor:pointer">关闭</button>
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
