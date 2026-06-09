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

def extract_sections(body):
    # 匹配 ## 一、开篇锚点 等，直到下一个 ## 或文件结束
    pattern = r'## (一、开篇锚点|二、史实还原|三、破局解读|四、现实映射|五、史镜启示录)\s*\n(.*?)(?=\n## |\Z)'
    sections = {}
    for match in re.finditer(pattern, body, re.DOTALL):
        title, content = match.group(1), match.group(2).strip()
        sections[title] = content
    return sections

def optimize_kai_pian(content):
    # 拆分段落，添加二级标题
    # 原内容分为两段：老梁故事 + 引出主题
    # 在“更让人心痛的是”前拆分
    lines = content.split('\n')
    # 合并为一个字符串
    text = ' '.join(lines)
    # 查找分割点
    split_idx = text.find('更让人心痛的是')
    if split_idx > 0:
        part1 = text[:split_idx].strip()
        part2 = text[split_idx:].strip()
    else:
        part1 = text
        part2 = ''
    # 精简part1：删除一些冗余细节
    # 例如缩短SKU描述
    part1 = re.sub(r'第 1 项统一选品（所有门店菜单从原来的 80 个 SKU 砍到 32 个 SKU 库存周转效率提升 50%）', 
                   '第1项统一选品（80个SKU砍到32个，提升库存效率）', part1)
    part1 = re.sub(r'第 2 项动态调价（根据原料成本和门店流量每周自动调整菜价）',
                   '第2项动态调价（按成本和流量每周调价）', part1)
    part1 = re.sub(r'第 3 项员工绩效改革（把"按月固定工资"改为"按门店毛利分成"）',
                   '第3项员工绩效改革（固定工资改毛利分成）', part1)
    part1 = re.sub(r'第 4 项配送中央化（每个区域设中央厨房直接配送到门店）',
                   '第4项配送中央化（设中央厨房统一配送）', part1)
    part1 = re.sub(r'第 5 项会员体系（上线 App 统一会员积分消费数据）',
                   '第5项会员体系（上线App统一会员数据）', part1)
    part1 = re.sub(r'第 6 项督导制度（每个区域 5 个督导每周巡店加评分）',
                   '第6项督导制度（区域督导每周巡店评分）', part1)
    # 精简part2：删除重复表述
    part2 = re.sub(r'老梁离职后和我喝酒想了很久说了一句让我至今难忘的话——', 
                   '老梁离职后说了一句让我难忘的话：', part2)
    # 合并
    new_content = '## 1.老梁的改革悲剧\n\n' + part1 + '\n\n' + '## 2.理想与现实的碰撞\n\n' + part2
    return new_content

def optimize_shi_shi(content):
    # 史实还原部分，拆分为多个二级标题
    # 原内容有多个段落，可以按主题拆分
    # 先按空行拆分段落
    paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
    # 段落0：北宋积弊
    # 段落1：王安石背景
    # 段落2：变法核心内容
    # 段落3：变法异化与失败
    # 段落4：变法失败后续
    # 但实际段落可能不同，需要根据内容判断
    # 简单起见，我们按现有段落拆分，并添加标题
    titles = [
        '3.北宋积弊神宗即位',
        '4.王安石背景与变法',
        '5.变法核心十项内容',
        '6.变法异化与失败',
        '7.变法失败与平反'
    ]
    new_paras = []
    for i, para in enumerate(paragraphs):
        if i < len(titles):
            new_paras.append('## ' + titles[i] + '\n\n' + para)
        else:
            new_paras.append(para)
    return '\n\n'.join(new_paras)

