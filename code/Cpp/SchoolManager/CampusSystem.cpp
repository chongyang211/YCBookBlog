//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】CampusSystem.cpp —— 系统核心业务层（所有规则与状态转移的唯一实现）
// ----------------------------------------------------------------------------
// 【设计思路】本类是"数据 + 规则"的中枢：
//   - 四大容器（users/rooms/reservations/speeches）全部 private，外界无法绕过方法直改；
//   - 每个写操作（预约/取消/审核/评分）内部自带"前置校验 → 修改主数据 → 同步冗余索引"
//     三段式流程，保证数据一致性；
//   - 算法层面大量使用 STL + lambda（find_if / count_if / accumulate / sort），
//     体会"数据在哪、算法怎么跑"的函数式风格。
// ============================================================================

#include "CampusSystem.h"   // 本类声明（同时带来了 User/Computer/Reservation 及 STL 头）

#include <algorithm>    // std::find_if / std::count_if / std::sort（算法库）
#include <iostream>     // std::cout / std::cin
#include <iomanip>      // std::setw / std::left（输出对齐格式化）
#include <numeric>      // std::accumulate（数值累计，配合 lambda 做计数统计）

#include "Reservation.h"
#include "Speech.h"     // Speech 的完整定义（multimap 中存的是其值对象，
                        //  取出后要访问 s.studentId/s.score 等成员，必须知道布局）

// 构造函数：目前只打印启动横幅（后续阶段可在此加载 CSV 数据恢复现场）
// 【技术原理】成员容器（map/set/vector/multimap）与 nextResId=1（NSDMI）
//             都在进入函数体前自动完成初始化，函数体无需再写任何代码。
CampusSystem::CampusSystem() {
    std::cout << "[System] 校园系统启动\n";
}


// ============ 用户管理 ============

// 添加用户：入库前的唯一性校验（账号是 map 的 key，天然不能重复）
// 【技术原理】参数 shared_ptr<User> 按值传递 = 与调用方共享所有权
//             （map 存一份拷贝，引用计数 +1；调用方的 u 析构后 map 仍持有对象）。
bool CampusSystem::addUser(std::shared_ptr<User> u) {
    std::string id = u->getId();            // 虚调用不涉及：getId 是非虚普通成员
    // 【技术原理】map::count(key) 返回 0 或 1（multimap 才可能 >1）：
    //             比 find()+end() 判空更简短，只判存在性时是惯用法。
    if (users.count(id) > 0) {
        std::cout << "[System] 用户 " << u->getId() << " 已存在\n";   // 幂等保护：重复添加给出明确提示
        return false;                       // 用返回值告知调用方结果（调用方可选择性提示）
    }
    users[id] = u;                          // operator[] 插入：key 不存在则新建键值对
    std::cout << "[System] 添加用户 " << u->getId() << " 成功\n";
    return true;
}

// 登录：两步校验（账号存在 → 密码匹配），任一失败返回 nullptr
// 【设计思路】返回 shared_ptr 而非裸指针：与 users 容器共享所有权，
//             即使登录后容器变动，会话期间对象也不会被销毁（安全的多态句柄）。
std::shared_ptr<User> CampusSystem::login(const std::string &id, const std::string &pwd) {
    // 【技术原理】map::find：红黑树 O(logN) 查找，返回迭代器；
    //             auto 让编译器推导类型（实际是 map<...>::iterator）。
    auto it = users.find(id);
    if (it == users.end()) {        // end() 表示"没找到"——迭代器哨兵
        std::cout << "[System] 账号不存在\n";
        return nullptr;             // 空指针表示登录失败，调用方用 if(!user) 判断
    }
    // it->second 是 map 的 value（shared_ptr<User>），-> 解引用再调 verify(pwd)
    // 【设计细节】"账号不存在"与"密码错误"分开提示，便于用户自查；
    //             （安全角度二者合并为"账号或密码错误"更防枚举探测，教学从简）
    if (!it->second->verify(pwd)) {
        std::cout << "[System] 密码错误\n";
        return nullptr;
    }
    return it->second;      // 登录成功：返回多态句柄，main 拿到后调 mainMenu() 动态分发
}


// ============ 学生业务 ============

