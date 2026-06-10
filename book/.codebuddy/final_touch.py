import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final_augmented2.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 在现实映射部分末尾添加一个小例子
    # 找到“## 15.基层异化提前设防”部分，在其后添加
    insert_point = content.find('## 15.基层异化提前设防')
    if insert_point != -1:
        end_section = content.find('\n##', insert_point + 1)
        if end_section == -1:
            end_section = len(content)
        new_example = '\n\n再比如，某工厂推行“安全生产零事故”KPI，结果员工为了不报事故隐瞒小伤害，反而酿成大祸。后来工厂改为“事故上报率+隐患整改率”双指标，并奖励主动上报，事故率反而下降。这说明设计KPI时要考虑人性，让异化方向与目标一致。'
        content = content[:end_section] + new_example + content[end_section:]
    
    # 重新编号二级标题（可能新增了标题，但这里只是添加段落）
    # 检查是否有新增标题，没有则不需要重新编号
    lines = content.split('\n')
    counter = 1
    for i, line in enumerate(lines):
        if line.startswith('## '):
            match = re.match(r'## (\d+)\.(.+)', line)
            if match:
                title = match.group(2).strip()
                lines[i] = f'## {counter}.{title}'
                counter += 1
    
    content = '\n'.join(lines)
    
    chinese = count_chinese_chars(content)
    print(f'最终中文字符数: {chinese}')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f'保存到: {output_path}')
    
    if 5000 <= chinese <= 6000:
        print('字数符合要求！')
    else:
        print(f'字数不在范围内，当前{chinese}')

if __name__ == '__main__':
    main()