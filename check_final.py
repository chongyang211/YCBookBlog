import re

with open('/Users/yc/YCBookBlog/ch19_body_split.md', 'r', encoding='utf-8') as f:
    content = f.read()

paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
print(f'段落总数: {len(paragraphs)}')

def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

paragraph_lengths = [count_chinese_chars(p) for p in paragraphs]
total_chars = sum(paragraph_lengths)
print(f'中文字符总数: {total_chars}')

max_len = max(paragraph_lengths)
max_idx = paragraph_lengths.index(max_len) + 1
print(f'单段最长: {max_len}字 (段号: {max_idx})')

sorted_lengths = sorted(paragraph_lengths)
mid = len(sorted_lengths) // 2
if len(sorted_lengths) % 2 == 0:
    median = (sorted_lengths[mid-1] + sorted_lengths[mid]) / 2
else:
    median = sorted_lengths[mid]
print(f'单段中位数: {median}字')

short_count = sum(1 for l in paragraph_lengths if l < 50)
print(f'短段（<50字）总数: {short_count}')

dialogue_paragraphs = [i+1 for i, p in enumerate(paragraphs) if '“' in p or '”' in p or '"' in p]
dialogue_count = len(dialogue_paragraphs)
print(f'对白段数: {dialogue_count}')

action_verbs = ['走', '站', '坐', '跪', '拿', '放', '擦', '烧', '握', '看', '盯', '转身', '离开', '拉', '推', '掏', '拨', '数', '汇', '寄', '跪', '起', '掀', '盖', '擦', '吹', '灭']
action_paragraphs = [i+1 for i, p in enumerate(paragraphs) if any(verb in p for verb in action_verbs)]
action_count = len(action_paragraphs)
print(f'动作镜头段数: {action_count}')

lamp_count = len(re.findall(r'灯', content))
print(f'主导意象“灯”出现次数: {lamp_count}')

symbols = {
    '弹孔补丁': len(re.findall(r'弹孔补丁', content)),
    '旧军号': len(re.findall(r'军号', content)),
    '欠条': len(re.findall(r'欠条', content))
}
print('统一符号出现次数:', symbols)

print('\n--- 自检结果 ---')
print(f'字数符合区间（4500-5500）: {4500 <= total_chars <= 5500}')
print(f'段落数符合区间（30-44）: {30 <= len(paragraphs) <= 44}')
print(f'单段最长 ≤240 字: {max_len <= 240}')
print(f'单段中位数 100-140 字: {100 <= median <= 140}')
print(f'短段总数 ≤4: {short_count <= 4}')
print(f'对白段 8-20: {8 <= dialogue_count <= 20}')
print(f'动作镜头段 ≥10: {action_count >= 10}')
print(f'主导意象出现 ≥5 次: {lamp_count >= 5}')
print(f'统一符号出现 ≥1 个: {any(symbols.values())}')