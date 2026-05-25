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

def remove_excess_metaphors(paragraphs, max_metaphors=18):
    """删除多余的比喻句，保留最多max_metaphors个"""
    print(f'原段落数: {len(paragraphs)}')
    
    # 统计每个段落的比喻句数量
    metaphor_count_per_para = []
    all_metaphor_sentences = []  # (para_idx, sent_idx, sentence)
    
    for para_idx, para in enumerate(paragraphs):
        sentences = re.split(r'[。！？；]', para)
        sentences = [s.strip() for s in sentences if s.strip()]
        
        for sent_idx, sent in enumerate(sentences):
            if '像' in sent or '如' in sent or '似' in sent or '仿佛' in sent or '好比' in sent or '犹如' in sent or '宛如' in sent or '好似' in sent:
                all_metaphor_sentences.append((para_idx, sent_idx, sent))
    
    print(f'总比喻句数: {len(all_metaphor_sentences)}')
    
    if len(all_metaphor_sentences) <= max_metaphors:
        return paragraphs
    
    # 需要删除的比喻句数量
    to_remove_count = len(all_metaphor_sentences) - max_metaphors
    
    # 确定哪些比喻句删除：优先删除重复、次要的比喻
    # 简单策略：删除后to_remove_count个比喻句（假设后面的可能冗余）
    to_remove = all_metaphor_sentences[-to_remove_count:]
    
    # 按段落分组删除
    para_sentences_dict = {}
    for para_idx, para in enumerate(paragraphs):
        sentences = re.split(r'[。！？；]', para)
        sentences = [s.strip() for s in sentences if s.strip()]
        para_sentences_dict[para_idx] = sentences
    
    # 标记要删除的句子
    for para_idx, sent_idx, sent in to_remove:
        if para_idx in para_sentences_dict:
            if sent_idx < len(para_sentences_dict[para_idx]):
                para_sentences_dict[para_idx][sent_idx] = None  # 标记删除
    
    # 重新构建段落
    new_paragraphs = []
    for para_idx in range(len(paragraphs)):
        if para_idx in para_sentences_dict:
            sentences = para_sentences_dict[para_idx]
            # 过滤掉标记为None的句子
            filtered = [s for s in sentences if s is not None]
            if filtered:
                # 重新组合句子，保留原标点（简化处理，用句号连接）
                new_para = '。'.join(filtered) + '。'
                new_paragraphs.append(new_para)
            else:
                # 如果段落所有句子都被删除，保留原段落（不应发生）
                new_paragraphs.append(paragraphs[para_idx])
        else:
            new_paragraphs.append(paragraphs[para_idx])
    
    print(f'删除了 {len(to_remove)} 个比喻句')
    return new_paragraphs

def add_dagger_symbol(paragraphs):
    """添加老所长的匕首符号"""
    # 在祁同伟心理挣扎处添加匕首回忆
    # 寻找合适位置：在段落14附近（他盯着那些名字，看了很久。）
    for i, para in enumerate(paragraphs):
        if '他盯着那些名字，看了很久。' in para:
            # 在此段落后面添加新段落
            dagger_para = '他想起老所长的匕首，那把匕首在月光下闪着寒光，曾经割断过毒贩的喉咙，也割断过他的犹豫。匕首的刀刃上还留着当年的血锈，就像他心里的伤，永远擦不干净。'
            paragraphs.insert(i + 1, dagger_para)
            print('已添加匕首符号')
            break
    return paragraphs

