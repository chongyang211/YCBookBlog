<template>
  <div class="share-float">
    <transition name="fade">
      <div v-if="showPanel" class="share-panel">
        <div class="share-item" @click="copyLink">
          <svg viewBox="0 0 24 24" width="20" height="20" fill="currentColor">
            <path d="M3.9 12c0-1.71 1.39-3.1 3.1-3.1h4V7H7a5 5 0 000 10h4v-1.9H7c-1.71 0-3.1-1.39-3.1-3.1zM8 13h8v-2H8v2zm9-6h-4v1.9h4c1.71 0 3.1 1.39 3.1 3.1s-1.39 3.1-3.1 3.1h-4V17h4a5 5 0 000-10z"/>
          </svg>
          <span>复制链接</span>
        </div>
        <div class="share-item" @click="shareWeibo">
          <svg viewBox="0 0 24 24" width="20" height="20" fill="#E6162D">
            <path d="M10.098 20.323c-3.977.391-7.414-1.406-7.672-4.02-.259-2.609 2.759-5.047 6.74-5.441 3.979-.394 7.413 1.404 7.671 4.018.259 2.6-2.759 5.049-6.739 5.443zm-2.607-5.109c-.25 2.029 1.269 3.998 3.387 4.398 2.121.396 4.042-.858 4.291-2.881.249-2.029-1.274-3.995-3.392-4.395-2.121-.396-4.037.859-4.286 2.878zm2.417-3.141c-2.109-.249-4.149.638-4.955 2.357l-.156.334.628-.085c.637-.086 1.273-.057 1.895.082l.366.084-.086.366c-.774 2.651.486 5.162 2.806 5.964l.283.098-.069.289c-.9 2.381-2.81 2.381-4.393 2.381-2.502 0-8.388-.201-7.076-9.693.785-5.33 5.267-8.037 10.145-7.37 4.878.668 7.234 4.47 5.345 7.52-.945 1.524-2.773 2.353-4.641 2.127l-.186-.024-.024.186-.086.367h.367c.67-.083 1.354-.053 1.978.082l.221.049-.049.221c-.626 2.817.962 5.339 3.067 5.339 2.211 0 2.649-2.085 1.533-4.252l-.102-.202.224.04c2.17.385 4.365-.943 5.194-2.952.83-2.009.028-4.326-1.785-5.414l-.313-.188.189-.313c.693-1.152.74-2.451.03-3.463-1.129-1.607-3.547-1.67-5.55-.394l-.337.215-.203-.343c-.874-1.489-2.226-2.341-3.639-2.616z"/>
          </svg>
          <span>微博</span>
        </div>
        <div class="share-item" @click="shareTwitter">
          <svg viewBox="0 0 24 24" width="20" height="20" fill="#1DA1F2">
            <path d="M22.46 6c-.85.38-1.78.64-2.73.76 1-.6 1.76-1.54 2.12-2.67-.93.55-1.96.95-3.06 1.17a4.77 4.77 0 00-8.14 4.35C6.88 9.5 3.92 7.5 1.92 4.6a4.77 4.77 0 001.48 6.38c-.78-.02-1.5-.24-2.14-.6v.06a4.78 4.78 0 003.83 4.69c-.7.19-1.44.22-2.16.08a4.78 4.78 0 004.46 3.32A9.58 9.58 0 010 19.54a13.5 13.5 0 007.33 2.15c8.8 0 13.6-7.3 13.6-13.63 0-.21 0-.41-.02-.62A9.7 9.7 0 0024 5.56c-.88.4-1.83.67-2.83.79a4.93 4.93 0 002.16-2.72"/>
          </svg>
          <span>Twitter</span>
        </div>
      </div>
    </transition>
    <button class="share-btn" @click="togglePanel" :title="copied ? '已复制' : '分享本文'">
      <svg v-if="!copied" viewBox="0 0 24 24" width="22" height="22" fill="currentColor">
        <path d="M18 16.08c-.76 0-1.44.3-1.96.77L8.91 12.7c.05-.23.09-.46.09-.7s-.04-.47-.09-.7l7.05-4.11c.54.5 1.25.81 2.04.81 1.66 0 3-1.34 3-3s-1.34-3-3-3-3 1.34-3 3c0 .24.04.47.09.7L8.04 9.81C7.5 9.31 6.79 9 6 9c-1.66 0-3 1.34-3 3s1.34 3 3 3c.79 0 1.5-.31 2.04-.81l7.12 4.16c-.05.21-.08.43-.08.65 0 1.61 1.31 2.92 2.92 2.92s2.92-1.31 2.92-2.92-1.31-2.92-2.92-2.92z"/>
      </svg>
      <svg v-else viewBox="0 0 24 24" width="22" height="22" fill="#4caf50">
        <path d="M9 16.17L4.83 12l-1.42 1.41L9 19 21 7l-1.41-1.41z"/>
      </svg>
    </button>
    <span v-if="copied" class="copy-tip">已复制</span>
  </div>
