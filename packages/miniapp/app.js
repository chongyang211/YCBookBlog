/**
 * 杨充的书架 - 小程序入口
 * 初始化 CloudBase，加载目录索引
 */

const catalog = require('./static/catalog.json');
const { initCloud, fetchChapter } = require('./utils/cloud');

App({
  /** 全局共享的目录树 */
  catalog: catalog,

  /** 由 catalog.json 构建的分类树 */
  categoryTree: [],

  onLaunch() {
    // 初始化 CloudBase 云开发
    initCloud();

    // 构建分类树
    this.categoryTree = this.buildCategoryTree(catalog);

    // 预加载用户阅读历史
    this.history = this.loadHistory();
  },

  /**
   * 将平铺的 catalog 数组构建成分层分类树
   * [{ top: '书籍', children: [{ sub: '小人物的进修', items: [...] }] }]
   */
  buildCategoryTree(items) {
    const root = new Map();

    for (const item of items) {
      const cats = item.categories || [];
      if (cats.length === 0) continue;

      const top = cats[0];
      const sub = cats[1] || '其他';

      if (!root.has(top)) {
        root.set(top, new Map());
      }
      const subMap = root.get(top);
      if (!subMap.has(sub)) {
        subMap.set(sub, []);
      }
      subMap.get(sub).push(item);
    }

    // 转为数组结构，按文章数量排序
    const tree = [];
    for (const [top, subMap] of root) {
      const children = [];
      for (const [sub, items] of subMap) {
        children.push({
          name: sub,
          count: items.length,
          items: items.sort((a, b) => (a.date > b.date ? -1 : 1)),
        });
      }
      children.sort((a, b) => b.count - a.count);
      let total = 0;
      children.forEach(c => total += c.count);
      tree.push({ name: top, count: total, children });
    }
    tree.sort((a, b) => b.count - a.count);
    return tree;
  },

  /**
   * 获取章节全文（云端 + 本地缓存）
   */
  async getChapterContent(id) {
    // 先查缓存
    const cached = wx.getStorageSync(`ch_${id}`);
    if (cached) return cached;

    // 从云端拉取
    const content = await fetchChapter(id);
    if (content) {
      wx.setStorageSync(`ch_${id}`, content);
    }
    return content;
  },

  /** 阅读历史（最近 50 条） */
  loadHistory() {
    try {
      return wx.getStorageSync('read_history') || [];
    } catch (e) {
      return [];
    }
  },

  addToHistory(item) {
    const h = [item, ...this.history.filter(i => i.id !== item.id)].slice(0, 50);
    this.history = h;
    wx.setStorageSync('read_history', h);
  },
});
