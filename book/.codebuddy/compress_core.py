import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def compress_core_section(text):
    # 将十项内容压缩为简短描述
    # 查找“第一项青苗法”等模式
    items = re.findall(r'第[一二三四五六七八九十]项[^。]+。', text)
    if len(items) >= 10:
        # 构建简短描述
        short_items = [
            '青苗法：国家小额信贷，年息20%。',
            '免役法：出钱代役，公共服务市场化。',
            '方田均税法：重新丈量土地，按田定税。',
            '市易法：政府平价收购出售，平抑市场。',
            '均输法：政府集中采购物流优化。',
            '保甲法：全民义务兵役制雏形。',
            '保马法：官民合作风险分担。',
            '将兵法：纠正杯酒释兵权后遗症。',
            '太学三舍法：教育分级学分制。',
            '农田水利法：水利国家战略，基础设施投资。'
        ]
        # 替换原内容
        for i in range(10):
            text = text.replace(items[i], short_items[i], 1)
    return text

def compress_heterogeneity(text):
    # 压缩异化部分，删除重复细节
    # 将每个异化描述缩短
    # 青苗法异化
    text = re.sub(r'青苗法的悲剧从"小额信贷"到"强制借贷"——理想是农民春天没钱可以"自愿"向政府借 20% 年息比高利贷便宜，现实执行的异化链条是：王安石给各地下了 KPI"青苗法借出多少钱"作为考核指标，地方官为了完成 KPI 开始强制借贷你不要也得借，为了让"还钱率高"地方官只借给"还得起的富农"而真正穷的农民借不到，20% 是"半年息"一年要借两次实际年息 40% 已经接近高利贷，基层小吏在"借和还"两头吃回扣百姓实际付出的成本远超 40%，遇到灾年还不上政府强制收回农民丢田比高利贷还狠。最终结果是富农被迫借了用不着的钱交利息、穷农借不到救命的钱还是被高利贷逼死、政府数据漂亮基层骂声一片。',
                  '青苗法异化：KPI导致强制借贷，实际年息40%以上，基层吃回扣，农民丢田。', text)
    # 免役法异化
    text = re.sub(r'免役法的悲剧从"减负"到"加税"——理想是有钱人多交钱穷人少交钱花钱免徭役，现实是 KPI 改成"免役钱收上来多少"，地方官为了完成 KPI 把免役钱加到所有人头上包括原本就免役的人，很多自耕农以前不用服役现在要交免役钱纯增加负担，基层小吏层层加码朝廷收 1 文到百姓那里要交 3 文——理论上"减负"实际上"加税"百姓苦不堪言。',
                  '免役法异化：KPI导致摊派，自耕农加负，基层加码，百姓苦不堪言。', text)
    # 市易法异化
    text = re.sub(r'市易法的悲剧从"平抑市场"到"政府垄断"——市易司必须"赚钱"作为 KPI，开始压低收购价比商人还狠、抬高销售价独家垄断，小商贩被挤出市场从"被剥削"变成"没饭吃"——本来是要打豪强结果"政府变成新的豪强"。',
                  '市易法异化：KPI导致政府垄断，挤压小商贩。', text)
    # 保甲法异化
    text = re.sub(r'保甲法的悲剧从"民兵"到"骚扰"——保甲长开始按户摊派演练费用、演练时间从农闲变成全年间断、保甲长成了新的"乡村权贵"开始勒索百姓、百姓为了逃避保甲开始大量自残逃亡——民间武装没建成基层却乱了。',
                  '保甲法异化：保甲长摊派勒索，百姓逃亡，基层混乱。', text)
    return text

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.short.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.final.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 按章节分割
    sections = re.split(r'(# .+?\n)', content)
    new_sections = []
    for i, section in enumerate(sections):
        if i % 2 == 0:
            new_sections.append(section)
        else:
            # 处理章节标题
            new_sections.append(section)
            # 下一个section是内容
            if i+1 < len(sections):
                content_section = sections[i+1]
                # 根据标题决定压缩
                if '二、史实还原' in section:
                    # 压缩变法核心内容
                    content_section = compress_core_section(content_section)
                    # 压缩异化部分
                    content_section = compress_heterogeneity(content_section)
                new_sections.append(content_section)
                i += 1  # 跳过已处理的内容
            else:
                new_sections.append('')
    
    new_content = ''.join(new_sections)
    
    # 删除多余空行
    new_content = re.sub(r'\n\s*\n\s*\n', '\n\n', new_content)
    
    chinese = count_chinese_chars(new_content)
    print(f'压缩后中文字符数: {chinese}')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print(f'保存到: {output_path}')

if __name__ == '__main__':
    main()