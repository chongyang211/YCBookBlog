import re
import os
import string

file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 找到正文开始和结束
# 正文从 '合欢花开到第五天' 开始，到 '四个永远无法愈合的洞。' 结束
start_pos = content.find('合欢花开到第五天')
if start_pos == -1:
    start_pos = 0

# 找到正文结束：字数统计之前的 '---'
# 查找 '---' 在 start_pos 之后的位置
pattern = re.compile(r'\n---\n')
matches = list(pattern.finditer(content, start_pos))
if matches:
    # 取第一个匹配（正文结束）
    end_pos = matches[0].start()
else:
    end_pos = len(content)

body = content[start_pos:end_pos].strip()

# 按空行分割段落
paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]
# 过滤掉可能的分段错误：有些段落可能因为换行被错误分割
# 合并那些可能被错误分割的短行
merged_paragraphs = []
current_para = ''
for p in paragraphs:
    # 如果段落很短且不以句号等结束，可能是被错误分割
    if len(p) < 80 and not p.endswith(('。', '！', '？', '”', '」', '》')) and not p.startswith(('“', '‘', '「', '《')):
        if current_para:
            current_para += ' ' + p
        else:
            current_para = p
    else:
        if current_para:
            merged_paragraphs.append(current_para)
            current_para = p
        else:
            merged_paragraphs.append(p)
if current_para:
    merged_paragraphs.append(current_para)

print(f'段落数: {len(merged_paragraphs)}')

# 统计每段字数（中文字符，不含标点空白）
punct = set(string.punctuation + '，。！？；：“”‘’「」《》【】（）')
word_counts = []
for i, p in enumerate(merged_paragraphs, 1):
    # 统计中文字符（Unicode中文字符范围）
    chinese_chars = [c for c in p if '\u4e00' <= c <= '\u9fff']
    count = len(chinese_chars)
    word_counts.append(count)

print(f'总中文字符数: {sum(word_counts)}')
print(f'最长段: {max(word_counts)} 字 (段落 {word_counts.index(max(word_counts))+1})')
print(f'最短段: {min(word_counts)} 字 (段落 {word_counts.index(min(word_counts))+1})')

# 中位数
sorted_counts = sorted(word_counts)
mid = len(sorted_counts) // 2
if len(sorted_counts) % 2 == 0:
    median = (sorted_counts[mid-1] + sorted_counts[mid]) / 2
else:
    median = sorted_counts[mid]
print(f'中位数: {median} 字')

# 短段（<50字）数量
short_paragraphs = [c for c in word_counts if c < 50]
print(f'短段 (<50字) 数量: {len(short_paragraphs)}')
if len(short_paragraphs) <= 10:
    print(f'短段段落号: {[i+1 for i, c in enumerate(word_counts) if c < 50]}')

# 检查是否超过240字
over_240 = [c for c in word_counts if c > 240]
print(f'超过240字的段落数: {len(over_240)}')
if over_240:
    print(f'超长段段落号: {[i+1 for i, c in enumerate(word_counts) if c > 240]}')

# 输出段落长度分布（前20段）
print('\n段落长度分布（前20段）:')
for i, count in enumerate(word_counts[:20], 1):
    print(f'{i:3d}: {count:4d} 字')

print(f'\n段落长度分布（后20段）:')
for i, count in enumerate(word_counts[-20:], len(word_counts)-19):
    print(f'{i:3d}: {count:4d} 字')