</template>

<script>
export default {
  data() {
    return {
      showPanel: false,
      copied: false,
    }
  },
  methods: {
    togglePanel() {
      this.showPanel = !this.showPanel
    },
    copyLink() {
      const url = window.location.href
      if (navigator.clipboard) {
        navigator.clipboard.writeText(url).then(() => {
          this.showCopied()
        })
      } else {
        const textarea = document.createElement('textarea')
        textarea.value = url
        document.body.appendChild(textarea)
        textarea.select()
        document.execCommand('copy')
        document.body.removeChild(textarea)
        this.showCopied()
      }
    },
    showCopied() {
      this.copied = true
      this.showPanel = false
      setTimeout(() => { this.copied = false }, 2000)
    },
    shareWeibo() {
      const url = encodeURIComponent(window.location.href)
      const title = encodeURIComponent(document.title)
      window.open(`https://service.weibo.com/share/share.php?url=${url}&title=${title}`, '_blank')
      this.showPanel = false
    },
    shareTwitter() {
      const url = encodeURIComponent(window.location.href)
      const text = encodeURIComponent(document.title)
      window.open(`https://twitter.com/intent/tweet?url=${url}&text=${text}`, '_blank')
      this.showPanel = false
    },
    handleClickOutside(e) {
      if (!this.$el.contains(e.target)) {
        this.showPanel = false
      }
    },
  },
  mounted() {
    document.addEventListener('click', this.handleClickOutside)
  },
  beforeDestroy() {
    document.removeEventListener('click', this.handleClickOutside)
  },
}
</script>

<style scoped>
.share-float {
  position: fixed;
  bottom: 120px;
  right: 20px;
  z-index: 999;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
}

.share-btn {
  width: 44px;
  height: 44px;
  border-radius: 50%;
  border: none;
  background: var(--accentColor, #3eaf7c);
  color: #fff;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 4px 12px rgba(62, 175, 124, 0.35);
  transition: all 0.3s ease;
  outline: none;
}

.share-btn:hover {
  transform: scale(1.1);
  box-shadow: 0 6px 16px rgba(62, 175, 124, 0.5);
}

.share-panel {
  background: var(--background, #fff);
  border: 1px solid var(--borderColor, #eaecef);
  border-radius: 12px;
  padding: 8px;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.12);
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.share-item {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 14px;
  border-radius: 8px;
  cursor: pointer;
  transition: background 0.2s;
  white-space: nowrap;
  font-size: 14px;
  color: var(--textColor, #2c3e50);
}

.share-item:hover {
  background: var(--borderColor, #f3f4f5);
}

.copy-tip {
  font-size: 12px;
  color: #4caf50;
  background: var(--background, #fff);
  padding: 4px 10px;
  border-radius: 6px;
  box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.2s, transform 0.2s;
}
.fade-enter,
.fade-leave-to {
  opacity: 0;
  transform: translateY(8px);
}

/* 移动端适配 */
@media (max-width: 720px) {
  .share-float {
    bottom: 80px;
    right: 12px;
  }
  .share-btn {
    width: 38px;
    height: 38px;
  }
}
</style>
