#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
最终修复脚本：
1. 确保标题格式为 # 第X.Y章：书名作者
2. 确保核心命题行正确（## 命题）
3. 确保文件名包含作者
4. 优化【回响・当代落地】和【结尾・阅读延伸】部分（减少 - 列表）
"""

import re
from pathlib import Path

base_dir = Path("/Users/yc/YCBookBlog/09.出版作品的书籍/08.外国名著解读")

volume_map = {"一":1,"二":2,"三":3,"四":4,"五":5,"六":6}
chapter_map = {"一":1,"二":2,"三":3,"四":4,"五":5,"六":6,"七":7,"八":8,"九":9,"十":10}

DRY_RUN = False  # 设置为True预览

def parse_volume_number(dir_name):
    match = re.search(r'^第([一二三四五六])卷', dir_name)
    if match:
        return volume_map.get(match.group(1), 0)
    return 0

def parse_chapter_number(file_name):
    # 尝试匹配'第5.1章'或'第一章'
    match = re.search(r'第(\d+)\.(\d+)章', file_name)
    if match:
        return int(match.group(2))
    match = re.search(r'第([一二三四五六七八九十])章', file_name)
    if match:
        return chapter_map.get(match.group(1), 0)
    return 0

def get_full_book_author(lines):
    """从核心名著行获取完整书名作者"""
    for line in lines:
        if line.strip().startswith('**核心名著**：'):
            core = line.strip()
            # 移除标记
            core = re.sub(r'^\*\*核心名著\*\*：', '', core)
            return core.strip()
    return None

def get_existing_proposition(lines, after_title_idx):
    """在标题行后查找核心命题行"""
    for i in range(after_title_idx, min(after_title_idx + 5, len(lines))):
        line = lines[i].strip()
        if line.startswith('## ') and '《' not in line:
            # 可能是核心命题
            candidate = line[3:].strip()
            # 过滤掉短文本（可能是作者）
            if len(candidate) > 6 or ('？' in candidate or '，' in candidate or '！' in candidate):
                return candidate, i
    return None, -1

def optimize_section(content, section_title):
    """优化指定节，将 - 列表合并为段落"""
    lines = content.split('\n')
    in_section = False
    section_lines = []
    start_idx = -1
    for i, line in enumerate(lines):
        if line.strip() == f'### 【{section_title}】':
            in_section = True
            start_idx = i
            section_lines.append(line)
            continue
        if in_section and line.startswith('### 【'):
            break
        if in_section:
            section_lines.append(line)
    
    if not section_lines:
        return content
    
    # 处理 - 列表
    processed = []
    i = 0
    while i < len(section_lines):
        line = section_lines[i]
        if line.strip().startswith('-'):
            items = []
            while i < len(section_lines) and section_lines[i].strip().startswith('-'):
                items.append(section_lines[i].strip()[1:].strip())
                i += 1
            if items:
                # 合并为段落，用分号连接
                para = '；'.join(items) + '。'
                processed.append(para)
            continue
        else:
            processed.append(line)
            i += 1
    
    # 替换原节
    old_section = '\n'.join(section_lines)
    new_section = '\n'.join(processed)
    if old_section in content:
        content = content.replace(old_section, new_section)
    return content

def process_file(file_path, volume_num):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    lines = content.split('\n')
    
    # 1. 查找标题行
    title_idx = -1
    for i, line in enumerate(lines):
        if line.startswith('#') and line.strip() != '':
            title_idx = i
            break
    if title_idx == -1:
        print(f"警告：{file_path} 未找到标题行")
        return None, None
    
    # 2. 章节号
    chapter_num = parse_chapter_number(Path(file_path).stem)
    if chapter_num == 0:
        # 从标题行提取
        match = re.search(r'第([一二三四五六七八九十])章', lines[title_idx])
        if match:
            chapter_num = chapter_map.get(match.group(1), 0)
    if chapter_num == 0:
        print(f"警告：{file_path} 无法解析章节号")
        return None, None
    
    # 3. 完整书名作者
    full_book_author = get_full_book_author(lines)
    if not full_book_author:
        # 从标题行提取《》部分
        books = re.findall(r'《[^》]+》', lines[title_idx])
        if books:
            full_book_author = ''.join(books)
        else:
            # 从文件名提取
            stem = Path(file_path).stem
            books = re.findall(r'《[^》]+》', stem)
            if books:
                full_book_author = ''.join(books)
    
    if not full_book_author:
        print(f"警告：{file_path} 无法确定书名")
        return None, None
    
    # 4. 现有核心命题
    existing_prop, prop_idx = get_existing_proposition(lines, title_idx + 1)
    
    # 5. 新标题行
    new_title_line = f"# 第{volume_num}.{chapter_num}章：{full_book_author}"
    
    # 6. 新核心命题行
    # 如果现有命题可用，则使用它；否则从开篇提取
    if existing_prop:
        proposition = existing_prop
    else:
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
    
    # 7. 替换行范围
    replace_start = title_idx
    replace_end = title_idx
    if prop_idx != -1:
        replace_end = prop_idx
        # 如果后面有空行也包含
        if prop_idx + 1 < len(lines) and lines[prop_idx + 1].strip() == '':
            replace_end = prop_idx + 1
    
    # 构建新内容行
    new_lines = []
    new_lines.extend(lines[:replace_start])
    new_lines.append(new_title_line)
    new_lines.append(new_proposition_line)
    new_lines.append('')  # 空行
    new_lines.extend(lines[replace_end+1:])
    
    new_content = '\n'.join(new_lines)
    
    # 8. 优化【回响・当代落地】和【结尾・阅读延伸】
    new_content = optimize_section(new_content, '回响・当代落地')
    new_content = optimize_section(new_content, '结尾・阅读延伸')
    
    # 9. 新文件名
    new_file_name = f"第{volume_num}.{chapter_num}章{full_book_author}.md"
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
                preview = new_content.split('\n')[:6]
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
    
    print("处理完成。")

if __name__ == '__main__':
    main()