import re

def count_chinese_chars(text):
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def main():
    input_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.short.md'
    output_path = '09.出版作品的书籍/05.中华历史故事/第六卷：变局中博弈/6.5 王安石变法·理想与现实的鸿沟.optimized_final.md'
    
    with open(input_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 替换开篇锚点
    new_kai_pian = """## 1.老梁的改革悲剧

我有一个朋友老梁，2017年加入一家大型连锁餐饮集团担任副总裁，负责门店标准化。他拿出了一套17项改革方案，每一项都是行业最佳实践。董事长拍桌叫好，让他大胆推行。

但一年后，改革全面崩盘。统一选品遭遇地方抵制，动态调价引发顾客流失，绩效改革导致员工钻空子，督导制度沦为表面文章。公司毛利率下降，顾客流失25%，员工流失50%，食品安全事件频发，股价跌了40%。老梁被调离岗位。

## 2.理想与现实的碰撞

更让人深思的是，这些方案单独看都没错，为什么在麦当劳成功，在这家公司却失败？老梁后来感慨：他改变了表面方案，却砸碎了底下运行二十年的人情网、员工默契、供应商关系。这些看不见的网，才是公司真正运转的东西。

读历史读到王安石变法，我恍然大悟：理想的方案加上现实的土壤，等于走样的执行。这是中国历史上最深刻的“理想 vs. 现实”碰撞，也是每个改革者必须刻在心里的一课。"""
    
    # 找到开篇锚点部分
    # 使用正则匹配 # 一、开篇锚点 到 # 二、史实还原
    pattern = r'(# 一、开篇锚点\n\n)(.*?)(?=\n# 二、史实还原)'
    match = re.search(pattern, content, re.DOTALL)
    if match:
        old = match.group(2)
        content = content.replace(old, new_kai_pian)
    
    # 压缩变法核心内容：将十项内容简化为列表
    # 找到 ## 5.变法核心十项内容 部分
    pattern_core = r'(## 5\.变法核心十项内容\n\n)(.*?)(?=\n## 6\.)'
    match_core = re.search(pattern_core, content, re.DOTALL)
    if match_core:
        old_core = match_core.group(2)
        new_core = """王安石变法的核心是财政改革，目标是“民不加赋而国用饶”。他推出了十项改革：
1. 青苗法：国家小额信贷，年息20%。
2. 免役法：出钱代役，公共服务市场化。
3. 方田均税法：重新丈量土地，按田定税。
4. 市易法：政府平价收购出售，平抑市场。
5. 均输法：政府集中采购物流优化。
6. 保甲法：全民义务兵役制雏形。
7. 保马法：官民合作风险分担。
8. 将兵法：纠正杯酒释兵权后遗症。
9. 太学三舍法：教育分级学分制。
10. 农田水利法：水利国家战略，基础设施投资。
这十项任何一项都是教科书级别的先进改革。"""
        content = content.replace(old_core, new_core)
    
    # 压缩变法异化与失败部分：简化描述
    pattern_hetero = r'(## 6\.变法异化与失败\n\n)(.*?)(?=\n## 7\.)'
    match_hetero = re.search(pattern_hetero, content, re.DOTALL)
    if match_hetero:
        old_hetero = match_hetero.group(2)
        new_hetero = """变法启动的第一年指标漂亮：财政收入增长30%，国库存银增加，民兵50万，水利1.7万处。但基层正在异化：
- 青苗法：KPI导致强制借贷，实际年息40%以上，基层吃回扣，农民丢田。
- 免役法：KPI导致摊派，自耕农加负，基层加码，百姓苦不堪言。
- 市易法：KPI导致政府垄断，挤压小商贩。
- 保甲法：保甲长摊派勒索，百姓逃亡，基层混乱。
所有改革都出现类似异化，王安石严禁基层异化的诏令无效，问题在于执行的人。"""
        content = content.replace(old_hetero, new_hetero)
    
    # 压缩变法失败与平反部分：简化
    pattern_fail = r'(## 7\.变法失败与平反\n\n)(.*?)(?=\n---)'
    match_fail = re.search(pattern_fail, content, re.DOTALL)
    if match_fail:
        old_fail = match_fail.group(2)
        new_fail = """变法5年，反对派反扑：司马光骂战、地方告状、天灾归罪、流民图呈送。神宗动摇，王安石第一次罢相。之后拉锯战多年，神宗病逝后变法被废，王安石被列入奸臣传800年，直到清末才平反。"""
        content = content.replace(old_fail, new_fail)
    
    # 压缩破局解读部分：保留核心，删除冗余解释
    # 找到 # 三、破局解读 到 # 四、现实映射
    pattern_poju = r'(# 三、破局解读\n\n)(.*?)(?=\n# 四、现实映射)'
    match_poju = re.search(pattern_poju, content, re.DOTALL)
    if match_poju:
        old_poju = match_poju.group(2)
        # 保留四个二级标题，但精简内容
        # 简单删除每个段落的一些句子
        # 由于时间有限，暂时保留原样，但可以删除一些重复
        # 这里仅示例删除部分文字
        old_poju = re.sub(r'这是改革史上的"人才落差悖论"[^。]+。', '', old_poju)
        old_poju = re.sub(r'这是任何改革都会面临的"古德哈特定律"[^。]+。', '', old_poju)
        content = content.replace(match_poju.group(2), old_poju)
    
    # 压缩现实映射部分：简化
    pattern_xianshi = r'(# 四、现实映射\n\n)(.*?)(?=\n# 五、史镜启示录)'
    match_xianshi = re.search(pattern_xianshi, content, re.DOTALL)
    if match_xianshi:
        old_xianshi = match_xianshi.group(2)
        # 保留三个二级标题，但精简内容
        # 删除例子
        old_xianshi = re.sub(r'举个具体例子[^。]+。', '', old_xianshi)
        content = content.replace(match_xianshi.group(2), old_xianshi)
    
    # 压缩史镜启示录部分：保留核心
    pattern_shijing = r'(# 五、史镜启示录\n\n)(.*?)(?=\n---)'
    match_shijing = re.search(pattern_shijing, content, re.DOTALL)
    if match_shijing:
        old_shijing = match_shijing.group(2)
        # 删除重复论述
        old_shijing = re.sub(r'公元 1086 年[^。]+。', '', old_shijing)
        content = content.replace(match_shijing.group(2), old_shijing)
    
    # 修复二级标题编号
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
    
    if chinese <= 6000:
        print('字数符合要求')
    else:
        print('字数仍超过6000，需要进一步精简')

if __name__ == '__main__':
    main()