// src/shared/types.ts —— 五端共享的核心类型（对应 01.架构设计 §1.3）
//
// 这里的字段与 Qt 篇的 struct MediaItem、Web 篇的 interface MediaItem
// 一一对应。保持命名一致是跨端对照的前提。

export type MediaType = 'audio' | 'video';

export interface MediaItem {
  id: string;            // 稳定主键：SHA1(path+size+mtime)，用 string 规避 JS Number 精度
  type: MediaType;
  path: string;
  title: string;
  artist?: string;
  album?: string;
  durationMs: number;
  bitrateKbps?: number;
  coverPath?: string;
  rating: number;        // 0-5
  addedAt: number;
  lastPlayedAt?: number;
}

export type PlayState = 'idle' | 'preparing' | 'playing' | 'paused' | 'error';
export type PlayMode  = 'sequence' | 'repeat_one' | 'shuffle';
export type Aspect    = 'original' | '16:9' | '4:3' | 'fill' | 'fit';

export interface PlayerState {
  state: PlayState;
  currentMedia: MediaItem | null;
  positionMs: number;
  durationMs: number;
  speed: number;
  volume: number;
  muted: boolean;
  playMode: PlayMode;
  loopAB: { aMs: number; bMs: number } | null;
  isFullscreen: boolean;
  isPictureInPicture: boolean;
  aspectRatio: Aspect;
  rotationDeg: 0 | 90 | 180 | 270;
}

// native 层探测到的格式（对应 mp_native.h MpFormat enum）
export enum MpFormat {
  Unknown = 0,
  Mp3, Flac, M4a, Wav, Ogg,
  Mp4, Mkv, Mov, Webm,
}

export const MpFormatName: Record<MpFormat, string> = {
  [MpFormat.Unknown]: 'unknown',
  [MpFormat.Mp3]: 'mp3',
  [MpFormat.Flac]: 'flac',
  [MpFormat.M4a]: 'm4a',
  [MpFormat.Wav]: 'wav',
  [MpFormat.Ogg]: 'ogg',
  [MpFormat.Mp4]: 'mp4',
  [MpFormat.Mkv]: 'mkv',
  [MpFormat.Mov]: 'mov',
  [MpFormat.Webm]: 'webm',
};

// IPC 通道契约（main ↔ renderer）
export interface Api {
  getVersion(): Promise<{ app: string; electron: string; node: string; native: string }>;
  probeFile(path: string): Promise<{ format: string; code: number }>;
  readMetadata(path: string): Promise<{
    ok: boolean;
    reason?: string;
    title?: string;
    artist?: string;
    album?: string;
    durationMs?: number;
    bitrateKbps?: number;
  }>;
  parseLrc(text: string): Promise<Array<{ timeMs: number; text: string }>>;
  pickFile(): Promise<string | null>;
}

declare global {
  interface Window {
    api: Api;
  }
}
