import re

with open('/Users/yc/YCBookBlog/ch19_body_v2.md', 'r', encoding='utf-8') as f:
    content = f.read()

# 按空行分割段落
raw_paragraphs = content.split('\n\n')
paragraphs = [p.strip() for p in raw_paragraphs if p.strip()]
print(f'初始段落数: {len(paragraphs)}')

def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

# 标记分隔符段落（包含'---'）
separator_indices = [i for i, p in enumerate(paragraphs) if '---' in p]
print(f'分隔符段落数: {len(separator_indices)}')

# 目标段落数（不含分隔符）
target_paragraphs = 40  # 总段落数目标，包括分隔符
# 分隔符必须保留，所以非分隔符段落需要合并到 target_paragraphs - len(separator_indices)
non_sep_indices = [i for i in range(len(paragraphs)) if i not in separator_indices]
non_sep_paragraphs = [paragraphs[i] for i in non_sep_indices]
print(f'非分隔符段落数: {len(non_sep_paragraphs)}')

# 合并非分隔符段落
# 简单策略：将相邻的短段落合并
merged_non_sep = []
i = 0
while i < len(non_sep_paragraphs):
    p = non_sep_paragraphs[i]
    if i == len(non_sep_paragraphs) - 1:
        merged_non_sep.append(p)
        i += 1
        continue
    # 如果当前段长度小于80字，或者下一段长度小于80字，合并它们
    if count_chinese_chars(p) < 80 or count_chinese_chars(non_sep_paragraphs[i+1]) < 80:
        merged_p = p + '\n' + non_sep_paragraphs[i+1]
        merged_non_sep.append(merged_p)
        i += 2
    else:
        merged_non_sep.append(p)
        i += 1

print(f'合并后非分隔符段落数: {len(merged_non_sep)}')

# 如果仍然太多，进一步合并
while len(merged_non_sep) > target_paragraphs - len(separator_indices):
    # 找到最短的段落与其相邻段落合并
    lengths = [count_chinese_chars(p) for p in merged_non_sep]
    min_idx = lengths.index(min(lengths))
    # 合并 min_idx 与 min_idx+1（如果存在），否则与 min_idx-1
    if min_idx < len(merged_non_sep) - 1:
        merged_non_sep[min_idx] = merged_non_sep[min_idx] + '\n' + merged_non_sep[min_idx+1]
        merged_non_sep.pop(min_idx+1)
    elif min_idx > 0:
        merged_non_sep[min_idx-1] = merged_non_sep[min_idx-1] + '\n' + merged_non_sep[min_idx]
        merged_non_sep.pop(min_idx)
    else:
        break

print(f'最终非分隔符段落数: {len(merged_non_sep)}')

# 重新插入分隔符段落
# 我们需要保持分隔符在原位置的大致顺序
# 简化：先构建最终段落列表，按原始顺序插入分隔符
final_paragraphs = []
sep_idx = 0
non_sep_idx = 0
for i in range(len(paragraphs)):
    if i in separator_indices:
        # 插入分隔符
        final_paragraphs.append(paragraphs[i])
        sep_idx += 1
    else:
        # 插入合并后的非分隔符段落
        if non_sep_idx < len(merged_non_sep):
            final_paragraphs.append(merged_non_sep[non_sep_idx])
            non_sep_idx += 1
        else:
            # 如果合并后段落数不足，可能有问题
            pass

print(f'最终总段落数: {len(final_paragraphs)}')

# 计算统计
new_lengths = [count_chinese_chars(p) for p in final_paragraphs]
short_count = sum(1 for l in new_lengths if l < 50)
print(f'短段（<50字）数: {short_count}')
print(f'总字数: {sum(new_lengths)}')

# 保存
final_content = '\n\n'.join(final_paragraphs)
with open('/Users/yc/YCBookBlog/ch19_body_final.md', 'w', encoding='utf-8') as f:
    f.write(final_content)
print('已保存到 ch19_body_final.md')