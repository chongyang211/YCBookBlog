export function parse(text) {
  const doc = new DOMParser().parseFromString(text, 'text/xml');
  // ⭐⭐⭐ DOMParser 不会抛——错了把信息塞进文档
  const errNode = doc.querySelector('parsererror');
  if (errNode) throw new Error(errNode.textContent.slice(0, 200));

  const channelTitle = doc.querySelector('channel > title')?.textContent ?? '';
  const items = [...doc.querySelectorAll('item')].map((node) => ({
    title:   node.querySelector('title')?.textContent?.trim() ?? '',
    link:    node.querySelector('link')?.textContent?.trim() ?? '',
    pubDate: node.querySelector('pubDate')?.textContent?.trim() ?? '',
    desc:    node.querySelector('description')?.textContent?.trim() ?? '',
  }));
  return { title: channelTitle, items };
}
