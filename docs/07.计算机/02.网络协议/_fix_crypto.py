import re

FILE = "/Users/yc/YCBookBlog/docs/07.计算机/02.网络协议/02.通过购物熟悉加密.md"

with open(FILE, 'r', encoding='utf-8') as f:
    content = f.read()

# ====== 1. Fix heading levels: ### 0x. → ## 0x. and #### x.x → ### x.x ======
# Fix sections 01-07: change ### 0x. to ## XX.
# But keep pattern as-is initially, we'll renumber afterward
for i in range(1, 8):
    content = re.sub(rf'^### 0{i}\.', f'## SEC{i}.', content, flags=re.MULTILINE)

# Fix subsections for 01-07: #### x.x → ### XX.x 
# Use a unique pattern to avoid conflicts
for section_num in range(1, 8):
    content = re.sub(rf'^#### {section_num}\.', f'### SUB{section_num}.', content, flags=re.MULTILINE)

# ====== 2. Create the new case introduction section ======
new_section = """
## 01.工作案例引入

### 1.1 网上支付被截胡了

**场景**：小周是一名工作三年的后端工程师，负责公司电商平台的支付模块。某天运营紧急反馈：**"部分用户反馈下单后显示支付成功，但我们的后台没有收到任何订单"**。

小周登录后台查看：

```text
# 问题订单的共同特征：
1. 下单时间集中在晚上9-11点
2. 所有受害用户的IP都来自同一个地区
3. 支付成功的回调日志中，HTTP请求的来源（Referer）为空
4. 正常订单的来源应该是 weixin.pay.com 或 alipay.com
```

继续排查发现：这些用户在公共WiFi环境下下单。黑客在这个WiFi上做了一个**DNS劫持**——把 `pay.our-shop.com` 解析到了自己搭建的假支付页面。用户输入了真实的银行卡信息后，假页面显示"支付成功"，但黑客已经拿到了完整的银行卡号和密码。

更深入的取证表明，黑客不仅做了DNS劫持，还在用户和真实服务器之间截获并修改了数据包——这就是**中间人攻击（Man-in-the-Middle, MITM）**。

```text
正常流程：
  你 → [HTTPS加密] → 电商服务器
            ↑ 安全的加密通道

被攻击流程（HTTP）：
  你 →[明文]→ 黑客路由器 → 电商服务器
              ↓ 黑客能看到你的：收货地址、联系方式、支付密码...
```

**疑惑**：为什么用公共WiFi就会出这种问题？网站不是有账号密码保护吗？支付页面地址栏显示的是 https 开头，为什么还会被盗？

**追问链**：

- "账号密码不能保护安全吗？" → 账号密码只能验证身份，但**不能保护传输过程中的数据**。黑客不需要知道你的密码，只需在传输途中截获并篡改数据包
- "那改成 HTTPS 不就行了吗？" → HTTPS 正是用来解决这个问题的。但当时这个支付页面仍然用了 HTTP 协议发送敏感数据。更糟糕的是，即使地址栏显示 `https://`，黑客也可以通过伪造证书绕过浏览器的安全警告
- "https 为什么能防截获？" → 因为 HTTPS 对传输内容做了**加密**——即使黑客拿到数据包，没有密钥也解不开
- "那加密到底是怎么工作的？为什么有对称加密和非对称加密？" → 这就是本章要回答的核心问题
- "那怎么确保你手里的公钥真的是服务器的，而不是黑客伪造的？" → 这就需要**数字证书**（CA）来给公钥"盖章"担保
- "为什么现在几乎所有网站都用 HTTPS 了？性能损失大吗？" → TLS 1.3 和硬件加速已经让加密的开销降到了可以忽略的程度

小周最后重构了整个支付网关——强制 HTTPS、禁用对支付成功回调和页面 URL 的明文传输、接入网络安全扫描。但问题的根源，是团队对加密协议的理解停留在"用了 HTTPS 就安全了"的层面，而不知道 HTTPS 到底保护了什么、没保护什么。

这一串追问，答案全部写在加密协议的知识体系里。

### 1.2 为什么要学加密协议

```mermaid
flowchart LR
    A["你在电商网站<br/>提交订单"] --> B{传输加密？}
    B -->|HTTP 明文| C[黑客轻松窃取<br/>银行卡/密码]
    B -->|HTTPS 加密| D[安全传输<br/>到真实服务器]
    style C fill:#ff6b6b
    style D fill:#90EE90
```

作为开发者，你可能每天都在用 HTTPS 接口，但有多少人能回答这些问题：

- 为什么 HTTP 是明文，HTTPS 加密后为什么就安全了？加密到底是怎么算的？
- 对称加密和非对称加密有什么区别？为什么不用非对称一撸到底？
- 为什么浏览器会提示"该网站的安全证书有问题"？这个"证书"是什么？
- 为什么 HTTPS 网站的第一次连接总是会慢一些？TLS 握手到底做了什么？
- 什么是"中间人攻击"？HTTPS 能 100% 防住中间人吗？

本章的目标，就是通过一个"网上购物"的完整安全场景，把密码学的核心概念串起来：

- **HTTP 的安全缺陷**：为什么明文传输在购物场景下不可接受？
- **对称加密**：速度快，但密钥怎么安全地给到对方？
- **非对称加密**：解决了密钥分发，但性能太差
- **数字证书**：CA 如何为公钥做信用背书？
- **HTTPS 握手**：TLS 怎么把对称+非对称结合起来？
- **深度剖析**：加密算法演进、密钥交换协议、证书体系、性能优化

带着这六个问题，我们从一次支付攻击事件开始，走进加密协议的世界。
"""

