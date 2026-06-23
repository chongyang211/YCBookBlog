<template>
  <div class="qr-panel">
    <div class="qr-template-tabs">
      <button
        v-for="t in tplTabs"
        :key="t.id"
        class="qr-template-tab"
        :class="{ active: tpl === t.id }"
        @click="tpl = t.id"
      >{{ t.label }}</button>
    </div>

    <div class="qr-layout">
      <div class="qr-form">
        <!-- WiFi -->
        <template v-if="tpl === 'wifi'">
          <label class="qr-label">SSID</label>
          <input v-model="wifi.ssid" class="qr-input" placeholder="网络名称" />
          <label class="qr-label">密码</label>
          <input v-model="wifi.password" class="qr-input" placeholder="密码 (开放网络可留空)" />
          <div class="qr-form-row">
            <div>
              <label class="qr-label">加密方式</label>
              <select v-model="wifi.encryption" class="qr-select">
                <option value="WPA">WPA / WPA2</option>
                <option value="WEP">WEP</option>
                <option value="nopass">开放（无密码）</option>
              </select>
            </div>
            <div style="display:flex;align-items:flex-end;">
              <label style="font-size:13px;color:#555;">
                <input type="checkbox" v-model="wifi.hidden" /> 隐藏网络
              </label>
            </div>
          </div>
        </template>

        <!-- vCard -->
        <template v-if="tpl === 'vcard'">
          <div class="qr-form-row">
            <div>
              <label class="qr-label">姓</label>
              <input v-model="vcard.lastName" class="qr-input" />
            </div>
            <div>
              <label class="qr-label">名</label>
              <input v-model="vcard.firstName" class="qr-input" />
            </div>
          </div>
          <label class="qr-label">公司</label>
          <input v-model="vcard.org" class="qr-input" />
          <label class="qr-label">职位</label>
          <input v-model="vcard.title" class="qr-input" />
          <div class="qr-form-row">
            <div>
              <label class="qr-label">电话</label>
              <input v-model="vcard.phone" class="qr-input" />
            </div>
            <div>
              <label class="qr-label">邮箱</label>
              <input v-model="vcard.email" class="qr-input" />
            </div>
          </div>
          <label class="qr-label">网址</label>
          <input v-model="vcard.url" class="qr-input" />
          <label class="qr-label">地址</label>
          <input v-model="vcard.address" class="qr-input" />
          <label class="qr-label">备注</label>
          <textarea v-model="vcard.note" class="qr-textarea" rows="2"></textarea>
        </template>

        <!-- SMS -->
        <template v-if="tpl === 'sms'">
          <label class="qr-label">手机号</label>
          <input v-model="sms.phone" class="qr-input" placeholder="+86..." />
          <label class="qr-label">短信内容</label>
          <textarea v-model="sms.body" class="qr-textarea" rows="3"></textarea>
        </template>

        <!-- Mailto -->
        <template v-if="tpl === 'mailto'">
          <label class="qr-label">收件人</label>
          <input v-model="mail.to" class="qr-input" placeholder="someone@example.com" />
          <label class="qr-label">主题</label>
          <input v-model="mail.subject" class="qr-input" />
          <label class="qr-label">内容</label>
          <textarea v-model="mail.body" class="qr-textarea" rows="3"></textarea>
        </template>

        <!-- Geo -->
        <template v-if="tpl === 'geo'">
          <div class="qr-form-row">
            <div>
              <label class="qr-label">纬度</label>
              <input v-model.number="geo.lat" type="number" step="0.000001" class="qr-input" />
            </div>
            <div>
              <label class="qr-label">经度</label>
              <input v-model.number="geo.lng" type="number" step="0.000001" class="qr-input" />
            </div>
          </div>
        </template>

        <!-- 编码结果 -->
        <label class="qr-label" style="margin-top:14px;">编码字符串</label>
        <textarea :value="encoded" class="qr-textarea" rows="3" readonly></textarea>

        <div class="qr-toolbar">
          <button class="qr-btn qr-btn-primary" @click="generate" :disabled="loading || !encoded">
            {{ loading ? '生成中…' : '生成二维码' }}
          </button>
          <button class="qr-btn" @click="download" :disabled="!hasResult">下载 PNG</button>
          <button class="qr-btn" @click="copyContent" :disabled="!encoded">复制字符串</button>
        </div>

        <div v-if="msg" class="qr-msg" :class="{ 'qr-msg-err': isErr }">{{ msg }}</div>
      </div>

      <div class="qr-preview-box">
        <canvas ref="canvas" class="qr-preview-canvas"></canvas>
        <div v-if="!hasResult" class="qr-preview-empty">预览区</div>
      </div>
    </div>
  </div>
</template>

<script>
import {
  loadScript, SCRIPTS, copyText, downloadCanvas,
  buildWifi, buildVcard, buildSms, buildMailto, buildGeo
} from './utils.js';

export default {
  name: 'TemplatePanel',
  data() {
    return {
      tpl: 'wifi',
      tplTabs: [
        { id: 'wifi',   label: 'WiFi' },
        { id: 'vcard',  label: '名片 vCard' },
        { id: 'sms',    label: '短信' },
        { id: 'mailto', label: '邮件' },
        { id: 'geo',    label: '地理位置' }
      ],
      wifi: { ssid: 'MyWiFi', password: '12345678', encryption: 'WPA', hidden: false },
      vcard: {
        firstName: '三', lastName: '张', org: 'Example Inc.', title: '工程师',
        phone: '+86 13800138000', email: 'zhangsan@example.com', url: '', address: '', note: ''
      },
      sms: { phone: '+8613800138000', body: '你好' },
      mail: { to: 'someone@example.com', subject: '问候', body: '你好' },
      geo: { lat: 39.9042, lng: 116.4074 },
      loading: false, hasResult: false, msg: '', isErr: false
    };
  },
  computed: {
    encoded() {
      try {
        switch (this.tpl) {
          case 'wifi':   return buildWifi(this.wifi);
          case 'vcard':  return buildVcard(this.vcard);
          case 'sms':    return buildSms(this.sms);
          case 'mailto': return buildMailto(this.mail);
          case 'geo':    return buildGeo(this.geo);
        }
      } catch (e) { return ''; }
      return '';
    }
  },
  methods: {
    async generate() {
      if (!this.encoded) { this.show('请先填写完整内容', true); return; }
      this.loading = true; this.msg = '';
      try {
        await loadScript(SCRIPTS.qrcode);
        await window.QRCode.toCanvas(this.$refs.canvas, this.encoded, {
          width: 320, margin: 2, errorCorrectionLevel: 'M'
        });
        this.hasResult = true;
        this.show('生成成功', false);
      } catch (e) {
        this.show(e.message || '生成失败', true);
      } finally { this.loading = false; }
    },
    download() { if (this.hasResult) downloadCanvas(this.$refs.canvas, this.tpl + '.png'); },
    async copyContent() {
      const ok = await copyText(this.encoded);
      this.show(ok ? '已复制' : '复制失败', !ok);
    },
    show(t, err) { this.msg = t; this.isErr = !!err; }
  }
};
</script>
