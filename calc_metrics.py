import re

file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 找到正文开始
start = content.find('合欢花开到第五天')
if start == -1:
    start = 0

# 找到正文结束：字数统计之前的 '---'
pattern = re.compile(r'\n---\n')
matches = list(pattern.finditer(content, start))
if matches:
    end = matches[0].start()
else:
    end = len(content)

body = content[start:end].strip()
paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]

print(f'段落数: {len(paragraphs)}')

# 统计中文字符
chinese_counts = [sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in paragraphs]
total_chinese = sum(chinese_counts)
print(f'总中文字符数: {total_chinese}')

# 对白段：包含引号或冒号后接对话
dialogue_pattern = re.compile(r'[“”"「」].*?[“”"「」]|[:：][^。！？；]*[。！？；]')
dialogue_paragraphs = []
for i, para in enumerate(paragraphs, 1):
    if dialogue_pattern.search(para):
        dialogue_paragraphs.append(i)
print(f'对白段数量: {len(dialogue_paragraphs)}')
print(f'对白段段落号: {dialogue_paragraphs}')

# 动作镜头段：包含具体动作动词
action_verbs = ['站', '走', '跑', '拿', '握', '看', '望', '转', '蹲', '坐', '起', '躺', '伸', '缩', '举', '按', '推', '拉', '打', '敲', '写', '折', '叠', '放', '取', '掏', '摸', '扶', '撑', '跪', '爬', '跳', '踢', '踩', '踏', '迈', '跨', '停', '动', '移', '晃', '摇', '摆', '扭', '转', '回', '进', '出', '来', '去', '上', '下', '开', '关', '点', '燃', '抽', '吸', '吐', '吹', '咳', '咽', '咬', '嚼', '吞', '舔', '吻', '抱', '拥', '搂', '拍', '抚', '摸', '擦', '抹', '洗', '刷', '扫', '拖', '擦', '扔', '丢', '抛', '接', '抓', '捉', '握', '捏', '掐', '按', '压', '挤', '拧', '扭', '撕', '扯', '剪', '切', '割', '砍', '劈', '刺', '戳', '扎', '插', '穿', '戴', '脱', '穿', '戴', '脱', '系', '解', '绑', '捆', '扎', '包', '裹', '卷', '展', '铺', '盖', '蒙', '遮', '挡', '拦', '阻', '挡', '护', '守', '攻', '击', '打', '斗', '战', '杀', '伤', '救', '治', '医', '疗', '缝', '补', '修', '理', '建', '造', '筑', '搭', '拆', '装', '卸', '搬', '运', '扛', '抬', '挑', '担', '背', '驮', '载', '拉', '推', '拖', '拽', '牵', '引', '导', '领', '带', '跟', '随', '追', '赶', '逃', '跑', '奔', '驰', '飞', '游', '漂', '浮', '沉', '潜', '升', '降', '落', '坠', '跌', '摔', '倒', '趴', '跪', '坐', '蹲', '站', '立', '倚', '靠', '躺', '卧', '睡', '醒', '起', '坐', '站', '走']
action_pattern = re.compile('|'.join(action_verbs))
action_paragraphs = []
for i, para in enumerate(paragraphs, 1):
    if action_pattern.search(para):
        action_paragraphs.append(i)
print(f'动作镜头段数量: {len(action_paragraphs)}')
print(f'动作镜头段段落号: {action_paragraphs[:20]}')  # 只显示前20个

# 主导意象“刀”出现次数
knife_count = body.count('刀')
print(f'主导意象“刀”出现次数: {knife_count}')

# 统一符号
symbol1 = body.count('缉毒服三处弹孔补丁') + body.count('三处弹孔补丁') + body.count('弹孔补丁')
symbol2 = body.count('全村凑学费的欠条') + body.count('欠条')
symbol3 = body.count('老所长的匕首') + body.count('匕首')
print(f'统一符号出现次数: 缉毒服三处弹孔补丁={symbol1}, 全村凑学费的欠条={symbol2}, 老所长的匕首={symbol3}')

# 三连排比：查找连续三个或以上结构相似的句子（简化：查找“，”“、”等）
# 暂时跳过

# 长句（≥25字）占比
all_sentences = re.split(r'[。！？；]', body)
all_sentences = [s.strip() for s in all_sentences if s.strip()]
long_sentences = [s for s in all_sentences if sum(1 for c in s if '\u4e00' <= c <= '\u9fff') >= 25]
print(f'总句子数: {len(all_sentences)}')
print(f'长句（≥25字）数: {len(long_sentences)}')
print(f'长句占比: {len(long_sentences)/len(all_sentences)*100:.1f}%')

# 短句（≤11字）占比
short_sentences = [s for s in all_sentences if sum(1 for c in s if '\u4e00' <= c <= '\u9fff') <= 11]
print(f'短句（≤11字）数: {len(short_sentences)}')
print(f'短句占比: {len(short_sentences)/len(all_sentences)*100:.1f}%')

# 比喻总数：查找“像”“如”“似”等词
simile_pattern = re.compile(r'像|如|似|仿佛|好比|犹如|宛如|好似')
simile_count = len(simile_pattern.findall(body))
print(f'比喻总数: {simile_count}')

# 古诗词：查找引用的诗句（简化）
poem_pattern = re.compile(r'[“”"].*?[“”"]')
poem_count = 0  # 暂不统计
print(f'古诗词句数: {poem_count}')

# 连续3段无对白/动作
# 检查是否有连续3段既不是对白段也不是动作段
consecutive_count = 0
for i in range(len(paragraphs) - 2):
    if (i+1 not in dialogue_paragraphs) and (i+1 not in action_paragraphs) and \
       (i+2 not in dialogue_paragraphs) and (i+2 not in action_paragraphs) and \
       (i+3 not in dialogue_paragraphs) and (i+3 not in action_paragraphs):
        consecutive_count += 1
        print(f'连续3段无对白/动作起始段: {i+1}')
print(f'连续3段无对白/动作的组数: {consecutive_count}')

# 形容词链（4个以上连用）：查找连续4个形容词（简化）
adj_pattern = re.compile(r'[^，。！？；]{0,10}的[^，。！？；]{0,10}的[^，。！？；]{0,10}的[^，。！？；]{0,10}的')
adj_chains = adj_pattern.findall(body)
print(f'形容词链数量: {len(adj_chains)}')
if adj_chains:
    print(f'形容词链示例: {adj_chains[0]}')

# 心理独白超80字未落地：查找长段心理描写（简化）
# 暂不统计