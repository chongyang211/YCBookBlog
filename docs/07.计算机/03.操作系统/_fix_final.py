import re, os

DIR = "/Users/yc/YCBookBlog/docs/07.计算机/03.操作系统"

# Extract heading content (text after numbering prefix like "02." or "3.1 ")
def extract_heading_text(heading_line):
    # heading_line might start with "## " or "### " or be standalone
    stripped = heading_line.strip()
    # Remove ## or ### prefix and leading number
    m = re.match(r'#{2,3}\s+\d+\.?\d*\s*(.*)', stripped)
    if m:
        return m.group(1)
    return stripped

# Override map: full heading line -> new full heading line
overrides = {
    "01.进程与线程原理.md": {
        "## 02.什么是进程": "## 02.进程基本概念",
        "## 07.上下文切换": "## 07.上下文切换机制",
        "## 09.协程——更轻量的并发": "## 09.协程并发模型",
        "## 10.综合案例Web服务器": "## 10.Web服务器案例",
        "### 2.2 进程的组成": "### 2.2 进程组成详解",
        "### 2.4 进程=资源分配的基本单位": "### 2.4 进程资源分配单位",
        "### 3.1 五状态模型": "### 3.1 进程五状态模型",
        "### 3.2 七状态模型": "### 3.2 进程七状态模型",
        "### 4.2 Linux中的task_struct": "### 4.2 task_struct结构",
        "### 5.2 线程是什么": "### 5.2 线程定义与特点",
        "### 5.3 线程的组成": "### 5.3 线程组成结构",
        "### 6.1 用户级线程": "### 6.1 用户态线程模型",
        "### 6.2 内核级线程": "### 6.2 内核态线程模型",
        "### 7.4 系统调用vs上下文切换": "### 7.4 系统调用与切换",
        "### 9.1 协程是什么": "### 9.1 协程基本概念",
        "### 9.4 协程的调度": "### 9.4 协程调度机制",
        "### 10.1 场景与需求": "### 10.1 场景需求分析",
        "### 11.1 基础思考题": "### 11.1 基础思考题目",
        "### 11.2 进阶思考题": "### 11.2 进阶思考题目",
        "### 11.3 动手作业": "### 11.3 动手实践作业",
        "## 03.进程的状态与生命周期": "## 03.进程状态与模型",
    },
    "02.处理器调度策略.md": {
        "## 02.调度概述": "## 02.调度基本概念",
        "## 05.优先级调度": "## 05.优先级调度算法",
        "## 08.Linux CFS完全公平调度": "## 08.CFS完全公平调度",
        "## 09.实时调度": "## 09.实时调度算法",
        "## 10.多核调度": "## 10.多核调度算法",
        "## 11.综合案例延迟敏感服务调度": "## 11.延迟敏感服务调度",
        "### 1.1 为什么Nginx明明不忙却被投诉": "### 1.1 Nginx被投诉案例",
        "### 2.3 调度的时机": "### 2.3 调度触发时机",
        "### 5.2 静态优先级vs动态优先级": "### 5.2 静态与动态优先级",
        "### 5.3 优先级反转": "### 5.3 优先级反转问题",
        "### 7.2 多级反馈队列MLFQ": "### 7.2 多级反馈队列",
        "### 8.3 虚拟运行时间vruntime": "### 8.3 虚拟运行vruntime",
        "### 8.5 nice值与权重映射": "### 8.5 nice与权重映射",
        "### 9.2 SCHED_FIFO和SCHED_RR": "### 9.2 FIFO与RR实时调度",
        "### 10.2 负载均衡": "### 10.2 负载均衡策略",
        "### 11.1 场景三层服务的调度困境": "### 11.1 三层服务调度困境",
        "### 11.2 第一层Nginx事件驱动": "### 11.2 Nginx事件驱动",
        "### 11.3 第二层业务微服务CPU密集": "### 11.3 业务CPU密集特点",
        "### 11.4 第三层存储服务IO密集": "### 11.4 存储IO密集特点",
        "### 12.1 基础思考题": "### 12.1 基础思考题目",
        "### 12.2 进阶思考题": "### 12.2 进阶思考题目",
        "### 12.3 动手作业": "### 12.3 动手实践作业",
    },
    "03.进程间通信机制.md": {
        "## 02.IPC概述": "## 02.IPC机制概述",
        "## 04.消息队列": "## 04.消息队列详解",
        "## 05.共享内存": "## 05.共享内存详解",
        "## 06.信号量Semaphore": "## 06.信号量机制",
        "## 09.mmap文件映射IPC": "## 09.mmap文件映射",
        "## 11.综合案例日志采集系统": "## 11.日志采集系统案例",
        "### 1.2 为什么要学进程间通信": "### 1.2 学习IPC的意义",
        "### 3.1 匿名管道": "### 3.1 匿名管道机制",
        "### 4.3 System V消息队列": "### 4.3 SystemV消息队列",
        "### 5.2 POSIX共享内存（推荐）": "### 5.2 POSIX共享内存",
        "### 5.3 System V共享内存": "### 5.3 SystemV共享内存",
        "### 6.2 共享内存+信号量实战": "### 6.2 共享内存与信号量",
        "### 7.1 信号是什么": "### 7.1 信号机制概述",
        "### 8.1 Socket IPC原理": "### 8.1 Socket通信原理",
        "### 8.2 Unix Domain Socket": "### 8.2 Unix本地Socket",
        "### 8.3 UDS vs TCP本地回环": "### 8.3 UDS与TCP回环",
        "### 9.1 mmap怎么用作IPC": "### 9.1 mmap用作IPC",
        "### 9.2 mmap IPC的性能特点": "### 9.2 mmap性能特点",
        "### 10.2 选型决策树": "### 10.2 IPC选型决策树",
        "### 11.1 场景Agent采集进程日志": "### 11.1 Agent日志采集场景",
        "### 11.3 方案二Unix Domain Socket": "### 11.3 UDS方案详解",
        "### 11.4 方案三共享内存环形缓冲": "### 11.4 共享内存环形缓冲",
        "### 12.1 基础思考题": "### 12.1 基础思考题目",
        "### 12.2 进阶思考题": "### 12.2 进阶思考题目",
        "### 12.3 动手作业": "### 12.3 动手实践作业",
    },
}

