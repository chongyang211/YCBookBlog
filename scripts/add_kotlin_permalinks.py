#!/usr/bin/env python3
import random, re, glob, os

os.chdir('/Users/yc/YCBookBlog')

existing = set()
for f in glob.glob('docs/**/*.md', recursive=True):
    with open(f) as fh:
        for line in fh:
            m = re.search(r'permalink:\s*/pages/([0-9a-f]{6})/', line)
            if m: existing.add(m.group(1))

base = 'docs/12.Apps/01.Android提升进阶/01.Kotlin精通'
files = [
    '01.基础语法与核心类型.md',
    '02.类与对象详尽讲解.md',
    '03.高阶特性应用入门.md',
    '04.空安全类型机制原理.md',
    '05.数据类与等值比较原理.md',
    '06.协程调度器运行原理.md',
    '07.内联与具化泛型原理.md',
    '08.委托代理与属性委托.md',
]

for fname in files:
    fpath = os.path.join(base, fname)
    while True:
        h = ''.join(random.choices('0123456789abcdef', k=6))
        if h not in existing:
            existing.add(h)
            break

    with open(fpath) as f:
        content = f.read()

    content = content.replace(
        'categories:\n',
        f'permalink: /pages/{h}/\ncategories:\n'
    )

    with open(fpath, 'w') as f:
        f.write(content)
    print(f'{fname} → /pages/{h}/')

print('Done.')
