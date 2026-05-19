#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
统一外国名著解读章节文件的格式（步骤1）：
1. 标题风格：# 第一章：书名 + ## 核心命题
2. 文件名称：第5.1章《双城记》狄更斯，按照这种格式更新文件名称
内容优化（步骤2）稍后进行。
"""

import re
from pathlib import Path

# 基础目录
base_dir = Path("/Users/yc/YCBookBlog/09.出版作品的书籍/08.外国名著解读")

# 卷名到数字的映射
volume_map = {
    "一": 1, "二": 2, "三": 3, "四": 4, "五": 5, "六": 6
}

# 章节数字映射
chapter_map = {
    "一": 1, "二": 2, "三": 3, "四": 4, "五": 5, "六": 6, "七": 7, "八": 8, "九": 9, "十": 10
}

DRY_RUN = False  # 设置为False以应用更改

def parse_volume_number(dir_name):
    """从目录名提取卷号"""
    match = re.search(r'^第([一二三四五六])卷', dir_name)
    if match:
        return volume_map.get(match.group(1), 0)
    return 0

def parse_chapter_number(file_name):
    """从文件名提取章节号"""
    match = re.search(r'^第([一二三四五六七八九十])章', file_name)
    if match:
        return chapter_map.get(match.group(1), 0)
    return 0

def extract_book_author_from_core_line(line):
    """从'核心名著'行提取书名和作者"""
    # 移除标记
    line = line.strip()
    line = re.sub(r'^\*\*核心名著\*\*：', '', line)
    # 现在line可能是'《书名》作者' 或 '《书名1》与《书名2》作者1 & 作者2'
    # 我们保留原样
    return line

def extract_book_from_title(title_line):
    """从标题行提取书名部分（包含《》）"""
    books = re.findall(r'《[^》]+》', title_line)
    if books:
        return ''.join(books)
    return ''

def extract_proposition(title_line, content_lines):
    """提取核心命题"""
    # 如果标题行中包含'：'且后面有文本，且不是纯书名，则作为命题
    if '：' in title_line:
        after_colon = title_line.split('：', 1)[1].strip()
        # 移除书名部分
        after_without_books = re.sub(r'《[^》]+》', '', after_colon).strip()
        if after_without_books and after_without_books not in ['与', '']:
            # 可能是命题
            # 检查是否仅为作者名（无空格或短文本），但难以判断，暂定返回
            return after_without_books
    
    # 查找文件中已有的核心命题行（紧接标题行之后，以##开头，且不包含《》）
    lines = content_lines
    for i, line in enumerate(lines):
        if line.startswith('## ') and '《' not in line:
            # 可能是核心命题行
            # 但需排除作者名（短文本，无标点）
            candidate = line[3:].strip()
            if len(candidate) > 5 or ('？' in candidate or '，' in candidate):
                return candidate
    # 从开篇第一句提取
    for i, line in enumerate(lines):
        if line.strip() == '### 【开篇・困境锚点】':
            for j in range(i+1, len(lines)):
                if lines[j].strip() != '':
                    first_line = lines[j].strip()
                    # 截断
                    if len(first_line) > 80:
                        first_line = first_line[:80] + '...'
                    return first_line
            break
    return ''

def process_file(file_path, volume_num):
    """处理单个文件"""
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    if not lines:
        return None, None
    
    # 查找标题行（第一个以#开头的非空行）
    title_line_idx = -1
    title_line = ''
    for i, line in enumerate(lines):
        if line.startswith('#') and line.strip() != '':
            title_line_idx = i
            title_line = line
            break
    
    if title_line_idx == -1:
        print(f"警告：{file_path} 未找到标题行")
        return None, None
    
    # 提取章节号
    chapter_num = parse_chapter_number(Path(file_path).stem)
    if chapter_num == 0:
        match = re.search(r'第([一二三四五六七八九十])章', title_line)
        if match:
            chapter_num = chapter_map.get(match.group(1), 0)
    
    if chapter_num == 0:
        print(f"警告：{file_path} 无法解析章节号")
        return None, None
    
    # 查找核心名著行
    core_line = None
    for i, line in enumerate(lines):
        if line.strip().startswith('**核心名著**：'):
            core_line = line.strip()
            break
    
    # 确定书名和作者
    book_author = ''
    if core_line:
        book_author = extract_book_author_from_core_line(core_line)
    else:
        # 从标题行提取书名
        book_part = extract_book_from_title(title_line)
        if book_part:
            book_author = book_part  # 无作者信息
        else:
            # 从文件名提取
            stem = Path(file_path).stem
            books = re.findall(r'《[^》]+》', stem)
            if books:
                book_author = ''.join(books)
    
    if not book_author:
        print(f"警告：{file_path} 无法确定书名")
        return None, None
    
    # 提取核心命题
    proposition = extract_proposition(title_line, lines)
    
    # 构建新标题行
    new_title_line = f"# 第{volume_num}.{chapter_num}章：{book_author}"
    new_proposition_line = f"## {proposition}" if proposition else "## （核心命题待补充）"
    
    # 确定需要替换的行范围
    # 从标题行开始，直到下一个###节或文件结束，但保留核心名著行
    replace_start = title_line_idx
    replace_end = title_line_idx
    # 如果标题行后紧跟着一个##行（可能是现有核心命题），则将其包括在内
    if title_line_idx + 1 < len(lines) and lines[title_line_idx + 1].startswith('## '):
        replace_end = title_line_idx + 1
        # 如果再下一行是空行，也包含
        if title_line_idx + 2 < len(lines) and lines[title_line_idx + 2].strip() == '':
            replace_end = title_line_idx + 2
    
    # 构建新内容行
    new_lines = []
    # 保留替换范围之前的内容
    new_lines.extend(lines[:replace_start])
    # 添加新标题和命题
    new_lines.append(new_title_line)
    new_lines.append(new_proposition_line)
    new_lines.append('')  # 空行
    # 保留替换范围之后的内容，但跳过已包含的行
    new_lines.extend(lines[replace_end+1:])
    
    new_content = '\n'.join(new_lines)
    
    # 新文件名
    # 清理book_author中的多余空格
    book_author_clean = book_author.strip()
    new_file_name = f"第{volume_num}.{chapter_num}章{book_author_clean}.md"
    new_file_path = file_path.parent / new_file_name
    
    return new_content, new_file_path

def main():
    # 遍历卷目录
    for item in base_dir.iterdir():
        if not item.is_dir():
            continue
        dir_name = item.name
        if dir_name == '外国名著解读.md':
            continue
        vol_num = parse_volume_number(dir_name)
        if vol_num == 0:
            print(f"跳过无法解析卷号的目录：{dir_name}")
            continue
        
        print(f"处理卷{vol_num}：{dir_name}")
        
        # 遍历目录下的.md文件
        for file in item.glob("*.md"):
            if file.name == "卷尾总结.md":
                continue
            if file.name == "外国名著解读.md":
                continue
            
            print(f"  处理文件：{file.name}")
            new_content, new_file_path = process_file(file, vol_num)
            if new_content is None:
                continue
            
            if DRY_RUN:
                print(f"    新标题：第{vol_num}.{parse_chapter_number(file.name)}章...")
                print(f"    新文件名：{new_file_path.name}")
                # 打印前几行
                preview = new_content.split('\n')[:6]
                for line in preview:
                    print(f"      {line}")
            else:
                # 写回文件
                if new_file_path != file:
                    print(f"    重命名为：{new_file_path.name}")
                    with open(new_file_path, 'w', encoding='utf-8') as f:
                        f.write(new_content)
                    file.unlink()
                else:
                    with open(file, 'w', encoding='utf-8') as f:
                        f.write(new_content)
    
    print("处理完成。")

if __name__ == '__main__':
    main()