/**
 * 云函数: getContent
 * 按 ID 从云存储读取章节 JSON 全文
 *
 * 部署: 在微信开发者工具中右键 cloudfunctions/getContent → 上传并部署
 */
const cloud = require('wx-server-sdk');
cloud.init({ env: cloud.DYNAMIC_CURRENT_ENV });

exports.main = async (event) => {
  const { id } = event;
  if (!id) return { error: '缺少 id 参数' };

  try {
    const res = await cloud.downloadFile({
      fileID: `chapters/${id}.json`,
    });

    if (!res.fileContent) {
      return { error: '文件不存在' };
    }

    return JSON.parse(res.fileContent.toString());
  } catch (err) {
    return { error: err.message };
  }
};
