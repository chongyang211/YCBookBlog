import re, os

DIR = "/Users/yc/YCBookBlog/docs/07.计算机/03.操作系统"

# Manual overrides for headings that need custom fixes
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

def auto_fix(text):
    """Smartly adjust heading text to 6-9 chars range."""
    # Handle very short (4-5 chars)
    if len(text) < 6:
        expansions = {
            "调度概述": "调度基本概念",
            "优先级调度": "优先级调度算法", 
            "实时调度": "实时调度算法",
            "多核调度": "多核调度算法",
            "IPC概述": "IPC机制概述",
            "消息队列": "消息队列详解",
            "共享内存": "共享内存详解",
            "信号量机制": "信号量Semaphore机制",
            "m映射IPC": "mmap文件映射",
            "驱动概述": "驱动基本概念",
            "驱动定义": "驱动机制定义",
            "模块定义": "模块基本定义",
            "设备号机制": "设备号工作机制",
            "模块是什么": "模块基本概念",
            "驱动是什么": "驱动基本定义",
            "请求调页": "请求调页机制",
            "系统抖动": "系统抖动分析",
            "帧分配策略": "帧分配策略分析",
            "文件系统概念": "文件系统基本概念",
            "链接分配方式": "链接分配方式详解",
            "连续分配": "连续分配方式",
            "索引分配": "索引分配方式",
            "目录的本质": "目录本质分析",
            "目录结构详解": "目录结构详细分析",
            "内存碎片": "内存碎片分析",
            "紧凑技术": "紧凑技术详解",
            "分页管理": "分页管理机制",
            "快表TLB": "快表TLB详解",
            "条件变量": "条件变量详解",
            "内存屏障": "内存屏障详解",
            "银行家算法": "银行家算法详解",
            "活锁与饥饿": "活锁饥饿分析",
            "IO概述": "IO基本概述",
            "什么是IO": "IO基本概念",
            "信号驱动IO": "信号驱动IO",
            "映射IPC": "mmap文件映射IPC",
            "虚拟化概述": "虚拟化基本概念",
            "K8s基础": "K8s基础概念",
            "场景与分析": "问题场景分析",
            "模块是什么": "模块基本概念",
            "调试与加载": "调试与加载流程",
        }
        if text in expansions:
            return expansions[text]
        # Generic expansion: append 详解/机制/概念
        for suffix in ["详解", "机制", "概念", "流程", "分析", "方法"]:
            if 6 <= len(text) + len(suffix) <= 9:
                return text + suffix
        return text + "详解"
    
    # Handle long text (>9 chars)
    if len(text) > 9:
        # Map common long headings to shorter versions
        known_shorts = {
            "进程的状态与生命周期": "进程生命周期",
            "临界区Critical Section": "临界区概念",
            "竞态条件Race Condition": "竞态条件概念",
            "并发编程的基本问题": "并发编程问题",
            "内存序与原子操作": "内存序原子操作",
            "综合案例高并发转账系统": "转账系统案例",
            "内存管理原理": "内存管理",
            "内存泄漏把Redis拖死了": "Redis内存泄漏",
            "为什么要学内存管理原理": "学习内存管理",
            "一次页表遍历的硬件级追踪": "页表硬件遍历",
            "NUMA下的内存访问不对称": "NUMA内存访问",
            "TLB命中率的实践意义": "TLB命中率意义",
            "TLB Shootdown——多核下的TLB一致性": "TLB一致性",
            "两级TLB的缓存层次": "TLB缓存层次",
            "VMA红黑树——Linux如何管理进程的地址空间": "VMA红黑树",
            "malloc底层原理": "malloc底层",
            "大块分配mmap直接映射": "大块mmap分配",
            "ptmalloc的chunk结构精解": "chunk结构精解",
            "分配器的双面刃——性能与安全": "分配器性能安全",
            "内核内存分配Slab": "内核Slab分配",
            "内核也需要malloc": "内核malloc需求",
            "Buddy分裂融合的一次完整追踪": "Buddy融合追踪",
            "三个内存分配标志GFP": "内存GFP标志",
            "Slab着色——防止Cache Line冲突": "Slab着色机制",
            "综合案例内存管理排查": "内存排查案例",
            "场景服务内存持续增长": "内存持续增长",
            "Kubernetes反复杀Pod的诡异现象": "K8s杀Pod现象",
            "虚拟内存解决什么问题": "虚拟内存作用",
            "虚拟地址空间远大于物理内存": "虚拟地址空间",
            "按需加载页": "页面按需加载",
            "一次缺页的指令级别追踪": "缺页指令追踪",
            "什么时候缺页不是\"故障\"": "缺页非故障场景",
            "LRU最近最久未使用": "LRU最近未使用",
            "LRU的两种硬件实现": "LRU硬件实现",
            "增强型Clock完整走一遍": "增强Clock演练",
            "为什么 Linux 不用纯 Clock": "Linux非纯Clock",
            "平均分配vs比例分配": "平均与比例分配",
            "全局替换vs局部替换": "全局局部替换",
            "什么是抖动": "抖动现象定义",
            "抖动的原因": "抖动原因分析",
            "工作集原理": "工作集模型",
            "kswapd与直接回收": "kswapd直接回收",
            "LRU双链表Active/Inactive": "LRU双链表机制",
            "swap分区与交换机制": "Swap交换机制",
            "反向映射：从物理页找到所有用它的进程": "反向映射机制",
            "Linux页面状态机": "页面状态机",
            "场景数据库性能断崖下跌": "数据库性能下降",
            "排查路线图": "排查路线分析",
            "删除文件后空间没释放": "删除空间未释放",
            "inode里存了什么": "inode存储内容",
            "inode与空间耗尽": "inode空间耗尽",
            "EXT4的物理布局与块组结构": "EXT4块组结构",
            "extent tree的深度探索": "extent探索",
            "用debugfs亲手探索inode和extent": "debugfs探索",
            "目录项的删除与内部碎片": "目录删除碎片",
            "跨文件系统拷贝的完整流程": "跨FS拷贝流程",
            "延迟分配——EXT4的性能武器": "延迟分配机制",
            "inode的完整生命周期——从创建到消亡": "inode生命周期",
            "EXT4与XFS架构": "EXT4与XFS",
            "根因与修复": "根因修复分析",
            "select崩溃案例": "select案例",
            "IO基本概念": "IO概念",
            "一次read的全链路": "read全链路",
            "DMA——数据如何绕过CPU到达内存": "DMA传输机制",
            "中断处理的全链路——从设备到进程": "中断全链路",
            "阻塞IO（Blocking IO）": "阻塞IO模型",
            "非阻塞IO（Non-blocking IO）": "非阻塞IO模型",
            "IO多路复用（I/O Multiplexing）": "IO多路复用",
            "信号驱动IO（Signal-driven IO）": "信号驱动IO",
            "异步IO模型（Asynchronous IO）": "异步IO模型",
            "select的工作原理": "select工作",
            "select的三个致命缺陷": "select缺陷",
            "select的代码示例": "select示例",
            "poll的三个遗留问题": "poll问题",
            "epoll的三个核心API": "epoll核心API",
            "epoll的数据结构": "epoll结构",
            "水平触发LT vs 边缘触发ET": "LT与ET触发",
            "epoll_create的内核视角": "epoll创建内核",
            "io_uring vs epoll": "io_uring与epoll",
            "缓冲与直接IO对比": "缓冲直接IO",
            "sendfile零拷贝": "sendfile拷贝",
            "磁盘IO完整路径": "磁盘IO路径",
            "高并发echo服务器": "echo服务器",
            "一行内核代码让服务器宕机了": "内核宕机代码",
            "为什么要学设备驱动": "学习设备驱动",
            "CPU限制实战": "CPU限制实践",
            "内存限制实战": "内存限制实践",
        }
        if text in known_shorts:
            return known_shorts[text]
        
        # Remove English annotations: (xxx)
        text = re.sub(r'[\(（][^)）]*[\)）]', '', text)
        # Remove —— descriptions
        text = re.sub(r'[—\-–][^—\-–\n]*', '', text).strip()
        text = re.sub(r'——.*', '', text).strip()
        # Remove "的"
        while len(text) > 9 and '的' in text:
            text = text.replace('的', '', 1)
        # Remove "了"
        while len(text) > 9 and '了' in text:
            text = text.replace('了', '', 1)
        # Truncate if still too long
        if len(text) > 9:
            text = text[:9]
    
    return text

