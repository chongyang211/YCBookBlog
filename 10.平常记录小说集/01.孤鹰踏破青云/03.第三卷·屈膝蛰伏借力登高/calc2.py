import re
with open('body.txt', 'r', encoding='utf-8') as f:
    body = f.read()
print('总字符数（包括空格标点）:', len(body))
paragraphs = [p.strip() for p in re.split(r'\n\s*\n', body) if p.strip()]
print('段落数:', len(paragraphs))
def count_hanzi(text):
    return sum(1 for c in text if '\u4e00' <= c <= '\u9fff')
lengths = [count_hanzi(p) for p in paragraphs]
if lengths:
    print('总汉字数:', sum(lengths))
    print('最长段落（汉字数）:', max(lengths))
    sorted_len = sorted(lengths)
    mid = len(sorted_len)//2
    if len(sorted_len) % 2 == 0:
        median = (sorted_len[mid-1] + sorted_len[mid]) / 2
    else:
        median = sorted_len[mid]
    print('中位段长（汉字数）:', median)
    short = sum(1 for l in lengths if l <= 50)
    print('短段落数（≤50汉字）:', short)
