const { search } = require('../../utils/cloud');

Page({
  data: {
    categoryTree: [],
    searchResults: [],
    searchQuery: '',
    history: [],
    showSearch: false,
  },

  onLoad() {
    const app = getApp();
    this.setData({
      categoryTree: app.categoryTree,
      history: app.history,
    });
  },

  onShow() {
    const app = getApp();
    this.setData({ history: app.history });
  },

  /** 搜索输入 */
  onSearchInput(e) {
    const q = e.detail.value;
    this.setData({ searchQuery: q });

    if (!q.trim()) {
      this.setData({ searchResults: [], showSearch: false });
      return;
    }

    const results = search(q, getApp().catalog);
    this.setData({
      searchResults: results,
      showSearch: true,
    });
  },

  /** 点击某篇文章 */
  onTapArticle(e) {
    const { id, title } = e.currentTarget.dataset;
    wx.navigateTo({
      url: `/pages/reader/reader?id=${encodeURIComponent(id)}&title=${encodeURIComponent(title)}`,
    });
  },

  /** 点击分类（展开子分类） */
  onTapCategory(e) {
    const { name } = e.currentTarget.dataset;
    const app = getApp();
    const cat = app.categoryTree.find(c => c.name === name);
    if (!cat) return;

    // 如果只有一个子分类且子分类下有文章，直接跳转
    if (cat.children.length === 1) {
      const sub = cat.children[0];
      wx.navigateTo({
        url: `/pages/catalog/catalog?top=${encodeURIComponent(cat.name)}&sub=${encodeURIComponent(sub.name)}`,
      });
      return;
    }

    // 多个子分类：展开
    wx.navigateTo({
      url: `/pages/catalog/catalog?top=${encodeURIComponent(cat.name)}`,
    });
  },

  /** 从搜索中选择 */
  onSelectResult(e) {
    const { id, title } = e.currentTarget.dataset;
    wx.navigateTo({
      url: `/pages/reader/reader?id=${encodeURIComponent(id)}&title=${encodeURIComponent(title)}`,
    });
  },

  /** 清除搜索 */
  onClearSearch() {
    this.setData({
      searchQuery: '',
      searchResults: [],
      showSearch: false,
    });
  },

  /** 下拉刷新 */
  onPullDownRefresh() {
    this.setData({
      history: getApp().history,
    });
    wx.stopPullDownRefresh();
  },
});