// 列出所有机房的信息。const 表示该函数不会修改类的成员变量。
// 学生：1.浏览机房
void CampusSystem::listRooms() const {
    std::cout << "\n=== 机房列表 ===\n";
    // 输出机房列表的表头。std::left：设置输出左对齐。std::setw(n)：设置字段宽度为 n 个字符。
    // 表头包括四列：编号、容量、配置 和 状态。
    // 【技术原理】流操纵符是"粘性/一次性"混合的：
    //   - std::left 是粘性的：设置后一直生效直到改回 std::right；
    //   - std::setw(n) 是一次性的：只影响"下一个"输出项，每列都要重写 setw。
    //             中文表头按显示宽度估算对齐（GBK/UTF-8 下字宽与 setw 语义有出入，
    //             教学项目以近似对齐为准）。
    std::cout << std::left << std::setw(6) << "编号" << std::setw(8) << "容量"
         << std::setw(20) << "配置" << std::setw(8) << "状态\n";
    // std::string(42, '-')：构造 42 个连字符的分隔线（count, char 构造重载）
    std::cout << std::string(42, '-') << "\n";
    // 空表保护：无数据时给出友好提示并提前 return（卫语句风格，减少嵌套）
    if (rooms.empty()) { std::cout << "（暂无机房）\n"; return; }
    // 【技术原理】结构化绑定（C++17）："const auto& [id, room]" 一次解包 map 的
    //             键值对：id 绑定 key，room 绑定 value；const auto& 零拷贝遍历。
    //             map 按机房编号升序输出（红黑树中序遍历的有序性）。
    for (const auto& [id, room] : rooms) {
        // 查询冗余索引：set::count O(logN) 判断机房当前是否被占用
        bool occupied = reservedRooms.count(id) > 0;
        // bool occupied = false;
        // 三目运算符把布尔值翻译为中文状态列；最后一列不设 setw（行尾无需补齐）
        std::cout << std::left << std::setw(6) << id << std::setw(8) << room.capacity
            << std::setw(20) << room.spec
            << (occupied ? "占用中" : "空闲") << "\n";
    }
}

// 学生：2.预约机房
// 三段式：存在性校验 → 占用校验 → 写主数据 + 同步冗余索引
bool CampusSystem::reserveRoom(const std::string &sid, int roomId, const std::string &date) {
    // 【技术原理】find 返回迭代器，与 end() 比较判断机房编号是否存在
    //  （为什么不在这里用 count？find 的迭代器可复用于取值，是更通用的习惯）
    if (rooms.find(roomId) == rooms.end()) {
        std::cout << "[预约] 机房不存在\n";
        return false;
    }
    if (reservedRooms.count(roomId) > 0) {        // ⭐ 新增：校验未被占用
        std::cout << "[预约] 该机房已被预约\n";    // 冗余索引 set 一查即知，避免遍历 vector
        return false;
    }
    // nextResId++ 后缀自增：先取当前值构造预约单，再让计数器 +1（预约号从 1 开始递增）
    Reservation r(nextResId++, sid, roomId, date);
    reservations.push_back(r);    // 只塞 vector
    // 【技术原理】push_back 发生拷贝（r 是栈上局部对象）；
    //             更现代的写法是 emplace_back(nextResId, sid, roomId, date)
    //             在容器内原地构造，省去一次拷贝。
    reservedRooms.insert(roomId); // 同步冗余索引：立刻把机房标记为"占用"
    std::cout << "[预约] 提交成功，预约号 " << r.resId << "\n";    // 回显预约号，供后续取消/审核引用
    return true;
}

// 学生：3.取消预约
// 规则：只能取消"自己的"且"待审核"的预约 —— 越权与状态校验缺一不可
bool CampusSystem::cancelReservation(const std::string &sid, int resId) {
    // 【技术原理】find_if + lambda 按谓词线性查找（vector 无 key，只能扫）：
    //   - 捕获列表 [resId, &sid]：按值捕预约号、按引用捕学生账号（string 拷贝贵）；
    //   - lambda 参数 const Reservation& 避免逐元素拷贝；
    //   - 谓词同时匹配 resId 和 studentId：一次查找完成"存在性 + 归属"双重校验。
    auto it = std::find_if(reservations.begin(),reservations.end(), [resId,&sid](const Reservation& r) {
        return r.resId == resId && r.studentId == sid;
    });
    if (it == reservations.end()) {   // 没找到：预约不存在 或 不属于该学生（统一提示，防信息泄露）
        std::cout << "[取消] 预约不存在或不属于你\n";
        return false;
    }
    // 状态机校验：只有 Pending 状态允许学生自主取消
    // （已批准的预约已生效，不能单方面取消；已处理过的也不允许重复操作）
    if (it->status != ResStatus::Pending) {
        std::cout << "[取消] 该预约已被审核，无法取消\n";
        return false;
    }
    it->status = ResStatus::Cancelled;       // 状态转移：Pending → Cancelled（软删除：保留记录只改状态）
    reservedRooms.erase(it->computerId);    // ⭐ 同步清理 set（取消后机房空闲）
    // 【一致性要点】冗余索引必须与主数据同步：这里漏掉 erase 就会出现
    //             "预约已取消但机房永远显示占用"的经典不一致 bug。
    std::cout << "[取消] 已取消\n";
    return true;
}

