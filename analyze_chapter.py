#!/usr/bin/env python3
import re
import sys

def analyze_chapter(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 分割段落（空行分隔）
    paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
    
    # 过滤掉章节标题、分隔线等非正文段落
    # 保留以#开头的标题段落用于计数吗？先全部保留
    filtered_paragraphs = []
    for p in paragraphs:
        # 跳过纯分隔线
        if p.strip() in ['---', '**字数统计**', '**【第28章 沉淀建议行】**'] or p.startswith('```'):
            continue
        # 跳过章节标题行（包含#）
        if p.startswith('#') or p.startswith('**D. 上一章末段**'):
            continue
        filtered_paragraphs.append(p)
    
    print(f"总段落数（过滤后）: {len(filtered_paragraphs)}")
    
    # 统计每段字数（中文字符，不含标点空白）
    char_counts = []
    for p in filtered_paragraphs:
        # 移除标点符号和空白，只保留中文字符
        chinese_chars = re.findall(r'[\u4e00-\u9fff]', p)
        char_counts.append(len(chinese_chars))
    
    if char_counts:
        print(f"段落字数统计:")
        print(f"  最小: {min(char_counts)} 字")
        print(f"  最大: {max(char_counts)} 字")
        print(f"  中位数: {sorted(char_counts)[len(char_counts)//2]} 字")
        print(f"  平均: {sum(char_counts)/len(char_counts):.1f} 字")
        
        # 统计短段（<15字）
        short_paras = [c for c in char_counts if c < 15]
        print(f"  短段（<15字）数量: {len(short_paras)}")
        
        # 统计超长段（>280字）
        long_paras = [c for c in char_counts if c > 280]
        print(f"  超长段（>280字）数量: {len(long_paras)}")
    
    # 统计对白段（含引号的段）
    dialogue_paras = []
    for p in filtered_paragraphs:
        if '"' in p or '""' in p or '“' in p or '”' in p:
            dialogue_paras.append(p)
    print(f"对白段数量: {len(dialogue_paras)}")
    
    # 统计动作镜头段（包含特定动词）
    action_verbs = ['推', '拉', '抬', '掏', '转', '走', '跑', '站', '坐', '躺', '拿', '放', '按', '点', '拨', '打', '看', '盯', '闭', '睁', '握', '扔', '靠']
    action_paras = []
    for p in filtered_paragraphs:
        # 检查段落是否包含动作动词
        for verb in action_verbs:
            if verb in p:
                action_paras.append(p)
                break
    print(f"动作镜头段数量: {len(action_paras)}")
    
    # 总中文字符数
    total_chars = sum(char_counts)
    print(f"总中文字符数（不含标点空白）: {total_chars}")
    
    # 检查是否符合指标
    print("\n=== 符合性检查 ===")
    
    # 1. 全章段落总数 50–80 段
    para_count = len(filtered_paragraphs)
    if 50 <= para_count <= 80:
        print(f"✓ 段落总数: {para_count} (符合50-80)")
    else:
        print(f"✗ 段落总数: {para_count} (不符合50-80)")
    
    # 2. 单段字数中位数 60–120 字
    if char_counts:
        median = sorted(char_counts)[len(char_counts)//2]
        if 60 <= median <= 120:
            print(f"✓ 单段字数中位数: {median} (符合60-120)")
        else:
            print(f"✗ 单段字数中位数: {median} (不符合60-120)")
    
    # 3. 单段字数绝对上限 ≤ 280 字
    if max(char_counts) <= 280:
        print(f"✓ 单段字数上限: {max(char_counts)} (符合≤280)")
    else:
        print(f"✗ 单段字数上限: {max(char_counts)} (不符合≤280)")
    
    # 4. 单段字数绝对下限 ≥ 15 字（短独立句段除外，且全章不超过12段）
    short_count = len(short_paras)
    if short_count <= 12:
        print(f"✓ 短段（<15字）数量: {short_count} (符合≤12)")
    else:
        print(f"✗ 短段（<15字）数量: {short_count} (不符合≤12)")
    
    # 5. 全章对白段 ≥ 8 段，≤ 25 段
    dialogue_count = len(dialogue_paras)
    if 8 <= dialogue_count <= 25:
        print(f"✓ 对白段数量: {dialogue_count} (符合8-25)")
    else:
        print(f"✗ 对白段数量: {dialogue_count} (不符合8-25)")
    
    # 6. 全章动作镜头段 ≥ 12 段
    action_count = len(action_paras)
    if action_count >= 12:
        print(f"✓ 动作镜头段数量: {action_count} (符合≥12)")
    else:
        print(f"✗ 动作镜头段数量: {action_count} (不符合≥12)")
    
    # 7. 全章字数 4500–5500
    if 4500 <= total_chars <= 5500:
        print(f"✓ 总字数: {total_chars} (符合4500-5500)")
    else:
        print(f"✗ 总字数: {total_chars} (不符合4500-5500)")
    
    return {
        'paragraph_count': para_count,
        'total_chars': total_chars,
        'dialogue_count': dialogue_count,
        'action_count': action_count,
        'char_counts': char_counts
    }

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python analyze_chapter.py <chapter_file>")
        sys.exit(1)
    
    filepath = sys.argv[1]
    analyze_chapter(filepath)