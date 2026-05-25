#!/usr/bin/env python3
import re
import statistics

def count_chinese_chars(text):
    # 统计中文字符（Unicode范围），排除标点、空格、换行
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', text)
    return len(chinese_chars)

def analyze_paragraphs(text):
    # 按空行分割段落
    paragraphs = [p.strip() for p in text.split('\n\n') if p.strip()]
    para_stats = []
    for i, para in enumerate(paragraphs):
        char_count = count_chinese_chars(para)
        # 计算句子数（粗略按句号、问号、感叹号分割）
        sentences = re.split(r'[。！？；]', para)
        sentences = [s.strip() for s in sentences if s.strip()]
        # 计算平均句长
        avg_sent_len = char_count / len(sentences) if sentences else 0
        # 判断是否对白段（包含引号或冒号）
        is_dialogue = ('“' in para or '”' in para or '：' in para)
        # 判断是否动作段（包含动作动词，简单判断）
        action_verbs = ['走', '站', '坐', '跪', '拿', '放', '擦', '烧', '握', '看', '盯', '转身', '起身', '抬头', '低头']
        has_action = any(verb in para for verb in action_verbs)
        para_stats.append({
            'index': i,
            'char_count': char_count,
            'sentences': sentences,
            'avg_sent_len': avg_sent_len,
            'is_dialogue': is_dialogue,
            'has_action': has_action
        })
    return para_stats