def merge_dialogue_paragraphs(paragraphs, target_dialogue_count=20):
    """合并对白段以减少对白段数量"""
    # 识别对白段
    dialogue_pattern = re.compile(r'[“”"「」].*?[“”"「」]|[:：][^。！？；]*[。！？；]')
    dialogue_flags = []
    for para in paragraphs:
        if dialogue_pattern.search(para):
            dialogue_flags.append(True)
        else:
            dialogue_flags.append(False)
    
    dialogue_count = sum(dialogue_flags)
    print(f'原对白段数: {dialogue_count}')
    
    if dialogue_count <= target_dialogue_count:
        return paragraphs
    
    # 需要合并的对白段数量
    need_merge = dialogue_count - target_dialogue_count
    
    # 寻找相邻的对白段进行合并
    new_paragraphs = []
    i = 0
    merged = 0
    while i < len(paragraphs):
        if merged < need_merge and i < len(paragraphs) - 1 and dialogue_flags[i] and dialogue_flags[i+1]:
            # 合并当前段落和下一个段落
            merged_para = paragraphs[i] + '\n\n' + paragraphs[i+1]
            new_paragraphs.append(merged_para)
            i += 2
            merged += 1
        else:
            new_paragraphs.append(paragraphs[i])
            i += 1
    
    print(f'合并了 {merged} 个对白段')
    return new_paragraphs

def reduce_short_sentences(paragraphs, target_short_ratio=0.15):
    """减少短句比例，通过合并短句"""
    # 统计所有句子
    all_sentences = []
    para_sentences_list = []
    
    for para in paragraphs:
        sentences = re.split(r'[。！？；]', para)
        sentences = [s.strip() for s in sentences if s.strip()]
        para_sentences_list.append(sentences)
        all_sentences.extend(sentences)
    
    # 计算短句比例
    short_sentences = [s for s in all_sentences if sum(1 for c in s if '\u4e00' <= c <= '\u9fff') <= 11]
    current_ratio = len(short_sentences) / len(all_sentences) if all_sentences else 0
    print(f'当前短句比例: {current_ratio:.1%}')
    
    if current_ratio <= target_short_ratio:
        return paragraphs
    
    # 需要减少短句数量
    target_short_count = int(len(all_sentences) * target_short_ratio)
    need_reduce = len(short_sentences) - target_short_count
    
    # 合并短句：在同一段落内合并相邻的短句
    new_para_sentences_list = []
    for sentences in para_sentences_list:
        new_sentences = []
        i = 0
        while i < len(sentences):
            if need_reduce > 0 and i < len(sentences) - 1:
                # 检查当前句子和下一个句子是否都是短句
                curr_len = sum(1 for c in sentences[i] if '\u4e00' <= c <= '\u9fff')
                next_len = sum(1 for c in sentences[i+1] if '\u4e00' <= c <= '\u9fff')
                if curr_len <= 11 and next_len <= 11:
                    # 合并两个短句
                    merged = sentences[i] + '，' + sentences[i+1]
                    new_sentences.append(merged)
                    i += 2
                    need_reduce -= 1
                else:
                    new_sentences.append(sentences[i])
                    i += 1
            else:
                new_sentences.append(sentences[i])
                i += 1
        new_para_sentences_list.append(new_sentences)
    
    # 重新构建段落
    new_paragraphs = []
    for sentences in new_para_sentences_list:
        if sentences:
            new_para = '。'.join(sentences) + '。'
            new_paragraphs.append(new_para)
        else:
            new_paragraphs.append('')
    
    print(f'减少了短句数量，还需减少 {need_reduce} 个')
    return new_paragraphs

