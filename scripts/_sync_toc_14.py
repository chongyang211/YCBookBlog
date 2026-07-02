import re
fp = 'docs/12.Apps/04.Linux应用开发/01.QML基础入门/14.性能优化与真机调试.md'
with open(fp) as f: content = f.read()
lines = content.split('\n')
headings = []
for line in lines:
    m = re.match(r'(#{2,3}) (\d+(?:\.\d+)*) (.+)$', line)
    if m:
        headings.append((len(m.group(1)), m.group(2), m.group(3).strip()))
def to_anchor(text):
    t = text.lower()
    t = re.sub(r'[^\w\s\u4e00-\u9fff\ue000-uffff-]', '', t)
    t = re.sub(r'\s+', '-', t)
    return t.strip('-')
toc_lines = ['#### 目录介绍']
for level, num, title in headings:
    anchor = to_anchor(f'{num} {title}')
    prefix = '- ' if level == 2 else '    - '
    toc_lines.append(f'{prefix}[{num} {title}](#{anchor})')
toc_start = None
for i, line in enumerate(lines):
    if line.strip().startswith('#### 目录介绍'):
        toc_start = i; break
toc_end = toc_start
for i in range(toc_start + 1, len(lines)):
    if lines[i].strip() == '---' or (lines[i].startswith('## ') and i > toc_start + 5):
        toc_end = i; break
new_lines = lines[:toc_start] + toc_lines + [''] + lines[toc_end:]
with open(fp, 'w') as f: f.write('\n'.join(new_lines))
print(f'TOC: {len(headings)} entries')
