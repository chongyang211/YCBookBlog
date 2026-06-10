import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.optimized_final.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final_augmented.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 在开篇锚点部分增加一个现代案例
    # 在“## 2.理想与现实的碰撞”后添加一个新段落
    insert_point = content.find('## 2.理想与现实的碰撞')
    if insert_point != -1:
        # 找到该section的结束（下一个##或#）
        end_section = content.find('\n##', insert_point + 1)
        if end_section == -1:
            end_section = len(content)
        section = content[insert_point:end_section]
        # 在section末尾添加
        new_paragraph = '\n\n类似的故事在科技行业也屡见不鲜。2019年某互联网大厂推行“中台战略”，将原有分散的业务系统整合为统一中台，理论上看能提升效率、减少重复造轮子。但执行中，各业务线为了自身KPI拒绝接入，中台团队与业务团队陷入扯皮，项目延期两年，最终只实现了30%的功能，而市场已经变化。负责人后来总结：“我们设计了一个完美的架构，却忽略了组织内原有的权力结构和激励机制。”'
        content = content[:end_section] + new_paragraph + content[end_section:]
    
    # 在史实还原部分增加王安石与司马光的辩论细节
    # 在“## 7.变法失败与平反”前插入
    insert_point2 = content.find('## 7.变法失败与平反')
    if insert_point2 != -1:
        # 在前面插入
        new_paragraph2 = '\n\n## 7.变法论战与交锋\n\n变法期间，王安石与司马光进行了多次激烈辩论。司马光认为“祖宗之法不可变”，变法会扰乱社会秩序；王安石则反驳“天变不足畏，祖宗不足法，人言不足恤”。两人私交甚好，但政见截然相反。司马光连写三封长信劝王安石罢手，王安石回以《答司马谏议书》，坚持变法。这场辩论不仅是政策之争，更是两种治国理念的碰撞：保守 vs 激进，稳定 vs 变革。'
        # 调整编号：原来的7变为8，8变为9等
        # 简单插入，后续再重新编号
        content = content[:insert_point2] + new_paragraph2 + content[insert_point2:]
    
    # 在破局解读部分增加现代企业执行链例子
    # 在“## 11.人才储备反噬”后插入
    insert_point3 = content.find('## 11.人才储备反噬')
    if insert_point3 != -1:
        end_section3 = content.find('\n##', insert_point3 + 1)
        if end_section3 == -1:
            end_section3 = len(content)
        new_paragraph3 = '\n\n现代企业同样面临“长执行链”问题。一家跨国公司推行新的绩效考核系统，总部设计→区域总部→国家分公司→城市办事处→部门经理→员工，共6层。每经过一层，指标被重新解释，到员工那里只剩“完成数字”，而忽略了质量、创新、协作等软性目标。结果员工只做被测量的事，公司整体创新能力下降。解决之道是缩短链路：总部直接与试点团队沟通，收集反馈后迭代，再逐步推广。'
        content = content[:end_section3] + new_paragraph3 + content[end_section3:]
    
    # 在现实映射部分增加具体操作例子
    # 在“## 14.基层异化提前设防”后插入
    insert_point4 = content.find('## 14.基层异化提前设防')
    if insert_point4 != -1:
        end_section4 = content.find('\n##', insert_point4 + 1)
        if end_section4 == -1:
            end_section4 = len(content)
        new_paragraph4 = '\n\n例如，某公司销售团队KPI是“签约客户数”，结果销售为了冲数签下大量低质量客户，后期服务成本暴增。后来公司调整KPI为“签约客户数×客户满意度×回款率”，并设立匿名投诉通道，让客户直接反馈。虽然考核更复杂，但基层异化方向被引导至公司真正想要的方向：高质量客户和长期服务。'
        content = content[:end_section4] + new_paragraph4 + content[end_section4:]
    
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
    print(f'补充后中文字符数: {chinese}')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f'保存到: {output_path}')

if __name__ == '__main__':
    main()