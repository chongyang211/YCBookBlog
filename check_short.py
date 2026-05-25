import re

with open('/Users/yc/YCBookBlog/ch19_body_final.md', 'r', encoding='utf-8') as f:
    content = f.read()

paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

short_indices = []
for i, p in enumerate(paragraphs):
    if count_chinese_chars(p) < 50:
        short_indices.append(i+1)
        print(f'短段 {i+1}: {count_chinese_chars(p)}字')
        # 打印前50字符
        print(p[:100])
        print('---')

print(f'短段总数: {len(short_indices)}')
print('段号:', short_indices)