def reduce_total_length(paragraphs, target_chars=5500):
    """减少总字数到目标值"""
    # 计算当前总字数
    total_chars = 0
    for para in paragraphs:
        total_chars += sum(1 for c in para if '\u4e00' <= c <= '\u9fff')
    
    print(f'当前总字数: {total_chars}')
    
    if total_chars <= target_chars:
        return paragraphs
    
    need_remove = total_chars - target_chars
    print(f'需要删除 {need_remove} 字')
    
    # 策略：删除冗余的环境描写段落和重复心理描写
    # 识别环境描写段落（包含大量自然景物描写）
    env_keywords = ['花', '树', '风', '云', '天', '地', '光', '影', '色', '叶', '枝', '絮', '霜', '雪', '雨', '雾']
    # 识别心理描写段落（包含大量内心活动）
    psycho_keywords = ['想', '觉得', '心里', '心中', '内心', '暗想', '暗道', '回忆', '想起', '记得']
    
    new_paragraphs = []
    removed_chars = 0
    for para in paragraphs:
        if removed_chars >= need_remove:
            new_paragraphs.append(para)
            continue
        
        # 检查段落类型
        para_chars = sum(1 for c in para if '\u4e00' <= c <= '\u9fff')
        env_score = sum(1 for kw in env_keywords if kw in para)
        psycho_score = sum(1 for kw in psycho_keywords if kw in para)
        
        # 如果环境描写或心理描写分数较高，且字数较多，考虑删除
        if (env_score >= 3 or psycho_score >= 3) and para_chars > 100:
            # 删除整个段落
            removed_chars += para_chars
            print(f'删除段落，字数: {para_chars}, 内容: {para[:50]}...')
        else:
            new_paragraphs.append(para)
    
    # 如果删除的还不够，从较长段落中删除部分句子
    if removed_chars < need_remove:
        additional_need = need_remove - removed_chars
        # 从保留的段落中删除一些句子
        final_paragraphs = []
        for para in new_paragraphs:
            if additional_need <= 0:
                final_paragraphs.append(para)
                continue
            
            sentences = re.split(r'[。！？；]', para)
            sentences = [s.strip() for s in sentences if s.strip()]
            if len(sentences) <= 1:
                final_paragraphs.append(para)
                continue
            
            # 删除最后一个句子（通常可能是冗余的）
            removed_sentence = sentences.pop()
            removed_len = sum(1 for c in removed_sentence if '\u4e00' <= c <= '\u9fff')
            additional_need -= removed_len
            
            if sentences:
                new_para = '。'.join(sentences) + '。'
                final_paragraphs.append(new_para)
                print(f'从段落中删除一句，字数: {removed_len}, 内容: {removed_sentence[:50]}...')
            else:
                # 如果所有句子都被删除，跳过该段落
                pass
        
        new_paragraphs = final_paragraphs
    
    print(f'总共删除了约 {removed_chars} 字')
    return new_paragraphs

def adjust_paragraph_median(paragraphs, target_median=140):
    """调整段落长度中位数"""
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
    
    # 需要降低中位数：拆分一些较长的段落
    new_paragraphs = []
    for para in paragraphs:
        length = sum(1 for c in para if '\u4e00' <= c <= '\u9fff')
        if length > 180:  # 较长段落
            # 尝试按句子拆分
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
    
    # 重新计算中位数
    new_lengths = [sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in new_paragraphs]
    sorted_new = sorted(new_lengths)
    mid = len(sorted_new) // 2
    if len(sorted_new) % 2 == 0:
        new_median = (sorted_new[mid-1] + sorted_new[mid]) / 2
    else:
        new_median = sorted_new[mid]
    print(f'调整后段落中位数: {new_median}')
    
    return new_paragraphs

def main():
    content, start, end, body, paragraphs = read_body()
    
    print('开始精修章节...')
    
    # 1. 删除多余比喻
    print('\n1. 删除多余比喻...')
    paragraphs = remove_excess_metaphors(paragraphs, max_metaphors=18)
    
    # 2. 添加匕首符号
    print('\n2. 添加匕首符号...')
    paragraphs = add_dagger_symbol(paragraphs)
    
    # 3. 合并对白段
    print('\n3. 合并对白段...')
    paragraphs = merge_dialogue_paragraphs(paragraphs, target_dialogue_count=20)
    
    # 4. 减少短句比例
    print('\n4. 减少短句比例...')
    paragraphs = reduce_short_sentences(paragraphs, target_short_ratio=0.15)
    
    # 5. 减少总字数
    print('\n5. 减少总字数...')
    paragraphs = reduce_total_length(paragraphs, target_chars=5500)
    
    # 6. 调整段落中位数
    print('\n6. 调整段落中位数...')
    paragraphs = adjust_paragraph_median(paragraphs, target_median=140)
    
    # 重新组合正文
    new_body = '\n\n'.join(paragraphs)
    
    # 写入文件
    write_body(new_body, content, start, end)
    
    print('\n精修完成！')

if __name__ == '__main__':
    main()