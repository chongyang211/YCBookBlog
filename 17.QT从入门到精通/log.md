# 专栏笔记总结大全



```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 400

    Page {
        anchors.fill: parent

        // 标题栏
        header: Label {
            text: "我的页面"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            padding: 10
        }

        // 内容区域
        Column {
            anchors.centerIn: parent
            spacing: 10

            Button {
                text: "按钮 1"
                onClicked: console.log("按钮 1 被点击")
            }

            Button {
                text: "按钮 2"
                onClicked: console.log("按钮 2 被点击")
            }
        }

        // 页脚
        footer: Label {
            text: "页脚信息"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            padding: 10
        }
    }
}
```

**说明**：
- `header`：定义页面的标题栏。
- `footer`：定义页面的页脚。
- 内容区域可以放置任何 QML 组件（如 `Column`、`Row`、`Button` 等）。

---

### **2. 与 `StackView` 结合使用**

`StackView` 是用于管理多个页面的导航组件。通过 `push` 和 `pop` 方法，可以在页面之间切换。

#### **示例：多页面导航**

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 400

    StackView {
        id: stackView
        initialItem: mainPage
        anchors.fill: parent
    }

    // 主页面
    Component {
        id: mainPage
        Page {
            header: Label {
                text: "主页面"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                padding: 10
            }

            Column {
                anchors.centerIn: parent
                spacing: 10

                Button {
                    text: "跳转到页面 2"
                    onClicked: stackView.push(page2)
                }
            }
        }
    }

    // 页面 2
    Component {
        id: page2
        Page {
            header: Label {
                text: "页面 2"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                padding: 10
            }

            Column {
                anchors.centerIn: parent
                spacing: 10

                Button {
                    text: "返回主页面"
                    onClicked: stackView.pop()
                }
            }
        }
    }
}
```

**说明**：
- `StackView` 用于管理页面导航。
- `push` 方法用于跳转到新页面。
- `pop` 方法用于返回上一页面。

---

### **3. 与 `SwipeView` 结合使用**

`SwipeView` 允许用户通过滑动手势在多个页面之间切换。

#### **示例：滑动页面**

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 400

    SwipeView {
        id: swipeView
        anchors.fill: parent

        // 页面 1
        Page {
            header: Label {
                text: "页面 1"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                padding: 10
            }

            Label {
                text: "这是页面 1"
                anchors.centerIn: parent
            }
        }

        // 页面 2
        Page {
            header: Label {
                text: "页面 2"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                padding: 10
            }

            Label {
                text: "这是页面 2"
                anchors.centerIn: parent
            }
        }
    }

    // 页面指示器
    PageIndicator {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        count: swipeView.count
        currentIndex: swipeView.currentIndex
    }
}
```

**说明**：
- `SwipeView` 允许用户通过滑动手势切换页面。
- `PageIndicator` 用于显示当前页面的位置。

---

### **4. 自定义页面样式**

可以通过 `background` 属性自定义页面的背景。

