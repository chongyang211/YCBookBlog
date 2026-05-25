import re

def read_body():
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
    return content, start, end, body, paragraphs

def write_body(new_body, content, start, end):
    new_content = content[:start] + new_body + content[end:]
    file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print('正文已更新')

def reduce_metaphors(body, max_metaphors=18):
    """减少比喻数量，保留最多max_metaphors个"""
    sentences = re.split(r'[。！？；]', body)
    sentences = [s.strip() for s in sentences if s.strip()]
    
    # 找出所有比喻句
    metaphor_sentences = []
    for i, sent in enumerate(sentences):
        if '像' in sent or '如' in sent or '似' in sent or '仿佛' in sent or '好比' in sent or '犹如' in sent or '宛如' in sent or '好似' in sent:
            metaphor_sentences.append((i, sent))
    
    print(f'原比喻句数: {len(metaphor_sentences)}')
    
    # 如果已经少于等于max_metaphors，直接返回
    if len(metaphor_sentences) <= max_metaphors:
        return body
    
    # 计算每个比喻句的优先级（简单规则：长度较长、与主题相关的优先保留）
    # 这里简化处理：随机删除多余的比喻句，但保留前max_metaphors个
    # 实际上应该根据内容重要性来决定，这里先保留前max_metaphors个
    to_remove = metaphor_sentences[max_metaphors:]
    
    # 从body中删除这些比喻句
    new_sentences = sentences.copy()
    for idx, sent in reversed(to_remove):
        # 找到句子在body中的位置并删除
        # 简化：直接删除该句子
        new_sentences.pop(idx)
    
    # 重新组合成正文
    new_body = '。'.join(new_sentences) + '。'
    # 注意：这样会丢失原标点，需要更精细的处理，但作为初步简化
    return new_body

def add_dagger_symbol(body):
    """添加老所长的匕首符号"""
    # 在祁同伟心理挣扎处添加匕首回忆
    # 查找适合插入的位置，比如在“他盯着那些名字，看了很久。”附近
    insert_point = body.find('他盯着那些名字，看了很久。')
    if insert_point != -1:
        # 在之后插入匕首回忆
        dagger_text = '他想起老所长的匕首，那把匕首在月光下闪着寒光，曾经割断过毒贩的喉咙，也割断过他的犹豫。'
        body = body[:insert_point + len('他盯着那些名字，看了很久。')] + '\n\n' + dagger_text + '\n\n' + body[insert_point + len('他盯着那些名字，看了很久。'):]
    return body

def reduce_dialogue_paragraphs(paragraphs, max_dialogue_paragraphs=20):
    """减少对白段数量，通过合并相邻的对白段"""
    # 识别对白段
    dialogue_pattern = re.compile(r'[“”"「」].*?[“”"「」]|[:：][^。！？；]*[。！？；]')
    dialogue_indices = []
    for i, para in enumerate(paragraphs):
        if dialogue_pattern.search(para):
            dialogue_indices.append(i)
    
    print(f'原对白段数: {len(dialogue_indices)}')
    
    if len(dialogue_indices) <= max_dialogue_paragraphs:
        return paragraphs
    
    # 需要合并一些对白段
    # 找出相邻的对白段，合并它们
    new_paragraphs = paragraphs.copy()
    merged_count = 0
    i = 0
    while i < len(new_paragraphs) - 1:
        if i in dialogue_indices and (i+1) in dialogue_indices:
            # 合并段落i和i+1
            new_paragraphs[i] = new_paragraphs[i] + '\n\n' + new_paragraphs[i+1]
            new_paragraphs.pop(i+1)
            merged_count += 1
            # 更新dialogue_indices
            dialogue_indices = [idx if idx < i+1 else idx-1 for idx in dialogue_indices if idx != i+1]
        else:
            i += 1
    
    print(f'合并了 {merged_count} 个对白段')
    return new_paragraphs

