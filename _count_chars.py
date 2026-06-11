import re

with open('/Users/yc/YCBookBlog/book/09.出版作品的书籍/04.照见时代的人/第九卷：永恒的诗意/第9.3章《小桔灯》冰心.md', 'r') as f:
    content = f.read()

# Remove frontmatter
body = re.sub(r'^---.*?---\n', '', content, flags=re.DOTALL)

# Count total characters (excluding newlines)
total_chars = len(body.replace('\n', ''))
print(f'Total characters (no newlines): {total_chars}')

# Count by section
sections = body.split('\n## ')
main_title = sections[0].replace('\n', '')
print(f'  [Title]: {len(main_title)} chars')

total = len(main_title)
for s in sections[1:]:
    lines = s.split('\n')
    title = lines[0].strip()
    text = '\n'.join(lines[1:])
    count = len(text.replace('\n', ''))
    total += count
    print(f'  {title}: {count} chars')

print(f'\nSum total: {total} chars')

# Check paragraphs > 300 chars
paras = body.split('\n\n')
long_paras = []
for i, p in enumerate(paras):
    p = p.strip()
    if not p or p.startswith('#'):
        continue
    # Strip bold markers for accurate text length
    text = re.sub(r'\*\*', '', p).replace('\n', '')
    if len(text) > 300:
        first_30 = text[:30]
        long_paras.append(f'  Para #{i}: {len(text)} chars - "{first_30}..."')

if long_paras:
    print(f'\nParagraphs > 300 chars: {len(long_paras)}')
    for lp in long_paras:
        print(lp)
else:
    print('\nAll paragraphs <= 300 chars')

# Check heading lengths
print('\nHeading lengths:')
for s in sections[1:]:
    title = s.split('\n')[0].strip()
    # Extract Chinese part after number and dot
    chinese = title.split('.', 1)[1].strip() if '.' in title else title
    print(f'  {title}: {len(chinese)} Chinese chars')
