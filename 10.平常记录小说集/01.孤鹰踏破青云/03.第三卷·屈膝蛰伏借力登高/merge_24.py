import re

with open('第24章·周旋各方权势间.md', 'r', encoding='utf-8') as f:
    content = f.read()

# 分割文件：保留头部（直到“**正文**”）和尾部（沉淀模板）
header_end = content.find('**正文**')
if header_end == -1:
    print("未找到正文标记")
    exit()
header = content[:header_end]
body_start = header_end
# 查找“**字数统计**”
word_count_pos = content.find('**字数统计**', body_start)
if word_count_pos == -1:
    # 如果没有，则查找“---”作为分隔符
    dash1 = content.find('---', body_start)
    if dash1 == -1:
        body = content[body_start:]
        footer = ''
    else:
        dash2 = content.find('---', dash1 + 3)
        if dash2 == -1:
            body = content[body_start:dash1]
            footer = content[dash1:]
        else:
            body = content[body_start:dash2]
            footer = content[dash2:]
else:
    # 找到“**字数统计**”之前的“---”
    dash_before = content.rfind('---', body_start, word_count_pos)
    if dash_before == -1:
        body = content[body_start:word_count_pos]
        footer = content[word_count_pos:]
    else:
        body = content[body_start:dash_before]
        footer = content[dash_before:]

print(f"正文长度: {len(body)} 字符")
print(f"页脚长度: {len(footer)} 字符")

# 按空行分割正文段落
raw_paragraphs = re.split(r'\n\s*\n', body)
paragraphs = [p.strip() for p in raw_paragraphs if p.strip()]
print(f"原始段落数: {len(paragraphs)}")

# 合并短段落（长度小于80字符）且不是段位标题（如“**① 场景开篇**”）
merged = []
i = 0
while i < len(paragraphs):
    p = paragraphs[i]
    # 检查是否是段位标题
    if re.match(r'^\*\*[①②③④⑤⑥]\s*[^*]+?\*\*', p):
        merged.append(p)
        i += 1
        continue
    # 如果段落短且不是最后一个，且下一个段落不是段位标题，则合并
    if len(p) < 80 and i + 1 < len(paragraphs) and not re.match(r'^\*\*[①②③④⑤⑥]\s*[^*]+?\*\*', paragraphs[i+1]):
        merged.append(p + ' ' + paragraphs[i+1])
        i += 2
    else:
        merged.append(p)
        i += 1

print(f"第一次合并后段落数: {len(merged)}")

# 再次合并，针对仍然很短的段落（长度小于60字符）
merged2 = []
i = 0
while i < len(merged):
    p = merged[i]
    if re.match(r'^\*\*[①②③④⑤⑥]\s*[^*]+?\*\*', p):
        merged2.append(p)
        i += 1
        continue
    if len(p) < 60 and i + 1 < len(merged) and not re.match(r'^\*\*[①②③④⑤⑥]\s*[^*]+?\*\*', merged[i+1]):
        merged2.append(p + ' ' + merged[i+1])
        i += 2
    else:
        merged2.append(p)
        i += 1

print(f"第二次合并后段落数: {len(merged2)}")

# 如果段落数仍然超过46，继续合并最短的相邻段落
while len(merged2) > 46:
    # 找到最短的相邻段落对（忽略段位标题）
    min_len = float('inf')
    merge_idx = -1
    for j in range(len(merged2)-1):
        if re.match(r'^\*\*[①②③④⑤⑥]\s*[^*]+?\*\*', merged2[j]) or re.match(r'^\*\*[①②③④⑤⑥]\s*[^*]+?\*\*', merged2[j+1]):
            continue
        total_len = len(merged2[j]) + len(merged2[j+1])
        if total_len < min_len:
            min_len = total_len
            merge_idx = j
    if merge_idx == -1:
        break
    merged2[merge_idx] = merged2[merge_idx] + ' ' + merged2[merge_idx+1]
    del merged2[merge_idx+1]
    print(f"合并后段落数: {len(merged2)}")

print(f"最终段落数: {len(merged2)}")

# 重新构建正文
new_body = '\n\n'.join(merged2)
# 重新组合文件
new_content = header + new_body + footer

# 写回文件
with open('第24章·周旋各方权势间.md', 'w', encoding='utf-8') as f:
    f.write(new_content)
print("文件已更新")