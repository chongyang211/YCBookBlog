import os, re, random, datetime

start = datetime.datetime(2017, 1, 1, 19, 50, 29)
end = datetime.datetime(2025, 12, 30, 19, 50, 29)
delta = (end - start).total_seconds()

count = 0
for root, dirs, files in os.walk('docs/09.算法'):
    for f in files:
        if not f.endswith('.md'):
            continue
        path = os.path.join(root, f)
        with open(path, 'r', encoding='utf-8') as fh:
            content = fh.read()

        rand_sec = random.randint(0, int(delta))
        new_date = (start + datetime.timedelta(seconds=rand_sec)).strftime('%Y-%m-%d %H:%M:%S')

        new_content = re.sub(r'^date:\s*.*$', f'date: {new_date}', content, flags=re.MULTILINE)

        if new_content != content:
            with open(path, 'w', encoding='utf-8') as fh:
                fh.write(new_content)
            count += 1
            print(f'{path}  ->  {new_date}')

print(f'\n共修改 {count} 个文件')
