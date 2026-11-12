// electron.vite.config.ts
// Electron 三进程构建配置：main / preload / renderer 各自独立入口与打包
import { defineConfig, externalizeDepsPlugin } from 'electron-vite';
import react from '@vitejs/plugin-react';
import { resolve } from 'node:path';

export default defineConfig({
  main: {
    plugins: [externalizeDepsPlugin()],
    resolve: {
      alias: { '@shared': resolve('src/shared') }
    },
    build: {
      rollupOptions: {
        input: resolve('src/main/index.ts')
      }
    }
  },
  preload: {
    plugins: [externalizeDepsPlugin()],
    build: {
      rollupOptions: {
        input: resolve('src/preload/index.ts'),
        output: {
          // sandbox: true 下 preload 只支持 CommonJS——
          // .cjs 强制 CJS，不受 package.json "type": "module" 影响
          format: 'cjs',
          entryFileNames: '[name].cjs',
          chunkFileNames: '[name]-[hash].cjs'
        }
      }
    }
  },
  renderer: {
    root: '.',
    resolve: {
      alias: {
        '@renderer': resolve('src/renderer'),
        '@shared': resolve('src/shared')
      }
    },
    plugins: [react()],
    build: {
      rollupOptions: {
        input: resolve('index.html')
      }
    },
    server: {
      port: 5173
    }
  }
});
