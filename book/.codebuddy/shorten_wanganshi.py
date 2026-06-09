import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def remove_brackets(text):
    # 删除中文括号及其内容，但保留年份括号如（1069年）
    # 先处理非年份括号
    def replace(match):
        content = match.group(1)
        if re.search(r'^\d{4}年$', content):
            return '(' + content + ')'
        if re.search(r'^\d+个$', content):
            return '(' + content + ')'
        return ''
    text = re.sub(r'（([^）]*)）', replace, text)
    # 删除英文括号及其内容（类似处理）
    text = re.sub(r'\(([^)]*)\)', replace, text)
    return text

def shorten_sentences(text):
    # 缩短句子：删除一些修饰词，合并重复
    # 简单实现：删除“的”、“了”等停用词？但中文停用词删除可能不影响字数
    # 暂时跳过
    return text

def shorten_paragraph(para):
    # 删除例子：以“例如”、“比如”、“举例”开头的句子
    lines = re.split(r'[。！？]', para)
    new_lines = []
    for line in lines:
        line = line.strip()
        if not line:
            continue
        if line.startswith('例如') or line.startswith('比如') or line.startswith('举例'):
            # 跳过例子
            continue
        # 删除括号内容
        line = remove_brackets(line)
        new_lines.append(line)
    return '。'.join(new_lines) + '。'

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.optimized.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.short.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 按段落分割（空行）
    paragraphs = re.split(r'\n\s*\n', content)
    new_paragraphs = []
    for para in paragraphs:
        # 跳过二级标题行
        if para.startswith('## '):
            new_paragraphs.append(para)
            continue
        # 跳过一级标题行
        if para.startswith('# '):
            new_paragraphs.append(para)
            continue
        # 跳过front matter
        if para.startswith('---'):
            new_paragraphs.append(para)
            continue
        # 处理普通段落
        shortened = shorten_paragraph(para)
        new_paragraphs.append(shortened)
    
    new_content = '\n\n'.join(new_paragraphs)
    
    # 修复二级标题编号：重新编号，从1开始连续
    lines = new_content.split('\n')
    counter = 1
    for i, line in enumerate(lines):
        if line.startswith('## '):
            # 提取标题内容
            match = re.match(r'## \d+\.(.+)', line)
            if match:
                title = match.group(1).strip()
                lines[i] = f'## {counter}.{title}'
                counter += 1
    
    new_content = '\n'.join(lines)
    
    chinese = count_chinese_chars(new_content)
    print(f'精简后中文字符数: {chinese}')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print(f'保存到: {output_path}')

if __name__ == '__main__':
    main()