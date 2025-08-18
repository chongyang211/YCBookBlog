# 专栏笔记总结大全

python decrypt_log.py /Users/yangchong/Downloads/standardlog 0102030405060708090a0b0c0d0e0f10 aabbccddeeff00112233445566778899

python decrypt_log.py /Users/yangchong/Downloads/standardlog/yt_palm.log 0102030405060708090a0b0c0d0e0f10 aabbccddeeff00112233445566778899

python decrypt_log.py /Users/yangchong/Downloads/palm_logs_2025-08-14T09_31_37_253Z/app_log/yt_palm.log 0102030405060708090a0b0c0d0e0f10 aabbccddeeff00112233445566778899


python decrypt_log.py /Users/yangchong/Downloads/palm_logs_2025-08-15T02_30_23_165Z/app_log/yt_palm.log 0102030405060708090a0b0c0d0e0f10 aabbccddeeff00112233445566778899
python decrypt_log.py /Users/yangchong/Downloads/palm_logs_2025-08-14T09_31_37_253Z/app_log/yt_palm.log 0102030405060708090a0b0c0d0e0f30 101116171415a5ba98999e9f9c9d9293

git tag v1.2.0-std-2025081517-m3

git tag -d v1.2.0-std-20250815-m3
git push origin --delete v1.2.0-std-20250815-m3
git tag -d v1.2.0-std-2025081516-m3
git push origin --delete v1.2.0-std-2025081516-m3

git tag -l "v1.2.0-std*"


updateEngine --image_url=/data/system_ota/update.img --misc=update --savepath=/data/system_ota/update.img --version_url=/data/system_ota/version

updateEngine --image_url=/userdata/system_ota/update.img --misc=update --savepath=/userdata/system_ota/update.img --reboot &

kw5845KWF

aikit build  -p docker -i 3bf4efdb527e -f rebuild -n rv1109-m3

aikit build  -p docker -i 3bf4efdb527e -f rebuild -n rv1109-m3 -- -d 2>&1 | tee build_log.txt

Android:
用户：sg.neutron.palmintegration
空中录掌体验demo：com.tencent.wecard.oversea.airpalm
空中录掌新包名：com.tencent.youtu.mobilepalm.demo

iOS:
用户：sg.neutron.palmintegration
空中录掌体验demo：com.wecard.air
空中录掌新包名：com.tencent.palm.PalmMa

## 学习资源