// 学生：4.报名演讲
bool CampusSystem::signupSpeech(const std::string &sid, const std::string &topic, int round) {
    // 输入合法性校验（白名单式）：round 只接受 1（初赛）或 2（复赛）
    if (round != 1 && round != 2) {
        std::cout << "[演讲] round 只能是 1 或 2\n";
        return false;
    }
    // ⭐ multimap 允许同 key 多 value：一个 round 可以有很多人报名
    // 【技术原理】multimap::insert(pair)：key 已存在不会失败（这正是与 map 的区别），
    //             同 key 元素相邻存放（等价类），后续 equal_range 可整段取出。
    speeches.insert({round,Speech(sid, topic, round)});
    std::cout << "[演讲] 报名成功 - 第 " << round << " 轮 - " << topic << "\n";
    return true;
}


// ============ 教师业务 ============

// 教师查看全部"待审核"预约
void CampusSystem::listPendingReservations() const {
    std::cout << "\n=== 待审核预约 ===\n";
    // ⭐ count_if 先看有几个（没有就提前返回）
    // 【技术原理】count_if(begin, end, 谓词)：对区间内每个元素调用 lambda，
    //             统计返回 true 的个数；无捕获 lambda 可被内联，效率接近手写循环。
    // reservations 是 vector，没有 count 方法。这个是预约单集合
    auto pendingCount = std::count_if(reservations.begin(), reservations.end(),
        [](const Reservation& r) { return r.status == ResStatus::Pending; });
    std::cout << "共 " << pendingCount << " 条\n";
    // 卫语句：空列表提前返回，避免打印孤零零的表头
    if (pendingCount == 0) { std::cout << "（无待审）\n"; return; }

    // 再遍历一次输出明细（count 用于判断，遍历用于展示，各司其职）
    // 【技术原理】auto 类型推导 + 范围 for 遍历 vector（连续内存，最快的一种遍历）
    for (const auto& r : reservations) {
        if (r.status != ResStatus::Pending) continue;  // 跳过非待审状态（continue 提前进入下一轮）
        std::cout << "  预约 " << r.resId << " | 学生 " << r.studentId
             << " | 机房 " << r.computerId << " | 日期 " << r.date << "\n";
    }
}

// 教师审核预约：唯一入口实现状态转移 Pending → Approved / Rejected
bool CampusSystem::reviewReservation(int resId, bool approved) {
    // 按预约号查找（教师视角无需校验归属，任何教师可审任何单）
    auto it = std::find_if(reservations.begin(), reservations.end(),
        [resId](const Reservation& r) { return r.resId == resId; });
    if (it == reservations.end()) {     // 预约号不存在
        std::cout << "[审核] 预约不存在\n";
        return false;
    }
    // 幂等校验：已处理过的预约不允许二次审核（防止覆盖已有结论）
    if (it->status != ResStatus::Pending) {
        std::cout << "[审核] 该预约已被处理过\n";
        return false;
    }

    // 三目运算符完成状态转移：approved ? 批准 : 拒绝
    it->status = approved ? ResStatus::Approved : ResStatus::Rejected;
    if (!approved) reservedRooms.erase(it->computerId);   // ⭐ 拒绝时释放占用
    // 【一致性要点】批准则机房继续占用（保持不变）；拒绝必须释放冗余索引，
    //             否则被拒绝的机房会"永久占用"，学生再也约不上。
    std::cout << "[审核] 预约 " << resId << " - " << it->statusText() << "\n";  // statusText 把枚举翻译成中文
    return true;
}

// 教师评分：定位"某学生某轮"的报名记录并写入分数
void CampusSystem::scoreSpeech(const std::string &sid, int round, double score) {
    // ⭐ multimap::equal_range：拿到 round 这个 key 的所有 entry 的 [first, last)
    // 【技术原理】equal_range 返回 pair<iterator, iterator>：
    //             [first, last) 恰好是同 key 的所有元素区间（红黑树等价类），
    //             比逐个 find + ++ 更安全高效（一次 O(logN) 定位整段）。
    auto range = speeches.equal_range(round);
    for (auto it = range.first; it != range.second; ++it) {  // 只在等价类内线性扫描
        // multimap 迭代器解引用得到 pair<const int, Speech>：it->second 才是 Speech 对象
        if (it->second.studentId == sid) {
            it->second.score = score;        // 命中：就地更新分数（对象按值存在 multimap 中）
            std::cout << "[评分] " << sid << " 第 " << round
                 << " 轮 - " << score << " 分\n";
            return;                          // 找到即返回：函数到"第一个匹配"为止（同轮重复报名只改首条）
        }
    }
    std::cout << "[评分] 未找到该报名\n";     // 扫描完等价类仍未命中：报名不存在
}



