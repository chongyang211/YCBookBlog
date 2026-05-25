import re

with open('/Users/yc/YCBookBlog/ch19_body_no_sep.md', 'r', encoding='utf-8') as f:
    content = f.read()

paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

for i, p in enumerate(paragraphs):
    length = count_chinese_chars(p)
    if length > 240:
        print(f'段号 {i+1}: {length}字')
        print(p[:200])
        print('...')
        print(p[-200:])
        print('---')
        # 建议拆分点：找句号或逗号附近中间位置
        # 简单按句子拆分
        sentences = re.split(r'[。！？]', p)
        # 合并句子直到达到120字左右
        first_part = ''
        second_part = ''
        char_count = 0
        target = length // 2
        for s in sentences:
            if char_count < target:
                first_part += s + '。'
                char_count += count_chinese_chars(s)
            else:
                second_part += s + '。'
        print('建议拆分:')
        print('第一段:', first_part[:150])
        print('第二段:', second_part[:150])