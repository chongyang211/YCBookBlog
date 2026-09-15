/**
 * upload-weapp.js — 用 miniprogram-ci 上传小程序代码（生成「开发版本」）
 *
 * 前置准备（只需做一次）：
 *   1. 安装依赖：  npm i -D miniprogram-ci
 *   2. 下载密钥：  微信公众平台 → 管理 → 开发管理 → 开发设置
 *                  →「小程序代码上传密钥」→ 下载后重命名为 private.key
 *                  → 放到本 scripts/ 目录下（务必加入 .gitignore）
 *   3. IP 白名单：同页面「IP 白名单」中加入本机公网 IP，否则上传被拒
 *
 * 用法：
 *   node scripts/upload-weapp.js <版本号> [备注]
 *   node scripts/upload-weapp.js 1.0.0 "首次发布：书架小程序"
 *
 * 说明：
 *   上传只生成「开发版本」，真正发布需到公众平台：版本管理 → 提交审核 → 发布。
 */

const path = require('path');
const { appid } = require('../config');

let ci;
try {
  ci = require('miniprogram-ci');
} catch (e) {
  console.error('❌ 未安装 miniprogram-ci，请先执行：npm i -D miniprogram-ci');
  process.exit(1);
}

const version = process.argv[2];
const desc = process.argv[3];

if (!version) {
  console.error('用法：node scripts/upload-weapp.js <版本号> [备注]');
  console.error('示例：node scripts/upload-weapp.js 1.0.0 "首次发布"');
  process.exit(1);
}

const keyPath = path.resolve(__dirname, 'private.key');

(async () => {
  const project = new ci.Project({
    appid,
    type: 'miniProgram',
    projectPath: path.resolve(__dirname, '..'),
    privateKeyPath: keyPath,
    // 关键：39MB 的章节 JSON 不进小程序包，正文全部走云端
    ignores: ['node_modules/**/*', 'static/chapters/**', 'scripts/**', 'README.md'],
  });

  const result = await ci.upload({
    project,
    version,
    desc: desc || `上传于 ${new Date().toLocaleString('zh-CN')}`,
    setting: {
      es6: true,
      es7: true,
      minified: true,
      minifyJS: true,
      minifyWXML: true,
      minifyWXSS: true,
      codeProtect: false,
      autoPrefixWXSS: true,
    },
    onProgressUpdate: (task) => {
      if (task && task._status === 'doing') {
        console.log(`⏳ ${task._msg || '上传中'} ...`);
      }
    },
  });

  console.log(`✅ 上传成功，开发版本 ${version} 已生成`);
  if (result && result.subPackageInfo) {
    console.log('   分包信息:', result.subPackageInfo);
  }
  console.log('\n👉 下一步：微信公众平台 → 版本管理 → 提交审核');
})().catch((err) => {
  const msg = err && err.message ? err.message : err;
  console.error('❌ 上传失败:', msg);
  if (/private key|ENOENT|no such file/i.test(String(msg))) {
    console.error('\n   请确认 private.key 已放到 scripts/ 目录：', keyPath);
  }
  if (/ip|white|白名单/i.test(String(msg))) {
    console.error('\n   请到公众平台「开发设置 → IP 白名单」加入本机公网 IP');
  }
  process.exit(1);
});
