import re

fp = 'docs/12.Apps/04.Linux应用开发/01.QML基础入门/01.嵌入式GUI技术全景.md'
with open(fp) as f:
    content = f.read()

lines = content.split('\n')
headings = []
for line in lines:
    m = re.match(r'(#{2,3})\s+(\d+(?:\.\d+)*)\s+(.+)', line)
    if m:
        level = len(m.group(1))
        num = m.group(2)
        title = m.group(3).strip()
        headings.append((level, num, title))

def to_anchor(text):
    text = text.lower()
    text = re.sub(r'[^\w\s\u4e00-\u9fff-]', '', text)
    text = re.sub(r'\s+', '-', text)
    return text.strip('-')

toc = ['#### 目录介绍']
for level, num, title in headings:
    anchor = to_anchor(f'{num}-{title}')
    indent = '' if level == 2 else '  '
    prefix = '-' if level == 2 else '  -'
    toc.append(f'{prefix} [{num} {title}](#{anchor})')

toc_start = None
for i, line in enumerate(lines):
    if line.startswith('#### 目录介绍'):
        toc_start = i
        break

if toc_start is not None:
    toc_end = toc_start
    for i in range(toc_start + 1, len(lines)):
        if (lines[i].strip() == '' and i > toc_start + 10 and
            i + 1 < len(lines) and
            (lines[i+1].strip() == '' or lines[i+1].startswith('##'))):
            toc_end = i
            break
    new_lines = lines[:toc_start] + toc + [''] + lines[toc_end+1:]
    with open(fp, 'w') as f:
        f.write('\n'.join(new_lines))
    print(f'TOC updated: {len(headings)} entries with anchors')
