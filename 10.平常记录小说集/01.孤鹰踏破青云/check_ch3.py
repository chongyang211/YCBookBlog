#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
检查第3章《金榜题名有隐忧》正文合规性
"""

import re
import sys

def read_content():
    """读取正文内容"""
    with open('ch3_output.txt', 'r', encoding='utf-8') as f:
        content = f.read()
    # 找到正文开始（在"---"分隔符之后）
    parts = content.split('---')
    if len(parts) >= 4:
        # 第四个部分之后是正文
        body = '---'.join(parts[3:])
        # 去掉最后的字数统计部分
        if '**字数统计**' in body:
            body = body.split('**字数统计**')[0]
        return body.strip()
    return content

def count_chinese_chars(text):
    """统计中文字符数（不含标点空白）"""
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', text)
    return len(chinese_chars)

def count_paragraphs(text):
    """统计段落数"""
    paragraphs = [p.strip() for p in text.split('\n\n') if p.strip()]
    return len(paragraphs)

def check_dominate_image(text, image='尘'):
    """检查主导意象出现次数"""
    # 包括红尘、灰尘、车尘、煤尘、糠尘等变体
    pattern = re.compile(f'[红灰车煤糠]?{image}')
    matches = pattern.findall(text)
    return len(matches), matches

def check_unified_symbols(text):
    """检查统一符号出现情况"""
    symbols = []
    # 全村凑学费的欠条（二现）
    if text.count('欠条') >= 2 or text.count('借据') >= 2:
        symbols.append('全村凑学费的欠条（二现）')
    # 父亲坟前的旧军号（首现）
    if '军号' in text:
        symbols.append('父亲坟前的旧军号（首现）')
    return symbols

def check_three_events(text):
    """检查三件事是否发生"""
    events = []
    # 1. 祁同伟以全县第一考入汉东大学政法系，全村摆了一场粗酒席
    if ('全县第一' in text and '汉东大学政法系' in text and 
        ('粗酒席' in text or '摆了一桌' in text or '小米粥' in text)):
        events.append(('第一件事：全县第一+粗酒席', True))
    else:
        events.append(('第一件事：全县第一+粗酒席', False))
    
    # 2. 父亲在祠堂塞旧军号，说"少说话，多看人"
    if ('父亲' in text and '军号' in text and '少说话，多看人' in text):
        events.append(('第二件事：父亲塞军号+说那句话', True))
    else:
        events.append(('第二件事：父亲塞军号+说那句话', False))
    
    # 3. 钩子：穿打补丁衣服迷路在汉大校门口，陈海指引
    if ('打补丁' in text or '补丁' in text) and '汉大校门口' in text and '陈海' in text:
        events.append(('第三件事：补丁衣服+迷路+陈海指引', True))
    else:
        events.append(('第三件事：补丁衣服+迷路+陈海指引', False))
    
    return events

def check_red_lines(text):
    """检查专属红线"""
    red_lines = []
    
    # 1. 高育良、梁璐、陈阳、侯亮平不得出场
    forbidden_chars = ['高育良', '梁璐', '陈阳', '侯亮平']
    for char in forbidden_chars:
        if char in text:
            red_lines.append((f'红线1：{char}出场', False))
        else:
            red_lines.append((f'红线1：{char}未出场', True))
    
    # 2. 父亲只能说"少说话，多看人"这一句
    # 检查父亲是否说了其他话
    father_lines = re.findall(r'父亲[^。]*说[："「]([^"」]*)["」]', text)
    allowed = ['少说话，多看人']
    for line in father_lines:
        if line not in allowed:
            red_lines.append((f'红线2：父亲说了不允许的话"{line}"', False))
    if not red_lines or all('红线1' in r[0] for r in red_lines):
        red_lines.append(('红线2：父亲只说允许的话', True))
    
    # 3. 入学场景禁止写"他被同学嘲笑了"
    if '嘲笑' in text or '嘲笑了' in text:
        red_lines.append(('红线3：入学场景有嘲笑', False))
    else:
        red_lines.append(('红线3：入学场景无嘲笑', True))
    
    # 4. 旧军号不能吹响
    if '吹响' in text and '军号' in text:
        red_lines.append(('红线4：军号被吹响', False))
    else:
        red_lines.append(('红线4：军号未吹响', True))
    
    # 5. 至少3个有名有姓的老人/邻居出现
    # 检查王老栓、李木匠、王寡妇、赵铁匠、孙裁缝等
    named_chars = ['王老栓', '李木匠', '王寡妇', '赵铁匠', '孙裁缝', '老栓', '栓伯']
    found = [char for char in named_chars if char in text]
    if len(found) >= 3:
        red_lines.append((f'红线5：有{len(found)}个有名有姓人物', True))
    else:
        red_lines.append((f'红线5：只有{len(found)}个有名有姓人物', False))
    
    # 6. 不得展开汉东大学校园建筑细节（以门口为限）
    # 检查是否有校园内部描述
    campus_keywords = ['教学楼', '图书馆', '宿舍楼', '食堂', '操场']
    found_campus = [kw for kw in campus_keywords if kw in text]
    # 允许门口场景
    if '校门口' in text and len(found_campus) <= 1:  # 允许提到教学楼等但不展开
        red_lines.append(('红线6：校园描述未展开', True))
    else:
        red_lines.append((f'红线6：可能展开了校园细节', False))
    
    return red_lines

def check_hook_connection(text):
    """检查钩子连接"""
    # 上一章末段钩子：复习资料里的 50 元、"望君不辱寒门"
    # 本章开篇应接住：高考成绩公布日清晨，祁同伟从砖厂连夜赶回村口，村中老栓家挂出红纸
    if ('复习资料' in text or '五十元' in text or '50元' in text) and '红纸' in text and '砖厂' in text:
        return ('钩子连接：开篇接住上一章钩子', True)
    return ('钩子连接：开篇未明确接住钩子', False)

def main():
    """主函数"""
    text = read_content()
    print(f"正文长度: {len(text)} 字符")
    
    # 基本统计
    chinese_chars = count_chinese_chars(text)
    paragraphs = count_paragraphs(text)
    print(f"中文字符数: {chinese_chars}")
    print(f"段落数: {paragraphs}")
    
    # 主导意象
    dominate_count, matches = check_dominate_image(text, '尘')
    print(f"主导意象'尘'出现次数: {dominate_count}")
    if dominate_count < 5:
        print(f"  ❌ 不足5次！")
    else:
        print(f"  ✅ 符合≥5次要求")
    
    # 统一符号
    symbols = check_unified_symbols(text)
    print(f"统一符号: {', '.join(symbols) if symbols else '无'}")
    if len(symbols) >= 1:
        print(f"  ✅ 有统一符号调用")
    else:
        print(f"  ❌ 无统一符号调用")
    
    # 三件事
    print("\n三件事检查:")
    events = check_three_events(text)
    all_ok = True
    for event_name, ok in events:
        status = '✅' if ok else '❌'
        print(f"  {status} {event_name}")
        if not ok:
            all_ok = False
    if all_ok:
        print("  ✅ 三件事全部发生")
    else:
        print("  ❌ 三件事未全部发生")
    
    # 红线检查
    print("\n专属红线检查:")
    red_lines = check_red_lines(text)
    all_red_ok = True
    for line_name, ok in red_lines:
        status = '✅' if ok else '❌'
        print(f"  {status} {line_name}")
        if not ok:
            all_red_ok = False
    if all_red_ok:
        print("  ✅ 所有红线遵守")
    else:
        print("  ❌ 有红线违反")
    
    # 钩子连接
    print("\n钩子连接检查:")
    hook_conn = check_hook_connection(text)
    status = '✅' if hook_conn[1] else '❌'
    print(f"  {status} {hook_conn[0]}")
    
    # 总体评估
    print("\n" + "="*50)
    print("总体评估:")
    if (chinese_chars >= 4500 and chinese_chars <= 5500 and 
        dominate_count >= 5 and 
        len(symbols) >= 1 and 
        all_ok and 
        all_red_ok):
        print("✅ 第3章符合所有要求")
        return 0
    else:
        print("❌ 第3章不符合要求，需要修改")
        return 1

if __name__ == '__main__':
    sys.exit(main())