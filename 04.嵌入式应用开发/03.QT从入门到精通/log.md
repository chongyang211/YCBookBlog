# 专栏笔记总结大全


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





[root@RV1126_RV1109:/]# [2025-10-21 20:24:53.398] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:24:57.353] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:01.244] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:05.249] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:05.984] [info] [2662-2535]|[looper/yt_sync_user_looper.cpp:69]RequestSyncUser next_id: 12604, start_time: 1760690020, sync_type: incr
[2025-10-21 20:25:05.992] [info] [2661-2535]|[looper/yt_sync_palm_looper.cpp:163]RequestSyncPalm query_position:
[2025-10-21 20:25:05.993] [info] [2811-2535]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 51323578393438535569487746496841, reqId: 1761049505985-20
[2025-10-21 20:25:05.999] [info] [3618-2535]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 656554436b764551536f55697a4c344f, reqId: 1761049505985-21
[2025-10-21 20:25:06.013] [info] [2663-2535]|[pass/yt_pass_config_manager.cpp:638]No need to batch upload pass records
[2025-10-21 20:25:06.020] [info] [2811-2535]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/heartbeat, traceId: 674b5a614234394d4577363532786d73, reqId: 1761049506013-22
[2025-10-21 20:25:06.370] [info] [2613-2535]|[http/yt_qt_http_client.h:70]onResponse req_id 1761049505985-20, tag : /wecardpalm/device/incr-sync-user
[2025-10-21 20:25:06.370] [info] [2613-2535]|[looper/yt_sync_user_looper.cpp:106]Sync user succeed: empty response, next_id=0
[2025-10-21 20:25:06.403] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:06.402|INFO|[angstrong/ai_module.cpp:721]get psensor:0 count:858
[2025-10-21 20:25:06.468] [info] [2613-2535]|[http/yt_qt_http_client.h:70]onResponse req_id 1761049506013-22, tag : /wecardpalm/device/heartbeat
[2025-10-21 20:25:06.469] [info] [2613-2535]|[heartbeat/yt_heartbeat_manager.cpp:77]StartHeartbeat Response: 1
[2025-10-21 20:25:06.469] [info] [2613-2535]|[heartbeat/yt_heartbeat_manager.cpp:180]HandleSingleDesiredState identifier: device_verify_rule_info, state_version: 1
[I] <MMKV.cpp:1184::sync> MMKV::sync, SyncFlag = 1
[2025-10-21 20:25:06.501] [info] [2613-2535]|[storage/yt_verify_rule_db_manager.cpp:525]CleanupInvalidVerifyCountRecords Starting cleanup of invalid verify count records
[2025-10-21 20:25:06.504] [info] [2613-2535]|[storage/yt_verify_rule_db_manager.cpp:611]CleanupInvalidVerifyCountRecords Cleanup completed, deleted 0 invalid records
[2025-10-21 20:25:06.506] [info] [2613-2535]|[heartbeat/yt_heartbeat_manager.cpp:298]RefreshLocalStatusToSP json_str = [{"desired_state": "{\"scene_group\":\"66479b4b-3ff8-4138-8d5f-75ef6f5f76bc\",\"scene\":\"YCPalmM4Test\"}", "state_version": 25, "identifier": "device_scene_info"}, {"desired_state": "{\"useVerifyRule\":true, \"verifyRules\":[{\"verifyRuleId\":\"3b3ef2e8-a4d3-4991-86ad-42ee6b28dde1\", \"verifyRuleDetail\":{\"verifyRuleName\":\"YCTestPassConfig2\", \"useTimeRule\":true, \"timeRule\":{\"verifiableWeekdays\":[\"WEEKDAY_MONDAY\", \"WEEKDAY_TUESDAY\", \"WEEKDAY_WEDNESDAY\", \"WEEKDAY_THURSDAY\", \"WEEKDAY_FRIDAY\", \"WEEKDAY_SATURDAY\", \"WEEKDAY_SUNDAY\"], \"timeLimitations\":[{\"startTime\":\"19:00:00\", \"endTime\":\"20:59:00\", \"allowCount\":\"3\"}]}, \"useUserRule\":true, \"userRule\":{\"userInfos\":[{\"userId\":\"1000001\", \"userName\":\"yc211\"}, {\"userId\":\"111111\", \"userName\":\"ceshi\"}, {\"userId\":\"YourUserId11\", \"userName\":\"YourUserName1\"}, {\"userId\":\"666999\", \"userName\":\"Lisisi\"}]}}}, {\"verifyRuleId\":\"5609071e-0ca6-46fd-8a69-2d6ef6da6ffe\", \"verifyRuleDetail\":{\"verifyRuleName\":\"YCTestPassConfig\", \"useTimeRule\":true, \"timeRule\":{\"verifiableWeekdays\":[\"WEEKDAY_MONDAY\", \"WEEKDAY_WEDNESDAY\", \"WEEKDAY_THURSDAY\", \"WEEKDAY_TUESDAY\", \"WEEKDAY_FRIDAY\"], \"timeLimitations\":[{\"startTime\":\"17:30:00\", \"endTime\":\"20:59:00\", \"allowCount\":\"1\"}]}, \"useUserRule\":true, \"userRule\":{\"userInfos\":[{\"userId\":\"861300000001\", \"userName\":\"yanceyyang_flutter\"}, {\"userId\":\"VmBvWjdH\", \"userName\":\"syc_modify_user_new\"}, {\"userId\":\"1000001\", \"userName\":\"yc211\"}]}}}]}", "state_version": 1, "identifier": "device_verify_rule_info"}, {"desired_state": "{\"ocode\":\"1746854527\"}", "state_version": 5, "identifier": "device_bind_ocode_info"}]
[I] <MMKV_IO.cpp:543::writeActualSize> [mmkv.default] increase sequence to 1119, crc 1404259471, actualSize 2767
[I] <MMKV.cpp:1184::sync> MMKV::sync, SyncFlag = 1
[2025-10-21 20:25:06.509] [info] [2613-2535]|[heartbeat/yt_heartbeat_manager.cpp:169]HandleNewDesiredState Only verify rule commands received, skip ACK and report state
[2025-10-21 20:25:08.889] [info] [2613-2535]|[http/yt_qt_http_client.h:70]onResponse req_id 1761049505985-21, tag : /wecardpalm/device/synchronize_palm_feature
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=abcabc, palm_id=03109d3c-395a-4a29-a1b3-6ed70920c502, name=, status=1, direction=2, palm_data_version=7, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8615201806426, palm_id=1fb325e8-3ab3-4324-9487-448351a5b0ae, name=, status=2, direction=2, palm_data_version=15, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=aTnv9B6V, palm_id=219392e4-c06d-4f83-bd38-0e57791811da, name=, status=1, direction=1, palm_data_version=9, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=12345678, palm_id=4c4c8b9e-dd36-4c17-b594-044b27e2fc2b, name=, status=2, direction=1, palm_data_version=22, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=86188888889, palm_id=720aecca-acda-4974-9e1d-1c6ceb858b82, name=, status=2, direction=2, palm_data_version=13, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new_upgrade_1, palm_id=73035d09-8e04-4e01-be3c-a5b10b55e60f, name=, status=1, direction=1, palm_data_version=18, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_same_upgrade, palm_id=7c09d068-5de4-405b-b39c-30f5c395ed25, name=, status=1, direction=1, palm_data_version=16, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_same, palm_id=7fa8bb29-a51c-49ae-8c94-632e6b442696, name=, status=1, direction=1, palm_data_version=14, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new, palm_id=80006db0-7463-4f6e-bce6-2e423df5c18a, name=, status=2, direction=1, palm_data_version=12, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=vISX7nQL, palm_id=98d2e90a-442c-4fff-a212-8acaf8c4206d, name=, status=1, direction=1, palm_data_version=10, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8613012890285, palm_id=a021d71e-30f3-4373-b108-263d9946ed85, name=, status=2, direction=2, palm_data_version=14, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=EXMDBcDS, palm_id=bb0189a5-a4d3-4d35-b483-7ce318f72cec, name=, status=2, direction=1, palm_data_version=17, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_1_upgrade, palm_id=bc717db5-78bf-4217-9d42-dbba754c41d4, name=, status=2, direction=2, palm_data_version=6, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_user, palm_id=c077ecdc-dc61-4fe1-bbcf-0a9a4284f07c, name=, status=2, direction=2, palm_data_version=25, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=WC4KKQBv, palm_id=c874de3c-b9dc-437d-a699-9a735181f419, name=, status=1, direction=1, palm_data_version=8, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new_upgrade, palm_id=c97cf563-a36e-40d9-bd61-384890c01653, name=, status=1, direction=1, palm_data_version=13, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8617349756267, palm_id=d1c9f315-14c7-40d4-a461-54f430ee0964, name=, status=1, direction=2, palm_data_version=19, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_1, palm_id=e181b8b0-85d2-46c1-8319-a2332e96fc9f, name=, status=2, direction=2, palm_data_version=4, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8613770320839, palm_id=f62fe416-6fd2-4909-acf4-36917d3f1f32, name=, status=1, direction=2, palm_data_version=20, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.915] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=13012890285, palm_id=fd6c532c-df31-4eaa-b3d2-56a529553c25, name=, status=1, direction=2, palm_data_version=8, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049508, user_tags=}
[2025-10-21 20:25:08.916] [error] [2613-2535]|[storage/yt_database_manager.h:111]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-10-21 20:25:08.917] [error] [2613-2535]|[storage/yt_palm_db_manager.cpp:241]InsertOrUpdatePalms Failed to insert/update palms, ret: 161
[2025-10-21 20:25:08.917] [error] [2613-2535]|[devicectl/yt_devicectl_manager.cpp:220]update palms to db ret :161 in UpdatePalms
[2025-10-21 20:25:08.917] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:186]SyncPalmLooper response: hasMore=2, query_position=eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MjIsInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NjEwNDk1MDZ9LHsibmV4dF92ZXJzaW9uIjoyNSwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc2MTA0OTUwNn1dfQ==, count=20, updateResult=161
[2025-10-21 20:25:09.181] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:13.173] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:17.208] [info] [QML] GifPlayer Reached last frame, preparing to loop...

