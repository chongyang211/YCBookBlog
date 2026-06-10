import re

with open('5.8 丰子恺《缘缘堂随笔》看草木有情.md', 'r') as f:
    content = f.read()

body = content.split('---', 2)[-1] if content.startswith('---') else content
chinese_chars = len(re.findall(r'[\u4e00-\u9fff]', body))
print(f'Total Chinese chars in body: {chinese_chars}')

sections = re.split(r'^## \d+\..+$', body, flags=re.MULTILINE)
headings = re.findall(r'^## (\d+\..+)$', body, flags=re.MULTILINE)

for i, (h, sec) in enumerate(zip(headings, sections[1:]), 1):
    cn = len(re.findall(r'[\u4e00-\u9fff]', sec))
    h_title = re.sub(r'^\d+\.', '', h).strip()
    h_len = len(re.findall(r'[\u4e00-\u9fff]', h_title))
    # Check longest paragraph in section
    paras = [p.strip() for p in sec.split('\n\n') if p.strip() and not p.startswith('#')]
    max_para = max((len(re.findall(r'[\u4e00-\u9fff]', p)) for p in paras), default=0)
    print(f'  S{i}: {h_title} ({h_len}字) - {cn}中文, max段:{max_para}字')

print()
print(f'Target: 4500-5500, Actual: {chinese_chars}')
print(f'In range: {4500 <= chinese_chars <= 5500}')
