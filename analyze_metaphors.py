import re

file_path = '10.平常记录小说集/01.孤鹰踏破青云/02.第二卷·孤鹰砺刃以命换功/第16章·尊严前程难抉择.md'
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# 找到正文开始
start = content.find('合欢花开到第五天')
if start == -1:
    start = 0

# 找到正文结束：字数统计之前的 '---'
pattern = re.compile(r'\n---\n')
matches = list(pattern.finditer(content, start))
if matches:
    end = matches[0].start()
else:
    end = len(content)

body = content[start:end].strip()
paragraphs = [p.strip() for p in body.split('\n\n') if p.strip()]

# 找出所有比喻句
simile_pattern = re.compile(r'[^。！？；]*?像[^。！？；]*?[。！？；]|[^。！？；]*?如[^。！？；]*?[。！？；]|[^。！？；]*?似[^。！？；]*?[。！？；]|[^。！？；]*?仿佛[^。！？；]*?[。！？；]|[^。！？；]*?好比[^。！？；]*?[。！？；]|[^。！？；]*?犹如[^。！？；]*?[。！？；]|[^。！？；]*?宛如[^。！？；]*?[。！？；]|[^。！？；]*?好似[^。！？；]*?[。！？；]')
all_sentences = re.split(r'[。！？；]', body)
all_sentences = [s.strip() for s in all_sentences if s.strip()]

simile_sentences = []
for sent in all_sentences:
    if '像' in sent or '如' in sent or '似' in sent or '仿佛' in sent or '好比' in sent or '犹如' in sent or '宛如' in sent or '好似' in sent:
        simile_sentences.append(sent)

print(f'总比喻句数: {len(simile_sentences)}')
print('前20个比喻句:')
for i, sent in enumerate(simile_sentences[:20]):
    print(f'{i+1}: {sent}')
print('\n所有比喻句位置（段落号）:')
for i, para in enumerate(paragraphs, 1):
    sentences = re.split(r'[。！？；]', para)
    sentences = [s.strip() for s in sentences if s.strip()]
    for sent in sentences:
        if '像' in sent or '如' in sent or '似' in sent or '仿佛' in sent or '好比' in sent or '犹如' in sent or '宛如' in sent or '好似' in sent:
            print(f'段落 {i}: {sent[:50]}...')
            break