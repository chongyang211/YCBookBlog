

**01.ObjC基础语法**

- 1.1.1 ObjC简单说明：简称OC，是一种通用、高级、面向对象的编程语言。
- 1.1.2 ObjC文件说明：.h是头文件，.m是源文件，.mm是拓展文件可以包含Objective-C和C代码以外还可以包含C++代码。
- 1.1.3 HelloWorld：
- 1.1.4 理解OC中指针：使用指针时，我们其实是在引用一个对象的地址，而不是直接使用堆 (heap) 中创建的对象。
- 1.1.5 import导包介绍：
- 1.1.6 ObjC相关术语：类，对象，实例，消息，方法，接口，实现等等概念。
- 1.2.1 理解消息传递：与其说对象互相调用方法，不如说对象之间互相传递消息更为精确。消息传递是一种对象之间进行通信的机制。
- 1.2.2 C++和OC区别：二种风格的主要差异在于调用方法/消息传递这个动作。
- 1.2.4 二种风格优劣：C++强制要求所有的方法都必须有对应的动作，Objective-C运行期才处理消息，允许发送未知消息给对象。
- 1.3.1 字符串介绍：NSString类提供了字符串的类包装，字符串被双引号包括。
- 1.3.2 字符串的创建：使用助记符@创建字符串。
- 1.4.1 常见的基础类型：
- 1.4.2 常见的集合类型：
- 1.4.3 NSArray：是一种常用的集合类，用于存储有序的对象集合。
- 1.4.4 NSDictionary：字典，类似于其它编程语言中的 Map 的数据类型。
- 1.5.1 条件控制结构：`if` 语句，用于根据条件执行不同的代码块。

**02.面向对象编程**

- 2.1.1 类规格说明：类规格说明包含了两个部分，声明interface与实现implementation。
- 2.1.2 类的声明：类声明一般定义在.h头文件中。类声明以关键字@interface作为开始，@end作为结束。
- 2.1.3 类的实现：以关键字@implementation作为区块起头，@end结尾。类实现包含了公开方法的实现，以及定义私有（private） 变量及方法。
- 2.1.4 类的实例化：Objective-C创建对象需通过alloc以及init两个消息。alloc的作用是分配内存，init则是初始化对象。


### Objective-C 从入门到精通（详细指南）

---

#### **一、Objective-C 基础**

**1. 核心概念**
- **面向对象扩展**：基于 C 语言的面向对象超集，添加 Smalltalk 式消息传递。
- **动态运行时**：允许运行时修改类和方法（反射、方法调配）。
- **兼容性**：兼容标准 C 语法，可直接嵌入 C 代码。
- **内存管理**：支持手动引用计数（MRC）和自动引用计数（ARC）。

**2. 开发环境**
- **工具**：Xcode（IDE）、Clang（编译器）、LLDB（调试器）
- **框架依赖**：Foundation（基础类库）、UIKit（iOS）、AppKit（macOS）

---

#### **二、语法详解**
**1. 类与对象**
```objectivec
// 声明
@interface Person : NSObject
@property (nonatomic, copy) NSString *name;
@property (nonatomic, assign) NSInteger age;
- (void)introduce; // 实例方法
+ (void)species;   // 类方法
@end

// 实现
@implementation Person
- (void)introduce {
    NSLog(@"I'm %@, age %ld", self.name, self.age);
}
+ (void)species {
    NSLog(@"Homo sapiens");
}
@end

// 使用
Person *p = [[Person alloc] init];
p.name = @"Alice";
[p introduce];        // 输出：I'm Alice, age 0
[Person species];      // 输出：Homo sapiens
```

**2. 消息传递机制**
```objectivec
// 消息结构：[receiver selector]
[p performSelector:@selector(introduce)]; // 动态调用

// 多参数方法
[p setValue:@"Bob" forKey:@"name"]; // KVC 示例
```