// 教师查看排名：把 multimap 中某轮的报名抽出来，按分数降序排序展示
void CampusSystem::rankSpeechesByScore(int round) const {
    // 1. 把 multimap 中 round 对应的 entry 抽到 vector（multimap 不能直接 sort）
    // 【技术原理】multimap 是红黑树，迭代顺序固定为 key 升序（同 key 按插入序），
    //             没有"按 value 排序"的接口；想按分数排必须先拷贝到
    //             支持随机访问的 vector，再交给 std::sort。
    std::vector<Speech> arr;                 // 临时数组承接等价类元素
    auto range = speeches.equal_range(round);    // O(logN) 定位第 round 轮的整段区间
    for (auto it = range.first; it != range.second; ++it) {
        arr.push_back(it->second);           // 只拷 value（Speech），丢弃 key（轮次已知）
    }

    if (arr.empty()) { std::cout << "[排名] 第 " << round << " 轮无人报名\n"; return; }   // 空保护

    // 2. ⭐ lambda 比较器：分数高的在前
    // 【技术原理】std::sort(begin, end, comp)：comp(a, b) 返回 true 表示 a 应排在 b 前面；
    //             "a.score > b.score" 即降序（改成 < 就是升序）；
    //             introsort 混合快排/堆排/插入排，平均 O(NlogN)。
    std::sort(arr.begin(), arr.end(),
        [](const Speech& a, const Speech& b) { return a.score > b.score; });

    // 3. 输出排名
    std::cout << "\n=== 第 " << round << " 轮排名 ===\n";
    int rank = 1;                            // 名次计数器（从第 1 名开始）
    for (const auto& s : arr) {              // 遍历已排序数组
        // rank++ 后缀自增：先打印当前名次，再自增 —— 一行同时完成取值与更新
        std::cout << "  " << rank++ << ". " << s.studentId
             << " - " << s.topic << " - " << s.score << " 分\n";
    }
}


// ============ 管理员业务 ============

// 添加机房：与 addUser 同构 —— 先查重再入库
bool CampusSystem::addRoom(const Computer &c) {
    // map::count 检查编号是否已存在（机房编号是业务主键）
    if (rooms.count(c.id) > 0) {
        std::cout << "[Admin] 机房 " << c.id << " 已存在\n";
        return false;
    }
    rooms[c.id] = c;     // operator[] 插入：key=c.id，value=拷贝一份 Computer
    std::cout << "[Admin] 添加机房 " << c.id << " 成功\n";
    return true;
}

// 数据统计报表：只读聚合，充分展示 STL 算法 + lambda 的组合威力
void CampusSystem::statistics() const {
    std::cout << "\n=== 数据统计 ===\n";
    std::cout << "用户总数: " << users.size() << "\n";     // map::size：不同账号数（键唯一）
    std::cout << "机房总数: " << rooms.size() << "\n";

    // ⭐ accumulate + lambda 实现"按身份分类计数"
    // 【技术原理】std::accumulate(begin, end, 初值, op)：
    //             从初值 0 出发，逐元素执行 op(sum, kv) 折叠出结果。
    //             这里 op 是 lambda：遇到学生 sum+1，否则 sum 不变 ——
    //             "把计数问题转化为求和问题"的函数式思维。
    //             参数 auto& 自动推导为 map 的 value_type（const pair<string, shared_ptr<User>>&）。
    int students = std::accumulate(users.begin(), users.end(), 0,
        [](int sum, const auto& kv) {
            return sum + (kv.second->roleTag() == 'S' ? 1 : 0);    // kv.second 是 shared_ptr<User>，虚调用 roleTag()
        });
    int teachers = std::accumulate(users.begin(), users.end(), 0,
        [](int sum, const auto& kv) {
            return sum + (kv.second->roleTag() == 'T' ? 1 : 0);    // 同法统计教师
        });

    // 管理员数 = 总数 - 学生 - 教师（减法兜底，避免第三次遍历）
    std::cout << "  学生: " << students << " | 教师: " << teachers
         << " | 管理员: " << users.size() - students - teachers << "\n";

    std::cout << "预约总数: " << reservations.size() << "\n";      // vector::size：含已取消/已拒绝的历史单
    // count_if 统计待审单数量（lambda 谓词只匹配 Pending）
    int pending = std::count_if(reservations.begin(), reservations.end(),
        [](const Reservation& r) { return r.status == ResStatus::Pending; });
    std::cout << "  待审: " << pending
         << " | 已审: " << reservations.size() - pending << "\n";  // 已审 = 总数 - 待审（含批准/拒绝/取消）

    std::cout << "演讲报名: " << speeches.size() << " 人次\n";     // multimap::size：条目总数（同人两轮算两人次）
    std::cout << "  第 1 轮: " << speeches.count(1)     // multimap::count(key)：该轮次的报名条数
         << " | 第 2 轮: " << speeches.count(2) << "\n";
}
