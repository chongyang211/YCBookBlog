import re

with open('/Users/yc/YCBookBlog/ch19_body_no_sep.md', 'r', encoding='utf-8') as f:
    content = f.read()

paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

new_paragraphs = []
for i, p in enumerate(paragraphs):
    length = count_chinese_chars(p)
    if length <= 240:
        new_paragraphs.append(p)
        continue
    # 需要拆分
    # 在句号、感叹号、问号处拆分
    sentences = re.split(r'([。！？])', p)
    # 重组句子，保留标点
    sentences_with_punct = []
    for j in range(0, len(sentences)-1, 2):
        if j+1 < len(sentences):
            sentences_with_punct.append(sentences[j] + sentences[j+1])
        else:
            sentences_with_punct.append(sentences[j])
    # 如果还有剩余单句
    if len(sentences) % 2 == 1:
        sentences_with_punct.append(sentences[-1])
    
    # 合并句子直到达到长度一半
    target_len = length // 2
    first_part = ''
    second_part = ''
    current_len = 0
    for s in sentences_with_punct:
        s_len = count_chinese_chars(s)
        if current_len < target_len:
            first_part += s
            current_len += s_len
        else:
            second_part += s
    # 如果第二部分为空，则按句子数量平分
    if not second_part:
        mid = len(sentences_with_punct) // 2
        first_part = ''.join(sentences_with_punct[:mid])
        second_part = ''.join(sentences_with_punct[mid:])
    
    new_paragraphs.append(first_part.strip())
    new_paragraphs.append(second_part.strip())
    print(f'拆分段落 {i+1} ({length}字) 为两段: {count_chinese_chars(first_part)}字 + {count_chinese_chars(second_part)}字')

# 重新组合
new_content = '\n\n'.join(new_paragraphs)
with open('/Users/yc/YCBookBlog/ch19_body_split.md', 'w', encoding='utf-8') as f:
    f.write(new_content)

print(f'原始段落数: {len(paragraphs)}')
print(f'新段落数: {len(new_paragraphs)}')

# 验证最长段
new_paras = [p.strip() for p in new_content.split('\n\n') if p.strip()]
lengths = [count_chinese_chars(p) for p in new_paras]
print(f'新最长段: {max(lengths)}字')