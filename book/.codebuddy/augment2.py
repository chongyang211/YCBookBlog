import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final_augmented.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final_augmented2.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 在史镜启示录部分添加一段
    # 找到“## 16.王安石的启示”前插入
    insert_point = content.find('## 16.王安石的启示')
    if insert_point != -1:
        new_paragraph = '\n\n## 16.改革者的自我修养\n\n改革者常常陷入两种极端：要么过于理想主义，认为方案完美就能成功；要么过于悲观，认为现实无法改变。王安石的故事告诉我们，真正的改革者需要在理想与现实之间找到动态平衡。首先，保持谦逊，承认自己对复杂系统的无知，愿意从小处试点学习。其次，建立反馈机制，让一线执行者的声音能直达决策层。第三，培养团队，改革不是一个人的战斗，需要一批理解理念、有能力执行的伙伴。第四，耐心等待，改革效果往往需要数年才能显现，不能因短期挫折放弃。最后，时刻警惕自己是否爱上了方案本身，而非解决问题的实质。'
        content = content[:insert_point] + new_paragraph + content[insert_point:]
    
    # 重新编号二级标题（因为新增了一个）
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
    print(f'再次补充后中文字符数: {chinese}')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f'保存到: {output_path}')

if __name__ == '__main__':
    main()