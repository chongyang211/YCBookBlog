/**
 * cloud.js — CloudBase 初始化和内容拉取
 *
 * 使用前需在 project.config.json 中填写真实 appid，
 * 并在 config.js 中填写云环境 ID（cloudEnvId）。
 */

const { cloudEnvId } = require('../config');

/** 初始化云环境 */
function initCloud() {
  if (!wx.cloud) {
    console.warn('当前微信版本不支持云开发');
    return;
  }
  wx.cloud.init({
    env: cloudEnvId,
    traceUser: true,
  });
}

/**
 * 从云存储拉取章节 JSON
 * 路径格式: cloud://env-id/chapters/13.书籍--02.小人物的进修--01.学习的七大原则.json
 */
async function fetchChapter(id) {
  try {
    const res = await wx.cloud.downloadFile({
      fileID: `cloud://${cloudEnvId}.chapters/${id}.json`,
    });
    if (res.statusCode !== 200) return null;

    const fs = wx.getFileSystemManager();
    const data = fs.readFileSync(res.tempFilePath, 'utf-8');
    return JSON.parse(data);
  } catch (err) {
    console.warn(`获取章节失败: ${id}`, err);
    // 降级：尝试从云函数获取
    return await fetchChapterViaFunction(id);
  }
}

/** 云函数备用方案 */
async function fetchChapterViaFunction(id) {
  try {
    const res = await wx.cloud.callFunction({
      name: 'getContent',
      data: { id },
    });
    return res.result;
  } catch (err) {
    console.error('云函数调用失败:', err);
    return null;
  }
}

/**
 * 全站搜索（客户端侧，从全局 catalog 中过滤）
 */
function search(query, catalog) {
  if (!query || !query.trim()) return [];
  const q = query.trim().toLowerCase();
  return catalog.filter(item => {
    const title = (item.title || '').toLowerCase();
    const summary = (item.summary || '').toLowerCase();
    const cats = (item.categories || []).join(' ').toLowerCase();
    return title.includes(q) || summary.includes(q) || cats.includes(q);
  }).slice(0, 50);
}

module.exports = {
  initCloud,
  fetchChapter,
  search,
};