# Known heading text replacements for auto-generated cases (text only, no "## " prefix)
known_fixes = {
    "task_struct结构": "task结构体",
    "进程的状态与生命周期": "进程生命周期",
    "Nginx被投诉案例": "Nginx投诉案例",
    "虚拟运行vruntime": "vruntime原理",
    "FIFO与RR实时调度": "FIFO与RR调度",
    "SystemV消息队列": "SysV消息队列",
    "SystemV共享内存": "SysV共享内存",
    "Socket通信原理": "Socket原理",
    "Unix本地Socket": "UDS本地通信",
    "mmap文件映射IPC": "mmapIPC映射",
    "Agent日志采集场景": "Agent采集场景",
    "调度概述": "调度基本概念",
    "优先级调度": "优先级算法",
    "实时调度": "实时算法",
    "多核调度": "多核算法",
    "IPC概述": "IPC概述分析",
    "消息队列": "消息队列机制",
    "共享内存": "共享内存机制",
    "信号量Semaphore": "信号量机制",
    "临界区Critical Section": "临界区机制",
    "关中断——单核时代的方案": "关中断方案",
    "Test-and-Set指令": "TAS指令",
    "条件变量": "条件变量机制",
    "死锁Deadlock": "死锁机制",
    "综合案例高并发转账系统": "转账系统案例",
    "内存碎片": "内存碎片分析",
    "分页管理": "分页管理机制",
    "快表TLB": "快表TLB机制",
    "分段管理": "分段管理机制",
    "malloc底层原理": "malloc原理",
    "内核内存分配Slab": "内核Slab机制",
    "内核也需要malloc": "内核malloc需求",
    "综合案例内存泄漏排查": "内存排查案例",
    "请求调页": "请求调页机制",
    "页替换算法——理论篇": "页替换理论",
    "页替换算法——实践篇": "页替换实践",
    "为什么 Linux 不用纯 Clock": "Linux非纯Clock",
    "帧分配策略": "帧分配策略",
    "系统抖动": "系统抖动分析",
    "Linux的页回收机制": "页回收机制",
    "kswapd与直接回收": "kswapd回收",
    "Linux页面状态机": "页面状态机",
    "综合案例Swap风暴排查": "Swap案例",
    "目录结构": "目录结构分析",
    "EXT4 vs XFS": "EXT4与XFS",
    "综合案例文件系统故障排查": "文件系统排查",
    "IO概述": "IO基本概念",
    "中断处理的全链路——从设备到进程": "中断全链路",
    "异步IO（Asynchronous IO）": "异步IO机制",
    "epoll的三个核心API": "epollAPI",
    "epoll的内核实现": "epoll内核",
    "io_uring——下一代异步IO": "io_uring机制",
    "io_uring vs epoll": "io_uring对比",
    "缓冲IO vs 直接IO vs mmap": "IO路径对比",
    "零拷贝——sendfile与splice": "零拷贝机制",
    "磁盘IO的完整路径——从BIO到扇区": "磁盘IO路径",
    "IO调度器——谁先谁后": "IO调度策略",
    "综合案例高并发echo服务器": "echo服务器",
    "驱动概述": "驱动概念",
    "内核模块": "内核模块机制",
    "设备号——内核如何找到驱动": "设备号机制",
    "设备驱动基础": "驱动基础",
    "块设备驱动": "块设备驱动",
    "sysfs——内核对象的窗口": "sysfs窗口",
    "设备树——ARM/RISC-V如何发现设备": "设备树机制",
    "综合案例编写一个LED驱动": "LED驱动案例",
    "虚拟化概述": "虚拟化概念",
    "KVM——硬件辅助虚拟化原理": "KVM原理",
    "Linux Namespace——隔离的七把锁": "Namespace隔离",
    "容器安全——Capability, Seccomp, AppArmor": "容器安全机制",
    "容器安全——Capability与Seccomp": "Cap与Seccomp",
    "Cgroups——资源限制的盾牌": "Cgroups限制",
    "容器网络——kube-proxy的iptables与IPVS": "kube-proxy",
    "联合文件系统OverlayFS": "OverlayFS文件系统",
    "Docker架构与docker run全流程": "Docker流程",
    "runc——OCI容器运行时": "runc运行时",
    "K8s基础": "K8s基础概念",
    "Pod——最小的调度单位": "Pod调度单位",
    "综合案例排查容器内存问题": "容器内存排查",
    "基础思考题": "基础思考题目",
    "进阶思考题": "进阶思考题目",
    "动手作业": "动手实践作业",
    "选择合适IPC": "IPC选择指南",
    "m映射IPC": "mmap文件映射",
}


