import re
import sys

def analyze_chapter(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # 找到正文开始（分隔符后）
    content_start = 0
    for i, line in enumerate(lines):
        if line.strip() == '═════════════════════════════════════════════════':
            content_start = i + 1
            break
    if content_start == 0:
        for i, line in enumerate(lines):
            if not line.startswith('【') and not line.startswith('-') and not line.startswith('════'):
                content_start = i
                break
    body_lines = lines[content_start:]
    body = ''.join(body_lines)
    # 计算中文字符数
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', body)
    chinese_count = len(chinese_chars)
    # 总字符数
    total_chars = len(body)
    # 段落数（按空行分隔）
    paragraphs = [p for p in body.split('\n\n') if p.strip()]
    para_count = len(paragraphs)
    # 粗略视角分析
    ming = 0
    an = 0
    wai = 0
    for p in paragraphs:
        if re.search(r'沈驰', p):
            ming += 1
        if re.search(r'李梅|童瑾.*心里|胸口|毕业照|指尖', p):
            an += 1
        if re.search(r'林建军|对岸|酒店|望远镜|耳机', p):
            wai += 1
    total_p = ming + an + wai
    if total_p > 0:
        ming_percent = ming/para_count*100
        an_percent = an/para_count*100
        wai_percent = wai/para_count*100
    else:
        ming_percent = an_percent = wai_percent = 0
    print(f'正文开始行号: {content_start}')
    print(f'正文总字符数: {total_chars}')
    print(f'中文字符数: {chinese_count}')
    print(f'段落数: {para_count}')
    print(f'明视角段落数: {ming} ({ming_percent:.1f}%)')
    print(f'暗视角段落数: {an} ({an_percent:.1f}%)')
    print(f'外视角段落数: {wai} ({wai_percent:.1f}%)')
    # 检查是否符合T0骨架六段结构（粗略）
    # 可以根据段落内容判断，但这里先输出段落分布
    print('前5段内容预览:')
    for i, p in enumerate(paragraphs[:5]):
        print(f'  [{i}] {p[:50]}...')
    print('后5段内容预览:')
    for i, p in enumerate(paragraphs[-5:]):
        print(f'  [{i+len(paragraphs)-5}] {p[:50]}...')

if __name__ == '__main__':
    filepath = sys.argv[1] if len(sys.argv) > 1 else '第10章·律师的灵魂拷问.md'
    analyze_chapter(filepath)