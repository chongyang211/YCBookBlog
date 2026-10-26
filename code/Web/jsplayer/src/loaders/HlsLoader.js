import { parseM3u8 } from './parseM3u8.js';
import { MseEngine } from '../core/MseEngine.js';

// 注：真实 ts 分片需要 mux.js 之类的库做 ts→fmp4 转封装
// 本案例为简化教学，直接用 fmp4 分段 + 自定义清单
export class HlsLoader extends EventTarget {
  #video;
  #abort = null;
  #mime;

  constructor(videoEl, mime = 'video/mp4; codecs="avc1.42E01E,mp4a.40.2"') {
    super();
    this.#video = videoEl;
    this.#mime  = mime;
  }

  cancel() { this.#abort?.abort('hls cancel'); }

  async load(playlistUrl) {
    this.cancel();
    this.#abort = new AbortController();
    const signal = this.#abort.signal;

    try {
      // 1) 拉清单
      const text = await fetch(playlistUrl, { signal }).then(r => {
        if (!r.ok) throw new Error('清单 HTTP ' + r.status);
        return r.text();
      });
      const playlist = parseM3u8(text, playlistUrl);
      this.dispatchEvent(new CustomEvent('manifest', { detail: playlist }));

      // 2) 打开 MSE
      const mse = new MseEngine(this.#video, this.#mime);
      await mse.open();

      // 3) 串行拉分片（带回压：buffered 超过 30s 就等）
      for (const seg of playlist.segments) {
        if (signal.aborted) return;
        await this.#waitBufferRoom(30);
        const buf = await fetch(seg.url, { signal }).then(r => r.arrayBuffer());
        mse.appendChunk(buf);
        this.dispatchEvent(new CustomEvent('segment', { detail: seg }));
      }
      mse.endOfStream();
      this.dispatchEvent(new CustomEvent('done'));
    } catch (err) {
      if (err.name === 'AbortError') return;
      this.dispatchEvent(new CustomEvent('hls-error', { detail: err }));
    }
  }

  // ⭐ 回压：让 buffered 末端最多领先当前播放时间 maxAhead 秒
  async #waitBufferRoom(maxAhead) {
    while (true) {
      const v = this.#video;
      if (!v.buffered.length) return;             // 还没开始播
      const end = v.buffered.end(v.buffered.length - 1);
      const ahead = end - v.currentTime;
      if (ahead < maxAhead) return;
      await new Promise(r => setTimeout(r, 500)); // 500ms 后再问
    }
  }
}
