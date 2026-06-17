import re, os

f = '05.中华历史故事/第五卷：盛世的密码/5.6 武则天称帝·打破偏见靠实力证明自己.md'
path = os.path.join('/Users/yc/YCBookBlog/book/09.出版作品的书籍', f)
with open(path, 'r') as file:
    content = file.read()

body = re.sub(r'^---.*?---\n', '', content, flags=re.DOTALL)
cn = len(re.findall(r'[\u4e00-\u9fff]', body))
total = len(re.sub(r'\s+', '', re.sub(r'[#*\-`>|]', '', body)))

print(f'=== {f.split("/")[-1]} ===')
print(f'  Chinese chars: {cn}')
print(f'  Total content chars (no markup): {total}')

paragraphs = body.split('\n\n')
long_paras = [(i, p.replace('\n','')[:30], len(p.replace('\n',''))) for i, p in enumerate(paragraphs) if len(p.replace('\n','')) > 300]
if long_paras:
    print(f'  Paragraphs >300 chars: {len(long_paras)}')
    for idx, preview, length in long_paras:
        print(f'    #{idx}: {length} chars - {preview}...')
else:
    print('  No paragraphs exceed 300 chars.')

sections = re.findall(r'^##\s+\d+\.', body, re.MULTILINE)
print(f'  Level-2 headings (##): {len(sections)}')

headings = re.findall(r'^##\s+\d+\.(.+)$', body, re.MULTILINE)
print(f'  Heading titles:')
for h in headings:
    h_clean = h.strip()
    cn_count = len(re.findall(r'[\u4e00-\u9fff]', h_clean))
    print(f'    [{cn_count} chars] {h_clean}')
