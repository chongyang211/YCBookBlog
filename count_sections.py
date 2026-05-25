#!/usr/bin/env python3
import sys
import re

def count_chinese_chars(text):
    # 统计中文字符（包括标点）
    return sum(1 for char in text if '\u4e00' <= char <= '\u9fff' or char in '，。！？；：“”‘’（）【】《》')

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 count_sections.py <file>")
        sys.exit(1)
    
    filename = sys.argv[1]
    with open(filename, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # 找到正文开始
    in_body = False
    current_section = None
    section_text = []
    sections = {}
    
    for line in lines:
        line = line.rstrip('\n')
        if line.startswith('**正文**'):
            in_body = True
            continue
        if not in_body:
            continue
        # 检测段位标题，如 **① 场景开篇**
        match = re.match(r'^\*\*(\d+) (.+)\*\*$', line)
        if match:
            if current_section is not None:
                sections[current_section] = ''.join(section_text)
            current_section = match.group(1) + ' ' + match.group(2)
            section_text = []
        else:
            if current_section is not None:
                section_text.append(line)
    
    if current_section is not None:
        sections[current_section] = ''.join(section_text)
    
    total_chars = 0
    for section, text in sections.items():
        chars = count_chinese_chars(text)
        total_chars += chars
        print(f"{section}: {chars} 字")
    
    print(f"\n总计: {total_chars} 字")

if __name__ == '__main__':
    main()