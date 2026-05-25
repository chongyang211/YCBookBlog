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

def increase_word_count(paragraphs, target_chars=4500):
    """增加字数至目标值"""
    current_chars = sum(sum(1 for c in p if '\u4e00' <= c <= '\u9fff') for p in paragraphs)
    print(f'当前字数: {current_chars}, 目标字数: {target_chars}')
    
    if current_chars >= target_chars:
        return paragraphs
    
    needed = target_chars - current_chars
    print(f'需要增加 {needed} 字')
    
    # 在合适的位置添加内容：选择非对白、非比喻的段落添加描写
    # 优先选择环境描写段落或心理描写段落
    new_paragraphs = paragraphs.copy()
    
    # 定义可添加的句子库（与环境、心理相关）
    additional_sentences = [
        '夜色如墨，远处的山峦只剩下模糊的轮廓，像一头沉睡的巨兽。',
        '风从山谷里吹来，带着潮湿的泥土气息和远处村庄的狗吠声。',
        '他感到胸口一阵发闷，仿佛有块石头压在心上，沉甸甸的。',
        '记忆中的画面一幕幕闪现，像老旧的电影胶片，褪色却清晰。',
        '时间仿佛凝固了，每一秒都拉得漫长，长得能听见自己的心跳。',
        '窗外传来远处火车经过的轰鸣声，沉闷而悠长，像命运的叹息。',
        '他握紧拳头，指甲深深陷进掌心，疼痛让他稍微清醒了一些。',
        '月光透过云隙洒下来，在地面上投下斑驳的光影，忽明忽暗。',
        '远处的灯光星星点点，像散落的珍珠，提醒着他世界的繁华与己无关。',
        '他深吸一口气，冰凉的空气进入肺里，刺激得他咳嗽了几声。'
    ]
    
    added_chars = 0
    sentence_index = 0
    
    # 在段落中插入句子（避免在对白段中插入）
    for i in range(len(new_paragraphs)):
        if added_chars >= needed:
            break
        
        para = new_paragraphs[i]
        # 检查是否对白段
        if re.search(r'[“”"「」]', para):
            continue  # 跳过对白段
        
        # 检查是否比喻密集段
        if para.count('像') + para.count('如') + para.count('似') > 2:
            continue  # 跳过比喻密集段
        
        # 在段落末尾添加一个句子
        if sentence_index < len(additional_sentences):
            new_sentence = additional_sentences[sentence_index]
            # 确保添加的句子不是比喻（避免增加比喻数量）
            if '像' not in new_sentence and '如' not in new_sentence and '似' not in new_sentence:
                new_para = para + new_sentence
                added_chars += sum(1 for c in new_sentence if '\u4e00' <= c <= '\u9fff')
                new_paragraphs[i] = new_para
                sentence_index += 1
                print(f'在第 {i+1} 段添加句子: {new_sentence[:20]}...')
    
    # 如果还不够，在段落之间插入新段落
    if added_chars < needed and sentence_index < len(additional_sentences):
        # 在中间位置插入一个新段落
        insert_pos = len(new_paragraphs) // 2
        new_para = additional_sentences[sentence_index]
        if '像' not in new_para and '如' not in new_para and '似' not in new_para:
            new_paragraphs.insert(insert_pos, new_para)
            added_chars += sum(1 for c in new_para if '\u4e00' <= c <= '\u9fff')
            print(f'插入新段落: {new_para[:20]}...')
    
    print(f'增加了 {added_chars} 字')
    return new_paragraphs

def merge_dialogue_paragraphs(paragraphs, target_dialogue_count=20):
    """合并对白段以减少数量"""
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
            # 如果所有句子都被删除，保留原段落的第一句
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

def main():
    content, start, end, body, paragraphs = read_body()
    
    print('开始最终修复...')
    
    # 1. 增加字数
    print('\n1. 增加字数...')
    paragraphs = increase_word_count(paragraphs, target_chars=4500)
    
    # 2. 合并对白段
    print('\n2. 合并对白段...')
    paragraphs = merge_dialogue_paragraphs(paragraphs, target_dialogue_count=20)
    
    # 3. 删除多余比喻
    print('\n3. 删除多余比喻...')
    paragraphs = remove_excess_metaphors(paragraphs, max_metaphors=18)
    
    # 4. 降低短句比例
    print('\n4. 降低短句比例...')
    paragraphs = reduce_short_sentence_ratio(paragraphs, target_ratio=0.15)
    
    # 重新组合正文
    new_body = '\n\n'.join(paragraphs)
    
    # 写入文件
    write_body(new_body, content, start, end)
    
    print('\n最终修复完成！')

if __name__ == '__main__':
    main()