#!/usr/bin/env python3
import re

def split_paragraphs(text, target_para_count=40):
    # 分割句子
    sentences = re.split(r'(?<=[。！？])', text)
    sentences = [s.strip() for s in sentences if s.strip()]
    # 计算每个段落的句子数
    total_sentences = len(sentences)
    sentences_per_para = max(1, total_sentences // target_para_count)
    # 重新组合段落
    paragraphs = []
    current = []
    for sent in sentences:
        current.append(sent)
        if len(current) >= sentences_per_para:
            paragraphs.append(''.join(current))
            current = []
    if current:
        paragraphs.append(''.join(current))
    return paragraphs

def main():
    with open('/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎_new.md', 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # 提取正文
    start = -1
    end = -1
    for i, line in enumerate(lines):
        if '<!-- AI 输出粘贴区 -->' in line:
            start = i + 1
        if start != -1 and line.strip() == '---' and i > start:
            end = i
            break
    if start == -1 or end == -1:
        print('错误')
        return
    # 获取正文前后的部分
    header = lines[:start]
    footer = lines[end:]
    body_lines = lines[start:end]
    # 去除前后空行
    while body_lines and body_lines[0].strip() == '':
        body_lines.pop(0)
    while body_lines and body_lines[-1].strip() == '':
        body_lines.pop(-1)
    body = ''.join(body_lines)
    # 调整段落
    paras = split_paragraphs(body, 40)
    new_body = '\n\n'.join(paras) + '\n'
    # 计算新正文的字数
    chinese_pattern = re.compile(r'[\u4e00-\u9fff\u3000-\u303f\uff00-\uffef]')
    char_count = len(chinese_pattern.findall(new_body))
    print(f'新正文字数: {char_count}')
    print(f'新段落数: {len(paras)}')
    # 写回新文件（测试）
    with open('/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎_adjusted.md', 'w', encoding='utf-8') as f:
        f.writelines(header)
        f.write(new_body)
        f.writelines(footer)
    print('已生成调整后文件')

if __name__ == '__main__':
    main()