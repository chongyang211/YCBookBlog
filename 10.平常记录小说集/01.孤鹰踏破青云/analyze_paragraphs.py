import re
import sys

def count_chinese_chars(text):
    """统计中文字符数"""
    return len(re.findall(r'[\u4e00-\u9fff]', text))

def count_total_chars_no_spaces(text):
    """统计总字符数（不含空格换行）"""
    return len(text.replace('\n', '').replace(' ', ''))

def analyze_paragraphs(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    # 找到正文开始和结束
    start = 0
    end = len(lines)
    for i, line in enumerate(lines):
        if '<!-- AI 输出粘贴区 -->' in line:
            start = i + 1
            break
    for i, line in enumerate(lines):
        if '## 章末沉淀' in line:
            end = i
            break
    
    body_lines = lines[start:end]
    body_text = '\n'.join(body_lines)
    
    # 按空行分割段落（连续非空行为一段）
    paragraphs = []
    current_para = []
    for line in body_lines:
        if line.strip() == '':
            if current_para:
                paragraphs.append('\n'.join(current_para))
                current_para = []
        else:
            current_para.append(line)
    if current_para:
        paragraphs.append('\n'.join(current_para))
    
    print(f"正文段落数: {len(paragraphs)}")
    print("-" * 50)
    
    total_chinese = 0
    for i, para in enumerate(paragraphs):
        chinese = count_chinese_chars(para)
        total = count_total_chars_no_spaces(para)
        total_chinese += chinese
        print(f"段落 {i+1}:")
        print(f"  中文字数: {chinese}")
        print(f"  总字符数: {total}")
        # 显示前100字符
        preview = para[:100].replace('\n', ' ')
        if len(para) > 100:
            preview += "..."
        print(f"  预览: {preview}")
        print()
    
    print(f"正文总中文字数: {total_chinese}")
    print(f"目标字数区间: 4500-5500")
    print(f"超出: {total_chinese - 5500 if total_chinese > 5500 else 0} 字")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python analyze_paragraphs.py <filepath>")
        sys.exit(1)
    analyze_paragraphs(sys.argv[1])