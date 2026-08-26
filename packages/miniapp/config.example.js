/**
 * 小程序配置模板（此文件会提交到仓库，供协作者参考）
 *
 * 使用步骤：
 *   1. 复制本文件为 config.js：  cp config.example.js config.js
 *   2. 在 config.js 中填写你的真实值
 *   3. config.js 已被 .gitignore 忽略，不会提交到仓库（保护隐私）
 */
module.exports = {
  // 微信小程序 AppID（微信公众平台 → 开发 → 开发设置）
  // ⚠️ 此值仅供记录参考；真正生效的是 project.config.json 里的 appid 字段，
  //    因为微信开发者工具只读取 project.config.json，无法通过 JS 配置驱动。
  appid: 'YOUR_APPID',

  // CloudBase 云环境 ID（微信开发者工具 → 云开发控制台 → 环境列表）
  cloudEnvId: 'your-env-id',

  // 站点域名（用于分享 / 原文链接，一般无需修改）
  siteUrl: 'https://yccoding.com',
};
