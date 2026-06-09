import re
import sys

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def read_file(path):
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()

def write_file(path, content):
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

def extract_front_matter_and_title(content):
    lines = content.split('\n')
    front_matter_end = None
    for i, line in enumerate(lines):
        if line.strip() == '---':
            if front_matter_end is None:
                front_matter_end = i
                break
    if front_matter_end is not None:
        front_matter = '\n'.join(lines[:front_matter_end+1])
        rest = '\n'.join(lines[front_matter_end+1:])
        # 提取主标题（第一个#开头行）
        title_match = re.search(r'^# .+$', rest, re.MULTILINE)
        if title_match:
            title_line = title_match.group(0)
            title_start = rest.find(title_line)
            title_end = title_start + len(title_line)
            main_title = rest[title_start:title_end]
            body = rest[title_end:].lstrip()
            return front_matter, main_title, body
    return None, None, content

def split_paragraphs(text, max_chars_per_para=400):
    """将文本分割成段落，每段不超过max_chars_per_para字符"""
    paragraphs = []
    current = ''
    for char in text:
        current += char
        if char in '。！？；\n' and len(current) >= max_chars_per_para:
            paragraphs.append(current.strip())
            current = ''
    if current:
        paragraphs.append(current.strip())
    return paragraphs

def shorten_sentences(text, reduce_ratio=0.15):
    """缩短句子，删除冗余词汇，减少字符数"""
    # 简单实现：删除一些修饰词、重复表达
    # 这里仅示例，实际需要更复杂的逻辑
    # 暂时返回原文本
    return text

def optimize_section(section_title, section_content, section_number):
    """优化一个章节，添加二级标题，拆分段落"""
    # 根据章节类型决定如何拆分
    # 这里先简单返回原内容
    return section_content

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.md'
    
    content = read_file(input_path)
    front_matter, main_title, body = extract_front_matter_and_title(content)
    print(f'Front matter lines: {front_matter.count(chr(10)) if front_matter else 0}')
    print(f'Main title: {main_title}')
    
    # 暂时直接返回，不修改
    write_file(output_path, content)
    print('未修改，直接保存')

if __name__ == '__main__':
    main()