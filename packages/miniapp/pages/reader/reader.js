const { mdToHtml } = require('../../utils/markdown');

Page({
  data: {
    chapterId: '',
    title: '',
    body: '',
    htmlContent: '',
    loading: true,
    error: false,
    author: {},
    date: '',
    categories: [],
  },

  async onLoad(options) {
    const { id, title } = options;
    const decodedId = decodeURIComponent(id);
    const decodedTitle = decodeURIComponent(title || '');

    wx.setNavigationBarTitle({ title: decodedTitle || '阅读' });

    this.setData({
      chapterId: decodedId,
      title: decodedTitle,
      loading: true,
    });

    await this.loadChapter(decodedId);
  },

  async loadChapter(id) {
    try {
      const app = getApp();
      const chapter = await app.getChapterContent(id);

      if (!chapter) {
        this.setData({ loading: false, error: true });
        return;
      }

      const htmlContent = mdToHtml(chapter.body);

      // 构建分类标签
      const catTags = (chapter.categories || []).slice(0, 3);

      this.setData({
        body: chapter.body,
        htmlContent,
        loading: false,
        error: false,
        author: chapter.author || {},
        date: chapter.date ? chapter.date.slice(0, 10) : '',
        categories: catTags,
        title: chapter.title || this.data.title,
      });

      // 记录阅读历史
      app.addToHistory({
        id: chapter.id,
        title: chapter.title,
        summary: chapter.summary || '',
      });

      wx.setNavigationBarTitle({ title: chapter.title || '阅读' });
    } catch (err) {
      console.error('加载章节失败:', err);
      this.setData({ loading: false, error: true });
    }
  },

  /** 分享 */
  onShareAppMessage() {
    const url = `https://yccoding.com${this.data.permalink || ''}`;
    return {
      title: this.data.title,
      path: `/pages/reader/reader?id=${encodeURIComponent(this.data.chapterId)}&title=${encodeURIComponent(this.data.title)}`,
    };
  },

  /** 重试加载 */
  onRetry() {
    this.setData({ loading: true, error: false });
    this.loadChapter(this.data.chapterId);
  },

  /** 在浏览器打开（通过 web-view 跳转原文链接） */
  onOpenInBrowser() {
    const app = getApp();
    const item = app.catalog.find(c => c.id === this.data.chapterId);
    if (item && item.permalink) {
      wx.setClipboardData({
        data: `https://yccoding.com${item.permalink}`,
        success() {
          wx.showToast({ title: '链接已复制', icon: 'success' });
        },
      });
    }
  },
});
