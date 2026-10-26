export function parse(text) {
  const doc = new DOMParser().parseFromString(text, 'text/xml');
  if (doc.querySelector('parsererror')) throw new Error('atom parse error');

  const feedTitle = doc.querySelector('feed > title')?.textContent ?? '';
  const items = [...doc.querySelectorAll('entry')].map((node) => ({
    title:   node.querySelector('title')?.textContent?.trim() ?? '',
    link:    node.querySelector('link')?.getAttribute('href') ?? '',  // ⭐ atom 的 link 是属性
    pubDate: node.querySelector('updated, published')?.textContent ?? '',
    desc:    node.querySelector('summary, content')?.textContent?.trim() ?? '',
  }));
  return { title: feedTitle, items };
}
