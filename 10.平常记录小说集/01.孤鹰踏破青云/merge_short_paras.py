#!/usr/bin/env python3
import re

def count_chinese_chars(text):
    chinese_pattern = re.compile(r'[\u4e00-\u9fff\u3000-\u303f\uff00-\uffef]')
    return len(chinese_pattern.findall(text))

def main():
    # 读取当前文件（已更新）
    path = '/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎.md'
    with open(path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # 定位正文
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
    header = lines[:start]
    footer = lines[end:]
    body_lines = lines[start:end]
    # 去除前后空行
    while body_lines and body_lines[0].strip() == '':
        body_lines.pop(0)
    while body_lines and body_lines[-1].strip() == '':
        body_lines.pop(-1)
    # 将正文转换为段落列表
    paras = []
    current = []
    for line in body_lines:
        stripped = line.strip()
        if stripped:
            current.append(stripped)
        else:
            if current:
                paras.append(' '.join(current))
                current = []
    if current:
        paras.append(' '.join(current))
    # 合并短段落（字数<80）
    merged = []
    i = 0
    while i < len(paras):
        para = paras[i]
        if count_chinese_chars(para) < 80 and i+1 < len(paras):
            # 合并当前段落和下一个段落
            next_para = paras[i+1]
            merged_para = para + ' ' + next_para
            merged.append(merged_para)
            i += 2
        else:
            merged.append(para)
            i += 1
    # 重新构建正文，段落间用空行分隔
    new_body_lines = []
    for para in merged:
        new_body_lines.append(para)
        new_body_lines.append('')
    # 去除最后的空行
    if new_body_lines and new_body_lines[-1] == '':
        new_body_lines.pop()
    new_body = '\n'.join(new_body_lines) + '\n'
    # 统计
    char_count = count_chinese_chars(new_body)
    para_count = len(merged)
    print(f'合并后字数: {char_count}')
    print(f'合并后段落数: {para_count}')
    # 写回文件
    new_content = header + [new_body] + footer
    with open(path, 'w', encoding='utf-8') as f:
        f.writelines(new_content)
    print('文件已更新。')

if __name__ == '__main__':
    main()