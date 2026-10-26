import { defineConfig } from 'vite';

export default defineConfig({
  // 最简配置：Worker 通过 new URL(..., import.meta.url) + type:'module' 使用，
  // Vite 5 会自动构建，无需额外配置
});
