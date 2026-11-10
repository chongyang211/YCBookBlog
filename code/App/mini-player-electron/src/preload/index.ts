// src/preload/index.ts —— 预加载脚本
//
// 职责：通过 contextBridge 把白名单 API 透出到 renderer 的 window.api
// 对应文档的"三条进程纪律" §1.4：renderer 永远不直接访问 Node API

import { contextBridge, ipcRenderer } from 'electron';

const api = {
  getVersion:   ()                 => ipcRenderer.invoke('app:getVersion'),
  probeFile:    (p: string)        => ipcRenderer.invoke('native:probe', p),
  readMetadata: (p: string)        => ipcRenderer.invoke('native:readMetadata', p),
  parseLrc:     (text: string)     => ipcRenderer.invoke('native:parseLrc', text),
  pickFile:     ()                 => ipcRenderer.invoke('dialog:pickFile'),
};

try {
  contextBridge.exposeInMainWorld('api', api);
} catch (e) {
  // sandbox 关闭时直接挂到 window（兼容老 Electron）
  // @ts-ignore
  window.api = api;
}