**3. 内存管理**
- **MRC**：
  ```objectivec
  NSObject *obj = [[NSObject alloc] init]; // retainCount=1
  [obj retain]; // retainCount=2
  [obj release]; // retainCount=1
  ```
- **ARC**：编译器自动插入 `retain/release`（默认模式）
- **属性修饰符**：
  - `strong`：持有对象（默认）
  - `weak`：弱引用（自动置 nil）
  - `copy`：复制对象（用于 NSString/NSArray）
  - `assign`：基本数据类型（非对象）

**4. 协议与委托**
```objectivec
@protocol DataDelegate <NSObject>
@required
- (void)didReceiveData:(NSData *)data;
@optional
- (void)didFailWithError:(NSError *)error;
@end

@interface Downloader : NSObject
@property (nonatomic, weak) id<DataDelegate> delegate;
@end
```

**5. 类别（Category）与扩展（Extension）**
- **Category**（添加方法）：
  ```objectivec
  @interface NSString (Reverse)
  - (NSString *)reverseString;
  @end
  
  @implementation NSString (Reverse)
  - (NSString *)reverseString {
      NSMutableString *reversed = [NSMutableString new];
      for (NSInteger i = self.length - 1; i >= 0; i--) {
          [reversed appendFormat:@"%c", [self characterAtIndex:i]];
      }
      return reversed;
  }
  @end
  ```
- **Extension**（私有属性和方法）：
  ```objectivec
  @interface Person ()
  @property (nonatomic, strong) NSString *secretID;
  - (void)privateMethod;
  @end
  ```

**6. Block 语法**
```objectivec
typedef void (^CompletionBlock)(NSData *data); // 类型定义

- (void)fetchData:(CompletionBlock)completion {
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        NSData *data = // 网络请求
        dispatch_async(dispatch_get_main_queue(), ^{
            completion(data); // 回调主线程
        });
    });
}

// 避免循环引用
__weak typeof(self) weakSelf = self;
[self fetchData:^(NSData *data) {
    [weakSelf handleData:data];
}];
```

---

#### **三、高级编程**
**1. 运行时（Runtime）**
- **方法调配（Swizzling）**：
  ```objectivec
  Method original = class_getInstanceMethod([self class], @selector(viewDidLoad));
  Method swizzled = class_getInstanceMethod([self class], @selector(swizzled_viewDidLoad));
  method_exchangeImplementations(original, swizzled);
  ```
- **动态创建类**：
  ```objectivec
  Class newClass = objc_allocateClassPair([NSObject class], "NewClass", 0);
  objc_registerClassPair(newClass);
  ```

**2. KVO（键值观察）**
```objectivec
// 添加观察者
[person addObserver:self
         forKeyPath:@"age"
            options:NSKeyValueObservingOptionNew
            context:nil];

// 回调
- (void)observeValueForKeyPath:(NSString *)keyPath 
                      ofObject:(id)object 
                        change:(NSDictionary *)change 
                       context:(void *)context {
    if ([keyPath isEqualToString:@"age"]) {
        NSLog(@"New age: %@", change[NSKeyValueChangeNewKey]);
    }
}
```

**3. 多线程**
- **GCD**：
  ```objectivec
  dispatch_queue_t queue = dispatch_queue_create("com.example.queue", DISPATCH_QUEUE_CONCURRENT);
  dispatch_async(queue, ^{ /* 异步任务 */ });
  ```
- **NSOperationQueue**：
  ```objectivec
  NSOperationQueue *queue = [[NSOperationQueue alloc] init];
  [queue addOperationWithBlock:^{ /* 任务块 */ }];
  ```

---

#### **四、框架整合**
**1. Foundation 核心类**
- `NSString`/`NSMutableString`
- `NSArray`/`NSMutableArray`
- `NSDictionary`/`NSMutableDictionary`
- `NSSet`/`NSOrderedSet`
- `NSData`/`NSMutableData`

**2. Cocoa Touch（iOS）**
- `UIViewController` 生命周期管理
- `UITableView` 数据源与代理
- `Auto Layout` 自动布局

