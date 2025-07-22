


### **变量与数据类型**
- 基本数据类型：
  ```objective-c
  int age = 25;
  float height = 1.75;
  double pi = 3.14159;
  char initial = 'A';
  BOOL isStudent = YES; // BOOL 是 Objective-C 的布尔类型
  ```
- 字符串：
  ```objective-c
  NSString *name = @"John";
  ```

### **控制结构**
- 条件语句：
  ```objective-c
  int score = 85;
  if (score >= 90) {
      NSLog(@"优秀");
  } else if (score >= 60) {
      NSLog(@"及格");
  } else {
      NSLog(@"不及格");
  }
  ```
- 循环语句：
  ```objective-c
  for (int i = 0; i < 5; i++) {
      NSLog(@"i = %d", i);
  }

  int j = 0;
  while (j < 5) {
      NSLog(@"j = %d", j);
      j++;
  }
  ```

---

## **3. 面向对象编程**
### **类与对象**
- 定义类：
  ```objective-c
  // Person.h
  @interface Person : NSObject

  @property (nonatomic, strong) NSString *name;
  @property (nonatomic, assign) int age;

  - (void)introduce;

  @end

  // Person.m
  @implementation Person

  - (void)introduce {
      NSLog(@"我叫 %@，今年 %d 岁。", self.name, self.age);
  }

  @end
  ```
- 使用类：
  ```objective-c
  Person *person = [[Person alloc] init];
  person.name = @"Alice";
  person.age = 30;
  [person introduce];
  ```

### **方法**
- 实例方法：
  ```objective-c
  - (void)sayHello {
      NSLog(@"Hello!");
  }
  ```
- 类方法：
  ```objective-c
  + (void)sayHello {
      NSLog(@"Hello from class method!");
  }
  ```

### **继承**
```objective-c
// Student.h
@interface Student : Person

@property (nonatomic, strong) NSString *school;

@end

// Student.m
@implementation Student

- (void)introduce {
    [super introduce];
    NSLog(@"我在 %@ 上学。", self.school);
}

@end
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


