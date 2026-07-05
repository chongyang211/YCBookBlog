#!/usr/bin/env python3
import random, re, os, glob

BASE = '/Users/yc/YCBookBlog/docs/12.Apps'
os.chdir(BASE)

# Collect existing hex permalinks
existing = set()
for f in glob.glob('**/*.md', recursive=True):
    with open(f) as fh:
        for line in fh:
            m = re.search(r'permalink:\s*/pages/([0-9a-f]{6})/', line)
            if m:
                existing.add(m.group(1))

files_to_fix = [
    ('01.Android提升进阶/02.库的解读/01.LeakCanary内存收集.md', 'android'),
    ('01.Android提升进阶/03.专栏博客/01.系统启动Zygote.md', 'androidblog'),
    ('02.iOS开发和进阶/00.README.md', 'iosapp'),
    ('02.iOS开发和进阶/01.ObjC编程入门/01.基础语法快速入门.md', 'ios'),
    ('02.iOS开发和进阶/03.专栏博客/06.AutoLayout布局系统.md', 'ios-layout'),
    ('02.iOS开发和进阶/03.专栏博客/13.多线程与GCD实践.md', 'ios-gcd'),
    ('02.iOS开发和进阶/03.专栏博客/11.网络层架构设计.md', 'ios-network'),
    ('02.iOS开发和进阶/03.专栏博客/08.CoreAnimation动画实战.md', 'ios-coreanim'),
    ('02.iOS开发和进阶/03.专栏博客/10.转场动画与导航设计.md', 'ios-transition'),
    ('02.iOS开发和进阶/03.专栏博客/16.KVO与KVC实现机制.md', 'ios-kvo'),
    ('02.iOS开发和进阶/03.专栏博客/09.ViewController容器管理.md', 'ios-vc-life'),
    ('02.iOS开发和进阶/03.专栏博客/07.自定义View绘制实践.md', 'ios-customview'),
    ('02.iOS开发和进阶/03.专栏博客/02.Runtime消息转发机制.md', 'ios-runtime'),
    ('02.iOS开发和进阶/03.专栏博客/20.跨平台与混编方案.md', 'ios-cross'),
    ('02.iOS开发和进阶/03.专栏博客/15.Block底层原理与应用.md', 'ios-block'),
    ('02.iOS开发和进阶/03.专栏博客/17.性能优化与启动加速.md', 'ios-perf'),
    ('02.iOS开发和进阶/03.专栏博客/05.事件响应与手势识别.md', 'ios-event'),
    ('02.iOS开发和进阶/03.专栏博客/19.安全加密与逆向防护.md', 'ios-security'),
    ('02.iOS开发和进阶/03.专栏博客/01.iOS系统架构概览.md', 'ios-arch'),
    ('02.iOS开发和进阶/03.专栏博客/03.RunLoop运行循环揭秘.md', 'ios-runloop'),
    ('02.iOS开发和进阶/03.专栏博客/12.数据持久化方案选型.md', 'ios-storage'),
    ('02.iOS开发和进阶/03.专栏博客/04.UIView生命周期与渲染.md', 'ios-view-life'),
    ('02.iOS开发和进阶/03.专栏博客/18.组件化与模块设计.md', 'ios-modular'),
    ('02.iOS开发和进阶/03.专栏博客/14.内存管理与ARC原理.md', 'ios-arc'),
    ('02.iOS开发和进阶/02.Swift编程入门/01.Switf入门到精通上.md', 'swift'),
]

used = {}
count = 0
for fpath, old_val in files_to_fix:
    while True:
        h = ''.join(random.choices('0123456789abcdef', k=6))
        if h not in existing and h not in used:
            used[h] = (fpath, old_val)
            existing.add(h)
            break

    filepath = os.path.join(BASE, fpath)
    with open(filepath) as f:
        content = f.read()

    old = f'permalink: /pages/{old_val}/'
    new = f'permalink: /pages/{h}/'
    if old not in content:
        print(f'⚠ SKIP {fpath}: old "{old}" not found')
        continue
    content = content.replace(old, new)
    with open(filepath, 'w') as f:
        f.write(content)
    print(f'✅ {fpath}: /pages/{old_val}/ → /pages/{h}/')
    count += 1

print(f'\nDone. Fixed {count}/{len(files_to_fix)} files.')