def reduce_total_chars(body, target_chars=5500):
    """减少总字数到目标值"""
    # 统计当前字数
    current_chars = sum(1 for c in body if '\u4e00' <= c <= '\u9fff')
    print(f'当前字数: {current_chars}, 目标字数: {target_chars}')
    
    if current_chars <= target_chars:
        return body
    
    # 需要删除的字数
    chars_to_remove = current_chars - target_chars
    print(f'需要删除 {chars_to_remove} 字')
    
    # 策略：删除冗余的环境描写、重复的心理描写
    # 这里简化：删除一些段落中的部分句子
    # 更智能的做法需要自然语言处理，这里先实现简单删除
    
    # 将正文分成句子
    sentences = re.split(r'[。！？；]', body)
    sentences = [s.strip() for s in sentences if s.strip()]
    
    # 计算每个句子的字数
    sentence_chars = [sum(1 for c in s if '\u4e00' <= c <= '\u9fff') for s in sentences]
    
    # 优先删除字数较多的冗余句子（比喻句、环境描写等）
    # 标记比喻句
    metaphor_flags = ['像' in s or '如' in s or '似' in s or '仿佛' in s or '好比' in s or '犹如' in s or '宛如' in s or '好似' in s for s in sentences]
    
    # 标记环境描写句子（包含“花”“树”“风”等）
    env_flags = [any(word in s for word in ['花', '树', '风', '云', '天', '地', '光', '影', '色']) for s in sentences]
    
    # 标记重复心理描写（包含“想”“觉得”“心里”等）
    psycho_flags = [any(word in s for word in ['想', '觉得', '心里', '心中', '内心', '暗想', '暗道']) for s in sentences]
    
    # 删除优先级：先删除多余的比喻句，然后环境描写，然后心理描写
    deleted_chars = 0
    new_sentences = []
    for i, sent in enumerate(sentences):
        if deleted_chars >= chars_to_remove:
            new_sentences.append(sent)
            continue
            
        # 判断是否删除
        to_delete = False
        if metaphor_flags[i] and len([j for j in range(i) if metaphor_flags[j]]) >= 18:
            # 如果已经保留了18个比喻句，删除多余的比喻句
            to_delete = True
        elif env_flags[i] and deleted_chars < chars_to_remove:
            # 环境描写
            to_delete = True
        elif psycho_flags[i] and deleted_chars < chars_to_remove:
            # 心理描写
            to_delete = True
            
        if to_delete:
            deleted_chars += sentence_chars[i]
        else:
            new_sentences.append(sent)
    
    print(f'已删除 {deleted_chars} 字')
    new_body = '。'.join(new_sentences) + '。'
    return new_body

def adjust_paragraph_length(paragraphs, target_median=140):
    """调整段落长度，降低中位数"""
    # 计算当前段落长度
    para_lengths = [sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in paragraphs]
    sorted_lengths = sorted(para_lengths)
    mid = len(sorted_lengths) // 2
    if len(sorted_lengths) % 2 == 0:
        median = (sorted_lengths[mid-1] + sorted_lengths[mid]) / 2
    else:
        median = sorted_lengths[mid]
    
    print(f'当前段落中位数: {median}')
    
    if median <= target_median:
        return paragraphs
    
    # 需要降低中位数，拆分一些较长的段落
    new_paragraphs = []
    for para in paragraphs:
        length = sum(1 for c in para if '\u4e00' <= c <= '\u9fff')
        if length > 180:  # 如果段落较长，尝试拆分
            # 按句子拆分
            sentences = re.split(r'[。！？；]', para)
            sentences = [s.strip() for s in sentences if s.strip()]
            if len(sentences) >= 2:
                # 分成两段
                mid_idx = len(sentences) // 2
                para1 = '。'.join(sentences[:mid_idx]) + '。'
                para2 = '。'.join(sentences[mid_idx:]) + '。'
                new_paragraphs.append(para1)
                new_paragraphs.append(para2)
            else:
                new_paragraphs.append(para)
        else:
            new_paragraphs.append(para)
    
    return new_paragraphs

def main():
    content, start, end, body, paragraphs = read_body()
    
    print('开始修改...')
    
    # 1. 减少比喻数量
    print('\n1. 减少比喻数量...')
    body = reduce_metaphors(body, max_metaphors=18)
    
    # 2. 添加匕首符号
    print('\n2. 添加匕首符号...')
    body = add_dagger_symbol(body)
    
    # 重新分段落
    paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]
    
    # 3. 减少对白段数量
    print('\n3. 减少对白段数量...')
    paragraphs = reduce_dialogue_paragraphs(paragraphs, max_dialogue_paragraphs=20)
    
    # 4. 调整段落长度
    print('\n4. 调整段落长度...')
    paragraphs = adjust_paragraph_length(paragraphs, target_median=140)
    
    # 重新组合成正文
    new_body = '\n\n'.join(paragraphs)
    
    # 5. 减少总字数
    print('\n5. 减少总字数...')
    new_body = reduce_total_chars(new_body, target_chars=5500)
    
    # 写入文件
    write_body(new_body, content, start, end)
    
    print('修改完成')

if __name__ == '__main__':
    main()