def process_file(filename):
    path = os.path.join(DIR, filename)
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    changes = 0
    file_overrides = overrides.get(filename, {})
    lines = content.split('\n')

    for i, line in enumerate(lines):
        stripped = line.strip()
        # Match heading lines: ## xxx or ### xxx
        m = re.match(r'^(#{2,3}\s+\d+\.?\d*\s*)(.+)$', stripped)
        if not m:
            continue
        
        prefix = m.group(1)       # e.g. "## 02." or "### 3.1 "
        heading_text = m.group(2) # e.g. "什么是进程"
        full_heading_stripped = f"{prefix}{heading_text}".strip()
        
        # Check manual overrides (match by full line including ##/### prefix)
        if full_heading_stripped in file_overrides:
            new_full = file_overrides[full_heading_stripped]
            # Extract new_text from new_full
            nm = re.match(r'^(#{2,3}\s+\d+\.?\d*\s*)(.+)$', new_full.strip())
            if nm and nm.group(2):
                new_text = nm.group(2)
                orig_line = line
                lines[i] = line.replace(heading_text, new_text, 1)
                if lines[i] != orig_line:
                    changes += 1
                continue
        
        # Check known_fixes by heading text
        if heading_text in known_fixes:
            new_text = known_fixes[heading_text]
            if len(new_text) < 6 or len(new_text) > 9:
                continue  # skip bad fixes
            orig_line = line
            lines[i] = line.replace(heading_text, new_text, 1)
            if lines[i] != orig_line:
                changes += 1
            continue
        
        # Auto-fix if out of range and not in lookup
        if len(heading_text) < 6 or len(heading_text) > 9:
            # Remove —— suffix
            fixed = re.sub(r'[—\-–][^—\-–\n]*$', '', heading_text).strip()
            # Remove parentheticals
            fixed = re.sub(r'[\(（][^)）]*[\)）]', '', fixed).strip()
            # Remove "的" if still too long
            while len(fixed) > 9 and '的' in fixed:
                fixed = fixed.replace('的', '', 1)
            # Handle "vs"
            if len(fixed) > 9 and ' vs ' in fixed:
                fixed = fixed.replace(' vs ', '与')
            if len(fixed) > 9 and 'vs' in fixed:
                fixed = fixed.replace('vs', '与')
            # Remove spaces
            fixed = fixed.replace(' ', '')
            # Truncate if still > 9
            if len(fixed) > 9:
                fixed = fixed[:9]
            # Expand if < 6
            if len(fixed) < 6:
                for suff in ['详解', '机制', '概念', '分析']:
                    if 6 <= len(fixed) + len(suff) <= 9:
                        fixed = fixed + suff
                        break
                if len(fixed) < 6:
                    fixed = fixed + '论' * (6 - len(fixed))
            
            if fixed != heading_text and 6 <= len(fixed) <= 9:
                orig_line = line
                lines[i] = line.replace(heading_text, fixed, 1)
                if lines[i] != orig_line:
                    changes += 1

    # Also fix TOC entries
    new_content = '\n'.join(lines)
    for old_heading, new_heading in file_overrides.items():
        old_text = extract_heading_text(old_heading)
        new_text = extract_heading_text(new_heading)
        # Update TOC: [old_heading_text](# -> [new_heading_text](# 
        new_content = new_content.replace(f"[{old_text}](#", f"[{new_text}](#")
    
    for old_text, new_text in known_fixes.items():
        new_content = new_content.replace(f"[{old_text}](#", f"[{new_text}](#")

    if changes > 0 or new_content != '\n'.join(lines):
        # Final check: replace any remaining old-style TOC entries
        with open(path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"  ✅ {filename}: {changes} 处修改")
    else:
        print(f"  ⚠️ {filename}: 无需修改")


files = [
    "01.进程与线程原理.md", "02.处理器调度策略.md", "03.进程间通信机制.md",
    "04.同步与互斥机制.md", "05.内存管理原理.md", "06.虚拟内存机制.md",
    "07.文件系统原理.md", "08.输入输出模型.md", "09.设备驱动基础.md",
    "10.容器与虚拟化.md",
]

for fname in files:
    print(f"处理: {fname}")
    process_file(fname)

print("\n处理完成！")