# Insert new section before the old section 01
content = re.sub(r'^## SEC1\.点外卖可以Http吗', new_section + '\n## SEC1.点外卖可以Http吗', content, flags=re.MULTILINE)

# ====== 3. Renumber sections and subsections ======
# Renumber ## sections: SEC1→02, SEC2→03, ..., SEC7→08
for old_n, new_n in [('7', '08'), ('6', '07'), ('5', '06'), ('4', '05'),
                      ('3', '04'), ('2', '03'), ('1', '02')]:
    content = re.sub(rf'^## SEC{old_n}\.', f'## {new_n}.', content, flags=re.MULTILINE)

# Renumber the existing 08-11 sections (they need to shift by +1)
for old_n, new_n in [('11', '12'), ('10', '11'), ('09', '10'), ('08', '09')]:
    content = re.sub(rf'^## {old_n}\.', f'## {new_n}.', content, flags=re.MULTILINE)

# Renumber ### subsections: SUBx.y → (x+1).y
# First, renumber subsections that used #### x.x format (sections 1-7 → 2-8)
for old_s, new_s in [('7', '08'), ('6', '07'), ('5', '06'), ('4', '05'),
                      ('3', '04'), ('2', '03'), ('1', '02')]:
    content = re.sub(rf'^### SUB{old_s}\.', f'### {new_s}.', content, flags=re.MULTILINE)

# Renumber the ### subsections for sections 08-11 (→ 09-12)
for old_s, new_s in [('11.', '12.'), ('10.', '11.'), ('09.', '10.'), ('08.', '09.')]:
    content = re.sub(rf'^### {old_s}', f'### {new_s}', content, flags=re.MULTILINE)

# ====== 4. Remove 小结一下 section or merge it ======
# Keep it as-is, just renumbered

# ====== 5. Fix TOC ======
toc_new = """#### 目录介绍
- 01.工作案例引入
  - 1.1 网上支付被截胡了
  - 1.2 为什么要学加密协议
- 02.点外卖可以Http吗
  - 2.1 HTTP的安全隐患
  - 2.2 加密的基本思路
  - 2.3 为什么需要加密
- 03.对称加密
  - 3.1 对称加密的原理
  - 3.2 密钥分发的困境
  - 3.3 对称加密的实际应用
- 04.非对称加密
  - 4.1 非对称加密思路
  - 4.2 公钥私钥的配合
  - 4.3 双向公私钥体系
- 05.数字证书
  - 5.1 证书解决了什么
  - 5.2 CA的信任链
  - 5.3 证书的生成过程
  - 5.4 证书验证的完整流程
- 06.HTTPS的工作模式
- 07.重放与篡改
- 08.小结一下
- 09.加密算法深度解析
  - 9.1 对称加密算法对比
  - 9.2 非对称加密原理
  - 9.3 HTTPS加密演进
- 10.密钥交换协议设计
  - 10.1 RSA密钥交换的缺陷
  - 10.2 DH密钥交换原理
  - 10.3 ECDHE密钥交换
  - 10.4 前向安全性
- 11.证书体系深度剖析
  - 11.1 X.509证书结构
  - 11.2 证书链验证过程
  - 11.3 证书吊销机制
  - 11.4 证书透明度(CT)
- 12.HTTPS性能优化
  - 12.1 TLS握手开销分析
  - 12.2 会话复用技术
  - 12.3 OCSP Stapling
  - 12.4 HTTP/2与TLS
- 13.思考题与作业
  - 13.1 基础思考题目
  - 13.2 进阶思考题目
  - 13.3 动手实践作业"""

# Replace the old TOC
content = re.sub(r'#### 目录介绍\n(?:- .*\n?)+', toc_new + '\n', content)

