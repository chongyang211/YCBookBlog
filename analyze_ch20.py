#!/usr/bin/env python3
import re
import sys

def count_chinese_chars(text):
    """统计中文字符数（Unicode范围 \\u4e00-\\u9fff）"""
    chinese_pattern = re.compile(r'[\u4e00-\u9fff]')
    return len(chinese_pattern.findall(text))

def analyze_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 分割段落（空行分隔）
    paragraphs = [p.strip() for p in content.split('\n\n') if p.strip()]
    # 过滤掉可能存在的空段落
    paragraphs = [p for p in paragraphs if p]
    
    # 中文字符统计
    total_chars = count_chinese_chars(content)
    
    # 段落长度（中文字符）
    para_lengths = []
    for para in paragraphs:
        para_chars = count_chinese_chars(para)
        para_lengths.append(para_chars)
    
    # 统计信息
    num_paragraphs = len(paragraphs)
    max_length = max(para_lengths) if para_lengths else 0
    median_length = sorted(para_lengths)[len(para_lengths)//2] if para_lengths else 0
    short_paragraphs = sum(1 for l in para_lengths if l < 50)
    
    # 输出结果
    print(f"文件: {filepath}")
    print(f"中文字符数: {total_chars}")
    print(f"段落数: {num_paragraphs}")
    print(f"最长段落: {max_length} 字")
    print(f"中位数段落长度: {median_length} 字")
    print(f"短段落 (<50字): {short_paragraphs} 段")
    
    # T6 章节要求检查
    print("\n=== T6 留白冷章技术指标检查 ===")
    # 1. 字数区间 3500–4200
    if 3500 <= total_chars <= 4200:
        print(f"✅ 字数符合 T6 区间 (3500-4200): {total_chars}")
    else:
        print(f"❌ 字数不符合 T6 区间 (3500-4200): {total_chars}")
    
    # 2. 段落数 28–40
    if 28 <= num_paragraphs <= 40:
        print(f"✅ 段落数符合 T6 区间 (28-40): {num_paragraphs}")
    else:
        print(f"❌ 段落数不符合 T6 区间 (28-40): {num_paragraphs}")
    
    # 3. 单段最长 ≤240 字（通用要求）
    if max_length <= 240:
        print(f"✅ 最长段落 ≤240 字: {max_length}")
    else:
        print(f"❌ 最长段落 >240 字: {max_length}")
    
    # 4. 单段中位数 100-140 字（通用要求）
    if 100 <= median_length <= 140:
        print(f"✅ 中位数段落长度 100-140 字: {median_length}")
    else:
        print(f"❌ 中位数段落长度超出 100-140 字: {median_length}")
    
    # 5. 短段（<50 字）总数 ≤4（通用要求）
    if short_paragraphs <= 4:
        print(f"✅ 短段落数 ≤4: {short_paragraphs}")
    else:
        print(f"❌ 短段落数 >4: {short_paragraphs}")
    
    # 段落长度分布
    print(f"\n段落长度分布:")
    for i, (para, length) in enumerate(zip(paragraphs[:10], para_lengths[:10]), 1):
        print(f"  第{i}段: {length}字")
    if len(paragraphs) > 10:
        print(f"  ... 共 {len(paragraphs)} 段")
    
    return total_chars, num_paragraphs, max_length, median_length, short_paragraphs

if __name__ == '__main__':
    filepath = sys.argv[1] if len(sys.argv) > 1 else 'ch20_body.md'
    analyze_file(filepath)