# iOS开发和实践论


## 04.Objective-C编程上

- 5.1 在OC中类规格是什么样：类规格说明包含了两个部分，声明interface与实现implementation。
- 5.2 在OC中类的声明：类声明一般定义在.h头文件中。类声明以关键字@interface作为开始，@end作为结束。可以声明一些属性和方法！
- 5.3 在OC中类的实现：以关键字@implementation作为区块起头，@end结尾。类实现包含了公开方法的实现，以及定义私有（private） 变量及方法。
- 5.4 类的实例化如何理解：Objective-C创建对象需通过alloc以及init两个消息。alloc的作用是分配内存，init则是初始化对象。
- 5.7 类继承和实现理解：Objective-C 是一种面向对象的语言，支持单继承。子类可以从父类继承属性和方法，同时也可以重写父类的方法或添加新的方法和属性。






## 06.UIKit框架原理探索

**01.UIKit框架概述**

1. UIKit框架说明：提供了一套丰富的控件和API，其中包含：视图（Views），控制器（ViewControllers），窗口（Windows），布局（Layouts）。
2. UIView视图设计：UIView是UIKit框架里面最基础的视图类，UIView则负责内容的显示和布局
3. ViewController设计：用于管理一个或多个视图的显示和行为。它可以包含其他视图控制器、视图和控件，以及处理与这些元素相关的逻辑。


**06.Window窗口设计思想**



## 参考

- iOS 学习笔记：https://github.com/xiusl/ObjectiveC_Day
- iOS 学习视频：https://space.bilibili.com/226689868/lists/45553?type=season
- iOS微信支付接入指南：https://developers.weixin.qq.com/doc/oplatform/Mobile_App/Access_Guide/iOS.html
- Android微信支付接入指南：https://developers.weixin.qq.com/doc/oplatform/Mobile_App/Access_Guide/Android.html

- 从0到1搞定iOS支付接入：微信、支付宝、订阅一文通关：https://blog.csdn.net/bryant_liu24/article/details/144997299
- Swift调用微信支付宝SDK(Swift4.0):https://www.cnblogs.com/vlucht/p/11147540.html
- iOS-底层文章汇总：https://juejin.cn/post/6987548539198898183
- iOS 底层原理文章列表：https://juejin.cn/post/7047391805167370253?from=search-suggest


apple开发者账号申请好了，你可以到时候试试，我到时候去研究下怎么报销
bingyiwei@tencent.com
wby1830248E@

收付通app包名： com.wecard.zf
微卡刷掌app包名：com.wecard.palm
微卡空中录掌app包名：com.wecard.air
收付通正式包名：com.johannli.WecardSDKDemo


支付：
1.微信调通
2.缺少调试支付宝，支付宝缺少配置scheme
3.加载本地html白屏
