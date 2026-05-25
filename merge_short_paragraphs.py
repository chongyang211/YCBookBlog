#!/usr/bin/env python3
import re
import sys

def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

def merge_short_paragraphs(content, min_length=50):
    paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
    print(f'初始段落数: {len(paragraphs)}')
    
    # 标记短段落
    short_indices = []
    for i, p in enumerate(paragraphs):
        if count_chinese_chars(p) < min_length:
            short_indices.append(i)
    print(f'短段落数 (<{min_length}字): {len(short_indices)}')
    
    # 合并短段落与相邻段落
    merged = []
    i = 0
    while i < len(paragraphs):
        if i in short_indices:
            # 合并当前短段落与下一个段落（如果存在），否则与前一个段落合并
            if i < len(paragraphs) - 1:
                merged.append(paragraphs[i] + '\n' + paragraphs[i+1])
                i += 2
            elif i > 0:
                # 最后一个段落，且是短段落，合并到前一个段落
                merged[-1] = merged[-1] + '\n' + paragraphs[i]
                i += 1
            else:
                # 只有一个段落且短，保留
                merged.append(paragraphs[i])
                i += 1
        else:
            merged.append(paragraphs[i])
            i += 1
    
    # 再次检查短段落
    new_short = sum(1 for p in merged if count_chinese_chars(p) < min_length)
    print(f'合并后段落数: {len(merged)}')
    print(f'合并后短段落数: {new_short}')
    
    # 如果仍有短段落，继续合并相邻的短段落
    while new_short > 0:
        # 找到短段落索引
        short_idx = [j for j, p in enumerate(merged) if count_chinese_chars(p) < min_length]
        if not short_idx:
            break
        # 合并第一个短段落与其相邻段落
        idx = short_idx[0]
        if idx < len(merged) - 1:
            merged[idx] = merged[idx] + '\n' + merged[idx+1]
            merged.pop(idx+1)
        elif idx > 0:
            merged[idx-1] = merged[idx-1] + '\n' + merged[idx]
            merged.pop(idx)
        else:
            # 只有一个段落且短，无法合并
            break
        new_short = sum(1 for p in merged if count_chinese_chars(p) < min_length)
    
    print(f'最终段落数: {len(merged)}')
    print(f'最终短段落数: {new_short}')
    
    # 计算统计
    lengths = [count_chinese_chars(p) for p in merged]
    print(f'总字数: {sum(lengths)}')
    print(f'最长段落: {max(lengths)} 字')
    print(f'中位数段落长度: {sorted(lengths)[len(lengths)//2]} 字')
    
    return '\n\n'.join(merged)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('用法: python3 merge_short_paragraphs.py <输入文件> [输出文件]')
        sys.exit(1)
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else input_file.replace('.md', '_merged.md')
    
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    merged_content = merge_short_paragraphs(content, min_length=50)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(merged_content)
    
    print(f'已保存到: {output_file}')