[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]# [2025-10-21 20:25:21.219] [info] [QML] GifPlayer Reached last frame, preparing to loop...

[root@RV1126_RV1109:/]#
[root@RV1126_RV1109:/]# [2025-10-21 20:25:22.046] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.046|INFO|[angstrong/ai_module.cpp:721]get psensor:200 count:447
[2025-10-21 20:25:22.056] [info] [QML] showFrameMessage: Move your palm closer
qml: FullscreenGifOverlay: showAndPlay called with source: qrc:/images/gif_palm_bg_1.gif
qml: FullscreenGifOverlay: Current state - visible: false _isShowing: false opacity: 0
qml: FullscreenGifOverlay: Showing overlay with source: qrc:/images/gif_palm_bg_1.gif
[2025-10-21 20:25:22.083] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.082|INFO|[angstrong/ai_module.cpp:721]get psensor:205 count:1
[2025-10-21 20:25:22.107] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.117] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.117|INFO|[angstrong/ai_module.cpp:721]get psensor:200 count:1
[2025-10-21 20:25:22.119] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.153] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.153|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-21 20:25:22.188] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.187|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:22.222] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.222|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:22.238] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.240] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.241] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.258] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.257|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:22.259] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.292] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.292|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:22.327] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.327|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:22.362] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.361|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:22.370] [info] [QML] FullscreenGifOverlay: Show animation completed
[2025-10-21 20:25:22.371] [info] [QML] FullscreenGifOverlay main animation started - hiding status bar
[2025-10-21 20:25:22.372] [info] [QML] FullscreenGifOverlay: Starting GIF playback after show animation , source: qrc:/images/gif_palm_bg_1.gifvisible:true
[2025-10-21 20:25:22.373] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.388] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.390] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.392] [info] [QML] FullscreenGifOverlay: About to call gifPlayer.play() - isPlaying: falsehasError:falseisLoading:false
[2025-10-21 20:25:22.392] [info] [QML] GifPlayer Debug - source: qrc:/images/gif_palm_bg_1.gif
[2025-10-21 20:25:22.393] [info] [QML] GifPlayer Debug - _isPlaying: false
[2025-10-21 20:25:22.393] [info] [QML] GifPlayer Debug - _hasError: false
[2025-10-21 20:25:22.394] [info] [QML] GifPlayer Debug - _isLoading: false
[2025-10-21 20:25:22.394] [info] [QML] GifPlayer Debug - AnimatedImage.playing: false
[2025-10-21 20:25:22.394] [info] [QML] GifPlayer Debug - AnimatedImage.status: 1
[2025-10-21 20:25:22.395] [info] [QML] GifPlayer Debug - AnimatedImage.frameCount: 100
[2025-10-21 20:25:22.395] [info] [QML] GifPlayer Debug - AnimatedImage.currentFrame: 45
qml: FullscreenGifOverlay: GIF play started
[2025-10-21 20:25:22.396] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.396|INFO|[angstrong/ai_module.cpp:721]get psensor:149 count:1
[2025-10-21 20:25:22.396] [info] [QML] FullscreenGifOverlay: Play result:
[2025-10-21 20:25:22.431] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.431|INFO|[angstrong/ai_module.cpp:721]get psensor:147 count:1
[2025-10-21 20:25:22.467] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.467|INFO|[angstrong/ai_module.cpp:721]get psensor:147 count:1
[2025-10-21 20:25:22.478] [info] [QML] showFrameMessage: Move your palm closer
qml: FullscreenGifOverlay: After play() call:
qml: FullscreenGifOverlay: AnimatedImage.playing: true
qml: FullscreenGifOverlay: AnimatedImage.visible: true
qml: FullscreenGifOverlay: AnimatedImage.width: 720 height: 1239.6773456490034
[2025-10-21 20:25:22.480] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.481] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.504] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.503|INFO|[angstrong/ai_module.cpp:721]get psensor:143 count:1
[2025-10-21 20:25:22.505] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.538] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.537|INFO|[angstrong/ai_module.cpp:721]get psensor:143 count:1
[2025-10-21 20:25:22.572] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.572|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-21 20:25:22.601] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.602] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.607] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.607|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-21 20:25:22.608] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.642] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.642|INFO|[angstrong/ai_module.cpp:721]get psensor:143 count:1
[2025-10-21 20:25:22.677] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.676|INFO|[angstrong/ai_module.cpp:721]get psensor:143 count:1
[2025-10-21 20:25:22.701] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.702] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.712] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.712|INFO|[angstrong/ai_module.cpp:721]get psensor:142 count:1
[2025-10-21 20:25:22.713] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.747] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.747|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-21 20:25:22.782] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.781|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-21 20:25:22.807] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.808] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.817] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.817|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-21 20:25:22.818] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.851] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.851|INFO|[angstrong/ai_module.cpp:721]get psensor:147 count:1
[2025-10-21 20:25:22.886] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.886|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-21 20:25:22.914] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.916] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.922] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.921|INFO|[angstrong/ai_module.cpp:721]get psensor:146 count:1
[2025-10-21 20:25:22.923] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:22.956] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.955|INFO|[angstrong/ai_module.cpp:721]get psensor:150 count:1
[2025-10-21 20:25:22.991] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:22.990|INFO|[angstrong/ai_module.cpp:721]get psensor:146 count:1
[2025-10-21 20:25:23.020] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.022] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.025] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.025|INFO|[angstrong/ai_module.cpp:721]get psensor:148 count:1
[2025-10-21 20:25:23.038] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.060] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.059|INFO|[angstrong/ai_module.cpp:721]get psensor:148 count:1
[2025-10-21 20:25:23.094] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.094|INFO|[angstrong/ai_module.cpp:721]get psensor:149 count:1
[2025-10-21 20:25:23.120] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.121] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.132] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.132|INFO|[angstrong/ai_module.cpp:721]get psensor:149 count:1
[2025-10-21 20:25:23.133] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.167] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.166|INFO|[angstrong/ai_module.cpp:721]get psensor:149 count:1
[2025-10-21 20:25:23.202] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.201|INFO|[angstrong/ai_module.cpp:721]get psensor:152 count:1
[2025-10-21 20:25:23.232] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.233] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.237] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.237|INFO|[angstrong/ai_module.cpp:721]get psensor:152 count:1
[2025-10-21 20:25:23.239] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.271] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.271|INFO|[angstrong/ai_module.cpp:721]get psensor:151 count:1
[2025-10-21 20:25:23.306] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.306|INFO|[angstrong/ai_module.cpp:721]get psensor:153 count:1
[2025-10-21 20:25:23.332] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.333] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.341] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.341|INFO|[angstrong/ai_module.cpp:721]get psensor:150 count:1
[2025-10-21 20:25:23.343] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.376] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.376|INFO|[angstrong/ai_module.cpp:721]get psensor:150 count:1
[2025-10-21 20:25:23.412] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.411|INFO|[angstrong/ai_module.cpp:721]get psensor:150 count:1
[2025-10-21 20:25:23.442] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.443] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.448] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.447|INFO|[angstrong/ai_module.cpp:721]get psensor:152 count:1
[2025-10-21 20:25:23.449] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.483] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.483|INFO|[angstrong/ai_module.cpp:721]get psensor:150 count:1
[2025-10-21 20:25:23.518] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.518|INFO|[angstrong/ai_module.cpp:721]get psensor:152 count:1
[2025-10-21 20:25:23.545] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.547] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.552] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.552|INFO|[angstrong/ai_module.cpp:721]get psensor:151 count:1
[2025-10-21 20:25:23.554] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.588] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.587|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:23.590] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.622] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.622|INFO|[angstrong/ai_module.cpp:721]get psensor:153 count:1
[2025-10-21 20:25:23.657] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.656|INFO|[angstrong/ai_module.cpp:721]get psensor:153 count:1
[2025-10-21 20:25:23.675] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.677] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.692] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.692|INFO|[angstrong/ai_module.cpp:721]get psensor:153 count:1
[2025-10-21 20:25:23.694] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.727] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.727|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:23.762] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.762|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:23.793] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.794] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.797] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.796|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:23.799] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.834] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.833|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:23.868] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.868|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:23.897] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.898] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.904] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.904|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:23.905] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:23.938] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.938|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:23.973] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:23.972|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:24.002] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.003] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.007] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.007|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.042] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.042|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:24.077] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.077|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.103] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.104] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.106] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.112] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.112|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.113] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.147] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.147|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.184] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.184|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:24.187] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.187] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.220] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.219|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:24.255] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.254|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:24.289] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.289|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.298] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.299] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.300] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.325] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.324|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:24.359] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.359|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.394] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.394|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.404] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.406] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.407] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.431] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.430|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:24.465] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.465|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:24.500] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.500|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:24.508] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.510] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.511] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.537] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.536|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.571] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.571|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.606] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.606|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.617] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.618] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.619] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.642] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.642|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.653] [info] [2535-2535]|[audio/yt_audio_manager.cpp:440]Processing audio queue
[2025-10-21 20:25:24.655] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.677] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.677|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.712] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.712|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:24.736] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.737] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.747] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.747|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:24.749] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.785] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.784|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.820] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.820|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:24.855] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.855|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:24.877] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.878] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.879] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.890] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.890|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.891] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.925] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.924|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.959] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.959|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:24.975] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.976] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:24.995] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:24.994|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:24.996] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.030] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.030|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:25.065] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.065|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:25.083] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.084] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.100] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.100|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:25.101] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.135] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.135|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:25.170] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.170|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:25.188] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.190] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.205] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.205|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:25.206] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.239] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.239|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:25.279] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.279|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:25.292] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.294] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.314] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.314|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:25.350] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.350|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:25.384] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.384|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:25.397] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.398] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.399] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.419] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.419|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:25.420] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.454] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.454|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:25.489] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.489|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:25.504] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.506] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.525] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.524|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:25.528] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.560] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.559|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:25.595] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.595|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:25.611] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.613] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.631] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.631|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:25.666] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.666|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:25.701] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.701|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:25.715] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.717] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.718] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.736] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.735|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:25.771] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.770|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:25.805] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.805|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:25.818] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.819] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.821] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.840] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.840|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:25.841] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.875] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.875|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:25.910] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.910|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:25.924] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.925] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.946] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.945|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:25.948] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:25.981] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:25.981|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:26.015] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.015|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:26.033] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.034] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.050] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.049|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:26.084] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.084|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:26.120] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.119|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:26.133] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.134] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.135] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.155] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.154|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:26.156] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.190] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.189|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.224] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.224|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:26.240] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.241] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.259] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.259|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:26.261] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.294] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.294|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:26.328] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.328|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:26.346] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.347] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.365] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.364|INFO|[angstrong/ai_module.cpp:721]get psensor:165 count:1
[2025-10-21 20:25:26.367] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.400] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.399|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:26.435] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.434|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:26.452] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.453] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.470] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.470|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:26.472] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.504] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.504|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.540] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.539|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:26.574] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.574|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.577] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.578] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.579] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.610] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.610|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.611] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.647] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.647|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.682] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.682|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.697] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.699] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.717] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.717|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.752] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.752|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:26.787] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.787|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:26.800] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.801] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.803] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.822] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.822|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:26.857] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.857|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:26.893] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.893|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.907] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.908] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.910] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.928] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.928|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:26.931] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:26.963] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.963|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:26.998] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:26.997|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:27.031] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.032] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.032|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:27.032] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.067] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.066|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:27.101] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.101|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:27.131] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.133] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.134] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.136] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.136|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:27.137] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.171] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.171|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:27.206] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.206|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:27.213] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.214] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.242] [info] [2535-2535]|[audio/yt_audio_manager.cpp:440]Processing audio queue
[2025-10-21 20:25:27.249] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.248|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:27.283] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.283|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:27.318] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.317|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:27.338] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.340] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.341] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.354] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.354|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:27.388] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.388|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:27.424] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.424|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:27.426] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.428] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.429] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.461] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.460|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:27.496] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.496|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:27.532] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.531|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:27.534] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.536] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.537] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.539] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-21 20:25:27.567] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.567|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:27.602] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.602|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:27.616] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.618] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.637] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.636|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:27.638] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.671] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.671|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:27.700] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.706] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.705|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:27.707] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.740] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.740|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:27.775] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.775|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:27.786] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.787] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.811] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.811|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:27.812] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.846] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.846|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:27.881] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.882] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.881|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:27.882] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:27.883] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.916] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.916|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:27.952] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.951|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:27.964] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.965] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:27.987] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:27.987|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:27.988] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.022] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.022|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:28.051] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.062] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.061|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:28.063] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.097] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.096|INFO|[angstrong/ai_module.cpp:721]get psensor:157 count:1
[2025-10-21 20:25:28.132] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.131|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:28.143] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.145] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.166] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.166|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:28.168] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.200] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.200|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:28.229] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.235] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.235|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:28.237] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.271] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.270|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:28.306] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.305|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:28.317] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.318] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.343] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.342|INFO|[angstrong/ai_module.cpp:721]get psensor:153 count:1
[2025-10-21 20:25:28.344] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.379] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.378|INFO|[angstrong/ai_module.cpp:721]get psensor:153 count:1
[2025-10-21 20:25:28.411] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.413] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.413|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:28.415] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.448] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.448|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:28.482] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.482|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:28.491] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.492] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.516] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.516|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:28.519] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.551] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.551|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:28.578] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.587] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.586|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:28.588] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.621] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.621|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:28.656] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.656|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:28.671] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.672] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.692] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.691|INFO|[angstrong/ai_module.cpp:721]get psensor:157 count:1
[2025-10-21 20:25:28.693] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.727] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.727|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:28.753] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.762] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.762|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:28.764] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.797] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.796|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:28.831] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.831|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:28.843] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.845] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.868] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.868|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:28.870] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.903] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.902|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:28.929] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.937] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.937|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:28.938] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:28.971] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:28.971|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:29.006] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.006|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:29.034] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.035] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.041] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.040|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:29.042] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.078] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.078|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:29.113] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.113|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:29.124] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.125] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.148] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.148|INFO|[angstrong/ai_module.cpp:721]get psensor:156 count:1
[2025-10-21 20:25:29.184] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.183|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:29.211] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.212] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.218] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.218|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:29.219] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.253] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.253|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:29.288] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.288|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:29.297] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.298] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.323] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.323|INFO|[angstrong/ai_module.cpp:721]get psensor:161 count:1
[2025-10-21 20:25:29.360] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.359|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:29.383] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.390] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.398] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.397|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:29.400] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.432] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.432|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:29.467] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.467|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:29.470] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.471] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.503] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.502|INFO|[angstrong/ai_module.cpp:721]get psensor:159 count:1
[2025-10-21 20:25:29.537] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.537|INFO|[angstrong/ai_module.cpp:721]get psensor:155 count:1
[2025-10-21 20:25:29.558] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.559] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.572] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.571|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:29.573] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.611] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.611|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:29.647] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.647|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:29.682] [info] [2535-2535]|[audio/yt_audio_manager.cpp:440]Processing audio queue
[2025-10-21 20:25:29.682] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.682|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:29.683] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.685] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.716] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.716|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:29.744] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.751] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.751|INFO|[angstrong/ai_module.cpp:721]get psensor:162 count:1
[2025-10-21 20:25:29.759] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.786] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.785|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:29.817] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.818] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.821] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.820|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:29.822] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.855] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.855|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:29.889] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.889|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:29.907] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.908] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.929] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.929|INFO|[angstrong/ai_module.cpp:721]get psensor:164 count:1
[2025-10-21 20:25:29.931] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.964] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.964|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:29.993] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:29.999] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:29.999|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:30.008] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.033] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.033|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:30.068] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.068|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:30.079] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.081] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.103] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.103|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:30.104] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.138] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.138|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:30.174] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.176] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.176|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:30.178] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.211] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.211|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:30.245] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.245|INFO|[angstrong/ai_module.cpp:721]get psensor:177 count:1
[2025-10-21 20:25:30.261] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.262] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.281] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.280|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:30.282] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.315] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.315|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:30.346] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.350] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.350|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:30.351] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.385] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.385|INFO|[angstrong/ai_module.cpp:721]get psensor:187 count:1
[2025-10-21 20:25:30.420] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.420|INFO|[angstrong/ai_module.cpp:721]get psensor:187 count:1
[2025-10-21 20:25:30.433] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.435] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.456] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.455|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-21 20:25:30.457] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.492] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.491|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-21 20:25:30.525] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.527] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.526|INFO|[angstrong/ai_module.cpp:721]get psensor:186 count:1
[2025-10-21 20:25:30.528] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.562] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.562|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-21 20:25:30.597] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.597|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-21 20:25:30.611] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.612] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.632] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.631|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-21 20:25:30.634] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.666] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.666|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:30.695] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.702] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.701|INFO|[angstrong/ai_module.cpp:721]get psensor:187 count:1
[2025-10-21 20:25:30.703] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.737] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.737|INFO|[angstrong/ai_module.cpp:721]get psensor:186 count:1
[2025-10-21 20:25:30.771] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.771|INFO|[angstrong/ai_module.cpp:721]get psensor:183 count:1
[2025-10-21 20:25:30.786] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.788] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.807] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.806|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:30.808] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.846] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.846|INFO|[angstrong/ai_module.cpp:721]get psensor:186 count:1
[2025-10-21 20:25:30.872] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.880] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.880|INFO|[angstrong/ai_module.cpp:721]get psensor:185 count:1
[2025-10-21 20:25:30.882] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.915] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.915|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:30.950] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.949|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:30.963] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.964] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:30.985] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:30.984|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:31.020] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.019|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:31.044] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.046] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.054] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.054|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:31.056] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.089] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.089|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:31.125] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.125|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:31.135] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.137] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.161] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.161|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:31.163] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.195] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.195|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:31.223] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.230] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.230|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:31.231] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.265] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.265|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:31.300] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.299|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:31.311] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.312] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.335] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.334|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:31.336] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.370] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.369|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:31.397] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.404] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.404|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:31.406] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.438] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.438|INFO|[angstrong/ai_module.cpp:721]get psensor:178 count:1
[2025-10-21 20:25:31.472] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.472|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:31.486] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.487] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.512] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.511|INFO|[angstrong/ai_module.cpp:721]get psensor:181 count:1
[2025-10-21 20:25:31.513] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.546] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.545|INFO|[angstrong/ai_module.cpp:721]get psensor:181 count:1
[2025-10-21 20:25:31.575] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.580] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.580|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:31.581] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.615] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.615|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-21 20:25:31.649] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.649|INFO|[angstrong/ai_module.cpp:721]get psensor:177 count:1
[2025-10-21 20:25:31.663] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.664] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.685] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.685|INFO|[angstrong/ai_module.cpp:721]get psensor:181 count:1
[2025-10-21 20:25:31.720] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.719|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:31.747] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.748] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.754] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.754|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:31.755] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.789] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.789|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:31.824] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.824|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:31.837] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.839] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.859] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.858|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:31.860] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.895] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.894|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:31.920] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.929] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.929|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:31.948] [info] [2535-2535]|[audio/yt_audio_manager.cpp:440]Processing audio queue
[2025-10-21 20:25:31.950] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:31.964] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.964|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:31.999] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:31.998|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:32.011] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.012] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.034] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.034|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:32.070] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.070|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:32.093] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.095] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.105] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.105|INFO|[angstrong/ai_module.cpp:721]get psensor:177 count:1
[2025-10-21 20:25:32.107] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.140] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.140|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.178] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.178|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.186] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.199] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.216] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.215|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:32.250] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.250|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.268] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.269] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.285] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.285|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:32.288] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.322] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.321|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:32.356] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.356|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:32.358] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.359] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.392] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.392|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:32.428] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.428|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:32.446] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.447] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.463] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.463|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:32.464] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.498] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.498|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:32.533] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.535] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.534|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.536] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.570] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.569|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.604] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.604|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.624] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.625] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.640] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.640|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.642] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.674] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.674|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:32.709] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.708|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:32.709] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.711] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.746] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.745|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.781] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.781|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.804] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.805] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.816] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.816|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:32.817] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.851] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.850|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:32.885] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.885|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:32.886] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.887] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.921] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.920|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.957] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.957|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:32.975] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.976] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:32.991] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:32.991|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:32.992] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.026] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.026|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:33.056] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.061] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.060|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:33.062] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.097] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.096|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:33.131] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.130|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:33.155] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.157] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.165] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.164|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:33.166] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.199] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.199|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:33.234] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.234] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.234|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:33.235] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.269] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.269|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:33.304] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.304|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:33.324] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.325] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.339] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.339|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:33.341] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.374] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.374|INFO|[angstrong/ai_module.cpp:721]get psensor:173 count:1
[2025-10-21 20:25:33.409] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.412] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.412|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:33.414] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.448] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.447|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:33.482] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.482|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:33.499] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.501] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.517] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.517|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:33.519] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.552] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.551|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:33.584] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.586] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.586|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:33.587] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.622] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.621|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:33.657] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.656|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:33.679] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.680] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.691] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.691|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:33.694] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.726] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.726|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:33.757] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.761] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.761|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:33.763] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.797] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.796|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:33.831] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.831|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:33.852] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.853] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.865] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.865|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:33.866] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.900] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.900|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:33.935] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.936] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.936|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:33.937] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:33.971] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:33.971|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:34.005] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.005|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:34.034] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.035] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.044] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.044|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:34.047] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.079] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.078|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.113] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.114] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.113|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:34.115] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.149] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.148|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:34.183] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.183|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:34.202] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.204] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.220] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.220|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:34.222] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.256] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.256|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.283] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.291] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.291|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.315] [info] [2535-2535]|[audio/yt_audio_manager.cpp:440]Processing audio queue
[2025-10-21 20:25:34.326] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.326|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:34.361] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.361|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.387] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.388] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.389] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.390] [info] [QML] GifPlayer Reached last frame, preparing to loop...
[2025-10-21 20:25:34.396] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.395|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:34.397] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.431] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.430|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:34.456] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.465] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.465|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:34.466] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.500] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.500|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:34.535] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.535|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:34.548] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.550] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.569] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.569|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:34.570] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.604] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.604|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:34.630] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.641] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.641|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.642] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.678] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.678|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:34.706] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.712] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.712|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:34.714] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.747] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.747|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.782] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.782|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.795] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.796] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.817] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.817|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.818] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.852] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.851|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:34.881] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.887] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.887|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:34.888] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.921] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.921|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:34.956] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.956|INFO|[angstrong/ai_module.cpp:721]get psensor:166 count:1
[2025-10-21 20:25:34.968] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.969] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:34.991] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:34.991|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:34.993] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.027] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.027|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.053] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.061] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.061|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.063] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.096] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.095|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:35.131] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.130|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:35.143] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.144] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.165] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.165|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.167] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.200] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.199|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.234] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.234|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:35.234] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.236] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.270] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.270|INFO|[angstrong/ai_module.cpp:721]get psensor:170 count:1
[2025-10-21 20:25:35.305] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.304|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.320] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.322] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.341] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.340|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:35.343] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.379] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.379|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:35.408] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.413] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.413|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:35.415] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.448] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.448|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:35.483] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.483|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:35.495] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.495] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.520] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.519|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.521] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.554] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.554|INFO|[angstrong/ai_module.cpp:721]get psensor:171 count:1
[2025-10-21 20:25:35.582] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.589] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.589|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.590] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.624] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.623|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:35.658] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.658|INFO|[angstrong/ai_module.cpp:721]get psensor:168 count:1
[2025-10-21 20:25:35.672] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.674] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.694] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.693|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.696] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.728] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.728|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:35.755] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.763] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.763|INFO|[angstrong/ai_module.cpp:721]get psensor:167 count:1
[2025-10-21 20:25:35.765] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.798] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.798|INFO|[angstrong/ai_module.cpp:721]get psensor:169 count:1
[2025-10-21 20:25:35.833] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.833|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:35.844] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.845] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.868] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.867|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:35.871] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.902] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.902|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:35.934] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.937] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.936|INFO|[angstrong/ai_module.cpp:721]get psensor:178 count:1
[2025-10-21 20:25:35.938] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:35.970] [info] [2662-2535]|[looper/yt_sync_user_looper.cpp:69]RequestSyncUser next_id: 12604, start_time: 1760690020, sync_type: incr
[2025-10-21 20:25:35.973] [info] [2661-2535]|[looper/yt_sync_palm_looper.cpp:163]RequestSyncPalm query_position:
[2025-10-21 20:25:35.976] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:35.976|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:35.978] [info] [3618-2535]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 4a51515a7663524f41315265515a4d36, reqId: 1761049535970-24
[2025-10-21 20:25:35.985] [info] [2811-2535]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 66386561546e344b584f596c59723035, reqId: 1761049535969-23
[2025-10-21 20:25:36.011] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.011|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-21 20:25:36.035] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.036] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.045] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.045|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-21 20:25:36.046] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.080] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.080|INFO|[angstrong/ai_module.cpp:721]get psensor:203 count:1
[2025-10-21 20:25:36.115] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.114|INFO|[angstrong/ai_module.cpp:721]get psensor:202 count:1
[2025-10-21 20:25:36.119] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.120] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.150] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.150|INFO|[angstrong/ai_module.cpp:721]get psensor:208 count:1
[2025-10-21 20:25:36.152] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.184] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.184|INFO|[angstrong/ai_module.cpp:721]get psensor:206 count:1
[2025-10-21 20:25:36.213] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.221] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.220|INFO|[angstrong/ai_module.cpp:721]get psensor:214 count:1
[2025-10-21 20:25:36.222] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.255] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.255|INFO|[angstrong/ai_module.cpp:721]get psensor:217 count:1
[2025-10-21 20:25:36.290] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.290|INFO|[angstrong/ai_module.cpp:721]get psensor:218 count:1
[2025-10-21 20:25:36.299] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.300] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.326] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.325|INFO|[angstrong/ai_module.cpp:721]get psensor:213 count:1
[2025-10-21 20:25:36.328] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.361] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.361|INFO|[angstrong/ai_module.cpp:721]get psensor:222 count:1
[2025-10-21 20:25:36.374] [info] [2613-2535]|[http/yt_qt_http_client.h:70]onResponse req_id 1761049535970-24, tag : /wecardpalm/device/incr-sync-user
[2025-10-21 20:25:36.374] [info] [2613-2535]|[looper/yt_sync_user_looper.cpp:106]Sync user succeed: empty response, next_id=0
[2025-10-21 20:25:36.391] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.395] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.395|INFO|[angstrong/ai_module.cpp:721]get psensor:214 count:1
[2025-10-21 20:25:36.397] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.430] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.429|INFO|[angstrong/ai_module.cpp:721]get psensor:223 count:1
[2025-10-21 20:25:36.464] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.464|INFO|[angstrong/ai_module.cpp:721]get psensor:219 count:1
[2025-10-21 20:25:36.473] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.474] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.499] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.499|INFO|[angstrong/ai_module.cpp:721]get psensor:217 count:1
[2025-10-21 20:25:36.500] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.534] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.533|INFO|[angstrong/ai_module.cpp:721]get psensor:215 count:1
[2025-10-21 20:25:36.564] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.568] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.568|INFO|[angstrong/ai_module.cpp:721]get psensor:220 count:1
[2025-10-21 20:25:36.597] [info] [2535-2535]|[audio/yt_audio_manager.cpp:440]Processing audio queue
[2025-10-21 20:25:36.604] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.604|INFO|[angstrong/ai_module.cpp:721]get psensor:215 count:1
[2025-10-21 20:25:36.641] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.640|INFO|[angstrong/ai_module.cpp:721]get psensor:215 count:1
[2025-10-21 20:25:36.659] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.660] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.662] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.675] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.675|INFO|[angstrong/ai_module.cpp:721]get psensor:209 count:1
[2025-10-21 20:25:36.677] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.711] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.710|INFO|[angstrong/ai_module.cpp:721]get psensor:210 count:1
[2025-10-21 20:25:36.736] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.745] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.745|INFO|[angstrong/ai_module.cpp:721]get psensor:210 count:1
[2025-10-21 20:25:36.761] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.780] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.780|INFO|[angstrong/ai_module.cpp:721]get psensor:210 count:1
[2025-10-21 20:25:36.815] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.814|INFO|[angstrong/ai_module.cpp:721]get psensor:204 count:1
[2025-10-21 20:25:36.819] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.820] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.849] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.849|INFO|[angstrong/ai_module.cpp:721]get psensor:205 count:1
[2025-10-21 20:25:36.851] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.884] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.883|INFO|[angstrong/ai_module.cpp:721]get psensor:202 count:1
[2025-10-21 20:25:36.910] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.918] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.918|INFO|[angstrong/ai_module.cpp:721]get psensor:202 count:1
[2025-10-21 20:25:36.919] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.953] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.952|INFO|[angstrong/ai_module.cpp:721]get psensor:199 count:1
[2025-10-21 20:25:36.987] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:36.987|INFO|[angstrong/ai_module.cpp:721]get psensor:198 count:1
[2025-10-21 20:25:36.996] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:36.998] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.023] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.023|INFO|[angstrong/ai_module.cpp:721]get psensor:196 count:1
[2025-10-21 20:25:37.024] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.058] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.058|INFO|[angstrong/ai_module.cpp:721]get psensor:197 count:1
[2025-10-21 20:25:37.083] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.092] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.092|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-21 20:25:37.094] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.127] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.127|INFO|[angstrong/ai_module.cpp:721]get psensor:198 count:1
[2025-10-21 20:25:37.162] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.162|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-21 20:25:37.184] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.185] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.197] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.197|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-21 20:25:37.198] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.233] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.233|INFO|[angstrong/ai_module.cpp:721]get psensor:185 count:1
[2025-10-21 20:25:37.263] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.268] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.268|INFO|[angstrong/ai_module.cpp:721]get psensor:185 count:1
[2025-10-21 20:25:37.269] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.302] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.302|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-21 20:25:37.337] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.337|INFO|[angstrong/ai_module.cpp:721]get psensor:179 count:1
[2025-10-21 20:25:37.347] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.349] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.374] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.373|INFO|[angstrong/ai_module.cpp:721]get psensor:176 count:1
[2025-10-21 20:25:37.375] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.408] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.408|INFO|[angstrong/ai_module.cpp:721]get psensor:174 count:1
[2025-10-21 20:25:37.438] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.443] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.443|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:37.445] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.478] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.478|INFO|[angstrong/ai_module.cpp:721]get psensor:175 count:1
[2025-10-21 20:25:37.513] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.512|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:37.526] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.527] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.548] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.547|INFO|[angstrong/ai_module.cpp:721]get psensor:163 count:1
[2025-10-21 20:25:37.550] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.581] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.581|INFO|[angstrong/ai_module.cpp:721]get psensor:158 count:1
[2025-10-21 20:25:37.609] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.616] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.616|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:37.618] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.651] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.651|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:37.685] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.685|INFO|[angstrong/ai_module.cpp:721]get psensor:152 count:1
[2025-10-21 20:25:37.702] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.703] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.721] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.721|INFO|[angstrong/ai_module.cpp:721]get psensor:154 count:1
[2025-10-21 20:25:37.722] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.757] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.757|INFO|[angstrong/ai_module.cpp:721]get psensor:160 count:1
[2025-10-21 20:25:37.787] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.792] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.792|INFO|[angstrong/ai_module.cpp:721]get psensor:172 count:1
[2025-10-21 20:25:37.793] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.827] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.826|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-21 20:25:37.861] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.861|INFO|[angstrong/ai_module.cpp:721]get psensor:218 count:1
[2025-10-21 20:25:37.874] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.875] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:37.898] [info] [2560-2535]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-21 20:25:37.897|INFO|[angstrong/ai_module.cpp:721]get psensor:237 count:1
[2025-10-21 20:25:37.898] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-21 20:25:38.890] [info] [2613-2535]|[http/yt_qt_http_client.h:70]onResponse req_id 1761049535969-23, tag : /wecardpalm/device/synchronize_palm_feature
[2025-10-21 20:25:38.911] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=abcabc, palm_id=03109d3c-395a-4a29-a1b3-6ed70920c502, name=, status=1, direction=2, palm_data_version=7, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.911] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8615201806426, palm_id=1fb325e8-3ab3-4324-9487-448351a5b0ae, name=, status=2, direction=2, palm_data_version=15, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.911] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=aTnv9B6V, palm_id=219392e4-c06d-4f83-bd38-0e57791811da, name=, status=1, direction=1, palm_data_version=9, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.911] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=12345678, palm_id=4c4c8b9e-dd36-4c17-b594-044b27e2fc2b, name=, status=2, direction=1, palm_data_version=22, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.911] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=86188888889, palm_id=720aecca-acda-4974-9e1d-1c6ceb858b82, name=, status=2, direction=2, palm_data_version=13, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new_upgrade_1, palm_id=73035d09-8e04-4e01-be3c-a5b10b55e60f, name=, status=1, direction=1, palm_data_version=18, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_same_upgrade, palm_id=7c09d068-5de4-405b-b39c-30f5c395ed25, name=, status=1, direction=1, palm_data_version=16, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_same, palm_id=7fa8bb29-a51c-49ae-8c94-632e6b442696, name=, status=1, direction=1, palm_data_version=14, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new, palm_id=80006db0-7463-4f6e-bce6-2e423df5c18a, name=, status=2, direction=1, palm_data_version=12, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=vISX7nQL, palm_id=98d2e90a-442c-4fff-a212-8acaf8c4206d, name=, status=1, direction=1, palm_data_version=10, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8613012890285, palm_id=a021d71e-30f3-4373-b108-263d9946ed85, name=, status=2, direction=2, palm_data_version=14, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=EXMDBcDS, palm_id=bb0189a5-a4d3-4d35-b483-7ce318f72cec, name=, status=2, direction=1, palm_data_version=17, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_1_upgrade, palm_id=bc717db5-78bf-4217-9d42-dbba754c41d4, name=, status=2, direction=2, palm_data_version=6, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_user, palm_id=c077ecdc-dc61-4fe1-bbcf-0a9a4284f07c, name=, status=2, direction=2, palm_data_version=25, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=WC4KKQBv, palm_id=c874de3c-b9dc-437d-a699-9a735181f419, name=, status=1, direction=1, palm_data_version=8, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new_upgrade, palm_id=c97cf563-a36e-40d9-bd61-384890c01653, name=, status=1, direction=1, palm_data_version=13, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8617349756267, palm_id=d1c9f315-14c7-40d4-a461-54f430ee0964, name=, status=1, direction=2, palm_data_version=19, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.912] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_1, palm_id=e181b8b0-85d2-46c1-8319-a2332e96fc9f, name=, status=2, direction=2, palm_data_version=4, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.913] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8613770320839, palm_id=f62fe416-6fd2-4909-acf4-36917d3f1f32, name=, status=1, direction=2, palm_data_version=20, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.913] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=13012890285, palm_id=fd6c532c-df31-4eaa-b3d2-56a529553c25, name=, status=1, direction=2, palm_data_version=8, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761049538, user_tags=}
[2025-10-21 20:25:38.914] [error] [2613-2535]|[storage/yt_database_manager.h:111]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-10-21 20:25:38.914] [error] [2613-2535]|[storage/yt_palm_db_manager.cpp:241]InsertOrUpdatePalms Failed to insert/update palms, ret: 161
[2025-10-21 20:25:38.914] [error] [2613-2535]|[devicectl/yt_devicectl_manager.cpp:220]update palms to db ret :161 in UpdatePalms
[2025-10-21 20:25:38.914] [info] [2613-2535]|[looper/yt_sync_palm_looper.cpp:186]SyncPalmLooper response: hasMore=2, query_position=eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MjIsInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NjEwNDk1MzZ9LHsibmV4dF92ZXJzaW9uIjoyNSwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc2MTA0OTUzNn1dfQ==, count=20, updateResult=161
[2025-10-21 20:25:38.950] [info] [2535-2535]|[audio/yt_audio_manager.c










