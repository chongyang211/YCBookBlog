import re

def analyze_file(filename):
    issues = []
    with open(filename, 'r') as f:
        lines = f.readlines()
    for i, line in enumerate(lines):
        stripped = line.strip()
        if stripped.startswith('## ') or stripped.startswith('### '):
            heading = re.sub(r'^#+\s*', '', stripped).strip()
            content = re.sub(r'^\d+\.\d*\s*', '', heading).strip()
            length = len(content)
            if length < 6 or length > 9:
                issues.append((i+1, stripped, length, content))
    return issues

files = [
    "01.进程与线程原理.md", "02.处理器调度策略.md", "03.进程间通信机制.md",
    "04.同步与互斥机制.md", "05.内存管理原理.md", "06.虚拟内存机制.md",
    "07.文件系统原理.md", "08.输入输出模型.md", "09.设备驱动基础.md",
    "10.容器与虚拟化.md"
]

for fname in files:
    issues = analyze_file(fname)
    if issues:
        print(f"=== {fname} ===")
        for line_no, line, length, content in issues:
            print(f"  L{line_no}: [{length}字] {line.strip()}")