def process_file(filename):
    path = os.path.join(DIR, filename)
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    changes = 0
    file_overrides = overrides.get(filename, {})
    
    lines = content.split('\n')
    for i, line in enumerate(lines):
        stripped = line.strip()
        # Match headings: ## X or ### X
        m = re.match(r'(#{2,3}\s+\d+\.?\d*\s+)(.*)', stripped)
        if not m:
            continue
        
        prefix = m.group(1)  # e.g. "## 02." or "### 3.1 "
        heading_text = m.group(2)
        
        # Check if this heading has a manual override
        full_heading = f"{prefix}{heading_text}".strip()
        if full_heading in file_overrides:
            new_heading = file_overrides[full_heading]
            # Extract the heading text from new heading
            new_m = re.match(r'(#{2,3}\s+\d+\.?\d*\s+)(.*)', new_heading.strip())
            if new_m:
                new_text = new_m.group(2)
                lines[i] = lines[i].replace(heading_text, new_text, 1)
                changes += 1
            continue
        
        # Auto-fix if still out of range
        if len(heading_text) < 6 or len(heading_text) > 9:
            new_text = auto_fix(heading_text)
            if new_text != heading_text and 6 <= len(new_text) <= 9:
                lines[i] = lines[i].replace(heading_text, new_text, 1)
                changes += 1
    
    if changes > 0:
        new_content = '\n'.join(lines)
        # Also fix TOC entries
        for old_line in content.split('\n'):
            m_toc = re.match(r'(\s*-\s*\[)\d+\.?\d*\s+(.*?)(\]\()', old_line)
            if m_toc:
                old_toc_text = m_toc.group(2).strip(' ')
                if len(old_toc_text) < 6 or len(old_toc_text) > 9:
                    new_toc = auto_fix(old_toc_text)
                    if new_toc != old_toc_text and 6 <= len(new_toc) <= 9:
                        new_content = new_content.replace(old_toc_text, new_toc, 1)
                        changes += 1
        
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
