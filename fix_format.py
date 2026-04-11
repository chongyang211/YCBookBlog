#!/usr/bin/env python3
"""批量修复 02.源码设计思想 目录下所有 md 文件的格式"""
import os
import re

BASE_DIR = "/Users/yc/YCBookBlog/00.通用技术的提升/02.源码设计思想"

# 文件名到期望标题的映射
TITLE_MAP = {
    "01.字符串设计的灵魂.md": "01.字符串设计的灵魂",
    "02.浮点型数据设计灵魂.md": "02.浮点型数据设计灵魂",
    "03.值型变量和引用.md": "03.值型变量和引用",
    "04.泛型设计灵魂思想.md": "04.泛型设计灵魂思想",
    "05.序列化数据的思想.md": "05.序列化数据的思想",
    "06.数据解析设计思想.md": "06.数据解析设计思想",
    "07.类的加载核心原理.md": "07.类的加载核心原理",
    "08.对象创建流程原理.md": "08.对象创建流程原理",
    "09.对象和函数访问原理.md": "09.对象和函数访问原理",
    "11.线程前世今生探索.md": "11.线程前世今生探索",
    "12.线程通信设计思想.md": "12.线程通信设计思想",
    "13.线程异常设计原理.md": "13.线程异常设计原理",
    "14.多线程并发经典案例.md": "14.多线程并发经典案例",
    "15.并发编程设计思想.md": "15.并发编程设计思想",
    "16.并发Bug源头由来.md": "16.并发Bug源头由来",
    "17.并发编程安全设计.md": "17.并发编程安全设计",
    "18.锁核心设计和思想.md": "18.锁核心设计和思想",
    "19.并发上下文切换原理.md": "19.并发上下文切换原理",
    "20.理解CAS设计和由来.md": "20.理解CAS设计和由来",
    "21.异步和同步的设计.md": "21.异步和同步的设计",
    "22.单线程模型的思想.md": "22.单线程模型的思想",
    "25.线程池的设计思想.md": "25.线程池的设计思想",
    "26.线程池使用技巧.md": "26.线程池使用技巧",
    "27.线程池设计核心原理.md": "27.线程池设计核心原理",
    "31.内存模型技术设计.md": "31.内存模型技术设计",
    "32.堆和栈内存的设计.md": "32.堆和栈内存的设计",
    "33.内存回收机制设计.md": "33.内存回收机制设计",
    "34.多种引用技术设计.md": "34.多种引用技术设计",
    "35.数据拷贝设计原理.md": "35.数据拷贝设计原理",
    "41.消息机制设计思想.md": "41.消息机制设计思想",
    "42.手势事件设计灵魂.md": "42.手势事件设计灵魂",
}

def generate_anchor(text):
    """生成 Markdown 锚点，匹配 GitHub/标准格式"""
    anchor = text.strip().lower()
    # 移除括号和特殊符号，但保留空格
    anchor = re.sub(r'[（）()《》<>【】\[\]{}""\'\"!！?？,，;；:：、。.·`~@#$%^&*+=|\\/<>]', '', anchor)
    # 空格转连字符
    anchor = anchor.replace(' ', '-')
    # 移除多余连字符
    anchor = re.sub(r'-+', '-', anchor)
    anchor = anchor.strip('-')
    return anchor

def fix_file(filepath, expected_title):
    """修复单个文件"""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    
    # Step 1: 确保文件以 # 标题 开头
    first_content_idx = 0
    for i, line in enumerate(lines):
        if line.strip():
            first_content_idx = i
            break
    
    has_correct_title = False
    if lines[first_content_idx].startswith('# ') and not lines[first_content_idx].startswith('## '):
        current_title = lines[first_content_idx][2:].strip()
        if current_title == expected_title:
            has_correct_title = True
        else:
            lines[first_content_idx] = f"# {expected_title}"
            has_correct_title = True
    
    if not has_correct_title:
        lines.insert(first_content_idx, f"# {expected_title}")
    
    # Step 2: 确保有 #### 目录介绍
    toc_start = -1
    for i, line in enumerate(lines):
        if '#### 目录介绍' in line:
            toc_start = i
            break
    
    if toc_start < 0:
        # 查找第一个 - 开头的行并在其前面插入
        for i, line in enumerate(lines):
            if line.strip().startswith('- ') and i > first_content_idx:
                lines.insert(i, '#### 目录介绍')
                toc_start = i
                break
    
    # Step 3: 给 TOC 添加锚点链接 - 移除旧锚点后重新生成
    if toc_start >= 0:
        toc_lines_start = toc_start + 1
        toc_lines_end = toc_lines_start
        for i in range(toc_lines_start, len(lines)):
            stripped = lines[i].strip()
            if stripped.startswith('- ') or stripped.startswith('[') or not stripped:
                if stripped:
                    toc_lines_end = i + 1
                elif toc_lines_end > toc_lines_start:
                    if i + 1 < len(lines):
                        next_stripped = lines[i+1].strip()
                        if next_stripped.startswith('- ') or next_stripped.startswith('['):
                            continue
                        else:
                            break
                    else:
                        break
            else:
                break
        
        # 处理 TOC 行
        for i in range(toc_lines_start, toc_lines_end):
            line = lines[i]
            # 先移除已有的锚点链接，恢复纯文本
            line = re.sub(r'\[([^\]]+)\]\(#[^)]*\)', r'\1', line)
            
            # 匹配 TOC 行
            m = re.match(r'^(\s*-\s+)(.+)$', line)
            if m:
                prefix = m.group(1)
                title = m.group(2).rstrip()
                if title:  # 非空标题
                    anchor = generate_anchor(title)
                    lines[i] = f"{prefix}[{title}](#{anchor})"
    
    content = '\n'.join(lines)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"Fixed: {os.path.basename(filepath)}")

def main():
    for filename, title in TITLE_MAP.items():
        filepath = os.path.join(BASE_DIR, filename)
        if os.path.exists(filepath):
            fix_file(filepath, title)
        else:
            print(f"NOT FOUND: {filename}")

if __name__ == '__main__':
    main()