#### **示例：自定义背景**

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 400

    Page {
        anchors.fill: parent

        // 自定义背景
        background: Rectangle {
            color: "lightblue"
            border.color: "gray"
            border.width: 2
        }

        Label {
            text: "自定义背景页面"
            anchors.centerIn: parent
        }
    }
}
```

---

### **5. 总结**

`Page` 是 QML 中用于构建页面的核心组件，通常与 `StackView` 或 `SwipeView` 结合使用，实现多页面导航。它的主要特点包括：
- 支持标题栏、内容区域和页脚。
- 可以与其他导航组件（如 `StackView`、`SwipeView`）无缝集成。
- 支持自定义样式和布局。

如果有更多问题或需要进一步的示例，请随时告诉我！





## 书籍

- Qt 5编程入门


4.11小结212
第5章图形动画基础213
5.1颜色、渐变和调色板213
5.1.1颜色213
5.1.2渐变216
5.1.3系统调色板216
5.2图片、边界图片和动态图片217
5.2.1图片217
5.2.2边界图片219
5.2.3动态图片221
5.3缩放、旋转和平移变换221
5.3.1使用属性实现简单变换221
5.3.2使用Transform实现高级变换223
5.4状态224
5.4.1创建状态225
5.4.2默认状态和when属性226
5.5动画和过渡227
5.5.1使用属性动画227
5.5.2使用预定义的目标和属性229
5.5.3在状态改变时使用过渡230
5.5.4使用默认的行为动画231
5.5.5使用并行或顺序动画组232
5.5.6使用动画师动画233
5.5.7控制动画的执行234
5.5.8共享动画实例235
5.6精灵动画236
5.6.1精灵引擎介绍236
5.6.2AnimatedSprite240
5.7Flickable和Flipable241
5.7.1弹动效果241
5.7.2翻转效果243
5.8小结244
第6章图形效果245
6.1混合效果245
6.2颜色效果248
6.3渐变效果251
6.3.1锥形渐变251
6.3.2线性渐变252
6.3.3辐射渐变253
6.4变形效果254
6.5阴影效果255
6.5.1投影255
6.5.2内阴影256
6.6模糊效果257
6.6.1快速模糊257
6.6.2高斯模糊258
6.6.3递归模糊259
6.6.4遮罩模糊260
6.7动感模糊效果261
6.7.1方向模糊261
6.7.2径向模糊262
6.7.3缩放模糊263
6.8发光效果263
6.8.1发光263
6.8.2矩形发光264
6.9遮罩效果266
6.9.1不透明遮罩266
6.9.2阈值遮罩267
6.10小结268
第7章粒子系统269
7.1ParticleSystem269
7.2发射器271
7.3渲染器272
7.4TrailEmitter275
7.5粒子组276
7.6随机参数278
7.7影响器279
7.7.1Age279
7.7.2Attractor280
7.7.3Friction281
7.7.4Gravity281
7.7.5Turbulence282
7.7.6Wander282
7.7.7GroupGoal282
7.7.8SpriteGoal283
7.8小结284


第10章Qt图表346
10.1创建一个图表项目347
10.2坐标轴Axes348
10.2.1数值坐标轴和对数数值坐标轴349
10.2.2分类坐标轴350
10.2.3柱形图分类坐标轴351
10.2.4日期时间坐标轴352
10.2.5坐标轴的共有属性354
10.3图例Legend354
10.4ChartView356
10.4.1设置主题357
10.4.2启用动画358
10.5使用外部数据动态创建图表359
10.6常用图表类型362
10.6.1折线图、曲线图和散点图362
10.6.2面积图364
10.6.3柱形图、堆积柱形图、百分比堆积柱形图365
10.6.4饼状图366
10.6.5盒须图（箱形图）367
10.6.6蜡烛图（K线图）368
10.6.7极坐标图369
10.7小结371
第11章Qt Data Visualization数据可视化372
11.13D柱形图372
11.1.13D坐标轴375
11.1.2数据代理377
11.1.33D系列380
11.2自定义3D场景381
11.3设置主题382
11.4选择模式和切片视图385
11.53D散点图387
11.63D曲面图389
11.7小结391
第12章多媒体应用392
12.1多媒体模块介绍392
12.2播放音频393
12.2.1播放压缩音频393
12.2.2播放未压缩音频395
12.3播放视频396
12.3.1使用Video播放视频文件396
12.3.2对视频使用图形效果397
12.4媒体播放器399
12.4.1播放音视频399
12.4.2使用Windows平台附加功能400
12.5小结406
第13章QML与C++的集成407
13.1快速选择正确的集成方式408
13.2QML运行时的C++类411
13.2.1QQmlEngine411
13.2.2QQmlContext412
13.2.3QQmlComponent414
13.2.4QQmlExpression415
13.3在QML中使用C++特性416
13.3.1数据类型处理和拥有权416
13.3.2数据类型的转换416
13.3.3使用C++属性419
13.3.4使用函数和槽423
13.3.5使用信号425
13.4注册QML类型426
13.4.1注册可实例化对象类型426
13.4.2注册不可实例化对象类型427
13.4.3注册单例类型427
13.4.4类型的修订和版本428
13.5定义QML特定类型和属性429
13.5.1提供附加对象注解数据429
13.5.2属性修饰符类型432
13.5.3指定QML对象类型的默认属性434
13.5.4接收对象初始化通知435
13.6在C++中使用QML对象436
13.6.1使用C++加载QML对象436
13.6.2使用对象名字访问加载的QML对象438
13.6.3使用C++访问QML对象成员439
13.7小结442
第14章使用Qt Quick设计器443
14.1Qt Quick UI表单文件444
14.2常用操作448
14.2.1管理项目层次448
14.2.2QML类型库449
14.2.3指定项目属性449
14.2.4添加状态452
14.2.5在画布上操作QML类型452
14.3使用设计器编辑QML文档457
14.3.1创建项目457
14.3.2创建主视图458
14.3.3添加视图461
14.3.4为视图添加动画462
14.4小结464
第15章Qt移动开发入门465
15.1Qt Android开发配置465
15.1.1工具软件的下载465
15.1.2工具软件的安装466
15.1.3使用虚拟机运行Qt Android程序468
15.1.4使用手机运行Qt Android程序471
15.1.5发布项目472
15.2Qt iOS开发配置476
15.2.1使用模拟器运行Qt iOS程序477
15.2.2使用手机运行Qt iOS程序478
15.3创建演示程序481
15.3.1创建应用主窗口481
15.3.2拍照功能486
15.3.3多点触控488
15.3.4传感器489
15.3.5地图491
15.3.6蓝牙493