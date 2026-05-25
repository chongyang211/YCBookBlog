import re

with open('/Users/yc/YCBookBlog/ch19_body_final.md', 'r', encoding='utf-8') as f:
    content = f.read()

# 按空行分割段落
raw_paragraphs = content.split('\n\n')
# 过滤掉只包含'---'的段落
filtered = [p.strip() for p in raw_paragraphs if p.strip() and p.strip() != '---']
print(f'移除分隔符后段落数: {len(filtered)}')

# 重新组合，保留空行分隔
filtered_content = '\n\n'.join(filtered)

with open('/Users/yc/YCBookBlog/ch19_body_no_sep.md', 'w', encoding='utf-8') as f:
    f.write(filtered_content)

print('已保存到 ch19_body_no_sep.md')

# 统计
def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

lengths = [count_chinese_chars(p) for p in filtered]
short = sum(1 for l in lengths if l < 50)
print(f'短段（<50字）数: {short}')
print(f'总字数: {sum(lengths)}')
print('短段段号:', [i+1 for i, l in enumerate(lengths) if l < 50])