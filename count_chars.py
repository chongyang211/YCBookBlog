#!/usr/bin/env python3
import sys
import os
import re

def count_chinese_chars(text):
    """统计中文字符数（不包括标点？这里统计所有非ASCII字符）"""
    # 简单统计：中文字符 Unicode 范围
    count = 0
    for char in text:
        if '\u4e00' <= char <= '\u9fff':
            count += 1
    return count

def count_paragraphs(text):
    """统计段落数：空行分隔的段落"""
    # 去掉开头的元数据部分，从“正文”开始
    lines = text.split('\n')
    in_body = False
    body_lines = []
    for line in lines:
        if line.strip() == '**正文**':
            in_body = True
            continue
        if in_body:
            body_lines.append(line)
    body_text = '\n'.join(body_lines)
    # 按空行分割段落
    paras = [p for p in body_text.split('\n\n') if p.strip()]
    return len(paras)

def main():
    if len(sys.argv) != 2:
        print("Usage: python count_chars.py <filepath>")
        sys.exit(1)
    filepath = sys.argv[1]
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    char_count = count_chinese_chars(content)
    para_count = count_paragraphs(content)
    print(f"中文字符数: {char_count}")
    print(f"段落数: {para_count}")

if __name__ == '__main__':
    main()