- [Qt 官方文档：Qt Quick](https://doc.qt.io/qt-6/qtquick-index.html)
- [Qt Quick 示例](https://doc.qt.io/qt-6/qtquick-examples.html)
- [QML 教程](https://qmlbook.github.io/)

## 书籍资料

- https://github.com/cwc1987/QmlBook-In-Chinese

## 学习资料

- 比较完整的QT学习资料：https://github.com/lichangke/QT

## 推荐资料

- QML6(Qt Quick)开发教程（完整版）：https://www.bilibili.com/video/BV1j14y1e7Hf
- Qt5/Qt6/Qml 手把手带你从零基础手搓一个网易云音乐桌面前端界面：https://www.bilibili.com/video/BV1YUcGeQEBq

## 视频资料

- 零基础入门学习qt，项目实战教学：https://www.bilibili.com/video/BV1qhSiYwEdk
- 最新QT从入门到实战完整版|传智教育|Qt Widget开发：https://www.bilibili.com/video/BV1g4411H78N
- C++QT2024年最新版：https://www.bilibili.com/video/BV1GW42197ff/
- Qt6 C++实战1：从零搭建《阿西拜教学管理系统》完整版：https://www.bilibili.com/video/BV1GQfpYCE96


## 一般视频

- C++-QT新手入门项目：Qt音乐播放器：https://www.bilibili.com/video/BV1bz9FYREEQ
- QT快速入门 | 最简单最简洁的QT入门教程 | 嵌入式UI：https://www.bilibili.com/video/BV1N34y1H7x7
- Qt开发实战项目教程：https://www.bilibili.com/video/BV1ng4y1w7w7

## 优秀项目

- https://github.com/cppqtdev
- C++和qml小说阅读器项目：https://github.com/RownH/qReader
- 中文版《QmlBook》，原作地址QmlBook：https://github.com/cwc1987/QmlBook-In-Chinese
- UI库：https://github.com/zhuzichu520/FluentUI

## 书籍

- Qt 5编程入门

第1章走进Qt Quick的世界1
1.1全新的QML和Qt Quick1
1.1.1历史变迁1
1.1.2QML2
1.1.3Qt Quick2
1.1.4Qt QML和Qt Quick框架3
1.2Qt Quick项目4
1.2.1Qt 5的下载与安装4
1.2.2创建Qt Quick应用6
1.2.3创建Qt Quick UI项目11
1.2.4运行示例程序13
1.2.5Qt Quick程序的发布14
1.3小结16
第2章QML语法17
2.2import导入语句20
2.2.1模块（命名空间）导入语句20
2.2.2目录导入语句21
2.2.3JavaScript资源导入语句23
2.3QML类型系统24
2.3.1基本类型24
2.3.2JavaScript类型26
2.3.3对象类型26
2.4对象特性26
2.4.1id特性27
2.4.2属性特性28
2.4.3信号和信号处理器特性36
2.4.4方法特性40
2.4.5附加属性和附加信号处理器41
2.4.6枚举43
2.5集成JavaScript43
2.5.1JavaScript表达式43
2.5.2从JavaScript动态创建QML对象49
2.5.3在QML中定义JavaScript资源53
2.5.4在QML中导入JavaScript资源56
2.5.5JavaScript宿主环境58
2.6QML文档59
2.6.1通过QML文档定义对象类型60
2.6.2QML组件62
2.6.3作用域和命名解析64
2.6.4资源加载和网络透明性70
2.6.5QML的国际化71
2.6.6QML的编码约定75
2.7QML模块78
2.7.1定义一个QML模块78
2.7.2支持的QML模块类型79
2.8小结79
第3章Qt Quick基础80
3.1基础可视项目81
3.1.1Item81
3.1.2Rectangle85
3.1.3Text85
3.1.4TextInput92
3.1.5TextEdit97
3.2布局管理99
3.2.1定位器99
3.2.2基于锚的布局103
3.2.3Layouts106
3.3事件处理110
3.3.1MouseArea110
3.3.2鼠标事件和滚轮事件112
3.3.3拖放事件114
3.3.4键盘事件和焦点作用域117
3.3.5定时器123
3.4使用Loader动态加载组件124
3.4.1Loader的大小与行为124
3.4.2从加载的项目中接收信号125
3.4.3焦点和键盘事件126
3.5小结127
第4章Qt Quick控件128
4.1Qt Quick控件基础129
4.2按钮类控件139
4.2.1AbstractButton139
4.2.2Button141
4.2.3CheckBox142
4.2.4RadioButton144
4.2.5ButtonGroup145
4.2.6RoundButton146
4.2.7Switch147
4.2.8DelayButton148
4.2.9Action和ActionGroup149
4.3指示类控件151
4.3.1Label151
4.3.2ProgressBar151
4.3.3BusyIndicator152
4.3.4PageIndicator153
4.4输入类控件154
4.4.1TextField154
4.4.2TextArea155
4.4.3ComboBox156
4.4.4SpinBox159
4.4.5Dial162
4.4.6Slider163
4.4.7RangeSlider165
4.4.8Tumbler166
4.5菜单类控件167
4.5.1Menu167
4.5.2MenuBar171
4.6容器类控件173
4.6.1Pane173
4.6.2Frame174
4.6.3GroupBox174
4.6.4Page175
4.6.5ScrollView176
4.6.6ScrollBar和ScrollIndicator178
4.6.7StackView180
4.6.8TabBar185
4.6.9ToolBar187
4.6.10SwipeView188
4.7弹出类控件190
4.7.1Overlay190
4.7.2Popup191
4.7.3Dialog194
4.7.4ToolTip195
4.7.5Drawer196
4.8标准对话框197
4.8.1ColorDialog197
4.8.2FileDialog198
4.8.3FontDialog199
4.8.4MessageDialog200
4.9Qt Quick Controls 2配置文件202
4.10共享业务逻辑代码202
4.10.1图片查看器203
4.10.2文件选择器207
4.10.3重构图片选择器207
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
第8章Canvas 2D285
8.1Canvas的使用285
8.2绘制操作287
8.2.1绘制参数设置288
8.2.2绘制矩形290
8.2.3状态的保存与恢复291
8.2.4绘制文本293
8.2.5绘制路径294
8.3渐变填充299
8.3.1线性渐变299
8.3.2辐射渐变300
8.3.3锥型渐变301
8.4阴影301
8.5使用图像302
8.6坐标变换304
8.6.1平移305
8.6.2缩放305
8.6.3旋转306
8.6.4斜切307
8.6.5transform()函数总结308
8.7小结309
第9章模型和视图310
9.1模型/视图架构简介310
9.2数据模型312
9.2.1整数作为模型312
9.2.2ListModel313
9.2.3WorkerScript315
9.2.4ObjectModel316
9.2.5DelegateModel317
9.2.6Package319
9.2.7XmlListModel320
9.2.8LocalStorage322
9.2.9使用C++扩展QML模型326
9.3视图类型330
9.3.1ListView330
9.3.2GridView336
9.3.3视图过渡338
9.3.4PathView341
9.4调整性能344
9.5小结345
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
13.2QML运行时C++类411
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



CHONGYYANG-MB1:~ yangchong$ adb push /Users/yangchong/yt-palm /oem/yt-palm/
/Users/yangchong/yt-palm: 1 file pushe...ed. 2.4 MB/s (8255388 bytes in 3.226s)
CHONGYYANG-MB1:~ yangchong$ adb shell
[root@RV1126_RV1109:/]# /oem/yt-palm/run.sh
LD_LIBRARY_PATH=/oem/yt-palm/libs:
qt.qpa.wayland: qtvirtualkeyboard currently is not supported at client-side, use QT_IM_MODULE=qtvirtualkeyboard at compositor-side.
QStandardPaths: wrong permissions on runtime directory /var/run, 7755 instead of 7700
[I] <MMKV.cpp:172::initialize> version v2.1.0, page size 4096, arch armeabi-v7a/NEON (hard-float)
[I] <MMKV.cpp:229::initializeMMKV> root dir: /oem/yt-palm/preference
[I] <MMKV.cpp:259::mmkvWithID> prepare to load mmkv.default (id 731bea0aecca1fdc1986674f2bb78e64) from rootPath /oem/yt-palm/preference
[I] <MemoryFile.cpp:103::open> open fd[8], flag 0x7, /oem/yt-palm/preference/mmkv.default
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6f8e000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default]
[I] <MemoryFile.cpp:103::open> open fd[9], flag 0x7, /oem/yt-palm/preference/mmkv.default.crc
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6f8d000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default.crc]
[I] <MMKV_IO.cpp:244::loadMetaInfoAndCheck> meta file [mmkv.default] has flag [0]
[I] <MMKV_IO.cpp:84::loadFromFile> loading [mmkv.default] with 2072 actual size, file size 4096, InterProcess 0, meta info version:4
[I] <MMKV_IO.cpp:89::loadFromFile> loading [mmkv.default] with crc 3262232614 sequence 1 version 4
[I] <MMKV_IO.cpp:133::loadFromFile> loaded [mmkv.default] with 21 key-values
[2025-08-01 12:23:45.089] [info] [1225-1225]|[src/main.cpp:69]Application started with log level: 2
[2025-08-01 12:23:45.106] [info] [1225-1225]|[register/yt_palm_register.cpp:42]switch palm register mode to: StdQRCodeRegister
[2025-08-01 12:23:45.175] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:172]GetWifiState wifiState : 4
[2025-08-01 12:23:45.187] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:186]IsEthernetConnected isConnected : 0
[2025-08-01 12:23:45.187] [info] [1225-1225]|[hal/yt_device_hal_manager.cpp:156]RefreshNetworkStatus network connected changed: 1 network connected: 1 network type: 0 wifi state: CONNECTED ethernet connected: 0
[2025-08-01 12:23:45.187] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:204]StartObserveNetworkStatus
[2025-08-01 12:23:45.190] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:230]Network status monitoring started
[2025-08-01 12:23:45.190] [info] [1225-1225]|[update/yt_update_manager.cpp:65]Detect Xike device to config OTA mode :1
[2025-08-01 12:23:45.191] [warning] [1225-1225]|[access/yt_access_manager.cpp:187]YtAccessManager Open door not supported
[2025-08-01 12:23:45.191] [error] [1225-1225]|[access/yt_access_manager.cpp:21]Failed to initialize strategy
[2025-08-01 12:23:45.191] [info] [1225-1225]|[access/yt_access_manager.cpp:23]YtAccessManager created
[2025-08-01 12:23:45.192] [info] [1225-1225]|[audio/yt_audio_manager.cpp:29]AudioManager constructor called
[2025-08-01 12:23:45.192] [info] [1225-1225]|[audio/yt_audio_manager.cpp:359]Initializing audio priorities
[2025-08-01 12:23:45.192] [info] [1225-1225]|[audio/yt_audio_manager.cpp:121]Initializing audio resources
[2025-08-01 12:23:45.192] [info] [1225-1225]|[audio/yt_audio_manager.cpp:122]Audio resources initialized
[2025-08-01 12:23:45.192] [info] [1225-1225]|[audio/yt_audio_manager.cpp:48]Created new AudioManager instance
[2025-08-01 12:23:45.192] [info] [1225-1225]|[audio/yt_audio_manager.cpp:69]Initializing audio manager
[2025-08-01 12:23:45.409] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:106]GetCurrentVolume volume : 30
[2025-08-01 12:23:45.410] [info] [1225-1225]|[audio/yt_audio_manager.cpp:88]Synced audio volume with system: 30
[2025-08-01 12:23:45.410] [info] [1225-1225]|[audio/yt_audio_manager.cpp:95]Audio manager initialized successfully
[2025-08-01 12:23:45.411] [info] [1225-1225]|[src/main.cpp:171]Detect Xike device to config app:1
qrc:/qml/main.qml:203: TypeError: Cannot call method 'info' of undefined
[2025-08-01 12:23:45.716] [warning] [Qt-Warning] qrc:/qml/main.qml:203: TypeError: Cannot call method 'info' of undefined (qrc:/qml/main.qml:203)
qt.qpa.wayland: No shell integration named "xdg-shell" found
[2025-08-01 12:23:45.780] [warning] [Qt-Warning] No shell integration named "xdg-shell" found
qml: LogService singleton initialized
qml: YTLoggerImpl可用，发送测试日志
[2025-08-01 12:23:45.828] [info] [QML] LogService初始化成功
qml: 已发送测试信号
[2025-08-01 12:23:45.831] [info] [QML] Application started
[2025-08-01 12:23:45.832] [info] [QML] Log service available: true
[2025-08-01 12:23:45.833] [info] [QML] Keyboard height configured: 480
[2025-08-01 12:23:45.838] [info] [QML] WifiStatusIndicator initialized
[2025-08-01 12:23:45.850] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:345]StartWatchdog feed_divide : 30 seconds
[2025-08-01 12:23:45.850] [info] [1225-1225]|[app/yt_app_watchdog_looper.cpp:36]Watchdog interval calculation - system timeout: 30s, feed interval: 10s, actual interval: 20s
[2025-08-01 12:23:45.850] [info] [1225-1225]|[app/yt_app_watchdog_looper.cpp:36]Watchdog interval calculation - system timeout: 30s, feed interval: 10s, actual interval: 20s
[2025-08-01 12:23:45.850] [info] [1225-1225]|[app/yt_app_watchdog_looper.cpp:12]WatchdogLooper initialized with interval: 20000ms
[2025-08-01 12:23:45.850] [info] [1225-1225]|[utils/yt_future_task.cpp:22]Starting future task name WatchdogLooper, interval ms 20000, single_shot 0, auto_stop 1
[2025-08-01 12:23:45.850] [info] [1236-1225]|[xike/yt_xike_device_hal_impl.cpp:75]GetSerialNumber sn : CK0A3KP02526000185
[2025-08-01 12:23:45.850] [info] [1225-1225]|[utils/yt_future_task.cpp:83]Stop future task WatchdogLooper
[2025-08-01 12:23:45.850] [info] [1236-1225]|[xike/yt_xike_device_hal_impl.cpp:79]GetSerialNumber ota version : 110
[2025-08-01 12:23:45.850] [info] [1236-1225]|[palm/yt_palm_service.cpp:436]InitPalm start app version: v1.2.0.2025080102-9c1ab75, SN : CK0A3KP02526000185
[2025-08-01 12:23:45.851] [info] [1236-1225]|[palm/yt_palm_service.cpp:976]PalmManager log level set to 2
[2025-08-01 12:23:45.852] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022225851765|INFO|[src/yt_palm_manager_impl.cpp:68]init palm manager
[2025-08-01 12:23:45.852] [info] [1253-1225]|[utils/yt_future_task.cpp:55]handleThreadStarted WatchdogLooper in thread 0x84dfb0
[2025-08-01 12:23:45.855] [info] [1253-1225]|[xike/yt_xike_device_hal_impl.cpp:367]FeedWatchdog ret : 0
2025-08-01 12:23:47.858 [info]  [ ANG_SDK ] attached
2025-08-01 12:23:55.732 [info]  [ ANG_SDK ] open camera successful
2025-08-01 12:23:55.733 [info]  [ ANG_SDK ] start the device listener
[2025-08-01 12:23:55.733] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235733154|INFO|[src/yt_palm_manager_impl.cpp:90]module connected
[2025-08-01 12:23:55.734] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235733681|INFO|[src/yt_palm_manager_impl.cpp:112]open camera ret:0
[2025-08-01 12:23:55.785] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235784775|INFO|[src/yt_palm_manager_impl.cpp:1578]sync time: 2025-08-01 04:23:55 ret:0
[2025-08-01 12:23:55.789] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235788874|INFO|[src/yt_palm_manager_impl.cpp:635]switch device status:working2 success, cost: 3ms
[2025-08-01 12:23:55.791] [info] [1256-1225]|[palm/yt_palm_service.cpp:504]HandlePalmDeviceStatusmodule status changed from Unknown to Working2
[2025-08-01 12:23:55.791] [info] [1237-1225]|[palm/yt_palm_service.cpp:511]_HandlePalmDeviceStatusmodule status changed from Unknown to Working2
[2025-08-01 12:23:55.855] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235854974|INFO|[src/yt_palm_manager_impl.cpp:410]=== Module SN: YTA8000F20000047
[2025-08-01 12:23:55.856] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235855247|INFO|[src/yt_palm_manager_impl.cpp:412]=== PalmManager Version: v1.1.13.2025053006-153994ad
[2025-08-01 12:23:55.856] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235856200|INFO|[src/yt_palm_manager_impl.cpp:414]=== AngstrongSDK Version: v4.9.7.20250516
[2025-08-01 12:23:55.859] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235859112|INFO|[src/yt_palm_manager_impl.cpp:417]=== Firmware Version: V02.0.6_202506242120V4.9.5.2025061921
[2025-08-01 12:23:55.872] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235872386|INFO|[src/yt_palm_manager_impl.cpp:420]=== AlgoSDK Version: v1.2.8.2025061912-60e8ea92
[2025-08-01 12:23:55.877] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022235876800|INFO|[src/yt_palm_manager_impl.cpp:422]=== Algo Version: v1.0
2025-08-01 12:23:58.172 [error]  [ ANG_SDK ] exec DHCP fail! ret 255
[2025-08-01 12:23:58.174] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238172807|INFO|[src/yt_palm_manager_impl.cpp:426]=== Module Run DHCP, ret:-1
2025-08-01 12:23:58.271 [error]  [ ANG_SDK ] Network not working!!!
[2025-08-01 12:23:58.272] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238271584|INFO|[src/yt_palm_manager_impl.cpp:428]=== verifyNetwork, ret:-1
[2025-08-01 12:23:58.272] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238271759|INFO|[src/yt_palm_manager_impl.cpp:212]switch mode: 2
[2025-08-01 12:23:58.272] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238271873|INFO|[src/yt_palm_manager_impl.cpp:321]=== setConfig solution: 3
[2025-08-01 12:23:58.272] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238271986|INFO|[src/yt_palm_manager_impl.cpp:322]=== setConfig mode: 2
[2025-08-01 12:23:58.273] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238272085|INFO|[src/yt_palm_manager_impl.cpp:323]=== setConfig register_mode: 3
[2025-08-01 12:23:58.273] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238272197|INFO|[src/yt_palm_manager_impl.cpp:324]=== setConfig recognize_mode: 4
[2025-08-01 12:23:58.285] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238284904|INFO|[src/yt_palm_manager_impl.cpp:327]=== setConfig for prefer quality: normal, ret: 0
[2025-08-01 12:23:58.294] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238294284|INFO|[src/yt_palm_manager_impl.cpp:332]=== setConfig for prefer disable_feature: 0, ret: 0
[2025-08-01 12:23:58.299] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238298979|INFO|[src/yt_palm_manager_impl.cpp:338]=== setConfig for prefer disable_retrieval: 0, ret: 0
[2025-08-01 12:23:58.306] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238306188|INFO|[src/yt_palm_manager_impl.cpp:345]=== setConfig for prefer retrieval_level: normal, ret: 0
[2025-08-01 12:23:58.312] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238312051|INFO|[src/yt_palm_manager_impl.cpp:353]=== setConfig for prefer disable_preferred_image: 0, ret: 0
[2025-08-01 12:23:58.320] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238320089|INFO|[src/yt_palm_manager_impl.cpp:361]=== setConfig for recognize disable_preferred_image: 0, ret: 0
[2025-08-01 12:23:58.343] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238343343|INFO|[src/yt_palm_manager_impl.cpp:370]=== setConfig for recognize export_image_after_retrieval_empty: 1, ret: 0
[2025-08-01 12:23:58.358] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238358273|INFO|[src/yt_palm_manager_impl.cpp:445]=== setConfig for report image: 0, ret: 0
[2025-08-01 12:23:58.363] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238363134|INFO|[src/yt_palm_manager_impl.cpp:456]=== setConfig for enable session: 1, ret: 0
[2025-08-01 12:23:58.374] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238374353|INFO|[src/yt_palm_manager_impl.cpp:462]=== setConfig for report threshold: 1, ret: 0
2025-08-01 12:23:58.384 [info]  [ ANG_SDK ] setEventCallback PREFER
[2025-08-01 12:23:58.384] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238384335|INFO|[src/yt_palm_manager_impl.cpp:468]=== setConfig for report score: 1, ret: 0
2025-08-01 12:23:58.386 [info]  [ ANG_SDK ] setEventCallback RECOGNIZEPRO
[2025-08-01 12:23:58.386] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238384845|INFO|[src/yt_palm_manager_impl.cpp:479]=== setConfig for prefer event callback, ret: 0
[2025-08-01 12:23:58.387] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238386497|INFO|[src/yt_palm_manager_impl.cpp:490]=== setConfig for recognize event callback, ret: 0
[2025-08-01 12:23:58.409] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238409553|INFO|[src/yt_palm_manager_impl.cpp:503]=== setConfig for int8 feature: 1
2025-08-01 12:23:58.410 [debug]  [ ANG_SDK ] go to get psensor value
[2025-08-01 12:23:58.409] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238409696|INFO|[src/yt_palm_manager_impl.cpp:509]=== setConfig for local metrics: 64
[2025-08-01 12:23:58.410] [warning] [1254-1225]|[palm/yt_palm_service.cpp:989][Palmmgr]1754022238409975|WARN|[src/yt_local_metric_mgr.cpp:173]LocalMetricMgr::LoadMetrics not exist: /userdata/palm_metrics/v1.2.8.2025061912-60e8ea92/metrics.json
[2025-08-01 12:23:58.410] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238410265|INFO|[src/yt_palm_manager_impl.cpp:138]Test device status...
2025-08-01 12:23:58.720 [debug]  [ ANG_SDK ] go to get psensor value finish
[2025-08-01 12:23:58.783] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238783401|INFO|[src/yt_palm_manager_impl.cpp:1578]sync time: 2025-08-01 04:23:58 ret:0
[2025-08-01 12:23:58.787] [info] [1256-1225]|[palm/yt_palm_service.cpp:504]HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-08-01 12:23:58.787] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022238786440|INFO|[src/yt_palm_manager_impl.cpp:635]switch device status:working success, cost: 2ms
[2025-08-01 12:23:58.787] [info] [1237-1225]|[palm/yt_palm_service.cpp:511]_HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-08-01 12:23:59.098] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022239097696|INFO|[src/yt_palm_manager_impl.cpp:1984]stop all with reason:1
[2025-08-01 12:23:59.112] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022239112372|INFO|[src/yt_palm_manager_impl.cpp:2015]stop all success, cost:14ms
[2025-08-01 12:23:59.113] [warning] [1254-1225]|[palm/yt_palm_service.cpp:989][Palmmgr]1754022239112632|WARN|[src/yt_palm_manager_impl_verify.cpp:36]ignore ret 1100
[2025-08-01 12:23:59.113] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022239113126|INFO|[src/yt_palm_manager_impl.cpp:140]Test device status finish, ret: 0
[2025-08-01 12:23:59.113] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022239113520|INFO|[src/yt_palm_manager_impl.cpp:146]init finish, ret: 0 cost: 13260ms open:9880ms
[2025-08-01 12:23:59.136] [info] [1236-1225]|[palm/yt_palm_service.cpp:472]Init PalmManager ret  0 palmManagerVersion: v1.1.13.2025053006-153994ad, firmwareVersion: V02.0.6_202506242120V4.9.5.2025061921, algoSdkVersion: v1.2.8.2025061912-60e8ea92, algoVersion: v1.0, palmMode: 2
[2025-08-01 12:23:59.142] [info] [1236-1225]|[palm/yt_palm_service.cpp:496]PalmManager currentPalmCount: 0
[2025-08-01 12:23:59.143] [info] [1236-1225]|[palm/yt_palm_context.h:529][Success] Module return code: 0
[2025-08-01 12:23:59.143] [info] [1236-1225]|[palm/yt_palm_context.h:541]NotifyPalmAuthStatusChangedfrom 0 to 1
[2025-08-01 12:23:59.143] [info] [1225-1225]|[palm/yt_palm_context.h:149]DoOnPalmAuthorizeSucceed
[2025-08-01 12:23:59.144] [info] [QML] onPalmAuthorizeSucceed
[2025-08-01 12:23:59.158] [info] [1236-1225]|[http/yt_qt_http_client.cpp:57]SSL supported, version: OpenSSL 1.1.1h  22 Sep 2020
[2025-08-01 12:23:59.301] [info] [1236-1225]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-01 12:23:59.301] [warning] [1236-1225]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-08-01 12:24:01.291: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-08-01 12:24:01.291: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-08-01 12:24:01.291: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-08-01 12:24:01.292] [info] [1236-1225]|[storage/yt_database_manager.cpp:101]Database opened with new key directly
[2025-08-01 12:24:03.286] [info] [1236-1225]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-08-01 12:24:03.286] [info] [1236-1225]|[storage/yt_database_manager.cpp:129]CheckVersion current :2 config :2
[2025-08-01 12:24:03.286] [info] [1236-1225]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-01 12:24:03.286] [error] [1236-1225]|[storage/yt_database_manager.cpp:33]duplicate init in Initialize
[2025-08-01 12:24:03.298] [info] [1236-1225]|[devicectl/yt_devicectl_manager.cpp:129]SyncPalmsFromDB begin
[2025-08-01 12:24:03.298] [info] [1236-1225]|[storage/yt_palm_db_manager.cpp:141]QueryAllPalms page size  1000
[2025-08-01 12:24:03.299] [info] [1236-1225]|[storage/yt_database_manager.cpp:181]QueryCount count:0 ret:161
[2025-08-01 12:24:03.299] [error] [1236-1225]|[storage/yt_palm_db_manager.cpp:147]QueryAllPalms QueryCount failed
[2025-08-01 12:24:03.299] [info] [1236-1225]|[devicectl/yt_devicectl_manager.cpp:148]SyncPalmsFromDB total : 0 done : 0, to sync : 0, invalid count : 0
[2025-08-01 12:24:03.299] [info] [1236-1225]|[devicectl/yt_devicectl_manager.cpp:160]SyncPalmsFromDB end
[2025-08-01 12:24:03.299] [info] [1236-1225]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-01 12:24:03.300] [warning] [1236-1225]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-08-01 12:24:05.298: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-08-01 12:24:05.298: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-08-01 12:24:05.298: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-08-01 12:24:05.300] [info] [1236-1225]|[storage/yt_database_manager.cpp:101]Database opened with new key directly
[2025-08-01 12:24:05.609] [info] [1253-1225]|[xike/yt_xike_device_hal_impl.cpp:367]FeedWatchdog ret : 0
[2025-08-01 12:24:07.278] [info] [1236-1225]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-08-01 12:24:07.278] [info] [1236-1225]|[storage/yt_database_manager.cpp:129]CheckVersion current :1 config :1
[2025-08-01 12:24:07.279] [info] [1236-1225]|[palm/yt_palm_service.cpp:391]Network status changed: 1, has_network: 1
[2025-08-01 12:24:07.279] [info] [1236-1225]|[palm/yt_palm_service.cpp:396]SyncTime ret: 0
[2025-08-01 12:24:07.279] [info] [1236-1225]|[card/yt_card_manager.cpp:34]Init CardManager: unsupported vendorType 1
[2025-08-01 12:24:07.279] [info] [1236-1225]|[card/yt_card_manager.cpp:77]SetCardNoCallback SetCardNoCallback
[2025-08-01 12:24:07.279] [info] [1236-1225]|[palm/yt_palm_service.cpp:1126]Old App Version: v1.2.1.2025073111-4df4a9c, Solution: 2
[2025-08-01 12:24:07.279] [info] [1236-1225]|[palm/yt_palm_service.cpp:1139]Updated App Version: v1.2.0.2025080102-9c1ab75, Solution: 2
[2025-08-01 12:24:07.279] [info] [1236-1225]|[palm/yt_palm_service.cpp:1180]NotifyModuleReadyStatus ready: 1, current: 0
[2025-08-01 12:24:07.288] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022247288214|INFO|[src/yt_palm_manager_impl_ota.cpp:67]sdk_version:v1.2.8.2025061912-60e8ea92 firm_version:V02.0.6_202506242120V4.9.5.2025061921
[2025-08-01 12:24:07.288] [info] [1236-1225]|[palm/yt_palm_service.cpp:1197]module ready, ParseOTAVersion ret: 0, firmwareVersion: 02.0.6.202506242120, algo version: v1.2.8, algo build: 2025061912
[2025-08-01 12:24:07.288] [error] [1236-1225]|[palm/yt_palm_service.cpp:1064]not init in CheckAndUpdateAlgoVersion
[2025-08-01 12:24:07.288] [info] [1236-1225]|[palm/yt_palm_service.cpp:183]EnablePalmWorking current working 0
[2025-08-01 12:24:07.288] [error] [1236-1225]|[palm/yt_palm_service.cpp:185]not init in EnablePalmWorking
[2025-08-01 12:24:07.288] [info] [1236-1225]|[palm/yt_palm_service.cpp:413]PalmService init succeed
[2025-08-01 12:24:07.289] [info] [1225-1225]|[palm/yt_palm_context.h:125]DoOnServiceInitSucceed
[2025-08-01 12:24:07.289] [info] [QML] Palm service initialized successfully
[2025-08-01 12:24:07.290] [info] [QML] Palm service initialized successfully
[2025-08-01 12:24:07.290] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:115]UnInit react InitSucceedEvent
[2025-08-01 12:24:07.291] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:144]NeedPalmAuth entryGuard, isPalmAuthorized 1
[2025-08-01 12:24:07.291] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:146]NeedPalmAuth entryGuard, already authorized, change to next status
[2025-08-01 12:24:07.291] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:178]NeedIoTAuth enter
[2025-08-01 12:24:07.291] [info] [1225-1225]|[auth/yt_auth_manager.cpp:66]StartCheckActivated Local key_id: 0f00636f-bb1d-45fe-a438-5000c1526725
[2025-08-01 12:24:07.292] [info] [1225-1225]|[auth/yt_auth_manager.cpp:69]Device has been activated
[2025-08-01 12:24:07.292] [info] [1225-1225]|[utils/yt_future_task.cpp:22]Starting future task name ExchangeKeyLooper, interval ms 600000, single_shot 0, auto_stop 1
[2025-08-01 12:24:07.292] [info] [1225-1225]|[utils/yt_future_task.cpp:83]Stop future task ExchangeKeyLooper
[2025-08-01 12:24:07.292] [info] [1225-1225]|[auth/yt_auth_manager.cpp:174]NotifyActivationSucceed
[2025-08-01 12:24:07.292] [info] [1225-1225]|[devicectl/yt_devicectl_manager.cpp:78]AuthManager activation succeed callback
[2025-08-01 12:24:07.293] [info] [1225-1225]|[palm/yt_palm_context.h:201]DoOnIoTRegistrationSucceed
[2025-08-01 12:24:07.293] [info] [1345-1225]|[utils/yt_future_task.cpp:55]handleThreadStarted ExchangeKeyLooper in thread 0xa9b780
[2025-08-01 12:24:07.293] [info] [QML] onIoTRegistrationSucceed
[2025-08-01 12:24:07.294] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:184]NeedIoTAuth react IoTAuthSucceed
[2025-08-01 12:24:07.294] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:267]Idle entryGuard, isPalmWorking 0
[2025-08-01 12:24:07.294] [info] [1225-1225]|[palm/yt_palm_service.cpp:110]CheckAndSetEnable current enable 0
[2025-08-01 12:24:07.306] [info] [1225-1225]|[palm/yt_palm_service.cpp:1074][Service enable]CheckAndUpdateAlgoVersion current : v1.0, stored :
[2025-08-01 12:24:07.307] [info] [1225-1225]|[utils/yt_future_task.cpp:22]Starting future task name SyncPalmLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-08-01 12:24:07.307] [info] [1225-1225]|[utils/yt_future_task.cpp:83]Stop future task SyncPalmLooper
[2025-08-01 12:24:07.307] [info] [1225-1225]|[utils/yt_future_task.cpp:22]Starting future task name SyncUserLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-08-01 12:24:07.307] [info] [1225-1225]|[utils/yt_future_task.cpp:83]Stop future task SyncUserLooper
[2025-08-01 12:24:07.308] [info] [1346-1225]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncPalmLooper in thread 0xa71140
[2025-08-01 12:24:07.308] [info] [1225-1225]|[utils/yt_future_task.cpp:22]Starting future task name StdHeartbeat, interval ms 60000, single_shot 0, auto_stop 1
[2025-08-01 12:24:07.308] [info] [1225-1225]|[utils/yt_future_task.cpp:83]Stop future task StdHeartbeat
[2025-08-01 12:24:07.308] [info] [1347-1225]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncUserLooper in thread 0xa9b840
[2025-08-01 12:24:07.308] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:204]Working enter, set service enable ret 0
[2025-08-01 12:24:07.309] [info] [1348-1225]|[utils/yt_future_task.cpp:55]handleThreadStarted StdHeartbeat in thread 0xaa0a28
[2025-08-01 12:24:07.310] [info] [1348-1225]|[pass/yt_pass_config_manager.cpp:516]CheckAndUploadPassRecords No unuploaded pass records
[2025-08-01 12:24:07.311] [info] [1348-1225]|[heartbeat/yt_heartbeat_manager.cpp:38]Init local heartbeat status from persistence: [{"desired_state": "{\"ocode\":\"1746854527\"}", "state_version": 3, "identifier": "device_bind_ocode_info"}]
[2025-08-01 12:24:07.311] [info] [QML] guardNavigateHome navigate to home
[2025-08-01 12:24:07.312] [info] [QML] StackView page changed: null
[2025-08-01 12:24:07.313] [info] [QML] Page type: null
qrc:/qml/main.qml:205: TypeError: Cannot read property 'statusBarColor' of null
[2025-08-01 12:24:07.314] [warning] [Qt-Warning] qrc:/qml/main.qml:205: TypeError: Cannot read property 'statusBarColor' of null (qrc:/qml/main.qml:205)
[2025-08-01 12:24:07.322] [info] [1349-1225]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 6e386d6d4d5075436b435443494a7270, reqId: 1754022247309-1
[2025-08-01 12:24:07.328] [info] [1350-1225]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/heartbeat, traceId: 4851356c5a4b4d33634243666758514e, reqId: 1754022247311-2
[2025-08-01 12:24:07.334] [info] [1351-1225]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 7236527a7032355166703952734c5539, reqId: 1754022247308-0
qml: [VirtualKeyboard] Performance monitoring: enabled
[2025-08-01 12:24:07.800] [info] [QML] Home page initialized, check host connection state
[2025-08-01 12:24:07.800] [info] [1225-1225]|[palm/yt_palm_service.cpp:779]GetCurrentPalmMode: recognize
qml: MainKeyboard onCompleted
[2025-08-01 12:24:07.810] [info] [1225-1225]|[xike/yt_xike_device_hal_impl.cpp:144]SetScreenOn expect screen : 1, result : 1
[2025-08-01 12:24:07.814] [info] [QML] HomePage in background
[2025-08-01 12:24:07.814] [info] [QML] Current active page type: Home
[2025-08-01 12:24:07.816] [info] [QML] Disabling palm recognition for page: Home
[2025-08-01 12:24:07.816] [info] [1225-1225]|[palm/yt_palm_service.cpp:201]DisablePalmWorking current working 0
[2025-08-01 12:24:07.816] [info] [1225-1225]|[palm/yt_palm_service.cpp:207]DisablePalmWorking not working
[2025-08-01 12:24:07.817] [info] [QML] HomePage in foreground - Enabling palm recognition
[2025-08-01 12:24:07.817] [info] [QML] Enable Palm work
[2025-08-01 12:24:07.818] [info] [1225-1225]|[palm/yt_palm_service.cpp:183]EnablePalmWorking current working 0
[2025-08-01 12:24:07.818] [info] [1225-1225]|[palm/yt_palm_service.cpp:227]CheckAndNotifyPalmWorkingfrom 0 to 1
[2025-08-01 12:24:07.818] [info] [1225-1225]|[palm/yt_palm_service.cpp:779]GetCurrentPalmMode: recognize
[2025-08-01 12:24:07.820] [info] [QML] StackView page changed: Unknown
[2025-08-01 12:24:07.820] [info] [QML] Page type: Object
[2025-08-01 12:24:07.821] [info] [QML] Status bar color updated to: #FFFFFF
[2025-08-01 12:24:07.821] [info] [QML] Status bar color reset to default:#FFFFFF
rga_api version 1.3.0_[11] (RGA is compiling with meson base: $PRODUCT_BASE)
[2025-08-01 12:24:07.844] [info] [1225-1225]|[palm/yt_palm_context.h:223]DoOnPalmServiceWorkingStatus palm_working_enable: 1
[2025-08-01 12:24:07.844] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:277]Idle ServiceWorkingStatusEvent,  palm_working_enable 1
[2025-08-01 12:24:07.844] [info] [1236-1225]|[palm/yt_palm_service.cpp:779]GetCurrentPalmMode: recognize
[2025-08-01 12:24:07.844] [info] [1236-1225]|[palm/yt_palm_service.cpp:246]_StartProcess palmMode: recognize
[2025-08-01 12:24:07.844] [info] [1225-1225]|[palm/yt_palm_service.cpp:779]GetCurrentPalmMode: recognize
[2025-08-01 12:24:07.844] [info] [1236-1225]|[palm/yt_palm_service.cpp:267][PalmMgr]StartRecognize
[2025-08-01 12:24:07.845] [info] [1225-1225]|[palm/yt_palm_context.h:244]DoOnReceivePalmProcessEvent session:  , event : 1, code : 0, msg : , palm_manager code : 0, msg :
[2025-08-01 12:24:07.846] [info] [QML] onPalmProcessEvent session_id , event : 1 error : 0, 0
[2025-08-01 12:24:07.847] [info] [1225-1225]|[palm/yt_palm_service.cpp:779]GetCurrentPalmMode: recognize
[2025-08-01 12:24:07.848] [info] [1225-1225]|[fsm/yt_palm_state_machine.h:312]Detecting react PalmProcessEvent  1
[2025-08-01 12:24:08.120] [info] [1318-1225]|[http/yt_qt_http_client.h:46]onResponse req_id 1754022247311-2, tag : /wecardpalm/device/heartbeat
[2025-08-01 12:24:08.120] [info] [1318-1225]|[heartbeat/yt_heartbeat_manager.cpp:73]StartHeartbeat Response: 0
[2025-08-01 12:24:08.121] [info] [1318-1225]|[heartbeat/yt_heartbeat_manager.cpp:100]HandleNewDesiredState size = 0
[2025-08-01 12:24:08.121] [info] [1318-1225]|[devicectl/yt_devicectl_manager.cpp:531]OnHeartbeatSucceed
[2025-08-01 12:24:08.187] [info] [1318-1225]|[http/yt_qt_http_client.h:46]onResponse req_id 1754022247309-1, tag : /wecardpalm/device/incr-sync-user
[2025-08-01 12:24:08.188] [info] [1318-1225]|[looper/yt_sync_user_looper.cpp:76]Sync user succeed: 0
[2025-08-01 12:24:09.143] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022249142454|INFO|[src/yt_palm_manager_impl.cpp:635]switch device status:working2 success, cost: 3ms
[2025-08-01 12:24:09.143] [info] [1256-1225]|[palm/yt_palm_service.cpp:504]HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-08-01 12:24:09.145] [info] [1237-1225]|[palm/yt_palm_service.cpp:511]_HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-08-01 12:24:09.188] [info] [1254-1225]|[palm/yt_palm_service.cpp:993][Palmmgr]1754022249188129|INFO|[src/yt_palm_manager_impl.cpp:1578]sync time: 2025-08-01 04:24:09 ret:0
[2025-08-01 12:24:10.702] [info] [1318-1225]|[http/yt_qt_http_client.h:46]onResponse req_id 1754022247308-0, tag : /wecardpalm/device/synchronize_palm_feature
[2025-08-01 12:24:10.726] [info] [1318-1225]|[looper/yt_sync_palm_looper.cpp:174]SyncPalmLooper RequestSyncPalmStandard response: hasMore: 2, query_position: eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MjIsInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NTQwMjIyNDh9LHsibmV4dF92ZXJzaW9uIjoyNSwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc1NDAyMjI0OH1dfQ==, datas size: 20
[2025-08-01 12:24:10.727] [info] [1318-1225]|[devicectl/yt_devicectl_manager.cpp:188]UpdatePalms entities.size: 20
[2025-08-01 12:24:10.727] [info] [1318-1225]|[storage/yt_palm_db_manager.cpp:205]InsertOrUpdatePalms sql: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?);
[2025-08-01 12:24:10.728] [error] [1318-1225]|[storage/yt_database_manager.h:105]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-08-01 12:24:10.728] [error] [1318-1225]|[storage/yt_palm_db_manager.cpp:227]InsertOrUpdatePalms Failed to insert/update palms, error code: 161
[2025-08-01 12:24:10.729] [error] [1318-1225]|[devicectl/yt_devicectl_manager.cpp:211]update palms to db ret :161 in UpdatePalms
[2025-08-01 12:24:10.729] [info] [1318-1225]|[looper/yt_sync_palm_looper.cpp:183]SyncPalmLooper RequestSyncPalmStandard -> RemoteDeviceCtlManager UpdatePalms ret: 161




