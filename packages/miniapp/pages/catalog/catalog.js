Page({
  data: {
    title: '',
    subCategories: [],   // 当前分类下的子目录列表
    articles: [],        // 当前子目录下的文章列表
    currentSub: '',      // 当前选中的子目录
    showSubs: true,      // 是否显示子目录列表（多个子目录时）
  },

  onLoad(options) {
    const { top, sub } = options;
    const app = getApp();
    const cat = app.categoryTree.find(c => c.name === decodeURIComponent(top));
    if (!cat) return;

    wx.setNavigationBarTitle({ title: decodeURIComponent(top) });

    if (sub) {
      // 直接跳转到子目录的文章列表
      const subName = decodeURIComponent(sub);
      const subCat = cat.children.find(c => c.name === subName);
      this.setData({
        title: subName,
        articles: subCat ? subCat.items : [],
        showSubs: false,
        currentSub: subName,
      });
    } else {
      // 显示子目录列表
      this.setData({
        title: decodeURIComponent(top),
        subCategories: cat.children,
        showSubs: true,
      });
    }
  },

  /** 点击子目录 → 展开文章 */
  onTapSub(e) {
    const { name } = e.currentTarget.dataset;
    const app = getApp();
    const cat = app.categoryTree.find(c =>
      c.children.some(sub => sub.name === name)
    );
    if (!cat) return;

    const subCat = cat.children.find(c => c.name === name);
    this.setData({
      articles: subCat ? subCat.items : [],
      currentSub: name,
      showSubs: false,
    });
    wx.setNavigationBarTitle({ title: name });
  },

  /** 返回子目录列表 */
  onBackToSubs() {
    this.setData({ showSubs: true, articles: [], currentSub: '' });
  },

  /** 点击文章 → 阅读页 */
  onTapArticle(e) {
    const { id, title } = e.currentTarget.dataset;
    wx.navigateTo({
      url: `/pages/reader/reader?id=${encodeURIComponent(id)}&title=${encodeURIComponent(title)}`,
    });
  },

  onShareAppMessage() {
    return {
      title: this.data.title,
      path: `/pages/catalog/catalog?top=${encodeURIComponent(this.data.title)}`,
    };
  },
});
