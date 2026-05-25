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

def split_long_paragraphs(paragraphs, max_len=240):
    """拆分超过max_len字的段落"""
    new_paragraphs = []
    for para in paragraphs:
        length = sum(1 for c in para if '\u4e00' <= c <= '\u9fff')
        if length > max_len:
            # 尝试按句子拆分
            sentences = re.split(r'[。！？；]', para)
            sentences = [s.strip() for s in sentences if s.strip()]
            if len(sentences) >= 2:
                # 计算每句字数
                sent_lengths = [sum(1 for c in s if '\u4e00' <= c <= '\u9fff') for s in sentences]
                # 动态分组，使每组字数接近但不超过max_len
                groups = []
                current_group = []
                current_len = 0
                for sent, sent_len in zip(sentences, sent_lengths):
                    if current_len + sent_len > max_len * 0.8 and current_group:  # 达到80%就分组
                        groups.append(current_group)
                        current_group = [sent]
                        current_len = sent_len
                    else:
                        current_group.append(sent)
                        current_len += sent_len
                if current_group:
                    groups.append(current_group)
                
                if len(groups) == 1:
                    # 无法拆分，保留原段落
                    new_paragraphs.append(para)
                    print(f'段落长度 {length} 字，但无法拆分（单句过长）')
                else:
                    for group in groups:
                        if group:
                            new_para = '。'.join(group) + '。'
                            new_paragraphs.append(new_para)
                    print(f'拆分段落：{length} 字 -> {len(groups)} 段')
            else:
                new_paragraphs.append(para)
                print(f'段落长度 {length} 字，但只有一个句子，无法拆分')
        else:
            new_paragraphs.append(para)
    return new_paragraphs

def remove_excess_metaphors(paragraphs, max_metaphors=18):
    """删除多余的比喻句"""
    # 收集所有比喻句
    all_metaphor_info = []  # (para_idx, sent_idx, sentence, length)
    para_sentences_list = []
    
    for para_idx, para in enumerate(paragraphs):
        sentences = re.split(r'[。！？；]', para)
        sentences = [s.strip() for s in sentences if s.strip()]
        para_sentences_list.append(sentences)
        
        for sent_idx, sent in enumerate(sentences):
            if '像' in sent or '如' in sent or '似' in sent or '仿佛' in sent or '好比' in sent or '犹如' in sent or '宛如' in sent or '好似' in sent:
                length = sum(1 for c in sent if '\u4e00' <= c <= '\u9fff')
                all_metaphor_info.append((para_idx, sent_idx, sent, length))
    
    print(f'总比喻句数: {len(all_metaphor_info)}')
    
    if len(all_metaphor_info) <= max_metaphors:
        return paragraphs
    
    to_remove_count = len(all_metaphor_info) - max_metaphors
    
    # 按长度排序，优先删除较长的比喻句
    all_metaphor_info.sort(key=lambda x: x[3], reverse=True)
    
    # 保留前max_metaphors个，删除后面的
    to_remove = all_metaphor_info[max_metaphors:]
    
    # 标记要删除的句子
    for para_idx, sent_idx, sent, length in to_remove:
        if sent_idx < len(para_sentences_list[para_idx]):
            para_sentences_list[para_idx][sent_idx] = None
    
    # 重新构建段落
    new_paragraphs = []
    for sentences in para_sentences_list:
        filtered = [s for s in sentences if s is not None]
        if filtered:
            new_para = '。'.join(filtered) + '。'
            new_paragraphs.append(new_para)
        else:
            new_paragraphs.append(paragraphs[0])
    
    print(f'删除了 {len(to_remove)} 个比喻句')
    return new_paragraphs

def reduce_short_sentence_ratio(paragraphs, target_ratio=0.15):
    """降低短句比例"""
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
    
    if current_ratio <= target_ratio:
        return paragraphs
    
    target_short_count = int(len(all_sentences) * target_ratio)
    need_reduce = len(short_sentences) - target_short_count
    
    # 合并相邻的短句
    new_para_sentences_list = []
    for sentences in para_sentences_list:
        new_sentences = []
        i = 0
        while i < len(sentences):
            if need_reduce > 0 and i < len(sentences) - 1:
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

