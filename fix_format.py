#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
修复格式统一问题：
1. 确保文件名包含作者（如果核心名著行有作者）
2. 确保核心命题正确
"""

import re
from pathlib import Path

base_dir = Path("/Users/yc/YCBookBlog/09.出版作品的书籍/08.外国名著解读")

volume_map = {"一":1,"二":2,"三":3,"四":4,"五":5,"六":6}
chapter_map = {"一":1,"二":2,"三":3,"四":4,"五":5,"六":6,"七":7,"八":8,"九":9,"十":10}

DRY_RUN = False

def parse_volume_number(dir_name):
    match = re.search(r'^第([一二三四五六])卷', dir_name)
    if match:
        return volume_map.get(match.group(1), 0)
    return 0

def parse_chapter_number(file_name):
    match = re.search(r'^第([一二三四五六七八九十])章', file_name)
    if match:
        return chapter_map.get(match.group(1), 0)
    # 也可能已经是'第5.1章'格式
    match = re.search(r'^第(\d+)\.(\d+)章', file_name)
    if match:
        return int(match.group(2))
    return 0

def extract_book_author_from_core_line(line):
    """返回完整字符串，如'《双城记》狄更斯'"""
    line = line.strip()
    line = re.sub(r'^\*\*核心名著\*\*：', '', line)
    return line

def find_existing_proposition(lines, title_line_idx):
    """在标题行后查找核心命题行"""
    for i in range(title_line_idx + 1, min(title_line_idx + 5, len(lines))):
        line = lines[i].strip()
        if line.startswith('## ') and '《' not in line:
            # 可能是核心命题
            candidate = line[3:].strip()
            # 过滤掉短文本（可能是作者）
            if len(candidate) > 6 or ('？' in candidate or '，' in candidate or '！' in candidate):
                return candidate, i
    return None, -1

def process_file(file_path, volume_num):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    lines = content.split('\n')
    
    # 查找标题行
    title_line_idx = -1
    for i, line in enumerate(lines):
        if line.startswith('#') and line.strip() != '':
            title_line_idx = i
            break
    if title_line_idx == -1:
        print(f"警告：{file_path} 未找到标题行")
        return None, None
    
    title_line = lines[title_line_idx]
    
    # 章节号
    chapter_num = parse_chapter_number(Path(file_path).stem)
    if chapter_num == 0:
        match = re.search(r'第([一二三四五六七八九十])章', title_line)
        if match:
            chapter_num = chapter_map.get(match.group(1), 0)
    if chapter_num == 0:
        print(f"警告：{file_path} 无法解析章节号")
        return None, None
    
    # 核心名著行
    core_line = None
    for i, line in enumerate(lines):
        if line.strip().startswith('**核心名著**：'):
            core_line = line.strip()
            break
    
    # 书名和作者
    book_author = ''
    if core_line:
        book_author = extract_book_author_from_core_line(core_line)
    else:
        # 从标题行提取《》部分
        books = re.findall(r'《[^》]+》', title_line)
        if books:
            book_author = ''.join(books)
        else:
            # 从文件名提取
            stem = Path(file_path).stem
            books = re.findall(r'《[^》]+》', stem)
            if books:
                book_author = ''.join(books)
    
    if not book_author:
        print(f"警告：{file_path} 无法确定书名")
        return None, None
    
    # 查找现有核心命题行
    existing_prop, prop_idx = find_existing_proposition(lines, title_line_idx)
    
    # 构建新标题行
    new_title_line = f"# 第{volume_num}.{chapter_num}章：{book_author}"
    
    # 决定核心命题
    if existing_prop:
        proposition = existing_prop
    else:
        # 从开篇提取
        proposition = ''
        for i, line in enumerate(lines):
            if line.strip() == '### 【开篇・困境锚点】':
                for j in range(i+1, len(lines)):
                    if lines[j].strip() != '':
                        first_line = lines[j].strip()
                        if len(first_line) > 80:
                            first_line = first_line[:80] + '...'
                        proposition = first_line
                        break
                break
    
    new_proposition_line = f"## {proposition}" if proposition else "## （核心命题待补充）"
    
    # 确定需要替换的行范围
    replace_start = title_line_idx
    replace_end = title_line_idx
    # 如果存在现有核心命题行，则包含它
    if prop_idx != -1:
        replace_end = prop_idx
        # 如果后面有空行也包含
        if prop_idx + 1 < len(lines) and lines[prop_idx + 1].strip() == '':
            replace_end = prop_idx + 1
    
    # 构建新内容
    new_lines = []
    new_lines.extend(lines[:replace_start])
    new_lines.append(new_title_line)
    new_lines.append(new_proposition_line)
    new_lines.append('')  # 空行
    new_lines.extend(lines[replace_end+1:])
    
    new_content = '\n'.join(new_lines)
    
    # 新文件名（确保包含作者）
    # 如果book_author不包含作者（只有《》），但core_line有作者，则使用core_line
    if core_line and not re.search(r'[a-zA-Z\u4e00-\u9fff]', book_author.replace('《', '').replace('》', '').strip()):
        # book_author可能只有书名，尝试从core_line提取完整信息
        full = extract_book_author_from_core_line(core_line)
        if full:
            book_author = full
    
    new_file_name = f"第{volume_num}.{chapter_num}章{book_author}.md"
    new_file_path = file_path.parent / new_file_name
    
    return new_content, new_file_path

def main():
    for item in base_dir.iterdir():
        if not item.is_dir():
            continue
        dir_name = item.name
        if dir_name == '外国名著解读.md':
            continue
        vol_num = parse_volume_number(dir_name)
        if vol_num == 0:
            continue
        
        print(f"处理卷{vol_num}：{dir_name}")
        
        for file in item.glob("*.md"):
            if file.name == "卷尾总结.md":
                continue
            if file.name == "外国名著解读.md":
                continue
            
            print(f"  处理：{file.name}")
            new_content, new_file_path = process_file(file, vol_num)
            if new_content is None:
                continue
            
            if DRY_RUN:
                print(f"    新文件名：{new_file_path.name}")
                preview = new_content.split('\n')[:5]
                for line in preview:
                    print(f"      {line}")
            else:
                if new_file_path != file:
                    print(f"    重命名为：{new_file_path.name}")
                    with open(new_file_path, 'w', encoding='utf-8') as f:
                        f.write(new_content)
                    file.unlink()
                else:
                    with open(file, 'w', encoding='utf-8') as f:
                        f.write(new_content)
    
    print("修复完成。")

if __name__ == '__main__':
    main()