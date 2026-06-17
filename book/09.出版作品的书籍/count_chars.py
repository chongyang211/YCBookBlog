import re
import os

files = [
    "05.中华历史故事/第五卷：盛世的密码/5.6 武则天称帝·打破偏见靠实力证明自己.md",
    "05.中华历史故事/第一卷：秩序的诞生/1.1 从零到一·大禹治水里的规则诞生.md",
]

for f in files:
    path = os.path.join("/Users/yc/YCBookBlog/book/09.出版作品的书籍", f)
    with open(path, 'r') as file:
        content = file.read()
    body = re.sub(r'^---.*?---\n', '', content, flags=re.DOTALL)
    cn = len(re.findall(r'[\u4e00-\u9fff]', body))
    total = len(re.sub(r'\s+', '', re.sub(r'[#*\-`>|]', '', body)))
    print(f"=== {f.split('/')[-1]} ===")
    print(f"  Chinese chars: {cn}")
    print(f"  Total content chars (no markup): {total}")
    # Count paragraph lengths
    paragraphs = body.split('\n\n')
    long_paras = [(i, p[:30], len(p.replace('\n',''))) for i, p in enumerate(paragraphs) if len(p.replace('\n','')) > 300]
    if long_paras:
        print(f"  Paragraphs >300 chars: {len(long_paras)}")
        for idx, preview, length in long_paras:
            print(f"    #{idx}: {length} chars - {preview}...")
    print()
