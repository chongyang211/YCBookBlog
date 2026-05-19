#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
统一格式最终脚本：
1. 统一标题格式：# 第X.Y章：书名作者
2. 统一文件名：第X.Y章《书名》作者.md
3. 优化【回响・当代落地】和【结尾・阅读延伸】部分
"""

import re
from pathlib import Path

base_dir = Path("/Users/yc/YCBookBlog/09.出版作品的书籍/08.外国名著解读")

DRY_RUN = True  # 设置为True预览，False实际写入

# 卷名映射
volume_map = {"一":1,"二":2,"三":3,"四":4,"五":5,"六":6}
# 章节映射
chapter_map = {"一":1,"二":2,"三":3,"四":4,"五":5,"六":6,"七":7,"八":8,"九":9,"十":10}

# 已知书籍作者映射（补充）
known_authors = {
    "《红与黑》": "司汤达",
    "《了不起的盖茨比》": "菲茨杰拉德",
    "《包法利夫人》": "福楼拜",
    "《嘉莉妹妹》": "德莱塞",
    "《大卫・科波菲尔》": "狄更斯",
    "《约翰・克里斯朵夫》": "罗曼・罗兰",
    "《安娜・卡列尼娜》": "托尔斯泰",
    "《革命之路》": "理查德・耶茨",
    "《傲慢与偏见》": "简・奥斯汀",
    "《理智与情感》": "简・奥斯汀",
    "《霍乱时期的爱情》": "马尔克斯",
    "《茶花女》": "小仲马",
    "《百年孤独》": "马尔克斯",
    "《雪国》": "川端康成",
    "《卡拉马佐夫兄弟》": "陀思妥耶夫斯基",
    "《李尔王》": "莎士比亚",
    "《追风筝的人》": "卡勒德・胡赛尼",
    "《麦田里的守望者》": "塞林格",
    "《双城记》": "狄更斯",
    "《1984》": "乔治・奥威尔",
    "《飘》": "玛格丽特・米切尔",
    "《局外人》": "加缪",
    "《第二十二条军规》": "约瑟夫・海勒",
    "《堂吉诃德》": "塞万提斯",
    "《悲惨世界》": "雨果",
    "《老人与海》": "海明威",
    "《动物农场》": "乔治・奥威尔",
    "《瓦尔登湖》": "梭罗",
    "《日瓦戈医生》": "帕斯捷尔纳克",
    "《西西弗斯神话》": "加缪",
    "《罪与罚》": "陀思妥耶夫斯基",
    "《活着》": "余华",
    "《圣经》": "",
    "《不能承受的生命之轻》": "米兰・昆德拉",
    "《愤怒的葡萄》": "斯坦贝克",
    "《纯真年代》": "伊迪丝・华顿",
}

def parse_volume_number(dir_name):
    match = re.search(r'^第([一二三四五六])卷', dir_name)
    if match:
        return volume_map.get(match.group(1), 0)
    return 0

def parse_chapter_number(file_name):
    # 匹配'第5.1章'或'第一章'
    match = re.search(r'第(\d+)\.(\d+)章', file_name)
    if match:
        return int(match.group(2))
    match = re.search(r'第([一二三四五六七八九十])章', file_name)
    if match:
        return chapter_map.get(match.group(1), 0)
    return 0

def extract_book_author_from_filename(file_name):
    """从文件名提取书名和作者"""
    # 模式：第X.Y章《书名》作者.md 或 第X.Y章《书名》.md
    # 也可能有多个书名，如《书名》《书名》作者
    pattern = r'第\d+\.\d+章(.*?)\.md'
    match = re.search(pattern, file_name)
    if not match:
        return None, None
    inner = match.group(1)
    # 查找《》对
    books = re.findall(r'《[^》]+》', inner)
    if not books:
        return None, None
    # 作者是《》之后的部分，去除空格
    author_part = re.sub(r'《[^》]+》', '', inner).strip()
    # 如果作者部分为空，则作者未知
    if author_part:
        author = author_part
    else:
        author = None
    # 合并多个书名（用空格分隔）
    book = ' '.join(books)
    return book, author

def find_author_in_content(lines):
    """在内容中查找作者行，例如'## 狄更斯'"""
    for i, line in enumerate(lines):
        stripped = line.strip()
        if stripped.startswith('## ') and '《' not in stripped and '：' not in stripped:
            # 可能是作者行
            candidate = stripped[3:].strip()
            # 过滤掉太长的文本（可能是命题）
            if len(candidate) < 20 and not candidate.endswith('？') and not candidate.endswith('。'):
                # 可能是作者
                return candidate, i
    return None, -1

def find_core_book_line(lines):
    """查找核心名著行"""
    for i, line in enumerate(lines):
        if line.strip().startswith('**核心名著**：'):
            return line.strip(), i
    return None, -1

def get_proposition(lines, after_title_idx):
    """查找核心命题行（在标题行之后）"""
    for i in range(after_title_idx, min(after_title_idx + 5, len(lines))):
        line = lines[i].strip()
        if line.startswith('## ') and '《' not in line:
            candidate = line[3:].strip()
            # 过滤掉作者行（短且不含标点）
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
    
    # 1. 从文件名提取书名和作者
    file_name = file_path.name
    book_from_name, author_from_name = extract_book_author_from_filename(file_name)
    
    # 2. 从内容中查找作者行
    author_from_content, author_line_idx = find_author_in_content(lines)
    
    # 3. 查找核心名著行
    core_book_line, core_line_idx = find_core_book_line(lines)
    if core_book_line:
        # 提取书名和作者
        core_text = core_book_line.replace('**核心名著**：', '').strip()
        # 假设格式为《书名》作者
        # 不做解析，直接使用
        pass
    
    # 确定作者
    author = None
    if multiple_books:
        # 多本书不添加作者
        author = None
    else:
        if author_from_name:
            author = author_from_name
        elif author_from_content:
            author = author_from_content
        else:
            # 从已知映射中查找
            if book_from_name:
                for book_key, author_val in known_authors.items():
                    if book_key in book_from_name:
                        author = author_val
                        break
    
    # 确定书名（用于标题）
    if book_from_name:
        book_display = book_from_name
    else:
        # 从标题行提取
        book_display = ''
        for line in lines:
            if line.startswith('#') and '《' in line:
                books = re.findall(r'《[^》]+》', line)
                if books:
                    book_display = ' '.join(books)
                    break
    
    # 提取书籍列表
    books_in_display = re.findall(r'《[^》]+》', book_display) if book_display else []
    multiple_books = len(books_in_display) > 1
    
    # 章节号
    chapter_num = parse_chapter_number(file_name)
    if chapter_num == 0:
        # 从标题行提取
        for line in lines:
            if line.startswith('#'):
                match = re.search(r'第([一二三四五六七八九十])章', line)
                if match:
                    chapter_num = chapter_map.get(match.group(1), 0)
                    break
    
    if chapter_num == 0:
        print(f"警告：{file_path} 无法解析章节号")
        return None, None
    
    # 标题行索引
    title_idx = -1
    for i, line in enumerate(lines):
        if line.startswith('#') and line.strip() != '':
            title_idx = i
            break
    
    if title_idx == -1:
        print(f"警告：{file_path} 未找到标题行")
        return None, None
    
    # 核心命题
    proposition, prop_idx = get_proposition(lines, title_idx + 1)
    
    # 构建新标题行
    if multiple_books:
        # 多本书不添加作者
        title_book = book_display
    else:
        if author and author not in book_display:
            # 将作者附加到书名后
            title_book = f"{book_display}{author}"
        else:
            title_book = book_display
    new_title_line = f"# 第{volume_num}.{chapter_num}章：{title_book}"
    
    # 新核心命题行
    if proposition:
        new_proposition_line = f"## {proposition}"
    else:
        new_proposition_line = "## （核心命题待补充）"
    
    # 确定需要替换的行范围
    replace_start = title_idx
    replace_end = title_idx
    if prop_idx != -1:
        replace_end = prop_idx
        # 如果后面有空行也包含
        if prop_idx + 1 < len(lines) and lines[prop_idx + 1].strip() == '':
            replace_end = prop_idx + 1
    # 如果作者行在命题行之后，也需要删除
    if author_line_idx != -1 and author_line_idx > replace_end:
        replace_end = author_line_idx
        if author_line_idx + 1 < len(lines) and lines[author_line_idx + 1].strip() == '':
            replace_end = author_line_idx + 1
    
    # 构建新内容行
    new_lines = []
    new_lines.extend(lines[:replace_start])
    new_lines.append(new_title_line)
    new_lines.append(new_proposition_line)
    new_lines.append('')  # 空行
    new_lines.extend(lines[replace_end+1:])
    
    new_content = '\n'.join(new_lines)
    
    # 优化【回响・当代落地】和【结尾・阅读延伸】
    new_content = optimize_section(new_content, '回响・当代落地')
    new_content = optimize_section(new_content, '结尾・阅读延伸')
    
    # 新文件名
    if multiple_books:
        # 多本书不添加作者
        new_file_name = f"第{volume_num}.{chapter_num}章{book_display}.md"
    else:
        if author and author not in book_display:
            new_file_name = f"第{volume_num}.{chapter_num}章{book_display}{author}.md"
        else:
            new_file_name = f"第{volume_num}.{chapter_num}章{book_display}.md"
    # 清理文件名中的多余空格
    new_file_name = re.sub(r'\s+', '', new_file_name)
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
            
            print(f"    新文件名：{new_file_path.name}")
            # 实际写入
            if not DRY_RUN:
                if new_file_path != file:
                    print(f"    重命名为：{new_file_path.name}")
                    with open(new_file_path, 'w', encoding='utf-8') as f:
                        f.write(new_content)
                    file.unlink()
                else:
                    with open(file, 'w', encoding='utf-8') as f:
                        f.write(new_content)
            else:
                print("    (预览模式，未实际写入)")
    
    print("处理完成。")

if __name__ == '__main__':
    main()