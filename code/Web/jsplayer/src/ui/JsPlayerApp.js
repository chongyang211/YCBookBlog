import { Player } from '../core/Player.js';
import { Controls } from './Controls.js';

const TEMPLATE = document.createElement('template');
TEMPLATE.innerHTML = /*html*/`
  <style>
    :host { display: block; position: relative; background:#000;
            width: 100%; aspect-ratio:16/9; color:#eee;
            font:14px/1.5 system-ui; user-select:none; }
    :host([hidden]) { display: none; }
    video { width:100%; height:100%; display:block; background:#000; }
    .danmu { position:absolute; inset:0; pointer-events:none; }
    .placeholder { position:absolute; inset:0; display:grid; place-items:center;
                   opacity:.5; pointer-events:none; }

    /* ─── 控制条 ─── */
    .ctrl { position:absolute; left:0; right:0; bottom:0; padding:8px 12px;
            background:linear-gradient(to top, rgba(0,0,0,.7), transparent);
            display:flex; align-items:center; gap:10px; opacity:0;
            transition:opacity .2s; }
    :host(:hover) .ctrl, :host([data-paused]) .ctrl { opacity:1; }

    button { background:transparent; border:0; color:#fff; font-size:16px;
             cursor:pointer; padding:4px 8px; }
    button:hover { color:#3ea6ff; }
    button.active { color:#3ea6ff; }

    /* ─── 进度条三色 ─── */
    .bar { position:relative; flex:1; height:4px; background:rgba(255,255,255,.2);
           cursor:pointer; border-radius:2px; }
    .bar:hover { height:6px; }
    .seekable, .buffered, .played {
      position:absolute; left:0; top:0; bottom:0; border-radius:2px;
    }
    .seekable { background:rgba(255,255,255,.2); width:100%; }
    .buffered { background:rgba(255,255,255,.5); }
    .played   { background:#3ea6ff; }
    .thumb    { position:absolute; top:50%; width:12px; height:12px;
                margin:-6px 0 0 -6px; background:#3ea6ff; border-radius:50%;
                opacity:0; transition:opacity .15s; }
    .bar:hover .thumb { opacity:1; }

    .time { font-variant-numeric: tabular-nums; opacity:.85; min-width:96px; }
  </style>

  <video playsinline></video>
  <canvas class="danmu"></canvas>
  <div class="placeholder"></div>

  <div class="ctrl" part="controls">
    <button class="play" title="播放/暂停">▶</button>
    <div class="bar">
      <div class="seekable"></div>
      <div class="buffered"></div>
      <div class="played"></div>
      <div class="thumb"></div>
    </div>
    <span class="time">00:00 / 00:00</span>
    <button class="speed" title="倍速">1.0×</button>
    <button class="mute" title="静音">🔊</button>
    <button class="pip" title="画中画">🎯</button>
    <button class="full" title="全屏">⛶</button>
  </div>
`;

/** Media Session：手机锁屏 / Mac 控制中心系统媒体控件 */
function setupMediaSession(player, meta = {}) {
  if (!('mediaSession' in navigator)) return;

  navigator.mediaSession.metadata = new MediaMetadata({
    title:  meta.title  || 'jsplayer 演示',
    artist: meta.artist || '杨充',
    album:  meta.album  || 'YCBookBlog',
    artwork: [
      { src: meta.cover || '/public/cover-512.png', sizes: '512x512', type: 'image/png' },
    ],
  });

  // 锁屏按钮回调
  navigator.mediaSession.setActionHandler('play',  () => player.play());
  navigator.mediaSession.setActionHandler('pause', () => player.pause());
  navigator.mediaSession.setActionHandler('seekbackward', d => player.seek(player.video.currentTime - (d.seekOffset || 10)));
  navigator.mediaSession.setActionHandler('seekforward',  d => player.seek(player.video.currentTime + (d.seekOffset || 10)));

  // ⭐ 给系统报"当前时间"——让锁屏进度条能实时滚动
  player.video.addEventListener('timeupdate', () => {
    if (!isFinite(player.video.duration)) return;
    navigator.mediaSession.setPositionState({
      duration: player.video.duration,
      playbackRate: player.video.playbackRate,
      position: player.video.currentTime,
    });
  });
}

