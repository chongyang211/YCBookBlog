//
// Created by 杨充 on 2026/5/28.
//
// ============================================================================
// 【文件定位】User.h —— 所有用户角色的抽象基类（整个继承体系的根）
// ----------------------------------------------------------------------------
// 【设计思路】
//   1. 面向对象的"模板方法"思想：把"用户"的共性（账号/姓名/密码、登录校验、
//      CSV 序列化）抽取到基类，把"差异化行为"（主菜单、身份标签）声明为纯虚函数，
//      强制子类各自实现 —— 这就是"同一接口，不同实现"的多态。
//   2. 遵循开闭原则：将来新增角色（如"宿管"），只需再派生一个子类，
//      基类和 CampusSystem 的代码完全不用修改。
//   3. 用抽象基类而不是"一个 User 类 + role 字段 + if-else 分支"，
//      可以避免类型分支逻辑散落在各处，代码可维护性更好。
// ============================================================================

#ifndef USER_H      // 头文件保护宏开始：防止同一翻译单元中被重复 #include 导致"类重定义"编译错误
#define USER_H      // 原理：第一次包含时定义宏；第二次包含时 #ifndef 不成立，整段内容被跳过
#include <string>   // std::string 所在头文件（用户名、密码等字符串成员都依赖它）
#include <iostream> // iostream（User.cpp 中用了未加 std:: 前缀的 cout，需要这里传递包含）

// User 类是一个抽象基类，用于定义用户的基本接口。
// 1.支持多态性，允许派生类实现不同的行为。2.拓展性，确保所有用户类型都具有一致的行为。
// 【技术原理】"抽象类"= 含有至少一个纯虚函数的类，无法实例化对象，
//             只能通过基类指针/智能指针来引用派生类对象（多态的载体）。
class User {
// protected 表示派生类可见（子类可以直接访问父类这些属性），外部不可见
// 【技术原理】三种访问控制的区别：
//   public    —— 类外部任意可见
//   protected —— 类内部 + 派生类可见（继承体系的"家族内部"约定）
//   private   —— 仅类内部可见（即使是儿子也看不到）
// 【设计权衡】如果设为 private，子类需要通过 public 的 getId()/getName() 访问；
//             设为 protected 可以让子类菜单直接打印 userName，省一层封装调用。
protected:
    std::string userId;     // 用户id（登录账号，全局唯一，作为 users.map 的 key）
    std::string userName;   // 用户名（显示用的姓名）
    std::string password;   // 用户密码（明文存储；真实项目必须哈希+加盐，绝不存明文）

public:
    // 基类构造函数：派生类构造时必须通过"初始化列表"调用它来初始化这三个成员
    // 【技术原理】成员声明为 private/protected 且没有默认构造需求时，
    //             子类无法在函数体内赋值初始化 const/引用成员，初始化列表是唯一途径。
    User(const std::string& id, const std::string& name, const std::string& pwd);
    // 虚析构函数，使用 = default 表示使用编译器生成的默认实现。
    // 确保在通过基类指针删除派生类对象时，能够正确调用派生类的析构函数。
    // 【技术原理】这是 C++ 继承的"铁律"：只要类会被继承且通过基类指针 delete，
    //             析构函数就必须是 virtual，否则只会执行 User 的析构（未定义行为，
    //             派生类部分资源泄漏）。= default 让编译器生成高效的无操作析构。
    virtual ~User() = default;
    // 纯虚函数，表示 User 类是一个抽象基类，不能直接实例化。派生类必须实现 mainMenu 方法。
    // `= 0`，表示纯虚函数，强制让子类实现
    // 【设计思路】"主菜单"是学生/教师/管理员行为的最大差异点：
    //             学生看到"预约机房"，教师看到"审核预约"——这正是多态要解决的场景。
    virtual void mainMenu() = 0;
    // 纯虚：返回身份标签 'S'/'T'/'A'，用于 CSV 反序列化（阶段 ⑦ 用）
    // 【技术原理】const 成员函数：承诺不修改对象状态，const 对象也能调用。
    //             返回值小而廉价（一个 char），按值返回即可，无需引用。
    virtual char roleTag() const = 0;
    // 校验密码
    // 【技术原理】const 成员函数 + 返回 bool，是典型的"谓词"接口；
    //             参数用 const std::string& 避免拷贝（只比较不修改）。
    // 【安全提示】明文比较密码存在时序侧信道风险，教学项目可接受，
    //             生产环境应使用恒定时间比较 + 密码哈希（bcrypt/scrypt 等）。
    bool verify(const std::string& pwd) const { return password == pwd; }

    // 访问器（getter）：返回 const 引用而非拷贝，零开销地暴露只读视图
    // 【技术原理】const std::string& 返回值：调用方拿到的对象不可修改，也不发生字符串拷贝
    const std::string& getId()   const { return userId; }
    const std::string& getName() const { return userName; }

    // 序列化为 CSV 行（阶段 ⑦ 才会真正用上）
    // 【技术原理】virtual + 有默认实现：子类可以不重写直接复用；
    //             std::string(1, roleTag()) 把单个 char 转成 string（"+" 两端类型必须一致）。
    // 【数据格式】每行形如 "S,S001,张三,123"：首列是角色标签，供加载时判断 new 哪个子类。
    virtual std::string toCsv() const {
        return std::string(1, roleTag()) + "," + userId + "," + userName + "," + password;
    }
};




#endif //USER_H  // 头文件保护宏结束（宏名与 #ifndef 对应，通常还带行注释标明所属宏）
