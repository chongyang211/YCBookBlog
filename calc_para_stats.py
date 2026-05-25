import re

file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 找到正文开始
start = content.find('合欢花开到第五天')
if start == -1:
    start = 0

# 找到正文结束：字数统计之前的 '---'
pattern = re.compile(r'\n---\n')
matches = list(pattern.finditer(content, start))
if matches:
    end = matches[0].start()
else:
    end = len(content)

body = content[start:end].strip()
paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]

# 计算每段字数
counts = [sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in paragraphs]

print(f'段落数: {len(paragraphs)}')
print(f'总字数: {sum(counts)}')
print(f'最长段: {max(counts)} 字 (段号: {counts.index(max(counts)) + 1})')
print(f'最短段: {min(counts)} 字 (段号: {counts.index(min(counts)) + 1})')

# 中位数
sorted_counts = sorted(counts)
mid = len(sorted_counts) // 2
if len(sorted_counts) % 2 == 0:
    median = (sorted_counts[mid-1] + sorted_counts[mid]) / 2
else:
    median = sorted_counts[mid]
print(f'中位数: {median} 字')

# 短段 (<50字) 数量
short = sum(1 for c in counts if c < 50)
short_indices = [i+1 for i, c in enumerate(counts) if c < 50]
print(f'短段 (<50字) 数量: {short}')
print(f'短段段号: {short_indices}')

# 超过240字段落数
over240 = sum(1 for c in counts if c > 240)
print(f'超过240字段落数: {over240}')

# 输出用于替换的字符串
print('\n--- 自检表更新建议 ---')
print(f'□ 单段最长 ≤240 字：最长段={max(counts)} 字（段号：{counts.index(max(counts)) + 1}）')
if max(counts) <= 240:
    print('✅')
else:
    print('❌')

print(f'□ 短段（<50 字）总数 ≤4：短段数={short} 段（列出段号：{short_indices}）')
if short <= 4:
    print('✅')
else:
    print('❌')