---
title: JSON与YAML
date: 2025-06-07 10:00:00
permalink: /pages/tool-json-yaml/
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
# JSON与YAML

> JSON 读写/美化/sort_keys、PyYAML 安全加载、JSON↔YAML 互转、CLI 一行格式化。

## 一、Python JSON 操作

### 1.1 基本读写

```python
#!/usr/bin/env python3
import json

# ---- 字典 → JSON 字符串 ----
data = {"name": "Alice", "age": 30, "skills": ["Python", "Go"]}
json_str = json.dumps(data)
print(json_str)  # {"name":"Alice","age":30,"skills":["Python","Go"]}

# ---- JSON 字符串 → 字典 ----
loaded = json.loads('{"name":"Bob","age":25}')
print(loaded["name"])  # Bob

# ---- 文件读写 ----
with open('config.json', 'w') as f:
    json.dump(data, f, indent=2)       # 写入文件

with open('config.json') as f:
    config = json.load(f)              # 读取文件
```

### 1.2 美化输出

```python
#!/usr/bin/env python3
import json

data = {"name": "Alice", "items": [1, 2, 3], "nested": {"key": "value"}}

# 缩进 + 排序 key
print(json.dumps(data, indent=2, sort_keys=True, ensure_ascii=False))
# 输出：
# {
#   "items": [1, 2, 3],
#   "name": "Alice",
#   "nested": {"key": "value"}
# }

# 紧凑格式（节省带宽）
print(json.dumps(data, separators=(',', ':')))  # {"name":"Alice","items":[1,2,3]}

# 处理 datetime / Decimal 等非标准类型
from datetime import datetime
class CustomEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, datetime):
            return obj.isoformat()
        return super().default(obj)

print(json.dumps({"time": datetime.now()}, cls=CustomEncoder))
```

### 1.3 JSON↔YAML 互转

```bash
pip install pyyaml
```

```python
#!/usr/bin/env python3
import json, yaml

# ---- JSON → YAML ----
with open('config.json') as f:
    data = json.load(f)
with open('config.yaml', 'w') as f:
    yaml.dump(data, f, allow_unicode=True, default_flow_style=False)

# ---- YAML → JSON ----
with open('config.yaml') as f:
    data = yaml.safe_load(f)          # safe_load 防代码注入！
with open('config.json', 'w') as f:
    json.dump(data, f, indent=2)

# ⚠️ 不要用 yaml.load (不安全)——始终用 yaml.safe_load
```

### 1.4 实战：JSON/YAML 互转 CLI 工具

```python
#!/usr/bin/env python3
"""json-yaml 互转命令行工具
用法: python convert.py data.json data.yaml       # JSON→YAML
      python convert.py data.yaml data.json       # YAML→JSON
"""
import sys, json, yaml

def convert(input_path, output_path):
    with open(input_path) as f:
        ext = input_path.rsplit('.', 1)[-1]
        data = yaml.safe_load(f) if ext in ('yaml', 'yml') else json.load(f)

    with open(output_path, 'w') as f:
        out_ext = output_path.rsplit('.', 1)[-1]
        if out_ext in ('yaml', 'yml'):
            yaml.dump(data, f, allow_unicode=True, default_flow_style=False)
        else:
            json.dump(data, f, indent=2, ensure_ascii=False)

    print(f"✅ {input_path} → {output_path}")

if __name__ == '__main__':
    convert(sys.argv[1], sys.argv[2])
```

## 二、Shell 一行 JSON/YAML

```bash
#!/bin/bash

# ===== JSON 格式化（python 内建）=====
echo '{"name":"Alice","age":30}' | python3 -m json.tool
# 输出美化 JSON

# ===== 用 jq 处理 JSON（需安装 brew/apt install jq）=====
echo '{"name":"Alice","age":30}' | jq .
cat data.json | jq '.name'               # 提取字段
cat data.json | jq '.items[]'            # 遍历数组
cat data.json | jq '.[] | select(.age > 25)'  # 条件过滤

# ===== YAML ↔ JSON（用 python）=====
python3 -c "import sys,json,yaml;json.dump(yaml.safe_load(sys.stdin),sys.stdout,indent=2)" < config.yaml
python3 -c "import sys,json,yaml;yaml.dump(json.load(sys.stdin),sys.stdout)" < config.json
```
