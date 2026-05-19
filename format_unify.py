#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
统一外国名著解读章节文件的格式：
1. 标题风格：# 第一章：书名 + ## 核心命题
2. 文件名称：第5.1章《双城记》狄更斯，按照这种格式更新文件名称
3. 所有文章的【回响・当代落地】按照第一卷第一章的格式优化，保证段落充实，避免太多的分段和段落，避免太多- 格式
4. 所有文章【结尾・阅读延伸】避免太多分段和段落，尽量保证段落充实。避免太多- 格式
"""

import os
import re
import shutil
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

DRY_RUN = True  # 设置为True只打印更改，不实际修改

def extract_number(text):
    """从中文数字提取整数，支持简单数字如'一'到'十'"""
    if text in chapter_map:
        return chapter_map[text]
    # 可能包含'第'和'章'，如'第一章'
    match = re.search(r'第([一二三四五六七八九十])', text)
    if match:
        return chapter_map.get(match.group(1), 0)
    # 尝试阿拉伯数字
    match = re.search(r'(\d+)', text)
    if match:
        return int(match.group(1))
    return 0

def parse_volume_number(dir_name):
    """从目录名提取卷号"""
    # 目录名格式：'第一卷：自我的迷宫'
    match = re.search(r'^第([一二三四五六])卷', dir_name)
    if match:
        return volume_map.get(match.group(1), 0)
    return 0

def parse_chapter_number(file_name):
    """从文件名提取章节号"""
    # 文件名格式：'第一章：...' 或 '第七章：...'
    match = re.search(r'^第([一二三四五六七八九十])章', file_name)
    if match:
        return chapter_map.get(match.group(1), 0)
    return 0

def extract_book_and_author(line):
    """从'核心名著'行提取书名和作者"""
    # 格式：'**核心名著**：《书名》作者' 或 '**核心名著**：《书名1》与《书名2》作者1 & 作者2' 等
    # 简化为提取整个内容
    books = re.findall(r'《[^》]+》', line)
    if books:
        book_part = ''.join(books)  # 合并多个书名
        # 作者是去除书名和标记后的剩余部分
        author_part = re.sub(r'^\*\*核心名著\*\*：', '', line)
        author_part = re.sub(r'《[^》]+》', '', author_part).strip()
        return book_part, author_part
    return None, None

def extract_title_proposition(title_line):
    """从标题行提取命题部分"""
    if '：' not in title_line:
        return ''
    parts = title_line.split('：', 1)
    after_colon = parts[1].strip()
    # 如果包含《》，则可能是书名，需要进一步判断
    if '《' in after_colon and '》' in after_colon:
        # 可能是纯书名，也可能是书名+命题
        # 简单处理：如果《》后还有文本，则作为命题
        match = re.search(r'《[^》]+》([^《]+)', after_colon)
        if match:
            return match.group(1).strip()
        else:
            return ''  # 只有书名，无命题
    else:
        return after_colon

def reformat_echo_section(content):
    """优化【回响・当代落地】部分，将 - 列表转换为段落"""
    lines = content.split('\n')
    in_echo = False
    echo_lines = []
    for i, line in enumerate(lines):
        if line.strip() == '### 【回响・当代落地】':
            in_echo = True
            echo_lines.append(line)
            continue
        if in_echo and line.startswith('### 【'):
            break
        if in_echo:
            echo_lines.append(line)
    
    if not echo_lines:
        return content
    
    # 处理 echo_lines，将 - 开头的列表项合并
    processed = []
    i = 0
    while i < len(echo_lines):
        line = echo_lines[i]
        if line.strip().startswith('-'):
            items = []
            while i < len(echo_lines) and echo_lines[i].strip().startswith('-'):
                items.append(echo_lines[i].strip()[1:].strip())
                i += 1
            # 合并为段落
            if items:
                # 检查是否有粗体标记，如果有则保留
                bold_items = []
                for item in items:
                    if '**' in item:
                        bold_items.append(item)
                    else:
                        bold_items.append(item)
                # 用分号连接
                para = '；'.join(bold_items) + '。'
                processed.append(para)
            continue
        else:
            processed.append(line)
            i += 1
    
    echo_content = '\n'.join(echo_lines)
    processed_content = '\n'.join(processed)
    new_content = content.replace(echo_content, processed_content)
    return new_content

def reformat_extension_section(content):
    """优化【结尾・阅读延伸】部分，将 - 列表转换为段落"""
    lines = content.split('\n')
    in_ext = False
    ext_lines = []
    for i, line in enumerate(lines):
        if line.strip() == '### 【结尾・阅读延伸】':
            in_ext = True
            ext_lines.append(line)
            continue
        if in_ext and line.startswith('### 【'):
            break
        if in_ext:
            ext_lines.append(line)
    
    if not ext_lines:
        return content
    
    # 处理 - 列表
    processed = []
    i = 0
    while i < len(ext_lines):
        line = ext_lines[i]
        if line.strip().startswith('-'):
            items = []
            while i < len(ext_lines) and ext_lines[i].strip().startswith('-'):
                items.append(ext_lines[i].strip()[1:].strip())
                i += 1
            if items:
                para = '；'.join(items) + '。'
                processed.append(para)
            continue
        else:
            processed.append(line)
            i += 1
    
    ext_content = '\n'.join(ext_lines)
    processed_content = '\n'.join(processed)
    new_content = content.replace(ext_content, processed_content)
    return new_content

def process_file(file_path, volume_num):
    """处理单个文件"""
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    if not lines:
        return None, None
    
    # 查找标题行（第一个以#开头的非空行）
    title_line = None
    title_line_idx = -1
    for i, line in enumerate(lines):
        if line.startswith('#') and line.strip() != '':
            title_line = line
            title_line_idx = i
            break
    
    if title_line is None:
        print(f"警告：{file_path} 未找到标题行")
        return None, None
    
    # 查找核心名著行
    core_book_line = None
    core_book_idx = -1
    for i, line in enumerate(lines):
        if line.strip().startswith('**核心名著**：'):
            core_book_line = line
            core_book_idx = i
            break
    
    # 提取章节号
    chapter_num = parse_chapter_number(Path(file_path).stem)
    if chapter_num == 0:
        # 尝试从标题行提取
        match = re.search(r'第([一二三四五六七八九十])章', title_line)
        if match:
            chapter_num = chapter_map.get(match.group(1), 0)
    
    # 提取书名和作者
    book_part = ''
    author_part = ''
    if core_book_line:
        book_part, author_part = extract_book_and_author(core_book_line)
    
    if not book_part:
        # 从标题行提取书名
        books = re.findall(r'《[^》]+》', title_line)
        if books:
            book_part = ''.join(books)
            author_part = ''
        else:
            # 从文件名提取
            stem = Path(file_path).stem
            books = re.findall(r'《[^》]+》', stem)
            if books:
                book_part = ''.join(books)
    
    # 提取核心命题
    proposition = extract_title_proposition(title_line)
    if not proposition:
        # 尝试从开篇第一句提取
        for i, line in enumerate(lines):
            if line.strip() == '### 【开篇・困境锚点】':
                for j in range(i+1, len(lines)):
                    if lines[j].strip() != '':
                        first_line = lines[j].strip()
                        if len(first_line) > 100:
                            first_line = first_line[:100] + '...'
                        proposition = first_line
                        break
                break
    
    # 构建新标题
    if author_part and author_part.strip() != '':
        book_display = f"{book_part}{author_part}"
    else:
        book_display = book_part
    
    new_title_line = f"# 第{volume_num}.{chapter_num}章：{book_display}"
    new_proposition_line = f"## {proposition}" if proposition else "## （核心命题待补充）"
    
    # 构建新内容
    new_lines = []
    if title_line_idx > 0:
        new_lines.extend(lines[:title_line_idx])
    
    new_lines.append(new_title_line)
    new_lines.append(new_proposition_line)
    new_lines.append('')  # 空行
    
    # 保留原标题行之后的所有行
    for i in range(title_line_idx + 1, len(lines)):
        new_lines.append(lines[i])
    
    new_content = '\n'.join(new_lines)
    
    # 优化回响和延伸部分
    new_content = reformat_echo_section(new_content)
    new_content = reformat_extension_section(new_content)
    
    # 新文件名
    new_file_name = f"第{volume_num}.{chapter_num}章{book_display}.md"
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
                preview = new_content.split('\n')[:5]
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
    
    if DRY_RUN:
        print("\nDRY_RUN 模式：未实际修改文件。将 DRY_RUN 设置为 False 以应用更改。")

if __name__ == '__main__':
    main()