def optimize_po_ju(content):
    # 破局解读部分，原有四个加粗段落
    # 提取加粗部分作为二级标题
    # 模式：**第一层心法是...**
    pattern = r'\*\*(.*?)\*\*'
    bold_parts = re.findall(pattern, content)
    # 移除加粗标记，将内容拆分为多个部分
    # 简单按加粗分割
    parts = re.split(pattern, content)
    # parts奇数索引是加粗内容，偶数索引是普通文本
    # 重建
    new_paras = []
    title_counter = 8  # 从8开始
    for i in range(len(parts)):
        if i % 2 == 1:
            # 加粗内容作为标题
            bold_text = parts[i]
            # 精简标题
            if '第一层心法' in bold_text:
                title = '人才落差悖论'
            elif '第二层心法' in bold_text:
                title = 'KPI异化诅咒'
            elif '第三层心法' in bold_text:
                title = '执行链长度决定'
            elif '第四层心法' in bold_text:
                title = '人才储备反噬'
            else:
                title = bold_text[:10]
            new_paras.append(f'## {title_counter}.{title}\n\n')
            title_counter += 1
        else:
            # 普通文本
            if parts[i].strip():
                new_paras.append(parts[i].strip())
    return '\n\n'.join(new_paras)

def optimize_xian_shi(content):
    # 现实映射部分，三个加粗段落
    pattern = r'\*\*(.*?)\*\*'
    bold_parts = re.findall(pattern, content)
    parts = re.split(pattern, content)
    new_paras = []
    title_counter = 12  # 从12开始
    for i in range(len(parts)):
        if i % 2 == 1:
            bold_text = parts[i]
            if '第一，永远不要做' in bold_text:
                title = '不超过团队承载力'
            elif '第二，所有改革必须配套' in bold_text:
                title = '短链路试点软评估'
            elif '第三，永远准备' in bold_text:
                title = '基层异化提前设防'
            else:
                title = bold_text[:10]
            new_paras.append(f'## {title_counter}.{title}\n\n')
            title_counter += 1
        else:
            if parts[i].strip():
                new_paras.append(parts[i].strip())
    return '\n\n'.join(new_paras)

def optimize_shi_jing(content):
    # 史镜启示录，一个段落，拆分为两个二级标题
    # 按句子拆分
    sentences = re.split(r'[。！？]', content)
    sentences = [s.strip() for s in sentences if s.strip()]
    # 前半部分：改革四定律，后半部分：王安石的启示
    mid = len(sentences) // 2
    part1 = '。'.join(sentences[:mid]) + '。'
    part2 = '。'.join(sentences[mid:]) + '。'
    return '## 14.改革四定律\n\n' + part1 + '\n\n' + '## 15.王安石的启示\n\n' + part2

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.optimized.md'
    
    content = read_file(input_path)
    # 提取 front matter 和主标题
    lines = content.split('\n')
    # 查找 front matter 结束
    fm_end = None
    for i, line in enumerate(lines):
        if line.strip() == '---':
            fm_end = i
            break
    if fm_end is not None:
        front_matter = '\n'.join(lines[:fm_end+1])
        rest = '\n'.join(lines[fm_end+1:])
    else:
        front_matter = ''
        rest = content
    # 提取主标题（第一个 # 开头）
    match = re.search(r'^# .+$', rest, re.MULTILINE)
    if match:
        main_title = match.group(0)
        body = rest[match.end():].lstrip()
    else:
        main_title = ''
        body = rest
    
    sections = extract_sections(body)
    
    # 优化各章节
    optimized_sections = {}
    for title, section_content in sections.items():
        if title == '一、开篇锚点':
            optimized_sections[title] = optimize_kai_pian(section_content)
        elif title == '二、史实还原':
            optimized_sections[title] = optimize_shi_shi(section_content)
        elif title == '三、破局解读':
            optimized_sections[title] = optimize_po_ju(section_content)
        elif title == '四、现实映射':
            optimized_sections[title] = optimize_xian_shi(section_content)
        elif title == '五、史镜启示录':
            optimized_sections[title] = optimize_shi_jing(section_content)
        else:
            optimized_sections[title] = section_content
    
    # 重建正文
    new_body = main_title + '\n\n'
    for title in ['一、开篇锚点', '二、史实还原', '三、破局解读', '四、现实映射', '五、史镜启示录']:
        new_body += '# ' + title + '\n\n' + optimized_sections[title] + '\n\n---\n\n'
    
    new_content = front_matter + '\n' + new_body.rstrip()
    
    # 统计字数
    chinese = count_chinese_chars(new_content)
    print(f'优化后中文字符数: {chinese}')
    
    write_file(output_path, new_content)
    print(f'优化后文件已保存到: {output_path}')

if __name__ == '__main__':
    main()