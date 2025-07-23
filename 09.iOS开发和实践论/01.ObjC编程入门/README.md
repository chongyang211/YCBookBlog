

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
- 2.1.5 类默认构造器：默认构造器是一个类的默认初始化方法，用于创建类的实例并对其进行初始化。返回值通常是 id
- 2.1.6 自定义构造器：指开发者自己定义的初始化方法，用于创建类的实例并对其进行初始化。
- 2.1.7 类继承和实现：Objective-C 是一种面向对象的语言，支持单继承。子类可以从父类继承属性和方法，同时也可以重写父类的方法或添加新的方法和属性。
- 2.2.1 方法的声明：方法的声明包括方法的返回类型、方法名和参数列表。`+` 表示类方法（类似静态方法），`-` 表示实例方法。
- 2.2.2 方法的实现：方法的实现部分包含了方法的具体代码逻辑。
- 2.2.3 方法的调用：[my doSomething];[myObject greetWithName:@"John"];
- 2.2.4 方法的嵌套调用：
- 2.2.5 方法访问权限：没有严格的方法访问控制机制，将方法声明在 .m 文件中，而不暴露在 .h 文件中。使用类别（Category）隐藏方法。
- 2.2.6 方法的重写：OC 提供了 super 关键字，可以在子类的方法中显示调用超类中的方法，向 super 发送消息等价于对超类发送消息。
- 2.3.1 属性的定义：
- 2.3.2 属性修饰符：
- 2.3.3 synthesize：
- 2.3.4 属性的访问：






## **1. 引用计数（Reference Counting）**

### **1.1 引用计数的规则**
- 当对象被创建时，引用计数为 1。
- 当对象被其他对象引用时，引用计数加 1。
- 当对象不再被引用时，引用计数减 1。
- 当引用计数为 0 时，对象被销毁，内存被释放。

### **1.2 手动管理引用计数的方法**
- **`retain`**：增加对象的引用计数。
- **`release`**：减少对象的引用计数。
- **`autorelease`**：将对象添加到自动释放池，稍后释放。
- **`dealloc`**：对象被销毁时调用的方法，用于释放资源。

#### 示例：
```objc
// 创建对象，引用计数为 1
NSObject *obj = [[NSObject alloc] init];

// 增加引用计数
[obj retain]; // 引用计数为 2

// 减少引用计数
[obj release]; // 引用计数为 1

// 将对象添加到自动释放池
[obj autorelease]; // 引用计数为 1，稍后释放

// 手动释放对象
[obj release]; // 引用计数为 0，对象被销毁
```

---

## **2. 自动释放池（Autorelease Pool）**
自动释放池用于延迟对象的释放。当对象被发送 `autorelease` 消息时，它会被添加到当前的自动释放池中。当自动释放池被释放时，池中的所有对象都会收到 `release` 消息。

### **2.1 使用自动释放池**
```objc
@autoreleasepool {
    // 创建对象并添加到自动释放池
    NSString *str = [[[NSString alloc] initWithFormat:@"Hello, %@", @"World"] autorelease];

    // 使用对象
    NSLog(@"%@", str);

    // 自动释放池结束时，str 会被释放
}
```

---

## **3. 自动引用计数（ARC, Automatic Reference Counting）**
ARC 是 Objective-C 的编译器特性，它自动管理对象的引用计数，开发者无需手动调用 `retain`、`release` 和 `autorelease`。

### **3.1 ARC 的规则**
- 编译器会自动在适当的位置插入 `retain`、`release` 和 `autorelease`。
- 开发者不能手动调用 `retain`、`release` 和 `autorelease`。
- 对象的生命周期由编译器管理。

### **3.2 ARC 的使用**
```objc
// 创建对象，ARC 自动管理引用计数
NSString *str = [[NSString alloc] initWithFormat:@"Hello, %@", @"World"];

// 使用对象
NSLog(@"%@", str);

// 不需要手动释放，ARC 会自动处理
```

---

## **4. 内存管理的最佳实践**
### **4.1 避免循环引用**
循环引用会导致内存泄漏。使用 `weak` 引用或 `__weak` 修饰符来打破循环引用。

#### 示例：
```objc
__weak typeof(self) weakSelf = self;
self.block = ^{
    [weakSelf doSomething]; // 使用 weakSelf 避免循环引用
};
```

### **4.2 使用 `@autoreleasepool` 管理临时对象**
在循环中创建大量临时对象时，使用 `@autoreleasepool` 及时释放内存。

#### 示例：
```objc
for (int i = 0; i < 1000; i++) {
    @autoreleasepool {
        NSString *tempStr = [NSString stringWithFormat:@"Temp %d", i];
        NSLog(@"%@", tempStr);
    }
}
```

### **4.3 使用 `dealloc` 释放资源**
在 `dealloc` 方法中释放对象持有的资源。

#### 示例：
```objc
- (void)dealloc {
    // 释放资源
    [_resource release];
    [super dealloc];
}
```

---

## **5. 手动内存管理与 ARC 的对比**
| **特性**               | **手动内存管理**                     | **ARC**                              |
|------------------------|--------------------------------------|--------------------------------------|
| **引用计数管理**        | 手动调用 `retain`、`release`         | 编译器自动管理                       |
| **代码复杂度**          | 较高，容易出错                       | 较低，减少内存管理错误               |
| **性能**               | 需要手动优化                         | 编译器优化，性能较好                 |
| **适用场景**           | 需要精细控制内存的场合               | 大多数现代 Objective-C 项目          |

---

## **6. 示例代码**
以下是一个综合示例，展示了手动内存管理和 ARC 的使用：

### **手动内存管理**
```objc
#import <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // 创建对象
        NSObject *obj = [[NSObject alloc] init];

        // 增加引用计数
        [obj retain];

        // 使用对象
        NSLog(@"Object: %@", obj);

        // 减少引用计数
        [obj release];

        // 将对象添加到自动释放池
        [obj autorelease];

        // 手动释放对象
        [obj release];
    }
    return 0;
}
```

### **ARC**
```objc
#import <Foundation/Foundation.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // 创建对象，ARC 自动管理引用计数
        NSString *str = [[NSString alloc] initWithFormat:@"Hello, %@", @"World"];

        // 使用对象
        NSLog(@"%@", str);

        // 不需要手动释放，ARC 会自动处理
    }
    return 0;
}
```

---

通过掌握 Objective-C 的内存管理机制，你可以编写出高效且稳定的应用程序。如果使用 ARC，可以大大减少内存管理的复杂性，但理解引用计数的原理仍然非常重要。





