import { DanmakuIndex } from '../danmaku/Index.js';
import { DanmakuEngine } from '../danmaku/Engine.js';
import { parseVtt } from '../subtitle/parseVtt.js';
import { SubtitleLayer } from '../subtitle/SubtitleLayer.js';

export class Controls {
  #host;        // 自定义元素本体（Shadow root 的宿主）
  #player;
  #els;
  #raf = null;
  #rvfcId = null;
  #index;
  #engine;
  #subs;
  #lastTs = null;
  #ro = null;

  constructor(hostEl, shadowRoot, player) {
    this.#host = hostEl;
    this.#player = player;
    this.#els = {
      play:     shadowRoot.querySelector('.play'),
      mute:     shadowRoot.querySelector('.mute'),
      speed:    shadowRoot.querySelector('.speed'),
      full:     shadowRoot.querySelector('.full'),
      pip:      shadowRoot.querySelector('.pip'),
      bar:      shadowRoot.querySelector('.bar'),
      played:   shadowRoot.querySelector('.played'),
      buffered: shadowRoot.querySelector('.buffered'),
      thumb:    shadowRoot.querySelector('.thumb'),
      time:     shadowRoot.querySelector('.time'),
    };
    this.#bind();

    // ── Canvas 弹幕层 ──────────────────────────
    const canvas = shadowRoot.querySelector('.danmu');
    this.#index  = new DanmakuIndex();
    this.#engine = new DanmakuEngine(canvas, this.#index);
    this.#syncCanvasSize();
    this.#ro = new ResizeObserver(() => this.#syncCanvasSize());
    this.#ro.observe(this.#host);

    // 加载弹幕数据
    fetch('/public/danmaku.json').then(r => r.json()).then(arr => {
      this.#index.load(arr);
      console.log('[Danmaku] loaded', arr.length);
    }).catch(err => console.warn('[Danmaku] 加载失败', err));

