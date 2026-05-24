#!/usr/bin/env python3
import re

def count_chinese_chars(text):
    chinese_pattern = re.compile(r'[\u4e00-\u9fff\u3000-\u303f\uff00-\uffef]')
    return len(chinese_pattern.findall(text))

def main():
    # 读取调整后的文件
    adjusted_path = '/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎_adjusted.md'
    with open(adjusted_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # 提取正文（从<!-- AI 输出粘贴区 -->之后到---之前）
    start = -1
    end = -1
    for i, line in enumerate(lines):
        if '<!-- AI 输出粘贴区 -->' in line:
            start = i + 1
        if start != -1 and line.strip() == '---' and i > start:
            end = i
            break
    if start == -1 or end == -1:
        print('无法定位正文')
        return
    body_lines = lines[start:end]
    # 去除前后空行
    while body_lines and body_lines[0].strip() == '':
        body_lines.pop(0)
    while body_lines and body_lines[-1].strip() == '':
        body_lines.pop(-1)
    new_body = ''.join(body_lines)
    # 确保正文以换行符结尾
    if not new_body.endswith('\n'):
        new_body += '\n'
    # 读取原文件
    original_path = '/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎.md'
    with open(original_path, 'r', encoding='utf-8') as f:
        orig_lines = f.readlines()
    # 定位原文件的正文区域
    orig_start = -1
    orig_end = -1
    for i, line in enumerate(orig_lines):
        if '<!-- AI 输出粘贴区 -->' in line:
            orig_start = i + 1  # 从下一行开始
        if orig_start != -1 and line.strip() == '---' and i > orig_start:
            orig_end = i
            break
    if orig_start == -1 or orig_end == -1:
        print('无法定位原文件正文区域')
        return
    # 构建新文件内容：保留开头到orig_start-1，插入新正文，然后从orig_end开始
    new_content = orig_lines[:orig_start] + [new_body] + orig_lines[orig_end:]
    # 写回原文件
    with open(original_path, 'w', encoding='utf-8') as f:
        f.writelines(new_content)
    # 统计信息
    char_count = count_chinese_chars(new_body)
    paras = [line.strip() for line in body_lines if line.strip()]
    print(f'第2章已更新。')
    print(f'正文字数: {char_count}')
    print(f'段落数: {len(paras)}')
    print(f'主导意象“尘”出现次数: {new_body.count("尘")}')
    print(f'“棉袄”出现次数: {new_body.count("棉袄")}')

if __name__ == '__main__':
    main()