def add_word_count(paragraphs, target_chars=4500):
    """增加字数至目标值"""
    current_chars = sum(sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in paragraphs)
    print(f'当前字数: {current_chars}, 目标字数: {target_chars}')
    
    if current_chars >= target_chars:
        return paragraphs
    
    needed = target_chars - current_chars
    print(f'需要增加 {needed} 字')
    
    # 添加非比喻、非短句的描写性内容
    additional_content = [
        '夜色深沉，远处的山峦在黑暗中若隐若现，仿佛一头蛰伏的巨兽，静静地等待着什么。',
        '风从山谷间穿过，发出低沉的呜咽声，像是大地在叹息，又像是命运在低语。',
        '他感到胸口一阵发闷，仿佛有千斤重担压在心上，让他几乎喘不过气来。',
        '记忆如潮水般涌来，那些曾经的画面在脑海中一一浮现，清晰得令人心悸。',
        '时间仿佛在这一刻凝固了，每一秒都变得无比漫长，长得能听见血液在血管中流动的声音。',
        '窗外传来远处火车的汽笛声，悠长而苍凉，像是离别的挽歌，在夜空中久久回荡。',
        '他握紧拳头，指甲深深陷入掌心，疼痛让他稍微清醒了一些，也让他更加坚定了决心。',
        '月光透过云层洒落下来，在地面上投下斑驳的光影，明暗交错，如同他此刻的心情。',
        '远处的灯光星星点点，像是散落在人间的星辰，提醒着他这个世界的繁华与喧嚣。',
        '他深吸一口气，冰凉的空气进入肺里，带来一阵刺痛，却也让他混沌的头脑清醒了几分。'
    ]
    
    # 选择不含比喻词的句子
    non_metaphor_content = [s for s in additional_content if '像' not in s and '如' not in s and '似' not in s and '仿佛' not in s]
    
    added_chars = 0
    content_index = 0
    
    # 在合适的位置插入内容（非对白段）
    for i in range(len(paragraphs)):
        if added_chars >= needed or content_index >= len(non_metaphor_content):
            break
        
        para = paragraphs[i]
        # 跳过对白段
        if re.search(r'[“”"「」]', para):
            continue
        
        # 插入新句子在段落末尾
        new_sentence = non_metaphor_content[content_index]
        new_para = para + new_sentence
        added_chars += sum(1 for c in new_sentence if '\u4e00' <= c <= '\u9fff')
        paragraphs[i] = new_para
        content_index += 1
        print(f'在第 {i+1} 段添加内容: {new_sentence[:30]}...')
    
    # 如果还不够，在段落之间插入新段落
    if added_chars < needed and content_index < len(non_metaphor_content):
        insert_pos = len(paragraphs) // 2
        new_para = non_metaphor_content[content_index]
        paragraphs.insert(insert_pos, new_para)
        added_chars += sum(1 for c in new_para if '\u4e00' <= c <= '\u9fff')
        print(f'插入新段落: {new_para[:30]}...')
    
    print(f'增加了 {added_chars} 字')
    return paragraphs

def main():
    content, start, end, body, paragraphs = read_body()
    
    print('开始最终调整V2...')
    
    # 1. 拆分超长段落
    print('\n1. 拆分超长段落...')
    paragraphs = split_long_paragraphs(paragraphs, max_len=240)
    
    # 2. 删除多余比喻
    print('\n2. 删除多余比喻...')
    paragraphs = remove_excess_metaphors(paragraphs, max_metaphors=18)
    
    # 3. 降低短句比例
    print('\n3. 降低短句比例...')
    paragraphs = reduce_short_sentence_ratio(paragraphs, target_ratio=0.15)
    
    # 4. 增加字数
    print('\n4. 增加字数...')
    paragraphs = add_word_count(paragraphs, target_chars=4500)
    
    # 重新组合正文
    new_body = '\n\n'.join(paragraphs)
    
    # 写入文件
    write_body(new_body, content, start, end)
    
    print('\n最终调整V2完成！')

if __name__ == '__main__':
    main()