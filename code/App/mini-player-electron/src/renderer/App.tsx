// src/renderer/App.tsx —— v0.1 MVP 页面
//
// 这个页面是**阶段① + 阶段② 的最小闭环**：
//   1. 启动时显示版本信息（证明 FFI 加载成功 → mp_version 可被调）
//   2. "选择文件"→ 通过 IPC 调 C++ 做文件头嗅探（mp_probe）→ 回显格式
//   3. 若文件被识别为音频，用 <audio> 直接播放
//
// 对应文档 §5 阶段① & 阶段②

import { useEffect, useState } from 'react';

interface VersionInfo { app: string; electron: string; node: string; native: string; }

export default function App() {
  const [ver, setVer] = useState<VersionInfo | null>(null);
  const [filePath, setFilePath] = useState<string | null>(null);
  const [probe, setProbe]       = useState<{ format: string; code: number } | null>(null);
  const [meta, setMeta]         = useState<any>(null);
  const [err, setErr]           = useState<string | null>(null);

  useEffect(() => {
    window.api.getVersion().then(setVer).catch((e) => setErr(e.message));
  }, []);

  const pickAndProbe = async () => {
    setErr(null); setProbe(null); setMeta(null);
    try {
      const p = await window.api.pickFile();
      if (!p) return;
      setFilePath(p);
      const r = await window.api.probeFile(p);
      setProbe(r);
      const m = await window.api.readMetadata(p);
      setMeta(m);
    } catch (e: any) {
      setErr(e.message);
    }
  };

  // 音频格式允许直接 <audio> 预览
  const isPlayable = probe && ['mp3','flac','m4a','wav','ogg'].includes(probe.format);
  const srcUrl     = filePath ? 'file://' + filePath : undefined;

  return (
    <div className="app">
      <header className="title-bar">
        <div>
          <span className="brand">MiniPlayer</span>
          <span className="tag">Electron · v0.1</span>
          <span className="tag">C++ 原生扩展</span>
        </div>
        <div style={{fontSize: 12, color: '#94a3b8'}}>
          Node.js + React 19 + koffi FFI
        </div>
      </header>

      {/* —— 1. 版本信息（证明 FFI 加载成功）—— */}
      <section className="panel">
        <h2>① 环境自检</h2>
        {ver ? (
          <>
            <div className="row"><span className="k">App</span><span className="v">v{ver.app}</span></div>
            <div className="row"><span className="k">Electron</span><span className="v">{ver.electron}</span></div>
            <div className="row"><span className="k">Node</span><span className="v">{ver.node}</span></div>
            <div className="row"><span className="k">C++ native</span><span className="v">{ver.native}</span></div>
            <p className="hint">
              若 <code>C++ native</code> 字段显示 <code>mp_native 0.1.0</code>，表示 <code>libmp_native.{'{so/dylib/dll}'}</code> 已成功通过 koffi FFI 加载。
              若显示 <code>error: ...</code>，请执行 <code>pnpm native:build</code> 构建 C++ 共享库。
            </p>
          </>
        ) : (
          <p>加载中…</p>
        )}
      </section>

      {/* —— 2. 文件探测（调 C++ mp_probe）—— */}
      <section className="panel">
        <h2>② 调 C++ 做文件头嗅探</h2>
        <div className="btn-row">
          <button onClick={pickAndProbe}>选择文件 → 调 mp_probe</button>
        </div>
        {filePath && (
          <div style={{marginTop: 16}}>
            <div className="row"><span className="k">文件路径</span><span className="v">{filePath}</span></div>
            {probe && (
              <>
                <div className="row"><span className="k">C++ 嗅探结果</span><span className="v">format = {probe.format} (code={probe.code})</span></div>
                {meta?.ok && (
                  <>
                    <div className="row"><span className="k">标题</span><span className="v">{meta.title}</span></div>
                    {meta.artist && <div className="row"><span className="k">艺术家</span><span className="v">{meta.artist}</span></div>}
                    {meta.durationMs > 0 && <div className="row"><span className="k">时长</span><span className="v">{(meta.durationMs/1000).toFixed(1)} s</span></div>}
                  </>
                )}
                {meta && !meta.ok && (
                  <div className="row">
                    <span className="k">元数据</span>
                    <span className="v">{meta.reason}</span>
                  </div>
                )}
              </>
            )}
          </div>
        )}
        <p className="hint">
          这一步**完全由 C++ 代码**（<code>native/src/probe.cpp</code>）执行——Node.js 把文件路径通过 koffi 传给 C 函数 <code>mp_probe()</code>，
          C++ 读文件前 16 字节 magic header，返回 <code>MpFormat</code> 枚举值。整个过程不经过 JS 的 fs，演示了"**C++ 作为服务提供者**"的架构。
        </p>
      </section>

      {/* —— 3. 原生 <audio> 播放（阶段① MVP）—— */}
      {isPlayable && srcUrl && (
        <section className="panel">
          <h2>③ Chromium 原生 &lt;audio&gt; 播放</h2>
          <audio src={srcUrl} controls autoPlay />
          <p className="hint">
            此处用的是 Electron 内嵌 Chromium 的原生 <code>&lt;audio&gt;</code> 元素——**播放引擎不下沉到 C++**（§1.3 的边界划分）。
            后续阶段③将用 Web Audio API 构建完整音频图（EQ、音量、A-B 循环、crossfade）。
          </p>
        </section>
      )}

      {err && (
        <section className="panel" style={{borderColor: 'rgba(239,68,68,0.4)'}}>
          <h2 style={{color: '#f87171'}}>错误</h2>
          <pre style={{fontSize: 12, color: '#fca5a5', whiteSpace: 'pre-wrap'}}>{err}</pre>
        </section>
      )}
    </div>
  );
}