export class JsPlayerApp extends HTMLElement {
  // ⭐ 声明哪些属性变化要触发回调
  static get observedAttributes() { return ['src', 'poster', 'autoplay', 'muted']; }

  #shadow;
  #video;
  #player;
  #controls;

  constructor() {
    super();
    // ⭐ Shadow DOM——样式沙箱
    this.#shadow = this.attachShadow({ mode: 'open' });
    this.#shadow.appendChild(TEMPLATE.content.cloneNode(true));
    this.#video = this.#shadow.querySelector('video');
    // ⭐ 核心层：UI / 核心 / 传输 三层分离
    this.#player = new Player(this.#video);
  }

  connectedCallback() {
    console.log('[JsPlayerApp] connected');
    this.#syncAttrs();
    // ⭐ UI 控制条（自绘，不用原生 controls）
    this.#controls = new Controls(this, this.#shadow, this.#player);

    // ⭐ 监听核心层的状态变化 → 更新 UI / 转发给外部
    this.#player.addEventListener('statechange', (e) => {
      const { from, to } = e.detail;
      console.log(`[JsPlayerApp] ${from} → ${to}`);
      this.#updatePlaceholder(to);
      // 透传出去（Shadow 边界要 composed）
      this.dispatchEvent(new CustomEvent('player-state',
        { detail: e.detail, bubbles: true, composed: true }));
    });

    // ⭐ 把内层 video 的关键事件转发到自定义元素（bubbles + composed 才能穿透 Shadow DOM）
    const forward = (type) => {
      this.#video.addEventListener(type, () => {
        this.dispatchEvent(new CustomEvent(type, { bubbles: true, composed: true }));
      });
    };
    ['play', 'pause', 'ended', 'error', 'loadedmetadata'].forEach(forward);

    setupMediaSession(this.#player, { title: this.getAttribute('title') });

    if (this.hasAttribute('src') && this.#player.state === 'idle') {
      this.#player.loadAuto(this.getAttribute('src'));
    }
  }

  disconnectedCallback() {
    console.log('[JsPlayerApp] disconnected');
    this.#controls?.destroy();
    this.#player.destroy();
  }

  attributeChangedCallback(name, _old, val) {
    if (!this.#video) return;
    if (name === 'src' && val) this.#player.loadAuto(val);
    if (name === 'muted') this.#video.muted = this.hasAttribute('muted');
    if (name === 'autoplay') this.#video.autoplay = this.hasAttribute('autoplay');
    if (name === 'poster') this.#video.poster = val ?? '';
  }

  #syncAttrs() {
    const v = this.#video;
    v.poster   = this.getAttribute('poster') ?? '';
    v.muted    = this.hasAttribute('muted');
    v.autoplay = this.hasAttribute('autoplay');
    v.controls = false;   // ⭐ 自定义控制条，关掉原生 controls
  }

  #updatePlaceholder(state) {
    const ph = this.#shadow.querySelector('.placeholder');
    const map = { idle: '待加载', loading: '加载中…', error: '播放失败',
                  ready: '', playing: '', paused: '', ended: '播放完毕' };
    ph.textContent = map[state] ?? '';
    ph.style.opacity = ph.textContent ? '0.7' : '0';
  }

  // ⭐ JS property 镜像 attribute（让 jsPlayer.src = '...' 也能工作）
  get src() { return this.getAttribute('src'); }
  set src(v) { v == null ? this.removeAttribute('src') : this.setAttribute('src', v); }

  get muted() { return this.hasAttribute('muted'); }
  set muted(v) { v ? this.setAttribute('muted', '') : this.removeAttribute('muted'); }

  // ⭐ 公开方法直接转发到 player
  play()    { return this.#player.play(); }
  pause()   { this.#player.pause(); }

  // ⭐ 透传 readyState / currentTime 这种只读属性
  get currentTime() { return this.#video.currentTime; }
  set currentTime(t) { this.#video.currentTime = t; }
  get duration()    { return this.#video.duration; }
  get paused()      { return this.#video.paused; }
  get state()       { return this.#player.state; }
  get player()      { return this.#player; }   // 进阶用户可拿到核心层
}

// ⭐ 注册自定义元素——必须含连字符（HTML 规范）
customElements.define('jsplayer-app', JsPlayerApp);
