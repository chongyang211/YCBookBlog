// ⭐ 显式状态枚举（虽然 JS 没有 enum，但用 const 对象语义等价）
export const PlayerState = Object.freeze({
  IDLE:    'idle',     // 还没 load
  LOADING: 'loading',  // 元数据加载中
  READY:   'ready',    // 可以播但还没播
  PLAYING: 'playing',
  PAUSED:  'paused',
  ENDED:   'ended',
  ERROR:   'error',
});

// ⭐ 合法状态转移表（状态机的灵魂）
const TRANSITIONS = {
  idle:    ['loading', 'error'],
  loading: ['ready', 'error'],
  ready:   ['playing', 'error', 'idle'],
  playing: ['paused', 'ended', 'error', 'idle'],
  paused:  ['playing', 'ended', 'error', 'idle'],
  ended:   ['playing', 'idle'],
  error:   ['idle'],
};

const DEFAULT_MIME = 'video/mp4; codecs="avc1.42E01E,mp4a.40.2"';

import { MseEngine } from './MseEngine.js';
import { HlsLoader } from '../loaders/HlsLoader.js';

export class Player extends EventTarget {
  // 私有字段
  #video;
  #state = PlayerState.IDLE;
  #lastError = null;
  #mse = null;
  #abort = null;     // AbortController，切清晰度 / 切源时用
  #hls = null;

  constructor(videoEl) {
    super();
    if (!(videoEl instanceof HTMLVideoElement)) {
      throw new TypeError('Player 需要一个 <video> 元素');
    }
    this.#video = videoEl;
    this.#bindMediaEvents();
  }

  get state()     { return this.#state; }
  get video()     { return this.#video; }
  get lastError() { return this.#lastError; }

  // ⭐ 状态转移——所有内部状态变更必须走这里
  #setState(next, payload = null) {
    const cur = this.#state;
    if (cur === next) return;
    const allowed = TRANSITIONS[cur] ?? [];
    if (!allowed.includes(next)) {
      console.warn(`[Player] 非法转移 ${cur} → ${next}`);
      return;
    }
    this.#state = next;
    this.dispatchEvent(new CustomEvent('statechange',
      { detail: { from: cur, to: next, payload } }));
  }

  // ⭐ 把 <video> 的 24 个原生事件折叠成"业务有意义"的状态转移
  #bindMediaEvents() {
    const v = this.#video;
    v.addEventListener('loadedmetadata', () => this.#setState(PlayerState.READY));
    v.addEventListener('playing', () => this.#setState(PlayerState.PLAYING));
    v.addEventListener('pause',   () => {
      if (this.#state === PlayerState.PLAYING) this.#setState(PlayerState.PAUSED);
    });
    v.addEventListener('ended',   () => this.#setState(PlayerState.ENDED));
    v.addEventListener('error',   () => {
      this.#lastError = v.error;
      this.#setState(PlayerState.ERROR, v.error);
    });
  }

  #createMse(mime) {
    this.#mse?.destroy();
    this.#mse = new MseEngine(this.#video, mime);
    this.#mse.addEventListener('mse-error', e => {
      this.#lastError = e.detail;
      this.#setState(PlayerState.ERROR, e.detail);
    });
    return this.#mse;
  }

  // ── 公开 API（方法名故意和 <video> 一致——降低使用心智成本）──

  async load(src) {
    this.#setState(PlayerState.LOADING);
    this.#video.src = src;
    this.#video.load();
  }

  // ⭐ MSE 流式加载：fetch + ReadableStream 边下边喂
  async loadMse(url, mime = DEFAULT_MIME) {
    this.#setState(PlayerState.LOADING);
    // 取消上一次加载（如果有）
    this.#abort?.abort('switch source');
    this.#abort = new AbortController();

    try {
      // 1) 打开 MSE
      const mse = this.#createMse(mime);
      await mse.open();

      // 2) 流式拉取
      const resp = await fetch(url, { signal: this.#abort.signal });
      if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
      const reader = resp.body.getReader();   // ⭐ ReadableStream 流式 reader
      while (true) {
        const { done, value } = await reader.read();
        if (done) break;
        // value 是 Uint8Array → 直接喂 MSE
        mse.appendChunk(value.buffer);
      }
      mse.endOfStream();
    } catch (err) {
      if (err.name === 'AbortError') return;   // 主动取消，不算错
      console.error('[Player] loadMse 失败', err);
      this.#lastError = err;
      this.#setState(PlayerState.ERROR, err);
    }
  }

  // ⭐ MSE 多段串行加载（fmp4 分段拼接）
  async loadMseSegments(urls, mime = DEFAULT_MIME) {
    this.#setState(PlayerState.LOADING);
    this.#abort?.abort('switch');
    this.#abort = new AbortController();

    try {
      const mse = this.#createMse(mime);
      await mse.open();

      for (const url of urls) {
        const resp = await fetch(url, { signal: this.#abort.signal });
        if (!resp.ok) throw new Error(`HTTP ${resp.status} @ ${url}`);
        const buf = await resp.arrayBuffer();
        mse.appendChunk(buf);
        // 等 MSE 把这段消化完再拉下一段（简化版回压）
        await new Promise(res => queueMicrotask(res));
      }
      mse.endOfStream();
    } catch (err) {
      if (err.name !== 'AbortError') {
        this.#lastError = err;
        this.#setState(PlayerState.ERROR, err);
      }
    }
  }

  // ⭐ 统一入口：错误降级链（原生 HLS → MSE+HlsLoader → 原生 src）
  async loadAuto(url) {
    this.#setState(PlayerState.LOADING);

    // 1) m3u8 + 浏览器原生支持（iOS Safari）→ 直接走原生
    const isHls = url.includes('.m3u8');
    if (isHls && this.#video.canPlayType('application/vnd.apple.mpegurl')) {
      console.log('[Player] iOS 原生 HLS 路径');
      this.#video.src = url;
      return;
    }

    // 2) m3u8 + MSE → 走自定义 HlsLoader
    if (isHls && 'MediaSource' in window) {
      console.log('[Player] MSE + 自定义 HLS 路径');
      this.#hls?.cancel();
      this.#hls = new HlsLoader(this.#video);
      this.#hls.addEventListener('hls-error', e => {
        this.#lastError = e.detail;
        this.#setState(PlayerState.ERROR, e.detail);
      });
      return this.#hls.load(url);
    }

    // 3) 兜底——普通 mp4 / webm
    console.log('[Player] 原生 src 路径');
    this.#video.src = url;
  }

  async play()  { return this.#video.play(); }
  pause()       { this.#video.pause(); }
  seek(t)       { this.#video.currentTime = t; }

  destroy() {
    this.#hls?.cancel();
    this.#mse?.destroy();
    this.#abort?.abort('destroy');
    this.#video.removeAttribute('src');
    this.#video.load();
    this.#setState(PlayerState.IDLE);
  }
}
