import re, os

DIR = "/Users/yc/YCBookBlog/docs/07.计算机/03.操作系统"

def auto_fix_heading(text):
    """Auto-fix a heading to 6-9 chars range."""
    # Remove parenthesized English annotations: (xxx) or （xxx）
    text = re.sub(r'[\(（][^)）]*[\)）]', '', text).strip()
    # Remove "——xxx" or "-xxx" suffix descriptions
    text = re.sub(r'[—\-–][^—\-–\d]*$', '', text).strip()
    # Remove leading "——" or "-"
    text = re.sub(r'^[—\-–]', '', text).strip()
    # Remove excessive spaces
    text = re.sub(r'\s+', ' ', text).strip()
    
    if 6 <= len(text) <= 9:
        return text
    
    # Too long (>9 chars)
    if len(text) > 9:
        # Replace "vs" with "与"
        text = text.replace(' vs ', '与').replace('vs', '与')
        # Remove "的" to shorten
        text = text.replace('的', '')
        # Remove "了"  
        text = text.replace('了', '')
        # Remove "——" or "-"
        text = text.replace('——', '').replace('--', '')
        
        # Try "与" in place of long connectors
        if len(text) > 9 and '和' in text:
            # try removing connector
            pass
        
        # Remove common verbose patterns
        shortcuts = {
            'Linux ': 'L', 'Linux的': 'L',
            'Task_struct': 'task',
            '结构体': '结构',
            '——': '',
            'Virtual ': '',
            'Page Cache': '缓存',
            'swap': 'Swap',
            'swappiness': 'Swap',
            'malloc': '内存',
            'mmap': '映射',
            'free': '释放',
        }
        for k, v in shortcuts.items():
            if len(text) > 9 and k in text:
                text = text.replace(k, v)
        
        # Try removing descriptive suffixes
        text = re.sub(r'（[^）]*）', '', text)
        text = re.sub(r'\([^)]*\)', '', text)
    
    # Too short (<6 chars)
    if len(text) < 6:
        # Append common suffixes
        suffixes = ['机制', '详解', '方法', '原理', '概念']
        for s in suffixes:
            if len(text) + len(s) <= 9 and len(text) + len(s) >= 6:
                text += s
                break
        # Last resort: pad to 6
        while len(text) < 6:
            text += '论'
    
    # Final check: still too long, truncate
    if len(text) > 9:
        # Try removing "的" first
        text = text.replace('的', '')
        if len(text) > 9:
            text = text[:9]
    
    return text

def process_file(filename):
    path = os.path.join(DIR, filename)
    with open(path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    new_lines = []
    changes_made = 0
    
    for line in lines:
        stripped = line.strip()
        # Match ## or ### headings
        m = re.match(r'(#{2,3}\s+\d+\.\d*\s+)(.*)', stripped)
        if m:
            prefix = m.group(1)  # e.g. "### 1.1 "
            heading_text = m.group(2)
            if len(heading_text) < 6 or len(heading_text) > 9:
                new_text = auto_fix_heading(heading_text)
                if new_text != heading_text:
                    # Replace in this line
                    old_line = line
                    line = line.replace(heading_text, new_text, 1)
                    if line != old_line:
                        changes_made += 1
                        print(f"  {filename}: '{heading_text}' ({len(heading_text)}) → '{new_text}' ({len(new_text)})")
        
        # Also handle TOC entries: [heading text](#anchor
        m2 = re.match(r'(\s*-\s*\[)\d+\.\d+\s+(.*?)(\]\()#\d+', line)
        if m2:
            toc_text = m2.group(2).rstrip()
            # The TOC entry text might differ from heading if previously modified
            # Check if it's still problematic
            if len(toc_text) < 6 or len(toc_text) > 9:
                new_toc = auto_fix_heading(toc_text)
                if new_toc != toc_text:
                    line = line.replace(toc_text, new_toc, 1)
                    changes_made += 1
        
        new_lines.append(line)
    
    if changes_made > 0:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(''.join(new_lines))
        print(f"\n✅ {filename}: 修改了 {changes_made} 处\n")
    else:
        print(f"  {filename}: 无需修改")
    
    return changes_made


files = [
    "04.同步与互斥机制.md",
    "05.内存管理原理.md",
    "06.虚拟内存机制.md",
    "07.文件系统原理.md",
    "08.输入输出模型.md",
    "09.设备驱动基础.md",
    "10.容器与虚拟化.md",
]

total = 0
for fname in files:
    print(f"处理: {fname}")
    total += process_file(fname)

print(f"\n总计修改: {total} 处")
