import re, os

target = '第9.2章《春》朱自清.md'
with open(target, 'r') as f:
    text = f.read()

text = re.sub(r'^---.*?---\n', '', text, flags=re.DOTALL)

sections = re.split(r'\n## ', text)

total = 0
for i, s in enumerate(sections):
    lines = s.strip().split('\n')
    title = lines[0].strip() if lines else '(no title)'
    body = '\n'.join(lines[1:]) if len(lines) > 1 else ''
    body_clean = re.sub(r'\s', '', body)
    total += len(body_clean)
    
    paragraphs = [p.strip() for p in body.split('\n') if p.strip()]
    long_paras = [(j, len(p), p[:50] + '...') for j, p in enumerate(paragraphs) if len(p) > 300]
    
    print(f'[{i}] {title}: {len(body_clean)} chars, {len(paragraphs)} paragraphs')
    if long_paras:
        for j, clen, preview in long_paras:
            print(f'    *** Para {j}: {clen} chars -> {preview}')

print(f'\n=== Total body chars: {total} ===')
