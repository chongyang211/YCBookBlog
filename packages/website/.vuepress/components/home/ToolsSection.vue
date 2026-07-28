<template>
  <section class="hp-section hp-tools">
    <div class="hp-tools-inner">
      <div class="hp-tools-left">
        <div class="hp-section-eyebrow hp-eyebrow-light">DEVELOPER TOOLS</div>
        <h2 class="hp-tools-title">浏览器端在线工具箱</h2>
        <p class="hp-tools-sub">
          18+ 个高频开发工具，无需注册，无需下载，<br/>
          所有数据本地处理，永不上传。
        </p>

        <div class="hp-tools-features">
          <div class="hp-tf">
            <div class="hp-tf-icon">🔒</div>
            <div class="hp-tf-text">
              <strong>零数据上传</strong>
              <span>全部浏览器端 JS 运算，隐私无忧</span>
            </div>
          </div>
          <div class="hp-tf">
            <div class="hp-tf-icon">⚡</div>
            <div class="hp-tf-text">
              <strong>开箱即用</strong>
              <span>无需注册账号，无需安装客户端</span>
            </div>
          </div>
          <div class="hp-tf">
            <div class="hp-tf-icon">🎯</div>
            <div class="hp-tf-text">
              <strong>聚焦高频场景</strong>
              <span>JSON · 文本 · 图片 · 加解密 · 二维码 ...</span>
            </div>
          </div>
        </div>

        <a href="/tools/" class="hp-btn hp-btn-primary hp-btn-lg">
          进入工具箱
          <svg viewBox="0 0 24 24" width="20" height="20" fill="none" stroke="currentColor" stroke-width="2.5">
            <path d="M5 12h14M13 5l7 7-7 7"/>
          </svg>
        </a>
      </div>

      <div class="hp-tools-right">
        <div class="hp-tools-cloud">
          <div
            v-for="t in toolCloud"
            :key="t.name"
            class="hp-tcloud"
          >
            <span class="hp-tcloud-icon">{{ t.icon }}</span>
            <span class="hp-tcloud-name">{{ t.name }}</span>
          </div>
        </div>
      </div>
    </div>
  </section>
</template>

<script>
export default {
  name: 'ToolsSection',
  props: {
    toolCloud: { type: Array, required: true },
  },
}
</script>

<style scoped>
.hp-tools {
  max-width: none;
  background: linear-gradient(135deg, #4a90d9 0%, #7b5fd9 50%, #ec4899 100%);
  color: #fff;
  padding: 100px 24px;
  position: relative;
  overflow: hidden;
}
.hp-tools::before {
  content: '';
  position: absolute; inset: 0;
  background-image:
    radial-gradient(circle at 20% 30%, rgba(255,255,255,0.15) 0%, transparent 40%),
    radial-gradient(circle at 80% 70%, rgba(255,255,255,0.1) 0%, transparent 40%);
  pointer-events: none;
}
.hp-tools-inner {
  position: relative;
  max-width: 1200px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 70px;
  align-items: center;
}
.hp-tools-title {
  font-size: 40px;
  font-weight: 800;
  margin: 12px 0 18px;
  color: #fff;
  letter-spacing: -1px;
}
.hp-tools-sub {
  font-size: 16px;
  line-height: 1.85;
  color: rgba(255, 255, 255, 0.85);
  margin: 0 0 32px;
}

.hp-tools-features {
  display: flex;
  flex-direction: column;
  gap: 16px;
  margin-bottom: 36px;
}
.hp-tf {
  display: flex;
  gap: 14px;
  align-items: flex-start;
  padding: 14px 18px;
  background: rgba(255, 255, 255, 0.14);
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 12px;
  transition: background 0.25s ease, transform 0.25s ease;
}
.hp-tf:hover {
  background: rgba(255, 255, 255, 0.18);
  transform: translateX(4px);
}
.hp-tf-icon {
  font-size: 24px;
  flex-shrink: 0;
}
.hp-tf-text {
  display: flex;
  flex-direction: column;
}
.hp-tf-text strong {
  font-size: 15px;
  font-weight: 600;
  color: #fff;
  margin-bottom: 2px;
}
.hp-tf-text span {
  font-size: 13px;
  color: rgba(255, 255, 255, 0.75);
}

.hp-tools >>> .hp-btn-primary,
.hp-tools /deep/ .hp-btn-primary {
  background: #fff;
  color: var(--hp-brand-2) !important;
  box-shadow: 0 8px 20px rgba(0, 0, 0, 0.15);
}
.hp-tools >>> .hp-btn-primary:hover,
.hp-tools /deep/ .hp-btn-primary:hover {
  background: #f9fafb;
  box-shadow: 0 12px 28px rgba(0, 0, 0, 0.2);
}

/* 工具云 */
.hp-tools-cloud {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 14px;
  position: relative;
}
.hp-tcloud {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
  padding: 18px 8px;
  background: rgba(255, 255, 255, 0.16);
  border: 1px solid rgba(255, 255, 255, 0.25);
  border-radius: 14px;
  transition: background 0.2s ease;
}
.hp-tcloud:hover { background: rgba(255, 255, 255, 0.25); }
.hp-tcloud-icon { font-size: 28px; }
.hp-tcloud-name { font-size: 12.5px; color: #fff; font-weight: 500; }

/* 响应式 */
@media (max-width: 960px) {
  .hp-tools-inner { grid-template-columns: 1fr; gap: 40px; }
}
@media (max-width: 768px) {
  .hp-tools-title { font-size: 30px; }
  .hp-tools-cloud { grid-template-columns: repeat(3, 1fr); }
}
</style>
