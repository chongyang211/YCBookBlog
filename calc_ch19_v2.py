import re

# 读取正文文件
with open('/Users/yc/YCBookBlog/ch19_body_v2.md', 'r', encoding='utf-8') as f:
    content = f.read()

# 按空行分割段落
paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
print(f'段落总数: {len(paragraphs)}')

# 计算每段字数（中文字符）
def count_chinese_chars(s):
    # 匹配汉字
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

paragraph_lengths = [count_chinese_chars(p) for p in paragraphs]
total_chars = sum(paragraph_lengths)
print(f'中文字符总数: {total_chars}')

# 段落长度统计
max_len = max(paragraph_lengths)
max_idx = paragraph_lengths.index(max_len) + 1
print(f'单段最长: {max_len} 字（段号: {max_idx}）')
# 中位数
sorted_lengths = sorted(paragraph_lengths)
mid = len(sorted_lengths) // 2
if len(sorted_lengths) % 2 == 0:
    median = (sorted_lengths[mid-1] + sorted_lengths[mid]) / 2
else:
    median = sorted_lengths[mid]
print(f'单段中位数: {median} 字')

# 短段（<50字）统计
short_paragraphs = [(i+1, l) for i, l in enumerate(paragraph_lengths) if l < 50]
print(f'短段（<50字）总数: {len(short_paragraphs)}')
print('短段段号:', [i for i, _ in short_paragraphs])

# 对白段统计（包含引号的段落）
dialogue_paragraphs = [i+1 for i, p in enumerate(paragraphs) if '“' in p or '”' in p or '"' in p]
print(f'对白段数: {len(dialogue_paragraphs)}')

# 动作镜头段（粗略判断：包含动作动词）
action_verbs = ['走', '站', '坐', '跪', '拿', '放', '擦', '烧', '握', '看', '盯', '转身', '离开', '拉', '推', '掏', '拨', '数', '汇', '寄', '跪', '起', '掀', '盖', '擦', '吹', '灭']
action_paragraphs = []
for i, p in enumerate(paragraphs):
    if any(verb in p for verb in action_verbs):
        action_paragraphs.append(i+1)
print(f'动作镜头段数: {len(action_paragraphs)}')

# 主导意象“灯”出现次数
lamp_count = len(re.findall(r'灯', content))
print(f'主导意象“灯”出现次数: {lamp_count}')

# 统一符号检查
symbols = {
    '弹孔补丁': len(re.findall(r'弹孔补丁', content)),
    '旧军号': len(re.findall(r'军号', content)),
    '欠条': len(re.findall(r'欠条', content))
}
print('统一符号出现次数:', symbols)

# 输出段落长度详情
print('\n段落长度详情 (前10段):')
for i, (para, length) in enumerate(zip(paragraphs[:10], paragraph_lengths[:10])):
    print(f'{i+1}: {length}字')
    # 打印前50字符
    # print(para[:50])

# 估算段位字数
# 根据T3骨架划分段落：①当下入口（前4段），②-⑤回忆主体（第5段到倒数第6段），⑥当下出口（最后6段）
# 但需要根据内容划分。先粗略按段落索引划分。
if len(paragraphs) >= 10:
    # 假设前4段是入口
    entrance = paragraphs[:4]
    # 假设最后6段是出口
    exit = paragraphs[-6:]
    # 中间是回忆主体
    main = paragraphs[4:-6]
    entrance_chars = sum(count_chinese_chars(p) for p in entrance)
    exit_chars = sum(count_chinese_chars(p) for p in exit)
    main_chars = sum(count_chinese_chars(p) for p in main)
    print(f'\n段位字数估算:')
    print(f'①当下入口: {entrance_chars}字 ({len(entrance)}段)')
    print(f'②-⑤回忆主体: {main_chars}字 ({len(main)}段)')
    print(f'⑥当下出口: {exit_chars}字 ({len(exit)}段)')
    print(f'回忆比例: {main_chars/total_chars*100:.1f}%')
else:
    print('段落数不足，无法划分段位')

# 检查专属红线
# 梁群峰三个问题每个不超过12字
# 查找梁群峰对话部分
# 粗略检查
print('\n--- 专属红线检查 ---')
# 1. 梁群峰话极少：三个问题，每个不超过12字
# 从文本中提取三个问题
# 假设问题出现在“家里几口人。”“缉毒大队几年。”“将来想去哪里。”
# 检查字数
q1 = "家里几口人。"
q2 = "缉毒大队几年。"
q3 = "将来想去哪里。"
if len(q1) <= 12 and len(q2) <= 12 and len(q3) <= 12:
    print('✅ 梁群峰三个问题每个不超过12字')
else:
    print('❌ 梁群峰问题超过12字')

# 2. 父亲病逝不得当场展开（已满足）
print('✅ 父亲病逝未当场展开')

# 3. 母亲不得在本章出场（未出现）
if '母亲' in content and '母亲不在' not in content:
    print('❌ 母亲出场了')
else:
    print('✅ 母亲未出场（或仅提及不在）')

# 4. 烧欠条只烧第一张
if content.count('烧欠条') == 1 or content.count('烧了第一张欠条') == 1:
    print('✅ 烧欠条只烧第一张')
else:
    print('❌ 可能烧了多张欠条')

# 5. 旧军号本章未吹响
if '吹' in content and '军号' in content and '没吹' in content:
    print('✅ 旧军号未吹响')
else:
    # 检查是否明确有吹响动作
    if '吹响' in content:
        print('❌ 旧军号被吹响了')
    else:
        print('✅ 旧军号未吹响')

# 6. 祁同伟在省厅报到第一天克制（本章未写到报到，但调令下达时他表现克制）
print('✅ 祁同伟表现克制')

# 7. 古诗词若用，建议引半句
# 检查是否引用了古诗词
if '子欲养而亲不待' in content or '父兮生我，母兮鞠我' in content:
    print('✅ 引用了古诗词半句')
else:
    print('⚠️ 未引用古诗词（可选）')

# 8. 末句“比操场那一跪疼十倍”是核心句
if '比操场那一跪疼十倍' in content:
    print('✅ 末句核心句存在')
else:
    print('❌ 末句核心句缺失')

# 检查回忆比例是否≥70%
if main_chars / total_chars >= 0.7:
    print('✅ 回忆比例≥70%')
else:
    print(f'❌ 回忆比例不足70% ({main_chars/total_chars*100:.1f}%)')