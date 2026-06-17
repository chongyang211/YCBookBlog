import re
import os

path = "/Users/yc/YCBookBlog/book/09.出版作品的书籍/05.中华历史故事/第五卷：盛世的密码/5.6 武则天称帝·打破偏见靠实力证明自己.md"
with open(path, 'r') as f:
    content = f.read()

body = re.sub(r'^---.*?---\n', '', content, flags=re.DOTALL)

# Split by double newline to get paragraphs
paragraphs = body.split('\n\n')
for i, para in enumerate(paragraphs):
    text = para.strip()
    if not text:
        continue
    # Check if it's a heading
    if text.startswith('#'):
        print(f"\n{'='*60}")
        print(f"[HEADING] {text}")
        print(f"{'='*60}")
    else:
        # Strip heading markers for counting
        clean = text.replace('\n', '')
        length = len(clean)
        flag = " *** OVER 300 ***" if length > 300 else ""
        preview = clean[:50] + ('...' if len(clean) > 50 else '')
        if length > 200:
            print(f"  [{length} chars]{flag} {preview}")
