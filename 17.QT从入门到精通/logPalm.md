[2025-08-13 11:35:12.963] [info] [775-1149]|[download/yt_downloader.cpp:105]Download finished - URL: https://youtu-palm-test-1258344699.cos.ap-guangzhou.tencentcos.cn/palmsaas/apk/m3firm/SP813Z-1.1.0-20250801-ota.zip, Total bytes: 281840771
[2025-08-13 11:35:12.964] [info] [775-1149]|[download/yt_downloader.cpp:152]Handling download end - URL: https://youtu-palm-test-1258344699.cos.ap-guangzhou.tencentcos.cn/palmsaas/apk/m3firm/SP813Z-1.1.0-20250801-ota.zip, Is error: 0
[2025-08-13 11:35:12.964] [info] [775-1149]|[download/yt_downloader.cpp:121]Resetting download state
[2025-08-13 11:35:12.965] [info] [775-1149]|[utils/yt_future_task.cpp:83]Stop future task DownloadTask
[2025-08-13 11:35:12.965] [info] [1149-1149]|[update/yt_update_manager.cpp:240]OnDownloadFinished
[2025-08-13 11:35:12.965] [info] [775-1149]|[download/yt_downloader.cpp:121]Resetting download state
[2025-08-13 11:35:12.965] [info] [1149-1149]|[update/yt_update_manager.cpp:366]NotifyUpdateStatus: 1, 2, current status : 2, 100, 0
[2025-08-13 11:35:12.966] [info] [QML] Update status changed: type=1, status=2, error=0, progress=100
[2025-08-13 11:35:12.966] [info] [1149-1149]|[update/yt_update_manager.cpp:178]StartVerify: 1
[2025-08-13 11:35:12.966] [info] [1175-1149]|[update/yt_update_manager.cpp:188]_StartVerify: 1
[2025-08-13 11:35:12.969] [info] [1149-1149]|[update/yt_update_manager.cpp:366]NotifyUpdateStatus: 1, 3, current status : 2, 0, 0
[2025-08-13 11:35:12.969] [info] [QML] Update status changed: type=1, status=3, error=0, progress=0
[2025-08-13 11:35:15.159] [info] [1175-1149]|[update/yt_update_manager.cpp:193]_StartVerify calc : 255d669f4a0ea6db981514130742e006, expected : bea4fbc8ddbcfc5b503db4dcf94d1043
[2025-08-13 11:35:15.159] [error] [1175-1149]|[update/yt_update_manager.cpp:195]_StartVerify: md5 not match
[2025-08-13 11:35:15.159] [info] [1149-1149]|[update/yt_update_manager.cpp:366]NotifyUpdateStatus: 1, 6, current status : 3, 0, 193
[2025-08-13 11:35:15.160] [info] [QML] Update status changed: type=1, status=6, error=193, progress=0
[2025-08-13 11:35:15.163] [info] [1149-1149]|[update/yt_update_manager.cpp:274]Reset: Update finished, type : 1, status : 6
[2025-08-13 11:35:15.163] [info] [1149-1149]|[palm/yt_palm_service.cpp:113]CheckAndSetEnable current enable 0
[2025-08-13 11:35:15.167] [info] [1149-1149]|[palm/yt_palm_service.cpp:1195][Service enable]CheckAndUpdateAlgoVersion current : v1.0, stored : v1.0
[2025-08-13 11:35:15.167] [info] [1149-1149]|[utils/yt_future_task.cpp:22]Starting future task name SyncPalmLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-08-13 11:35:15.167] [info] [1149-1149]|[utils/yt_future_task.cpp:83]Stop future task SyncPalmLooper
[2025-08-13 11:35:15.167] [info] [1149-1149]|[utils/yt_future_task.cpp:22]Starting future task name SyncUserLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-08-13 11:35:15.167] [info] [1149-1149]|[utils/yt_future_task.cpp:83]Stop future task SyncUserLooper
[2025-08-13 11:35:15.167] [info] [1149-1149]|[utils/yt_future_task.cpp:22]Starting future task name StdHeartbeat, interval ms 60000, single_shot 0, auto_stop 1
[2025-08-13 11:35:15.167] [info] [1149-1149]|[utils/yt_future_task.cpp:83]Stop future task StdHeartbeat
[2025-08-13 11:35:15.168] [info] [1149-1149]|[update/yt_update_manager.cpp:283]Reset Delete update package: /userdata/yt-palm/update/system/1.1.0-20250801.zip
[2025-08-13 11:35:15.168] [info] [1149-1149]|[update/yt_update_manager.cpp:294]Reset: Reset completed
[2025-08-13 11:35:15.169] [info] [1370-1149]|[utils/yt_future_task.cpp:55]handleThreadStarted StdHeartbeat in thread 0xf63048
[2025-08-13 11:35:15.169] [info] [1370-1149]|[pass/yt_pass_config_manager.cpp:504]No need to batch upload pass records
[2025-08-13 11:35:15.171] [info] [1368-1149]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncPalmLooper in thread 0x1085ed8
[2025-08-13 11:35:15.176] [info] [1369-1149]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncUserLooper in thread 0xec5b68
[2025-08-13 11:35:15.183] [info] [1372-1149]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/heartbeat, traceId: 4c67386e4a36786a4e35336c5233555a, reqId: 1755056115170-12
[2025-08-13 11:35:15.193] [info] [1374-1149]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 48713871654b5751644a3454306d6253, reqId: 1755056115177-14
[2025-08-13 11:35:15.199] [info] [1372-1149]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 63613938334b504e5053615457325132, reqId: 1755056115171-13
[2025-08-13 11:35:15.520] [info] [1303-1149]|[http/yt_qt_http_client.h:70]onResponse req_id 1755056115170-12, tag : /wecardpalm/device/heartbeat
[2025-08-13 11:35:15.521] [info] [1303-1149]|[heartbeat/yt_heartbeat_manager.cpp:73]StartHeartbeat Response: 0
[2025-08-13 11:35:15.521] [info] [1303-1149]|[heartbeat/yt_heartbeat_manager.cpp:100]HandleNewDesiredState size = 0
[2025-08-13 11:35:15.522] [info] [1303-1149]|[devicectl/yt_devicectl_manager.cpp:600]OnHeartbeatSucceed
[2025-08-13 11:35:15.688] [info] [1303-1149]|[http/yt_qt_http_client.h:70]onResponse req_id 1755056115177-14, tag : /wecardpalm/device/incr-sync-user
[2025-08-13 11:35:15.689] [info] [1303-1149]|[looper/yt_sync_user_looper.cpp:79]Sync user succeed: 0
[2025-08-13 11:35:15.700] [info] [1303-1149]|[http/yt_qt_http_client.h:70]onResponse req_id 1755056115171-13, tag : /wecardpalm/device/synchronize_palm_feature
[2025-08-13 11:35:15.701] [info] [1303-1149]|[looper/yt_sync_palm_looper.cpp:173]SyncPalmLooper RequestSyncPalmStandard response: hasMore: 1, query_position: eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MTI1LCJwYWxtX2RpcmVjdGlvbiI6MSwibGFzdF90aW1lIjoxNzU1MDU2MTEzfSx7Im5leHRfdmVyc2lvbiI6OTMsInBhbG1fZGlyZWN0aW9uIjoyLCJsYXN0X3RpbWUiOjE3NTUwNTYxMTN9XX0=, datas size: 0
[2025-08-13 11:35:15.701] [info] [1303-1149]|[devicectl/yt_devicectl_manager.cpp:194]UpdatePalms entities.size: 0
[2025-08-13 11:35:15.701] [info] [1303-1149]|[looper/yt_sync_palm_looper.cpp:182]SyncPalmLooper RequestSyncPalmStandard -> RemoteDeviceCtlManager UpdatePalms ret: 0
[2025-08-13 11:35:16.993] [info] [1192-1149]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-13 11:35:18.168] [info] [QML] SettingsPage activated - refreshing version information
[2025-08-13 11:35:18.171] [info] [1149-1149]|[palm/yt_palm_service.cpp:837]GetCurrentPalmMode: recognize