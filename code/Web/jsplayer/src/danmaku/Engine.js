const FONT_SIZE = 20;
const TRACK_HEIGHT = 28;
const SCROLL_DURATION = 8;   // 弹幕从右走到左 8 秒

class Bullet {
  text = ''; color = '#fff';
  x = 0; y = 0; w = 0;
  speed = 0;        // 像素/秒
  alive = false;

  reset(item, canvasW, ctx, trackY) {
    this.text  = item.text;
    this.color = item.color || '#fff';
    ctx.font   = `${FONT_SIZE}px system-ui`;
    this.w     = ctx.measureText(item.text).width;
    this.x     = canvasW;            // 从右边出场
    this.y     = trackY;
    this.speed = (canvasW + this.w) / SCROLL_DURATION;
    this.alive = true;
  }
}

export class DanmakuEngine {
  #canvas; #ctx;
  #pool = [];          // 对象池
  #active = [];        // 正在显示的 bullets
  #tracks;             // 每条 track 的"最右侧已占用 x"
  #lastRenderSec = -1;
  #index;

  constructor(canvas, index) {
    this.#canvas = canvas;
    this.#ctx    = canvas.getContext('2d');
    this.#index  = index;
    this.#initTracks();
    // 预分配 200 个对象
    for (let i = 0; i < 200; i++) this.#pool.push(new Bullet());
  }

  #initTracks() {
    const n = Math.floor(this.#canvas.height / TRACK_HEIGHT);
    this.#tracks = new Array(n).fill(0);
  }

  // ⭐ 核心：从池里取，没有就 new
  #acquire() {
    return this.#pool.pop() ?? new Bullet();
  }
  #release(b) {
    b.alive = false;
    this.#pool.push(b);
  }

  // ⭐ 找一条"右端不会撞车"的 track
  #pickTrack(bulletWidth) {
    const W = this.#canvas.width;
    for (let i = 0; i < this.#tracks.length; i++) {
      // 这条 track 上"最右弹幕的左端"已经离开屏幕右边一定距离
      if (this.#tracks[i] < W - 50) return i;
    }
    return -1;   // 全满，丢弃这条弹幕
  }

  // 视频时间推进 → 拉取新出现的弹幕
  spawnFor(currentTime) {
    const sec = Math.floor(currentTime);
    if (sec === this.#lastRenderSec) return;
    const items = this.#index.range(this.#lastRenderSec + 1, sec + 1);
    this.#lastRenderSec = sec;

    for (const item of items) {
      const tmpW = this.#ctx.measureText(item.text).width;
      const trackIdx = this.#pickTrack(tmpW);
      if (trackIdx < 0) continue;            // 满轨：丢弃
      const b = this.#acquire();
      b.reset(item, this.#canvas.width, this.#ctx, trackIdx * TRACK_HEIGHT + FONT_SIZE);
      this.#tracks[trackIdx] = this.#canvas.width + tmpW; // 占轨
      this.#active.push(b);
    }
  }

  // 每帧调一次：移动 + 绘制 + 回收
  render(dt) {
    const ctx = this.#ctx;
    ctx.clearRect(0, 0, this.#canvas.width, this.#canvas.height);
    ctx.font = `${FONT_SIZE}px system-ui`;
    ctx.textBaseline = 'alphabetic';

    for (let i = this.#active.length - 1; i >= 0; i--) {
      const b = this.#active[i];
      b.x -= b.speed * dt;
      // 出屏 → 回收
      if (b.x + b.w < 0) {
        this.#active.splice(i, 1);
        this.#release(b);
        continue;
      }
      ctx.fillStyle = b.color;
      ctx.fillText(b.text, b.x, b.y);
    }

    // 更新 track 占用（用 active 重新算）
    this.#tracks.fill(0);
    for (const b of this.#active) {
      const tIdx = Math.floor((b.y - FONT_SIZE) / TRACK_HEIGHT);
      this.#tracks[tIdx] = Math.max(this.#tracks[tIdx], b.x + b.w);
    }
  }

  resize(w, h) {
    this.#canvas.width = w; this.#canvas.height = h;
    this.#initTracks();
  }
}
