import re

# 读取正文文件
with open('/Users/yc/YCBookBlog/ch19_body.md', 'r', encoding='utf-8') as f:
    content = f.read()

# 提取正文部分（从“那盏灯是十五瓦的”开始，到“明天下午三点，去梁家坐坐。”）
# 实际上整个文件都是正文，但需要排除可能的标题行
lines = content.strip().split('\n')
# 假设正文就是整个文件
text = '\n'.join(lines)

# 按空行分割段落
paragraphs = [p.strip() for p in text.split('\n\n') if p.strip()]
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
print(f'单段最长: {max(paragraph_lengths)} 字（段号: {paragraph_lengths.index(max(paragraph_lengths)) + 1}）')
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
lamp_count = len(re.findall(r'灯', text))
print(f'主导意象“灯”出现次数: {lamp_count}')

# 统一符号检查
symbols = {
    '弹孔补丁': len(re.findall(r'弹孔补丁', text)),
    '旧军号': len(re.findall(r'军号', text)),
    '欠条': len(re.findall(r'欠条', text))
}
print('统一符号出现次数:', symbols)

# 输出段落长度详情
print('\n段落长度详情:')
for i, (para, length) in enumerate(zip(paragraphs[:10], paragraph_lengths[:10])):
    print(f'{i+1}: {length}字')
    # 打印前50字符
    # print(para[:50])

# 估算段位字数
# 我们需要根据T3骨架划分段落：①当下入口（前4段），②-⑤回忆主体（第5段到倒数第6段），⑥当下出口（最后6段）
# 但需要根据内容划分。先粗略按段落索引划分。
# 假设前4段是入口，最后6段是出口，中间是回忆主体。
if len(paragraphs) >= 10:
    entrance = paragraphs[:4]
    exit = paragraphs[-6:]
    main = paragraphs[4:-6]
    entrance_chars = sum(count_chinese_chars(p) for p in entrance)
    exit_chars = sum(count_chinese_chars(p) for p in exit)
    main_chars = sum(count_chinese_chars(p) for p in main)
    print(f'\n段位字数估算:')
    print(f'①当下入口: {entrance_chars}字 ({len(entrance)}段)')
    print(f'②-⑤回忆主体: {main_chars}字 ({len(main)}段)')
    print(f'⑥当下出口: {exit_chars}字 ({len(exit)}段)')
else:
    print('段落数不足，无法划分段位')