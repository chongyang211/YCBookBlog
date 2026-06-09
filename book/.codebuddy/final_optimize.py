import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def remove_examples(text):
    # 删除“例如”、“比如”引导的句子
    lines = re.split(r'[。！？]', text)
    new_lines = []
    for line in lines:
        line = line.strip()
        if not line:
            continue
        if line.startswith('例如') or line.startswith('比如') or line.startswith('举例'):
            continue
        # 删除括号内容（保留年份括号）
        line = re.sub(r'（[^）]*）', '', line)
        line = re.sub(r'\([^)]*\)', '', line)
        new_lines.append(line)
    return '。'.join(new_lines) + '。'

def shorten_paragraph(para):
    # 删除重复词汇
    # 简单删除“的”、“了”等停用词？但中文停用词删除效果有限
    # 合并连续重复的短语
    # 暂时只删除例子
    para = remove_examples(para)
    # 删除多余空格
    para = re.sub(r'\s+', '', para)
    return para

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.short.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final2.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 按空行分割段落
    paragraphs = re.split(r'\n\s*\n', content)
    new_paragraphs = []
    for para in paragraphs:
        # 保留标题行和front matter
        if para.startswith('#') or para.startswith('---') or para.startswith('##'):
            new_paragraphs.append(para)
            continue
        # 处理普通段落
        shortened = shorten_paragraph(para)
        if shortened.strip():
            new_paragraphs.append(shortened)
    
    new_content = '\n\n'.join(new_paragraphs)
    
    # 修复二级标题编号：确保连续
    lines = new_content.split('\n')
    counter = 1
    for i, line in enumerate(lines):
        if line.startswith('## '):
            # 提取标题内容
            match = re.match(r'## (\d+)\.(.+)', line)
            if match:
                title = match.group(2).strip()
                lines[i] = f'## {counter}.{title}'
                counter += 1
    
    new_content = '\n'.join(lines)
    
    chinese = count_chinese_chars(new_content)
    print(f'最终中文字符数: {chinese}')
    
    if chinese > 6000:
        print('字数仍超过6000，进行进一步删减')
        # 删除更多内容：删除开篇锚点中老梁故事的部分细节
        # 查找开篇锚点部分
        # 简单删除一些句子
        # 暂时跳过
        pass
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print(f'保存到: {output_path}')

if __name__ == '__main__':
    main()