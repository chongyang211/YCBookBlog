import re

file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 找到正文开始和结束
start_pos = content.find('合欢花开到第五天')
if start_pos == -1:
    start_pos = 0

# 找到正文结束：字数统计之前的 '---'
pattern = re.compile(r'\n---\n')
matches = list(pattern.finditer(content, start_pos))
if matches:
    end_pos = matches[0].start()
else:
    end_pos = len(content)

body = content[start_pos:end_pos].strip()

# 按空行分割段落
paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]
print(f'原始段落数: {len(paragraphs)}')

# 统计中文字符数
def count_chinese(text):
    return len([c for c in text if '\u4e00' <= c <= '\u9fff'])

# 处理每个段落：如果超过240字，尝试拆分
new_paragraphs = []
total_chars = 0
for i, para in enumerate(paragraphs, 1):
    char_count = count_chinese(para)
    total_chars += char_count
    print(f'段落 {i}: {char_count} 字')
    
    if char_count > 240:
        # 尝试按句子拆分
        sentences = re.split(r'([。！？；])', para)
        # 重新组合句子，保留标点
        sentences_with_punct = []
        for j in range(0, len(sentences)-1, 2):
            if j+1 < len(sentences):
                sentences_with_punct.append(sentences[j] + sentences[j+1])
            else:
                sentences_with_punct.append(sentences[j])
        
        # 尝试将句子分组，使每组100-180字
        current_group = []
        current_count = 0
        for sent in sentences_with_punct:
            sent_count = count_chinese(sent)
            if current_count + sent_count > 180 and current_group:
                # 开始新组
                new_paragraphs.append(''.join(current_group))
                current_group = [sent]
                current_count = sent_count
            else:
                current_group.append(sent)
                current_count += sent_count
        
        if current_group:
            new_paragraphs.append(''.join(current_group))
    else:
        new_paragraphs.append(para)

print(f'\n拆分后段落数: {len(new_paragraphs)}')

# 重新统计
new_counts = [count_chinese(p) for p in new_paragraphs]
new_total = sum(new_counts)
print(f'原总字数: {total_chars}')
print(f'新总字数: {new_total}')
print(f'最长段: {max(new_counts)} 字')
print(f'最短段: {min(new_counts)} 字')

# 中位数
sorted_counts = sorted(new_counts)
mid = len(sorted_counts) // 2
if len(sorted_counts) % 2 == 0:
    median = (sorted_counts[mid-1] + sorted_counts[mid]) / 2
else:
    median = sorted_counts[mid]
print(f'中位数: {median} 字')

# 短段（<50字）数量
short_count = len([c for c in new_counts if c < 50])
print(f'短段 (<50字) 数量: {short_count}')

# 超过240字的段落
over_240 = len([c for c in new_counts if c > 240])
print(f'超过240字的段落数: {over_240}')

# 输出新段落长度分布
print('\n新段落长度分布:')
for i, count in enumerate(new_counts, 1):
    print(f'{i:3d}: {count:4d} 字')

# 生成新正文
new_body = '\n\n'.join(new_paragraphs)

# 输出到文件以便检查
output_path = 'temp_new_body.txt'
with open(output_path, 'w', encoding='utf-8') as f:
    f.write(new_body)
print(f'\n新正文已写入: {output_path}')