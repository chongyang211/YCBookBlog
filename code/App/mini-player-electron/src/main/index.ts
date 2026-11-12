// src/main/index.ts —— Electron 主进程入口
//
// 职责：
//   1. 创建 BrowserWindow（安全配置：contextIsolation + nodeIntegration=false）
//   2. 加载 renderer（开发期从 Vite dev server，打包后从 file://）
//   3. 注册 IPC 通道（见 src/main/ipc/*.ts）

import { app, BrowserWindow, ipcMain, dialog, shell } from 'electron';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { getNative } from './native/bindings';
import { MpFormatName, MpFormat } from '@shared/types';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const isDev = !!process.env['ELECTRON_RENDERER_URL'];

function createWindow() {
  const win = new BrowserWindow({
    width: 1200,
    height: 800,
    minWidth: 900,
    minHeight: 600,
    show: false,
    autoHideMenuBar: true,
    backgroundColor: '#0f172a',
    titleBarStyle: process.platform === 'darwin' ? 'hiddenInset' : 'default',
    webPreferences: {
      // ⭐ 三条安全纪律
      contextIsolation: true,
      nodeIntegration:  false,
      sandbox:          true,
      preload:          path.join(__dirname, '../preload/index.cjs'),
    },
  });

  win.on('ready-to-show', () => win.show());

  // 让外部链接走系统浏览器而不是在应用内打开
  win.webContents.setWindowOpenHandler(({ url }) => {
    shell.openExternal(url);
    return { action: 'deny' };
  });

  if (isDev && process.env['ELECTRON_RENDERER_URL']) {
    win.loadURL(process.env['ELECTRON_RENDERER_URL']);
    win.webContents.openDevTools({ mode: 'detach' });
  } else {
    win.loadFile(path.join(__dirname, '../renderer/index.html'));
  }

  return win;
}

// ========== IPC 契约（对应 shared/types.ts 的 Api）==========

ipcMain.handle('app:getVersion', () => {
  let nativeVer = 'unloaded';
  try { nativeVer = getNative().version(); } catch (e: any) { nativeVer = `error: ${e.message}`; }
  return {
    app:      app.getVersion(),
    electron: process.versions.electron,
    node:     process.versions.node,
    native:   nativeVer,
  };
});

ipcMain.handle('native:probe', (_e, filePath: string) => {
  try {
    const code = getNative().probe(filePath);
    const key = (code in MpFormatName ? code : MpFormat.Unknown) as MpFormat;
    return { code, format: MpFormatName[key] };
  } catch (e: any) {
    return { code: 0, format: 'error', error: e.message };
  }
});

ipcMain.handle('native:readMetadata', (_e, filePath: string) => {
  try {
    const m = getNative().readMetadata(filePath);
    if (!m) {
      return { ok: false, reason: 'metadata_read failed（可能未启用 TagLib，见 README）' };
    }
    return {
      ok: true,
      title:        m.title  ?? path.basename(filePath),
      artist:       m.artist ?? undefined,
      album:        m.album  ?? undefined,
      durationMs:   m.duration_ms,
      bitrateKbps:  m.bitrate_kbps,
    };
  } catch (e: any) {
    return { ok: false, reason: e.message };
  }
});

ipcMain.handle('native:parseLrc', (_e, text: string) => {
  try {
    return getNative().parseLrc(text);
  } catch {
    return [];
  }
});

ipcMain.handle('dialog:pickFile', async () => {
  const res = await dialog.showOpenDialog({
    properties: ['openFile'],
    filters: [
      { name: 'Audio', extensions: ['mp3', 'flac', 'm4a', 'wav', 'ogg', 'aac'] },
      { name: 'Video', extensions: ['mp4', 'mkv', 'mov', 'webm'] },
      { name: 'All',   extensions: ['*'] },
    ],
  });
  return res.canceled ? null : res.filePaths[0];
});

// ========== 生命周期 ==========

app.whenReady().then(() => {
  createWindow();
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
