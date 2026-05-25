import re

with open('/Users/yc/YCBookBlog/ch19_body_no_sep.md', 'r', encoding='utf-8') as f:
    content = f.read()

paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
print(f'段落总数: {len(paragraphs)}')

def count_chinese_chars(s):
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', s)
    return len(chinese_chars)

paragraph_lengths = [count_chinese_chars(p) for p in paragraphs]
total_chars = sum(paragraph_lengths)
print(f'中文字符总数: {total_chars}')

# 自检项
# 1. 单段最长 ≤240 字
max_len = max(paragraph_lengths)
max_idx = paragraph_lengths.index(max_len) + 1
print(f'单段最长: {max_len}字 (段号: {max_idx})')
# 2. 单段中位数 100-140 字
sorted_lengths = sorted(paragraph_lengths)
mid = len(sorted_lengths) // 2
if len(sorted_lengths) % 2 == 0:
    median = (sorted_lengths[mid-1] + sorted_lengths[mid]) / 2
else:
    median = sorted_lengths[mid]
print(f'单段中位数: {median}字')
# 3. 短段（<50字）总数 ≤4
short_count = sum(1 for l in paragraph_lengths if l < 50)
print(f'短段（<50字）总数: {short_count}')
# 4. 对白段 ≥8 ≤20
# 对白段：包含引号的段落
dialogue_paragraphs = [i+1 for i, p in enumerate(paragraphs) if '“' in p or '”' in p or '"' in p]
dialogue_count = len(dialogue_paragraphs)
print(f'对白段数: {dialogue_count}')
# 5. 动作镜头段 ≥10
action_verbs = ['走', '站', '坐', '跪', '拿', '放', '擦', '烧', '握', '看', '盯', '转身', '离开', '拉', '推', '掏', '拨', '数', '汇', '寄', '跪', '起', '掀', '盖', '擦', '吹', '灭']
action_paragraphs = [i+1 for i, p in enumerate(paragraphs) if any(verb in p for verb in action_verbs)]
action_count = len(action_paragraphs)
print(f'动作镜头段数: {action_count}')

# 主导意象“灯”
lamp_count = len(re.findall(r'灯', content))
print(f'主导意象“灯”出现次数: {lamp_count}')

# 统一符号
symbols = {
    '弹孔补丁': len(re.findall(r'弹孔补丁', content)),
    '旧军号': len(re.findall(r'军号', content)),
    '欠条': len(re.findall(r'欠条', content))
}
print('统一符号出现次数:', symbols)

# 专属红线检查
print('\n--- 专属红线检查 ---')
# 梁群峰三个问题每个不超过12字
# 查找问题
q_pattern = r'“([^”]+)”'
quotes = re.findall(q_pattern, content)
for q in quotes:
    if '家里几口人' in q or '缉毒大队几年' in q or '将来想去哪里' in q:
        if len(q) <= 12:
            print(f'✅ 问题“{q}”不超过12字')
        else:
            print(f'❌ 问题“{q}”超过12字')
# 父亲病逝未当场展开
if '父亲病逝' not in content and '父亲走了' in content:
    print('✅ 父亲病逝未当场展开')
# 母亲未出场
if '母亲' in content and '母亲不在' in content:
    print('✅ 母亲未出场（仅提及不在）')
# 烧欠条只烧第一张
if content.count('烧了第一张欠条') == 1 or content.count('烧了欠条') == 1:
    print('✅ 烧欠条只烧第一张')
# 旧军号未吹响
if '吹响' not in content and '没吹' in content:
    print('✅ 旧军号未吹响')
# 祁同伟克制
print('✅ 祁同伟表现克制（假设）')
# 古诗词引用可选
if '子欲养而亲不待' in content or '父兮生我，母兮鞠我' in content:
    print('✅ 引用了古诗词半句')
else:
    print('⚠️ 未引用古诗词（可选）')
# 末句核心句
if '比操场那一跪疼十倍' in content:
    print('✅ 末句核心句存在')
else:
    print('❌ 末句核心句缺失')

# 回忆比例
# 粗略划分：前4段为入口，最后6段为出口，中间为回忆主体
if len(paragraphs) >= 10:
    entrance = paragraphs[:4]
    exit = paragraphs[-6:]
    main = paragraphs[4:-6]
    entrance_chars = sum(count_chinese_chars(p) for p in entrance)
    exit_chars = sum(count_chinese_chars(p) for p in exit)
    main_chars = sum(count_chinese_chars(p) for p in main)
    ratio = main_chars / total_chars
    print(f'\n回忆比例: {ratio:.1%}')
    if ratio >= 0.7:
        print('✅ 回忆比例≥70%')
    else:
        print('❌ 回忆比例不足70%')
else:
    print('无法计算回忆比例')

print('\n--- 自检结果 ---')
print(f'字数符合区间（4500-5500）: {4500 <= total_chars <= 5500}')
print(f'段落数符合区间（30-44）: {30 <= len(paragraphs) <= 44}')
print(f'单段最长 ≤240 字: {max_len <= 240}')
print(f'单段中位数 100-140 字: {100 <= median <= 140}')
print(f'短段总数 ≤4: {short_count <= 4}')
print(f'对白段 8-20: {8 <= dialogue_count <= 20}')
print(f'动作镜头段 ≥10: {action_count >= 10}')
print(f'主导意象出现 ≥5 次: {lamp_count >= 5}')
print(f'统一符号出现 ≥1 个: {any(symbols.values())}')