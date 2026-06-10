import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 在最后一个“---”之前添加一段
    last_dash = content.rfind('---')
    if last_dash != -1:
        new_paragraph = '\n\n## 17.写给改革者的最后一句话\n\n改革从来不是一帆风顺的，但失败不是终点，而是认知的起点。王安石变法虽然失败了，却给后世留下了宝贵的教训：任何改革都必须尊重现实土壤，匹配团队承载力，设计抗异化的制度，并保持开放的心态。今天，无论你是在推动组织变革、产品创新，还是个人成长，都要记住：理想是灯塔，现实是航道，只有两者结合，才能抵达彼岸。愿你能从历史中汲取智慧，在现实中找到出路。'
        content = content[:last_dash] + new_paragraph + content[last_dash:]
    
    # 重新编号二级标题
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
    print(f'已覆盖原文件: {output_path}')

if __name__ == '__main__':
    main()