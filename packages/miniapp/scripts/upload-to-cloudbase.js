/**
 * upload-to-cloudbase.js
 *
 * 把 miniapp/static/chapters/ 下的 JSON 文件批量上传到 CloudBase 云存储。
 *
 * 前提:
 *   1. 安装 @cloudbase/cli:  npm i -g @cloudbase/cli
 *   2. 登录:                 tcb login
 *   3. 确认环境:             tcb env list
 *
 * 用法:
 *   node miniapp/scripts/upload-to-cloudbase.js
 *
 * 它会在 chapters/ 前缀下上传所有 JSON，同名文件将自动覆盖。
 *
 * 另外也可以使用微信开发者工具的"云开发控制台 → 存储管理"手动上传。
 */

const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

const { cloudEnvId } = require('../config');

const CHAPTERS_DIR = path.resolve(__dirname, '../static/chapters');
const CLOUD_PREFIX = 'chapters';

const ENV_ID = cloudEnvId;

const files = fs.readdirSync(CHAPTERS_DIR).filter(f => f.endsWith('.json'));

console.log(`📤 准备上传 ${files.length} 个文件到 cloud://${ENV_ID}/${CLOUD_PREFIX}/`);

let uploaded = 0;
let failed = 0;
const batchSize = 20;

async function upload() {
  for (let i = 0; i < files.length; i += batchSize) {
    const batch = files.slice(i, i + batchSize);

    await Promise.all(
      batch.map(async (file) => {
        const localPath = path.join(CHAPTERS_DIR, file);
        const cloudPath = `${CLOUD_PREFIX}/${file}`;

        try {
          execSync(
            `tcb storage upload "${localPath}" --cloudPath "${cloudPath}" --env ${ENV_ID}`,
            { stdio: 'pipe' }
          );
          uploaded++;
        } catch (err) {
          // tcb 命令可能在 PATH 里找不到，提示用户手动操作
          if (err.message && err.message.includes('not found')) {
            console.error('\n⚠ 未找到 tcb CLI，请先安装: npm i -g @cloudbase/cli');
            console.error('  然后登录: tcb login');
            console.error('\n  或者使用微信开发者工具的云开发控制台手动上传。');
            process.exit(1);
          }
          failed++;
          console.error(`  ❌ ${file}`);
        }
      })
    );

    console.log(`  进度: ${Math.min(i + batchSize, files.length)}/${files.length} (成功 ${uploaded}, 失败 ${failed})`);
  }

  console.log(`\n✅ 上传完成！成功 ${uploaded} 个，失败 ${failed} 个`);
  if (uploaded > 0) {
    console.log(`   云路径示例: cloud://${ENV_ID}/${CLOUD_PREFIX}/${files[0]}`);
  }
}

upload();

// 提示备选方案（微信开发者工具上传）
console.log(`
💡 提示: 如果 tcb CLI 用不了，可以在微信开发者工具中:
   1. 打开"云开发"控制台
   2. 进入"存储" → 新建文件夹 "chapters"
   3. 批量上传 miniapp/static/chapters/ 下的全部 JSON 文件
`);
