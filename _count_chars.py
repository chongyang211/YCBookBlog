import re
p = '/Users/yc/YCBookBlog/book/09.出版作品的书籍/04.照见时代的人/第一卷：自我的觉醒/1.2 鲁迅《伤逝》自由的代价如此沉重.md'
with open(p) as f: c = f.read()
b = re.sub(r'^---.*?---\n', '', c, flags=re.DOTALL)
t = len(b.replace('\n','')); print(f'Body: {t}')
for s in b.split('\n## ')[1:]:
    l = s.split('\n'); title = l[0].strip(); txt = '\n'.join(l[1:])
    cnt = len(txt.replace('\n','')); ch = title.split('.',1)[1].strip() if '.' in title else title
    print(f'  {title}: {cnt} ({len(ch)}cn)')
lp = []
for i,p in enumerate(b.split('\n\n')):
    p=p.strip()
    if p and not p.startswith('#') and not p.startswith('---'):
        tx = re.sub(r'\*\*','',p).replace('\n','')
        if len(tx)>300: lp.append(f'Para #{i}: {len(tx)}')
print(f'Long paras: {lp if lp else "none"}')
