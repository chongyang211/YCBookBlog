import re
import sys

def count_chinese_chars(text):
    # 统计中文字符（包括汉字和中文标点）
    # 使用正则匹配中文字符范围
    chinese_pattern = re.compile(r'[\u4e00-\u9fff\u3000-\u303f\uff00-\uffef]')
    return len(chinese_pattern.findall(text))

def main():
    file_path = "/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎.md"
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 计算总字符数
        total_chars = len(content)
        
        # 计算中文字符数
        chinese_chars = count_chinese_chars(content)
        
        # 计算段落数（按空行分隔）
        paragraphs = [p for p in content.split('\n\n') if p.strip()]
        para_count = len(paragraphs)
        
        # 计算每段字数
        para_lengths = []
        for para in paragraphs:
            para_chars = count_chinese_chars(para)
            para_lengths.append(para_chars)
        
        # 统计短段（<50字）数量
        short_paras = sum(1 for length in para_lengths if length < 50)
        
        print(f"文件: {file_path}")
        print(f"总字符数: {total_chars}")
        print(f"中文字符数: {chinese_chars}")
        print(f"段落数: {para_count}")
        print(f"短段(<50字)数量: {short_paras}")
        print(f"段落字数分布: 最小 {min(para_lengths) if para_lengths else 0}, 最大 {max(para_lengths) if para_lengths else 0}, 平均 {sum(para_lengths)/len(para_lengths) if para_lengths else 0:.1f}")
        
        # 检查是否符合4500-5500字范围
        if 4500 <= chinese_chars <= 5500:
            print("✅ 字数符合要求 (4500-5500字)")
        else:
            print(f"❌ 字数不符合要求: {chinese_chars}字 (应在4500-5500字范围内)")
            
        # 检查段落数是否在35-50段范围内
        if 35 <= para_count <= 50:
            print("✅ 段落数符合要求 (35-50段)")
        else:
            print(f"❌ 段落数不符合要求: {para_count}段 (应在35-50段范围内)")
            
        # 检查短段数量是否≤4
        if short_paras <= 4:
            print("✅ 短段数量符合要求 (≤4段)")
        else:
            print(f"❌ 短段数量过多: {short_paras}段 (应≤4段)")
            
    except Exception as e:
        print(f"错误: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()