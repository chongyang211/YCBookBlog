export class SubtitleLayer {
  #host; #cues = []; #cur = null;
  constructor(hostEl) {
    this.#host = document.createElement('div');
    Object.assign(this.#host.style, {
      position: 'absolute', left: '0', right: '0', bottom: '60px',
      textAlign: 'center', color: '#fff', fontSize: '20px',
      textShadow: '0 0 4px #000', pointerEvents: 'none', whiteSpace: 'pre-line',
    });
    hostEl.appendChild(this.#host);
  }
  load(cues) { this.#cues = cues; }
  update(t) {
    const c = this.#cues.find(x => t >= x.start && t < x.end);
    if (c === this.#cur) return;
    this.#cur = c;
    this.#host.textContent = c?.text ?? '';
  }
}
