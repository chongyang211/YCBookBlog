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
                # 计算每句字数，尝试均匀拆分
                sent_lengths = [sum(1 for c in s if '\u4e00' <= c <= '\u9fff') for s in sentences]
                # 动态分组，使每组字数接近但不超过max_len
                groups = []
                current_group = []
                current_len = 0
                for sent, sent_len in zip(sentences, sent_lengths):
                    if current_len + sent_len > max_len and current_group:
                        groups.append(current_group)
                        current_group = [sent]
                        current_len = sent_len
                    else:
                        current_group.append(sent)
                        current_len += sent_len
                if current_group:
                    groups.append(current_group)
                
                # 如果只有一组，说明单句就超长，无法拆分，保留原段落
                if len(groups) == 1:
                    new_paragraphs.append(para)
                    print(f'段落长度 {length} 字，但无法拆分（单句过长）')
                else:
                    for group in groups:
                        if group:
                            new_para = '。'.join(group) + '。'
                            new_paragraphs.append(new_para)
                    print(f'拆分段落：{length} 字 -> {len(groups)} 段')
            else:
                # 只有一个句子，无法拆分
                new_paragraphs.append(para)
                print(f'段落长度 {length} 字，但只有一个句子，无法拆分')
        else:
            new_paragraphs.append(para)
    return new_paragraphs

def reduce_metaphors(paragraphs, max_metaphors=18):
    """减少比喻数量，保留最多max_metaphors个"""
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
    
    # 需要删除的比喻句数量
    to_remove_count = len(all_metaphor_info) - max_metaphors
    
    # 按长度排序，优先删除较长的比喻句（可能冗余）
    all_metaphor_info.sort(key=lambda x: x[3], reverse=True)  # 从长到短
    
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
            # 如果所有句子都被删除，保留原段落的第一句（不应发生）
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
    
    # 需要减少的短句数量
    target_short_count = int(len(all_sentences) * target_ratio)
    need_reduce = len(short_sentences) - target_short_count
    
    # 策略：合并相邻的短句
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

def main():
    content, start, end, body, paragraphs = read_body()
    
    print('开始最终调整...')
    
    # 1. 拆分超长段落
    print('\n1. 拆分超长段落...')
    paragraphs = split_long_paragraphs(paragraphs, max_len=240)
    
    # 2. 减少比喻数量
    print('\n2. 减少比喻数量...')
    paragraphs = reduce_metaphors(paragraphs, max_metaphors=18)
    
    # 3. 降低短句比例
    print('\n3. 降低短句比例...')
    paragraphs = reduce_short_sentence_ratio(paragraphs, target_ratio=0.15)
    
    # 重新组合正文
    new_body = '\n\n'.join(paragraphs)
    
    # 写入文件
    write_body(new_body, content, start, end)
    
    print('\n最终调整完成！')

if __name__ == '__main__':
    main()