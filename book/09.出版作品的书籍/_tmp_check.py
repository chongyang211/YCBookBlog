import re, os

f = '05.中华历史故事/第六卷：变局中博弈/6.6 苏轼的一生·人生最大的成功是逆境中的自愈力.md'
path = os.path.join('/Users/yc/YCBookBlog/book/09.出版作品的书籍', f)
with open(path, 'r') as file:
    content = file.read()

body = re.sub(r'^---.*?---\n', '', content, flags=re.DOTALL)
total = len(re.sub(r'\s+', '', re.sub(r'[#*\-`>|]', '', body)))
print(f'Total content chars: {total}')

paragraphs = body.split('\n\n')
long_paras = [(i, len(p.replace('\n',''))) for i, p in enumerate(paragraphs) if len(p.replace('\n','')) > 300]
print(f'Paragraphs >300: {len(long_paras)}')
for idx, length in long_paras[:8]:
    print(f'  #{idx}: {length} chars')

# Check sections  
all_heads = re.findall(r'^(#+)\s+', body, re.MULTILINE)
print(f'All headings: {len(all_heads)}')
# Find level-2 with ##
l2 = re.findall(r'^##\s+\d+\.', body, re.MULTILINE)
print(f'Formatted ## subsections: {len(l2)}')
# Find unformatted headings
unformatted = re.findall(r'^##\s+[^#]', body, re.MULTILINE)
print(f'Unformatted ## headings: {len(unformatted)}')