def main():
    with open('/Users/yc/YCBookBlog/ch19_body_split.md', 'r', encoding='utf-8') as f:
        text = f.read()
    
    para_stats = analyze_paragraphs(text)
    total_paragraphs = len(para_stats)
    
    # 计算总字数
    total_chars = sum(p['char_count'] for p in para_stats)
    
    # 最长段落
    longest = max(para_stats, key=lambda p: p['char_count'])
    longest_len = longest['char_count']
    longest_idx = longest['index'] + 1  # 1-based
    
    # 段落长度列表
    para_lengths = [p['char_count'] for p in para_stats]
    median_len = statistics.median(para_lengths)
    
    # 短段落数（<50字）
    short_paras = [p for p in para_stats if p['char_count'] < 50]
    short_count = len(short_paras)
    short_indices = [p['index']+1 for p in short_paras]
    
    # 对白段数
    dialogue_count = sum(1 for p in para_stats if p['is_dialogue'])
    
    # 动作段数
    action_count = sum(1 for p in para_stats if p['has_action'])
    
    # 长句（≥25字）和短句（≤11字）比例
    all_sentences = []
    for p in para_stats:
        all_sentences.extend(p['sentences'])
    
    long_sentences = [s for s in all_sentences if count_chinese_chars(s) >= 25]
    short_sentences = [s for s in all_sentences if count_chinese_chars(s) <= 11]
    total_sentences = len(all_sentences)
    long_ratio = len(long_sentences) / total_sentences if total_sentences > 0 else 0
    short_ratio = len(short_sentences) / total_sentences if total_sentences > 0 else 0
    
    # 比喻计数（简单模式）
    metaphor_patterns = [r'像', r'如', r'似', r'仿佛', r'宛若']
    metaphor_count = 0
    for para in text.split('\n\n'):
        for pattern in metaphor_patterns:
            metaphor_count += len(re.findall(pattern, para))
    
    # 三连排比（简单检测）
    # 这里简化：检测连续三个或以上相同结构的短句
    # 暂时跳过
    
    # 古诗词计数（检测引号或特定标记）
    poetry_pattern = r'《[^》]+》|「[^」]+」|"[^"]+"'
    poetry_matches = re.findall(poetry_pattern, text)
    poetry_count = len(poetry_matches)
    
    # 主导意象“灯”出现次数
    lamp_count = len(re.findall(r'灯', text))
    
    # 统一符号出现
    symbols = {
        '缉毒服三处弹孔补丁': len(re.findall(r'弹孔补丁|补丁', text)),
        '父亲坟前的旧军号': len(re.findall(r'军号', text)),
        '全村凑学费的欠条': len(re.findall(r'欠条', text))
    }
    
    print("=== 自检表统计数据 ===")
    print(f"总段落数: {total_paragraphs}")
    print(f"总字数: {total_chars}")
    print(f"最长段落: 第{longest_idx}段，{longest_len}字")
    print(f"段落长度中位数: {median_len:.1f}字")
    print(f"短段落数(<50字): {short_count}段，段号: {short_indices}")
    print(f"对白段数: {dialogue_count}")
    print(f"动作镜头段数: {action_count}")
    print(f"长句(≥25字)比例: {long_ratio:.1%}")
    print(f"短句(≤11字)比例: {short_ratio:.1%}")
    print(f"比喻总数: {metaphor_count}")
    print(f"古诗词句数: {poetry_count}")
    print(f"主导意象'灯'出现次数: {lamp_count}")
    print("统一符号出现:")
    for sym, count in symbols.items():
        print(f"  {sym}: {count}次")
    
    # 检查是否符合T3章节要求
    print("\n=== T3章节要求检查 ===")
    # 字数区间 4500-5500
    if 4500 <= total_chars <= 5500:
        print(f"✅ 字数符合 4500-5500: {total_chars}")
    else:
        print(f"❌ 字数不符合 4500-5500: {total_chars}")
    
    # 段数区间 35-48 (T3段数区间)
    if 35 <= total_paragraphs <= 48:
        print(f"✅ 段数符合 35-48: {total_paragraphs}")
    else:
        print(f"❌ 段数不符合 35-48: {total_paragraphs}")
    
    # 单段最长 ≤240字
    if longest_len <= 240:
        print(f"✅ 最长段落 ≤240字: {longest_len}")
    else:
        print(f"❌ 最长段落 >240字: {longest_len}")
    
    # 单段中位数 100-140字
    if 100 <= median_len <= 140:
        print(f"✅ 段落长度中位数 100-140字: {median_len:.1f}")
    else:
        print(f"❌ 段落长度中位数不在 100-140字: {median_len:.1f}")
    
    # 短段（<50字）总数 ≤4
    if short_count <= 4:
        print(f"✅ 短段落数 ≤4: {short_count}")
    else:
        print(f"❌ 短段落数 >4: {short_count}")
    
    # 对白段 ≥8 ≤20
    if 8 <= dialogue_count <= 20:
        print(f"✅ 对白段 8-20: {dialogue_count}")
    else:
        print(f"❌ 对白段不在 8-20: {dialogue_count}")
    
    # 动作镜头段 ≥10
    if action_count >= 10:
        print(f"✅ 动作镜头段 ≥10: {action_count}")
    else:
        print(f"❌ 动作镜头段 <10: {action_count}")
    
    # 长句比例 50%-70%
    if 0.50 <= long_ratio <= 0.70:
        print(f"✅ 长句比例 50%-70%: {long_ratio:.1%}")
    else:
        print(f"❌ 长句比例不在 50%-70%: {long_ratio:.1%}")
    
    # 短句比例 5%-15%
    if 0.05 <= short_ratio <= 0.15:
        print(f"✅ 短句比例 5%-15%: {short_ratio:.1%}")
    else:
        print(f"❌ 短句比例不在 5%-15%: {short_ratio:.1%}")
    
    # 比喻总数 ≤18
    if metaphor_count <= 18:
        print(f"✅ 比喻总数 ≤18: {metaphor_count}")
    else:
        print(f"❌ 比喻总数 >18: {metaphor_count}")
    
    # 古诗词 ≤2句
    if poetry_count <= 2:
        print(f"✅ 古诗词 ≤2句: {poetry_count}")
    else:
        print(f"❌ 古诗词 >2句: {poetry_count}")
    
    # 主导意象出现 ≥5次
    if lamp_count >= 5:
        print(f"✅ 主导意象'灯'出现 ≥5次: {lamp_count}")
    else:
        print(f"❌ 主导意象'灯'出现 <5次: {lamp_count}")
    
    # 统一符号出现 ≥1个
    symbol_present = any(count > 0 for count in symbols.values())
    if symbol_present:
        print(f"✅ 统一符号出现 ≥1个")
    else:
        print(f"❌ 统一符号未出现")
    
    return para_stats

if __name__ == '__main__':
    para_stats = main()