---

#### **五、项目实战技巧**
1. **内存优化**
  - 使用 `@autoreleasepool` 减少峰值内存
  - 懒加载对象：
    ```objectivec
    - (NSArray *)items {
        if (!_items) {
            _items = @[@"A", @"B", @"C"];
        }
        return _items;
    }
    ```

2. **性能调优**
  - 复用 `UITableViewCell`
  - 缓存图片（`NSCache`）
  - 避免主线程阻塞（耗时操作放入后台）

3. **安全实践**
  - 敏感数据用 `Keychain` 存储
  - 使用 `NSFileProtection` 文件加密
  - 预防常见漏洞（SQL注入、XSS）

---

#### **六、现代化适配**
1. **与 Swift 混编**
  - **桥接头文件**：`ProjectName-Bridging-Header.h`
  - 宏定义标记：
    ```objectivec
    #if __has_feature(modules)
    @import SwiftModule; // Swift 模块导入
    #endif
    ```

2. **CocoaPods 集成**
   ```ruby
   target 'MyApp' do
     pod 'AFNetworking', '~> 4.0'
   end
   ```

---

#### **七、学习资源**
1. **官方文档**
  - https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC
  - https://developer.apple.com/documentation

2. **经典书籍**
  - 《Effective Objective-C 2.0》 (Matt Galloway)
  - 《Objective-C 高级编程》 (iOS与OS X多线程和内存管理)

3. **进阶方向**
  - Core Data 数据库管理
  - Core Animation 高级动画
  - Mach-O 与 dyld 底层原理

> **重要提示**：虽然 Swift 是苹果现役首选语言，但 Objective-C 在以下场景仍不可替代：
> - 维护遗留代码库（如银行/政府系统）
> - 底层系统开发（越狱工具、逆向工程）
> - 高性能需求场景（C 级优化）

掌握 Objective-C 的核心在于理解消息传递机制、内存管理模型及动态运行时特性，配合框架实践可快速达到精通水平。



## **4. 异常处理**
### **4.1 `@try`、`@catch`、`@finally`**
用于捕获和处理异常。
```objc
@try {
    NSArray *array = @[@"Apple", @"Banana"];
    NSLog(@"%@", array[2]); // 越界访问
}
@catch (NSException *exception) {
    NSLog(@"Exception: %@", exception);
}
@finally {
    NSLog(@"Finally block executed");
}
```

---



## **4. 内存管理**
### **自动释放池**
Objective-C 使用自动引用计数（ARC）管理内存，但仍需了解自动释放池：
```objective-c
@autoreleasepool {
    NSString *str = [NSString stringWithFormat:@"Hello, %@", @"World"];
    NSLog(@"%@", str);
}
```

### **手动内存管理（非 ARC）**
- 保留计数：
  ```objective-c
  NSObject *obj = [[NSObject alloc] init]; // retainCount = 1
  [obj retain]; // retainCount = 2
  [obj release]; // retainCount = 1
  [obj release]; // retainCount = 0，对象被释放
  ```

---

## **5. 常用框架**
### **Foundation 框架**
- 提供基本数据类型、集合、文件操作等功能。
  ```objective-c
  NSArray *array = @[@"Apple", @"Banana", @"Orange"];
  NSDictionary *dict = @{@"name": @"John", @"age": @25};
  ```

### **UIKit 框架（iOS）**
- 提供用户界面组件。
  ```objective-c
  UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(50, 50, 200, 30)];
  label.text = @"Hello, iOS!";
  [self.view addSubview:label];
  ```

---

## **6. 学习资源**
- [Apple 官方文档](https://developer.apple.com/documentation/objectivec)
- [Objective-C 编程指南](https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html)
- [Ray Wenderlich 教程](https://www.raywenderlich.com/ios/paths/learn)

---

通过以上内容，你可以快速入门 Objective-C，并开始开发 macOS 或 iOS 应用程序。不断练习和探索，逐步掌握更多高级特性！


