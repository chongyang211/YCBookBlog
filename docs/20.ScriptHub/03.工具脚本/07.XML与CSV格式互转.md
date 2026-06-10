---
title: XML与CSV
date: 2025-06-07 10:00:00
permalink: /pages/tool-xml-csv/
categories:
  - ScriptHub
  - 工具脚本
  - 格式转换
tags:
  - 工具
  - 格式
author:
  name: 杨充
  link: https://github.com/yangchong211
---
# XML与CSV

> XML 解析/转字典、CSV 读写/行列转置、XML↔JSON 互转、大文件流式处理。

## 一、Python XML 处理

### 1.1 xml.etree——标准库解析

```python
#!/usr/bin/env python3
"""XML 解析——标准库，无需安装"""
import xml.etree.ElementTree as ET

xml_str = """<?xml version="1.0"?>
<users>
  <user id="1">
    <name>Alice</name><email>alice@example.com</email>
  </user>
  <user id="2">
    <name>Bob</name><email>bob@example.com</email>
  </user>
</users>"""

# ---- 解析 ----
root = ET.fromstring(xml_str)
for user in root.findall('user'):
    uid = user.get('id')
    name = user.find('name').text
    email = user.find('email').text
    print(f"User {uid}: {name} <{email}>")

# ---- 构建 XML ----
new_root = ET.Element('users')
user = ET.SubElement(new_root, 'user', id='3')
ET.SubElement(user, 'name').text = 'Charlie'
tree = ET.ElementTree(new_root)
tree.write('output.xml', encoding='utf-8', xml_declaration=True)
```

### 1.2 xmltodict——XML↔字典（更像 JSON）

```bash
pip install xmltodict
```

```python
#!/usr/bin/env python3
import xmltodict, json

xml_str = "<user><name>Alice</name><age>30</age></user>"

# ---- XML → 字典 ----
data = xmltodict.parse(xml_str)
print(data)  # {'user': {'name': 'Alice', 'age': '30'}}

# ---- 字典 → XML ----
new_xml = xmltodict.unparse({'root': {'item': [{'@id': '1'}, {'@id': '2'}]}},
                             pretty=True)
print(new_xml)

# ---- XML → JSON（两步）----
d = xmltodict.parse(xml_str)
json_str = json.dumps(d, indent=2, ensure_ascii=False)
print(json_str)
```

## 二、Python CSV 处理

### 2.1 基础读写

```python
#!/usr/bin/env python3
import csv

# ---- 写入 CSV ----
with open('users.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['ID', 'Name', 'Email'])
    writer.writerows([
        [1, 'Alice', 'alice@mail.com'],
        [2, 'Bob', 'bob@mail.com'],
    ])

# ---- 读取 CSV ----
with open('users.csv', newline='') as f:
    reader = csv.reader(f)
    header = next(reader)
    for row in reader:
        print(dict(zip(header, row)))  # 转为字典
```

### 2.2 DictReader / DictWriter

```python
#!/usr/bin/env python3
import csv

# ---- DictWriter——用字典写 ----
with open('users.csv', 'w', newline='') as f:
    writer = csv.DictWriter(f, fieldnames=['name', 'age', 'city'])
    writer.writeheader()
    writer.writerows([
        {'name': 'Alice', 'age': 30, 'city': 'Beijing'},
        {'name': 'Bob', 'age': 25, 'city': 'Shanghai'},
    ])

# ---- DictReader——读成字典列表 ----
with open('users.csv', newline='') as f:
    reader = csv.DictReader(f)
    for row in reader:
        print(f"{row['name']}, {row['age']}, {row['city']}")

# ---- CSV → JSON ----
with open('users.csv', newline='') as f:
    data = list(csv.DictReader(f))
with open('users.json', 'w') as f:
    json.dump(data, f, indent=2, ensure_ascii=False)
```

### 2.3 大文件流式处理

```python
#!/usr/bin/env python3
"""CSV 大文件逐行处理——不占内存"""
import csv

def filter_large_csv(input_path, output_path, min_age=18):
    """流式过滤——百万行无压力"""
    with open(input_path) as fin, open(output_path, 'w', newline='') as fout:
        reader = csv.DictReader(fin)
        writer = csv.DictWriter(fout, fieldnames=reader.fieldnames)
        writer.writeheader()
        for row in reader:
            if int(row.get('age', 0)) >= min_age:
                writer.writerow(row)
```

## 三、实战：格式互转工具

```python
#!/usr/bin/env python3
"""XML/CSV/JSON 三向互转
用法: python convert.py data.xml data.json
"""
import sys, json, csv, xmltodict

def convert(input_path, output_path):
    with open(input_path) as f:
        in_ext = input_path.rsplit('.', 1)[-1]
        if in_ext == 'json':  data = json.load(f)
        elif in_ext == 'csv': data = list(csv.DictReader(f))
        elif in_ext == 'xml': data = xmltodict.parse(f.read())

    with open(output_path, 'w', newline='') as f:
        out_ext = output_path.rsplit('.', 1)[-1]
        if out_ext == 'json':
            json.dump(data, f, indent=2, ensure_ascii=False)
        elif out_ext == 'csv' and isinstance(data, list):
            w = csv.DictWriter(f, fieldnames=data[0].keys())
            w.writeheader(); w.writerows(data)
        elif out_ext == 'xml':
            f.write(xmltodict.unparse(data if isinstance(data, dict)
                                       else {'root': {'item': data}}, pretty=True))
    print(f"✅ {input_path} → {output_path}")

if __name__ == '__main__':
    convert(sys.argv[1], sys.argv[2])
```
