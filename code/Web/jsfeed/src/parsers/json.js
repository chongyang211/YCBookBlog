export function parse(text) {
  const data = JSON.parse(text);
  return {
    title: data.title ?? '',
    items: (data.items ?? []).map((it) => ({
      title:   it.title ?? '',
      link:    it.url ?? it.external_url ?? '',
      pubDate: it.date_published ?? '',
      desc:    it.summary ?? it.content_text ?? '',
    })),
  };
}