    // ── WebVTT 字幕层 ──────────────────────────
    this.#subs = new SubtitleLayer(shadowRoot);
    fetch('/public/demo.vtt').then(r => r.text()).then(text => {
      this.#subs.load(parseVtt(text));
      console.log('[Subtitle] loaded');
    }).catch(err => console.warn('[Subtitle] 加载失败', err));
  }

  #bind() {
    const v = this.#player.video;
    const e = this.#els;

    // ⭐ 播放 / 暂停
    e.play.addEventListener('click', () =>
      v.paused ? this.#player.play().catch(() => {}) : this.#player.pause());

    // ⭐ 静音
    e.mute.addEventListener('click', () => {
      v.muted = !v.muted;
      e.mute.textContent = v.muted ? '🔇' : '🔊';
    });

    // ⭐ 倍速循环
    e.speed.addEventListener('click', () => {
      const seq = [1.0, 1.25, 1.5, 2.0, 0.75];
      const idx = (seq.indexOf(v.playbackRate) + 1) % seq.length;
      v.playbackRate = seq[idx];
      e.speed.textContent = seq[idx].toFixed(2) + '×';
    });

    // ⭐ 全屏
    e.full.addEventListener('click', () => this.#toggleFullscreen());

    // ⭐ 画中画（PiP）——特性检测三层
    e.pip.addEventListener('click', async () => {
      try {
        if (document.pictureInPictureElement) {
          await document.exitPictureInPicture();
        } else if (document.pictureInPictureEnabled) {
          await v.requestPictureInPicture();
        } else {
          console.warn('[PiP] 当前浏览器不支持画中画');
        }
      } catch (err) {
        console.warn('[PiP] 失败', err);
      }
    });
    v.addEventListener('enterpictureinpicture', () => e.pip.classList.add('active'));
    v.addEventListener('leavepictureinpicture', () => e.pip.classList.remove('active'));

    // ⭐ 进度条 seek
    e.bar.addEventListener('click', (ev) => {
      const rect = e.bar.getBoundingClientRect();
      const pct = (ev.clientX - rect.left) / rect.width;
      v.currentTime = pct * (v.duration || 0);
    });

    // ⭐ 状态镜像
    v.addEventListener('play',  () => { e.play.textContent = '⏸'; this.#host.removeAttribute('data-paused'); });
    v.addEventListener('pause', () => { e.play.textContent = '▶';  this.#host.setAttribute('data-paused', ''); });

    // ⭐ 关键：buffered 用 progress 事件（轻量）
    v.addEventListener('progress', () => this.#renderBuffered());
    v.addEventListener('loadedmetadata', () => this.#renderTime());

    // ⭐ played + currentTime + 弹幕 + 字幕 用 rVFC（高频且与帧同步）
    this.#startFrameLoop();
  }

  // ⭐ requestVideoFrameCallback——与视频帧同步的回调
  #startFrameLoop() {
    const v = this.#player.video;
    const tick = () => {
      this.#renderPlayed();
      this.#renderTime();

      // 弹幕：spawn + render（dt clamp 防止切后台后大跳）
      const dt = (performance.now() - (this.#lastTs ?? performance.now())) / 1000;
      this.#lastTs = performance.now();
      this.#engine.spawnFor(v.currentTime);
      this.#engine.render(Math.min(dt, 0.1));

      // 字幕
      this.#subs?.update(v.currentTime);

      if ('requestVideoFrameCallback' in v) {
        this.#rvfcId = v.requestVideoFrameCallback(tick);
      } else {
        this.#raf = requestAnimationFrame(tick);
      }
    };

    if ('requestVideoFrameCallback' in v) {
      this.#rvfcId = v.requestVideoFrameCallback(tick);
    } else {
      // 降级：rAF
      this.#raf = requestAnimationFrame(tick);
    }
  }

  #renderPlayed() {
    const v = this.#player.video;
    const pct = v.duration ? (v.currentTime / v.duration) * 100 : 0;
    this.#els.played.style.width = pct + '%';
    this.#els.thumb.style.left   = pct + '%';
  }

  #renderBuffered() {
    const v = this.#player.video;
    if (!v.duration || !v.buffered.length) return;
    // 拿"包含当前播放点"的那段 buffered
    let end = 0;
    for (let i = 0; i < v.buffered.length; i++) {
      if (v.buffered.start(i) <= v.currentTime && v.currentTime <= v.buffered.end(i)) {
        end = v.buffered.end(i); break;
      }
    }
    if (!end) end = v.buffered.end(v.buffered.length - 1);
    this.#els.buffered.style.width = (end / v.duration * 100) + '%';
  }

  #renderTime() {
    const v = this.#player.video;
    this.#els.time.textContent = `${fmt(v.currentTime)} / ${fmt(v.duration || 0)}`;
  }

  #syncCanvasSize() {
    const r = this.#host.getBoundingClientRect();
    this.#engine.resize(r.width, r.height);
  }

  #toggleFullscreen() {
    // 多前缀兼容（Safari iOS 还有更特殊的 webkitEnterFullscreen）
    const enter = this.#host.requestFullscreen ?? this.#host.webkitRequestFullscreen;
    const exit  = document.exitFullscreen ?? document.webkitExitFullscreen;
    const fsEl  = document.fullscreenElement ?? document.webkitFullscreenElement;
    if (fsEl) exit.call(document); else enter.call(this.#host);
  }

  destroy() {
    if (this.#raf) cancelAnimationFrame(this.#raf);
    if (this.#rvfcId && this.#player.video.cancelVideoFrameCallback) {
      this.#player.video.cancelVideoFrameCallback(this.#rvfcId);
    }
    this.#ro?.disconnect();
  }
}

function fmt(sec) {
  if (!isFinite(sec)) return '00:00';
  const m = Math.floor(sec / 60), s = Math.floor(sec % 60);
  return `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
}
