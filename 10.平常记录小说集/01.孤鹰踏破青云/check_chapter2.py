#!/usr/bin/env python3
import re
import sys

def count_chinese_chars(text):
    chinese_pattern = re.compile(r'[\u4e00-\u9fff\u3000-\u303f\uff00-\uffef]')
    return len(chinese_pattern.findall(text))

def main():
    with open('/Users/yc/YCBookBlog/10.平常记录小说集/01.孤鹰踏破青云/01.第一卷·寒岭孤苗求学破局/第02章·独木桥间苦挣扎_new.md', 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # 找到正文开始和结束
    start = -1
    end = -1
    for i, line in enumerate(lines):
        if '<!-- AI 输出粘贴区 -->' in line:
            start = i + 1  # 下一行开始
        if start != -1 and line.strip() == '---' and i > start:
            end = i
            break
    if start == -1 or end == -1:
        print('找不到正文范围')
        return
    body_lines = lines[start:end]
    # 去除前后的空行
    while body_lines and body_lines[0].strip() == '':
        body_lines.pop(0)
    while body_lines and body_lines[-1].strip() == '':
        body_lines.pop(-1)
    body = ''.join(body_lines)
    # 统计
    char_count = count_chinese_chars(body)
    print(f'正文字数: {char_count}')
    # 段落数（非空行）
    paras = [line.strip() for line in body_lines if line.strip()]
    print(f'段落数: {len(paras)}')
    # 主导意象“尘”出现次数
    dust_count = body.count('尘')
    print(f'主导意象“尘”出现次数: {dust_count}')
    # 检查“旧棉袄”出现（统一符号二现）
    cotton_count = body.count('棉袄')
    print(f'“棉袄”出现次数: {cotton_count}')
    # 检查三件事是否发生
    # 1. 乡镇高中食宿简陋，祁同伟靠帮学校食堂搬煤、洗碗换口粮，成绩稳居全县第一。
    if '食堂' in body and '搬煤' in body and '洗碗' in body:
        print('✅ 第一件事: 食堂搬煤洗碗')
    else:
        print('❌ 第一件事缺失')
    # 2. 高考前一周母亲再次病倒，他深夜骑借来的自行车去三十里外的砖厂打工凑药费。
    if '母病' in body or '病倒' in body:
        if '砖厂' in body and '打工' in body:
            print('✅ 第二件事: 母病砖厂打工')
        else:
            print('❌ 砖厂打工缺失')
    else:
        print('❌ 母病缺失')
    # 3. 钩子：返程天将亮，砖厂红尘扑面，他自行车爆胎滚下土坡——回到学校时，班主任老周把一摞复习资料压在他桌上，没说一句话。
    if '爆胎' in body or '车胎' in body:
        if '老周' in body and '复习资料' in body:
            print('✅ 第三件事: 爆胎、老周给资料')
        else:
            print('❌ 老周给资料缺失')
    else:
        print('❌ 爆胎缺失')
    # 检查上承钩子：王老栓家煤油灯影
    if '王老栓' in body:
        print('✅ 上承钩子: 提到王老栓')
    else:
        print('❌ 未提王老栓')
    # 检查末段钩子：复习资料夹50元
    if '五十元' in body or '50元' in body:
        print('✅ 末段钩子: 资料夹50元')
    else:
        print('❌ 末段钩子缺失')
    # 检查专属红线：砖厂红尘必须出现≥5次
    # 我们统计“尘”已经包括红尘，但为了准确，可以统计“红尘”
    red_dust = body.count('红尘')
    print(f'“红尘”出现次数: {red_dust}')
    # 输出正文前100字
    print('\n正文开头100字:')
    print(body[:100])
    # 输出正文结尾100字
    print('\n正文结尾100字:')
    print(body[-100:])

if __name__ == '__main__':
    main()