import re

with open('/Users/yc/YCBookBlog/ch19_body_v2.md', 'r', encoding='utf-8') as f:
    content = f.read()

# 按空行分割段落
raw_paragraphs = content.split('\n\n')
print(f'原始段落数: {len(raw_paragraphs)}')

# 过滤空段落
paragraphs = [p.strip() for p in raw_paragraphs if p.strip()]
print(f'非空段落数: {len(paragraphs)}')

# 计算每段字数（中文字符）
def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

# 合并短段落（长度<50字）且不是分隔符（不含'---'）
merged = []
i = 0
while i < len(paragraphs):
    p = paragraphs[i]
    chars = count_chinese_chars(p)
    # 如果是分隔符（包含'---'），单独保留
    if '---' in p:
        merged.append(p)
        i += 1
        continue
    # 如果当前段短，且不是最后一段，尝试与下一段合并
    if chars < 50 and i < len(paragraphs) - 1:
        # 检查下一段是否是分隔符
        next_p = paragraphs[i+1]
        if '---' in next_p:
            # 下一段是分隔符，则保留当前段单独
            merged.append(p)
            i += 1
            continue
        # 合并当前段和下一段
        combined = p + '\n' + next_p
        merged.append(combined)
        i += 2  # 跳过下一段
    else:
        merged.append(p)
        i += 1

print(f'合并后段落数: {len(merged)}')

# 输出合并后的内容
merged_content = '\n\n'.join(merged)
with open('/Users/yc/YCBookBlog/ch19_body_merged.md', 'w', encoding='utf-8') as f:
    f.write(merged_content)

print('已保存到 ch19_body_merged.md')

# 统计新段落的字数分布
new_paragraphs = merged
new_lengths = [count_chinese_chars(p) for p in new_paragraphs]
short_count = sum(1 for l in new_lengths if l < 50)
print(f'短段（<50字）数: {short_count}')
print(f'总字数: {sum(new_lengths)}')