# ====== 6. Add 思考题与作业 section ======
thinking = """

## 13.思考题与作业

### 13.1 基础思考题目

1. **对称加密 vs 非对称加密**：列出两者的三个核心区别。在 HTTPS 中，为什么先用非对称加密交换密钥，再用对称加密传输数据？而不是反过来？

2. **中间人攻击**：画一张图说明中间人攻击的完整过程。HTTPS 是如何防止中间人攻击的？如果用户忽略了浏览器的安全证书警告，中间人攻击还能成功吗？

3. **数字证书验证**：浏览器收到一个 HTTPS 网站的证书后，会做哪些检查？（至少列出 4 项）如果证书的域名和访问的域名不匹配，会发生什么？

4. **TLS 握手流程**：TLS 1.2 完整握手需要几次往返（RTT）？TLS 1.3 把这个优化到了几次？每次往返中交换了什么信息？

5. **前向安全性**：什么是前向安全性（Forward Secrecy）？为什么 RSA 密钥交换不支持前向安全？ECDHE 是如何做到每次连接使用不同密钥的？

### 13.2 进阶思考题目

1. **1.1 节复盘**：小周的电商支付安全问题。除了强制 HTTPS，你还能给出哪些防御措施？如果黑客使用了合法的 HTTPS 证书（通过 CA 漏洞签发），HTTPS 还能保护用户吗？该如何防御这种攻击？

2. **公共 WiFi 安全**：在咖啡店的免费 WiFi 上，一个没有做任何防护的 HTTP 网站和一个使用自签名证书的 HTTPS 网站，哪个更安全？请从数据加密、身份验证、中间人攻击三个角度分析。

3. **证书吊销的缺陷**：CRL 和 OCSP 两种证书吊销机制各有优缺点。你知道 OCSP"软失败"（soft-fail）的缺陷吗？2012 年 Comodo 和 DigiNotar CA 被攻破的事件中，如果浏览器严格执行 OCSP 硬检查，损失会减少多少？

4. **量子计算对加密的威胁**：量子计算机理论上能破解 RSA 和 ECC，但对对称加密（如 AES）的影响要小得多。这会对 TLS 的未来设计产生什么影响？后量子密码学（Post-Quantum Cryptography）目前有哪些候选方案？

5. **HTTPS 性能优化的取舍**：TLS 1.3 的 0-RTT 恢复在提升性能的同时，存在重放攻击的风险。哪些类型的 HTTP 请求可以安全地使用 0-RTT？哪些不可以？如果你是淘宝的技术负责人，你会对 0-RTT 做什么样的策略？

### 13.3 动手实践作业

**作业一（必做）**：用 OpenSSL 亲手创建自签名证书并配置 HTTPS。

```bash
# 1. 生成私钥
openssl genrsa -out mykey.pem 2048

# 2. 生成自签名证书
openssl req -new -x509 -key mykey.pem -out mycert.pem -days 365 \
  -subj "/CN=localhost"

# 3. 用 Python 启动一个 HTTPS 服务器
python3 -c "
from http.server import HTTPServer, BaseHTTPRequestHandler
import ssl
class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'Hello, HTTPS!')
httpd = HTTPServer(('localhost', 4443), Handler)
httpd.socket = ssl.wrap_socket(httpd.socket, certfile='mycert.pem', keyfile='mykey.pem', server_side=True)
httpd.serve_forever()
"

# 4. 用 curl 访问，观察 TLS 握手详情
curl -v https://localhost:4443/
# 注意：浏览器访问会显示安全警告，因为证书是自签名的
```

**作业二（选做）**：用 Wireshark 抓包分析 HTTPS 握手。

```bash
# 1. 启动 Wireshark 抓包，过滤 tcp.port == 443
# 2. 用浏览器访问 https://www.baidu.com
# 3. 找到 TLS 握手的数据包：
#    - ClientHello：包含 TLS 版本、加密套件列表、随机数
#    - ServerHello：服务器选择的加密套件
#    - Certificate：服务器证书
#    - ClientKeyExchange：密钥交换（如果是 ECDHE，会发送公钥参数）
# 4. 回答：你的浏览器和服务器协商使用了什么加密套件？
```

**作业三（选做）**：验证证书链。

```bash
# 查看 www.baidu.com 的证书链
openssl s_client -connect www.baidu.com:443 -showcerts

# 查看证书的详细信息
# 注意输出中的 Certificate chain 部分
# 从叶子证书到根证书，每一级的 Issuer 和 Subject
```

**作业四（架构思考）**：对你当前负责的一个服务，分析它的"安全协议全景图"。

- 列出所有对外接口的传输协议（HTTP/HTTPS/gRPC/WebSocket）
- 标注每个接口的 TLS 版本、加密套件配置
- 分析是否存在"混合内容"（页面通过 HTTPS 加载但引用了 HTTP 资源）
- 给出至少 3 条安全加固建议
"""

content = content.rstrip() + thinking

# ====== 7. Write result ======
with open(FILE, 'w', encoding='utf-8') as f:
    f.write(content)

print("Done!")
