import re

# 读取原文件
file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 读取新正文
with open('temp_new_body.txt', 'r', encoding='utf-8') as f:
    new_body = f.read()

# 找到正文开始和结束位置
# 正文从 '合欢花开到第五天' 开始
start_pattern = r'合欢花开到第五天'
start_match = re.search(start_pattern, content)
if not start_match:
    print("无法找到正文开始")
    exit(1)
start_pos = start_match.start()

# 找到正文结束：下一个 '---\n' 标记之前
# 查找从 start_pos 开始的第一个 '\n---\n'
end_pattern = re.compile(r'\n---\n')
end_matches = list(end_pattern.finditer(content, start_pos))
if not end_matches:
    print("无法找到正文结束标记")
    exit(1)
end_pos = end_matches[0].start()

# 构建新内容
new_content = content[:start_pos] + new_body + content[end_pos:]

# 写回文件
with open(file_path, 'w', encoding='utf-8') as f:
    f.write(new_content)

print("正文替换完成")

# 验证段落数
body = new_body
paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]
print(f'新段落数: {len(paragraphs)}')
# 统计中文字符
chinese_counts = [sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in paragraphs]
total_chinese = sum(chinese_counts)
print(f'总中文字符数: {total_chinese}')
if chinese_counts:
    print(f'最长段: {max(chinese_counts)} 字')
    sorted_counts = sorted(chinese_counts)
    mid = len(sorted_counts) // 2
    if len(sorted_counts) % 2 == 0:
        median = (sorted_counts[mid-1] + sorted_counts[mid]) / 2
    else:
        median = sorted_counts[mid]
    print(f'中位数: {median} 字')
    short = sum(1 for c in chinese_counts if c < 50)
    print(f'短段(<50字)数: {short}')
    over240 = sum(1 for c in chinese_counts if c > 240)
    print(f'超过240字段落数: {over240}')