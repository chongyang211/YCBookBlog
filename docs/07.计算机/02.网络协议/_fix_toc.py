import re

path = "/Users/yc/YCBookBlog/docs/07.计算机/02.网络协议/05.网络编程模型的概念.md"

with open(path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

new_toc = [
    '- 01.工作案例引入\n',
    '  - 1.1 万人聊天室项目崩了\n',
    '  - 1.2 为什么要学网络编程模型\n',
    '- 02.客户端和服务端\n',
    '  - 2.1 网络编程概念\n',
    '  - 2.2 什么叫客户端\n',
    '  - 2.3 什么叫服务端\n',
    '  - 2.4 客户端到服务端流程\n',
    '- 03.IP和端口\n',
    '  - 3.1 为何需要IP和端口\n',
    '  - 3.2 理解IP地址\n',
    '  - 3.3 理解端口号\n',
    '  - 3.4 套接字的出现\n',
    '- 04.保留网段和子网\n',
    '  - 4.1 保留网络是何物\n',
    '  - 4.2 子网掩码是什么\n',
    '- 05.全球域名系统\n',
    '  - 5.1 域名解析IP\n',
    '  - 5.2 DNS域名系统\n',
    '  - 5.3 DNS服务分类\n',
    '  - 5.4 域名解析缓存\n',
    '  - 5.5 域名解析流程\n',
    '  - 5.6 DNS负载均衡\n',
    '- 06.数据和字节流\n',
    '  - 6.1 认识数据传输\n',
    '  - 6.2 TCP的应用场景\n',
    '  - 6.3 UDP的应用场景\n',
    '  - 6.4 字节流的设计\n',
    '- 07.IP地址深度剖析\n',
    '  - 7.1 IPv4地址分类\n',
    '  - 7.2 CIDR与超网\n',
    '  - 7.3 NAT地址转换\n',
    '  - 7.4 IPv6的设计\n',
    '- 08.端口号设计原理\n',
    '  - 8.1 知名端口分配\n',
    '  - 8.2 临时端口与范围\n',
    '  - 8.3 端口复用设计\n',
    '  - 8.4 端口扫描与防护\n',
    '- 09.网络编程范式\n',
    '  - 9.1 阻塞与非阻塞IO\n',
    '  - 9.2 同步与异步IO\n',
    '  - 9.3 Reactor模式\n',
    '  - 9.4 Proactor模式\n',
    '- 10.思考题与作业\n',
    '  - 10.1 基础思考题目\n',
    '  - 10.2 进阶思考题目\n',
    '  - 10.3 动手实践作业\n',
    '\n',
]

# Find the range: old TOC is from line with "- 01." to the line before "## 01."
start = None
end = None
for i, line in enumerate(lines):
    if line.strip().startswith('- 01.'):
        start = i
    if line.strip().startswith('## 01.') and start is not None:
        end = i
        break

if start is not None and end is not None:
    # Remove old TOC lines and blank lines between TOC and first heading
    del lines[start:end]
    # Insert new TOC at start position
    for j, l in enumerate(new_toc):
        lines.insert(start + j, l)
    print(f"TOC updated: replaced lines {start}-{end-1}")
else:
    print(f"Could not find TOC: start={start}, end={end}")

with open(path, 'w', encoding='utf-8') as f:
    f.writelines(lines)
