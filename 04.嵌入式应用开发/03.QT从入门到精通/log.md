Last login: Wed Oct 22 16:17:30 on ttys004

The default interactive shell is now zsh.
To update your account to use zsh, please run `chsh -s /bin/zsh`.
For more details, please visit https://support.apple.com/kb/HT208050.
CHONGYYANG-MB1:~ yangchong$ adb push /Users/yangchong/PalmLinuxApp-v1.4.0-std-2025101315-m3-38-g0e3dd0d8-rv1126-20251022081957-0e3dd0d8/. /oem/yt-palm/
/Users/yangchong/PalmLinuxApp-v1.4.0-std-2025101315-m3-38-g0e3dd0d8-rv1126-2025...57-0e3dd0d8/./: 7 files pushed, 0 skipped. 2.2 MB/s (44779240 bytes in 19.029s)
CHONGYYANG-MB1:~ yangchong$ adb shell
[root@RV1126_RV1109:/]# /oem/yt-palm/run.sh
/bin/bash: warning: setlocale: LC_ALL: cannot change locale (zh_CN.utf8)
LD_LIBRARY_PATH=/oem/yt-palm/libs:
[root@RV1126_RV1109:/]# qt.qpa.wayland: qtvirtualkeyboard currently is not supported at client-side, use QT_IM_MODULE=qtvirtualkeyboard at compositor-side.
QStandardPaths: wrong permissions on runtime directory /var/run, 7755 instead of 7700
[I] <MMKV.cpp:172::initialize> version v2.1.0, page size 4096, arch armeabi-v7a/NEON (hard-float)
[I] <MMKV.cpp:229::initializeMMKV> root dir: /oem/yt-palm/preference
[I] <MMKV.cpp:259::mmkvWithID> prepare to load mmkv.default (id 731bea0aecca1fdc1986674f2bb78e64) from rootPath /oem/yt-palm/preference
[I] <MemoryFile.cpp:103::open> open fd[8], flag 0x7, /oem/yt-palm/preference/mmkv.default
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa3d24000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default]
[I] <MemoryFile.cpp:103::open> open fd[9], flag 0x7, /oem/yt-palm/preference/mmkv.default.crc
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6fc0000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default.crc]
[I] <MMKV_IO.cpp:244::loadMetaInfoAndCheck> meta file [mmkv.default] has flag [0]
[I] <MMKV_IO.cpp:84::loadFromFile> loading [mmkv.default] with 6769 actual size, file size 8192, InterProcess 0, meta info version:4
[I] <MMKV_IO.cpp:89::loadFromFile> loading [mmkv.default] with crc 3264058662 sequence 1166 version 4
[I] <MMKV_IO.cpp:133::loadFromFile> loaded [mmkv.default] with 29 key-values
[2025-10-22 16:25:41.287] [info] [2378-2378]|[src/main.cpp:71]Application started with log level: 2
[2025-10-22 16:25:41.305] [info] [2378-2378]|[register/yt_palm_register.cpp:42]switch palm register mode to: StdDeviceRegister
[2025-10-22 16:25:41.368] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:211]GetWifiState wifiState : 4
[2025-10-22 16:25:41.381] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:225]IsEthernetConnected isConnected : 0
[2025-10-22 16:25:41.409] [info] [2378-2378]|[hal/yt_device_hal_manager.cpp:175]RefreshNetworkStatus network connected changed: 1 network connected: 1 network type: 0 wifi state: CONNECTED ethernet connected: 0
[2025-10-22 16:25:41.409] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:243]StartObserveNetworkStatus
[2025-10-22 16:25:41.411] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:269]Network status monitoring started
[2025-10-22 16:25:41.412] [info] [2378-2378]|[update/yt_update_manager.cpp:65]Detect Xike device to config OTA mode :1
[2025-10-22 16:25:41.412] [warning] [2378-2378]|[access/yt_access_manager.cpp:236]YtAccessManager Open door not supported
[2025-10-22 16:25:41.412] [error] [2378-2378]|[access/yt_access_manager.cpp:22]Failed to initialize strategy
[2025-10-22 16:25:41.412] [info] [2378-2378]|[access/yt_access_manager.cpp:24]YtAccessManager created
[2025-10-22 16:25:41.412] [info] [2378-2378]|[app/yt_app_customize_manager.cpp:8]AppCustomizeManager constructor called
[2025-10-22 16:25:41.413] [info] [2378-2378]|[audio/yt_audio_manager.cpp:401]Initializing audio priorities
[2025-10-22 16:25:41.413] [info] [2378-2378]|[audio/yt_audio_manager.cpp:142]Using standard audio path: qrc:/audio/
[2025-10-22 16:25:41.413] [info] [2378-2378]|[audio/yt_audio_manager.cpp:49]Created new instance
[2025-10-22 16:25:41.413] [info] [2378-2378]|[audio/yt_audio_manager.cpp:70]Initializing audio manager
[2025-10-22 16:25:41.491] [info] [2378-2378]|[preference/yt_preference_manager.cpp:210]GetPlayerVolume volume : 39
[2025-10-22 16:25:41.492] [info] [2378-2378]|[audio/yt_audio_manager.cpp:87]Get system volume: 39
[2025-10-22 16:25:41.492] [info] [2378-2378]|[audio/yt_audio_manager.cpp:91]Synced audio volume with system: 39
[2025-10-22 16:25:41.492] [info] [2378-2378]|[audio/yt_audio_manager.cpp:106]Audio manager initialized successfully
[2025-10-22 16:25:41.493] [info] [2378-2378]|[src/main.cpp:175]Detect Xike device to config app:1
[2025-10-22 16:25:41.674] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:77]GetSerialNumber sn : CK0A3KP02526000185
[2025-10-22 16:25:41.674] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:81]GetSerialNumber ota version : 110
qrc:/qml/pages/Splash.qml:57: TypeError: Cannot call method 'info' of undefined
[2025-10-22 16:25:41.770] [warning] [Qt-Warning] qrc:/qml/pages/Splash.qml:57: TypeError: Cannot call method 'info' of undefined (qrc:/qml/pages/Splash.qml:57)
qrc:/qml/components/GifPlayer.qml:128: TypeError: Cannot call method 'info' of undefined
[2025-10-22 16:25:41.771] [warning] [Qt-Warning] qrc:/qml/components/GifPlayer.qml:128: TypeError: Cannot call method 'info' of undefined (qrc:/qml/components/GifPlayer.qml:128)
qrc:/qml/pages/Splash.qml:77: TypeError: Cannot call method 'error' of undefined
[2025-10-22 16:25:41.774] [warning] [Qt-Warning] qrc:/qml/pages/Splash.qml:77: TypeError: Cannot call method 'error' of undefined (qrc:/qml/pages/Splash.qml:77)
qrc:/qml/main.qml:227: TypeError: Cannot call method 'debug' of undefined
[2025-10-22 16:25:41.849] [warning] [Qt-Warning] qrc:/qml/main.qml:227: TypeError: Cannot call method 'debug' of undefined (qrc:/qml/main.qml:227)
qt.qpa.wayland: No shell integration named "xdg-shell" found
[2025-10-22 16:25:41.913] [warning] [Qt-Warning] No shell integration named "xdg-shell" found
qml: LogService singleton initialized
qml: YTLoggerImpl可用，发送测试日志
[2025-10-22 16:25:41.965] [info] [QML] LogService初始化成功
qml: 已发送测试信号
[2025-10-22 16:25:41.968] [info] [QML] Application started
[2025-10-22 16:25:41.968] [info] [QML] Log service available: true
[2025-10-22 16:25:41.969] [info] [QML] Keyboard height configured: 480
[2025-10-22 16:25:41.973] [info] [QML] SplashPage Splash page onCompleted
[2025-10-22 16:25:41.974] [info] [QML] WifiStatusIndicator initialized
qrc:/qml/main.qml:207: ReferenceError: currentItem is not defined
[2025-10-22 16:25:41.981] [warning] [Qt-Warning] qrc:/qml/main.qml:207: ReferenceError: currentItem is not defined (qrc:/qml/main.qml:207)
[2025-10-22 16:25:41.982] [info] [2393-2378]|[palm/yt_palm_service.cpp:397]PalmService _InitImpl Start
[2025-10-22 16:25:41.982] [info] [2393-2378]|[palm/yt_palm_service.cpp:406]device to init impl :1, isNetworkAvailable: 1
[2025-10-22 16:25:41.982] [info] [2393-2378]|[palm/yt_palm_service.cpp:490]InitPalm start app version: v1.4.0.2025102208-0e3dd0d8, SN : CK0A3KP02526000185
[2025-10-22 16:25:41.985] [info] [2393-2378]|[palm/yt_palm_service.cpp:1124]PalmManager log level set to 2
[2025-10-22 16:25:41.985] [error] [2422-2378]|[palm/yt_palm_service.cpp:1133][Palmmgr]2025-10-22 16:25:41.983|ERROR|[src/yt_config_mgr.cpp:38]ConfigMgr::LoadFromFile parse failed:unexpected end of input
[2025-10-22 16:25:41.985] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:41.984|INFO|[camera/camera_device.cpp:196]set log level: 2
[2025-10-22 16:25:41.985] [error] [2422-2378]|[palm/yt_palm_service.cpp:1133][Palmmgr]2025-10-22 16:25:41.984|ERROR|[src/yt_config_mgr.cpp:38]ConfigMgr::LoadFromFile parse failed:unexpected end of input
[2025-10-22 16:25:41.985] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:41.985|INFO|[src/yt_palm_manager.cpp:30]init palm manager
[2025-10-22 16:25:41.985] [info] [2378-2378]|[xike/yt_xike_device_hal_impl.cpp:384]StartWatchdog feed_divide : 30 seconds
[2025-10-22 16:25:41.985] [info] [2378-2378]|[app/yt_app_watchdog_looper.cpp:35]Watchdog interval calculation - system timeout: 30s, feed interval: 10s, actual interval: 20s
[2025-10-22 16:25:41.985] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:41.985|INFO|[camera/camera_device.cpp:20]init begin, memory usage: 56540KB
[2025-10-22 16:25:41.985] [info] [2378-2378]|[utils/yt_future_task.cpp:22]Starting future task name WatchdogLooper, interval ms 20000, single_shot 0, auto_stop 1
[2025-10-22 16:25:41.985] [info] [2378-2378]|[utils/yt_future_task.cpp:83]Stop future task WatchdogLooper
[2025-10-22 16:25:41.986] [info] [2378-2378]|[src/main.cpp:232]device is m3 :0
[2025-10-22 16:25:41.986] [info] [2378-2378]|[src/main.cpp:244]Core file cleanup result: failed
[2025-10-22 16:25:41.987] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:41.986|INFO|[angstrong/ai_module.cpp:20]ai module init
[2025-10-22 16:25:41.987] [info] [2423-2378]|[utils/yt_future_task.cpp:55]handleThreadStarted WatchdogLooper in thread 0x239ff38
[2025-10-22 16:25:41.988] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:41.987|INFO|[camera/fsm_event_queue.cpp:19]FsmEventQueue starting...
[2025-10-22 16:25:41.989] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:41.988|INFO| [ ANG_SDK ] start the device listener
[2025-10-22 16:25:43.993] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:43.992|INFO| [ ANG_SDK ] attached
[2025-10-22 16:25:51.868] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.868|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-22 16:25:51.870] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.870|INFO| [ ANG_SDK ] config uvc process cmd success, ret 0
[2025-10-22 16:25:51.900] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.900|INFO| [ ANG_SDK ] open camera successful
[2025-10-22 16:25:51.901] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.901|INFO| [ ANG_SDK ] start the device listener
[2025-10-22 16:25:51.902] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.901|INFO|[angstrong/common_module.cpp:250]camera open ret:0
[2025-10-22 16:25:51.904] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.902|INFO|[angstrong/common_module.cpp:239]common module connected
[2025-10-22 16:25:51.905] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.903|INFO|[angstrong/common_module.cpp:259]wait module connected...
[2025-10-22 16:25:51.905] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.904|INFO|[angstrong/common_module.cpp:265]wait module connected success
[2025-10-22 16:25:51.908] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.908|INFO|[angstrong/common_module.cpp:212]set algo log level success, level:2
[2025-10-22 16:25:51.912] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.912|INFO|[angstrong/common_module.cpp:223]set ufms log level success, level:2
[2025-10-22 16:25:51.961] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.960|INFO|[angstrong/ai_module.cpp:570]sync time: 2025-10-22 08:25:51 ret:0
[2025-10-22 16:25:51.964] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:51.964|INFO|[angstrong/ai_module.cpp:287]switch device status:working2 success, cost: 3ms
[2025-10-22 16:25:51.965] [warning] [2422-2378]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-22 16:25:51.964|WARN|[angstrong/ai_module.cpp:291]ignore device status callback, init_finish:0
[2025-10-22 16:25:52.006] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.005|INFO|[angstrong/ai_module.cpp:84]=== Module SN: YTA8000F20000047
[2025-10-22 16:25:52.006] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.006|INFO|[angstrong/ai_module.cpp:86]=== PalmManager Version: v1.2.2.2025091812-88118d5
[2025-10-22 16:25:52.007] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.006|INFO|[angstrong/ai_module.cpp:88]=== AngstrongSDK Version: 5.0.2.2025091812
[2025-10-22 16:25:52.011] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.011|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-22 16:25:52.012] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.011|INFO|[angstrong/ai_module.cpp:91]=== Firmware Version: v1.2.1_202508262216V5.0.1.2025082619
[2025-10-22 16:25:52.016] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.015|INFO|[angstrong/ai_module.cpp:94]=== AlgoSDK Version: v1.3.0.2025082611-fe99742c
[2025-10-22 16:25:52.020] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.020|INFO|[angstrong/ai_module.cpp:96]=== Algo Version: v1.0
[2025-10-22 16:25:52.025] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.025|INFO|[angstrong/ai_module.cpp:98]=== Blacklist Version: v1.0
[2025-10-22 16:25:52.025] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.025|INFO|[angstrong/ai_module.cpp:99]=== config wait_s0_hibernate_timeout_ms: 300000
[2025-10-22 16:25:52.026] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.025|INFO|[angstrong/ai_module.cpp:100]=== config wait_s1_hibernate_timeout_ms: 600000
[2025-10-22 16:25:52.027] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.026|INFO|[angstrong/ai_module.cpp:323]switch mode: 2
[2025-10-22 16:25:52.027] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.026|INFO|[angstrong/ai_module.cpp:446]=== config solution: 3
[2025-10-22 16:25:52.028] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.026|INFO|[angstrong/ai_module.cpp:447]=== config mode: 2
[2025-10-22 16:25:52.028] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.026|INFO|[angstrong/ai_module.cpp:448]=== config register_mode: 3
[2025-10-22 16:25:52.029] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.028|INFO|[angstrong/ai_module.cpp:449]=== config recognize_mode: 4
[2025-10-22 16:25:52.036] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.035|INFO|[angstrong/ai_module.cpp:452]=== config prefer quality: normal, ret: 0
[2025-10-22 16:25:52.039] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.038|INFO|[angstrong/ai_module.cpp:457]=== config prefer disable_feature: 0, ret: 0
[2025-10-22 16:25:52.042] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.042|INFO|[angstrong/ai_module.cpp:463]=== config prefer disable_retrieval: 0, ret: 0
[2025-10-22 16:25:52.046] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.045|INFO|[angstrong/ai_module.cpp:470]=== config prefer retrieval_level: normal, ret: 0
[2025-10-22 16:25:52.053] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.051|INFO|[angstrong/ai_module.cpp:478]=== config prefer disable_preferred_image: 0, ret: 0
[2025-10-22 16:25:52.059] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.058|INFO|[angstrong/ai_module.cpp:485]=== config recognize disable_preferred_image: 0, ret: 0
[2025-10-22 16:25:52.064] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.063|INFO|[angstrong/ai_module.cpp:494]=== config recognize export_image_after_retrieval_empty: 1, ret: 0
[2025-10-22 16:25:52.071] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.070|INFO|[angstrong/ai_module.cpp:119]=== config stop_session: true, ret: 0
[2025-10-22 16:25:52.075] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.075|INFO|[angstrong/ai_module.cpp:127]=== config report image: 1, ret: 0
[2025-10-22 16:25:52.079] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.079|INFO|[angstrong/ai_module.cpp:138]=== config enable session: 1, ret: 0
[2025-10-22 16:25:52.084] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.083|INFO|[angstrong/ai_module.cpp:144]=== config report threshold: 1, ret: 0
[2025-10-22 16:25:52.088] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.088|INFO|[angstrong/ai_module.cpp:150]=== config report score: 1, ret: 0
[2025-10-22 16:25:52.093] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.092|INFO|[angstrong/ai_module.cpp:157]=== config session_cache_limit: 5, ret: 0
[2025-10-22 16:25:52.099] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.099|INFO|[angstrong/ai_module.cpp:167]=== config live blacklist: 0, ret: 0
[2025-10-22 16:25:52.099] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.099|INFO| [ ANG_SDK ] setEventCallback PREFER
[2025-10-22 16:25:52.100] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.099|INFO|[angstrong/ai_module.cpp:176]=== config prefer event callback, ret: 0
[2025-10-22 16:25:52.101] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.100|INFO| [ ANG_SDK ] setEventCallback RECOGNIZEPRO
[2025-10-22 16:25:52.101] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.101|INFO|[angstrong/ai_module.cpp:187]=== config recognize event callback, ret: 0
[2025-10-22 16:25:52.102] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.101|INFO|[angstrong/ai_module.cpp:193]=== config local metrics: 0
[2025-10-22 16:25:52.105] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.105|INFO|[angstrong/ai_module.cpp:205]=== config report last image: 0
[2025-10-22 16:25:52.106] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.105|INFO|[angstrong/common_module.cpp:274]start test device...
[2025-10-22 16:25:52.774] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.772|INFO|[angstrong/common_module.cpp:377]module stream state: 1, psensor calibration file state: 1, psensor value: 253, extra info: {"ispserver":{"pid":552,"cpu":33.3,"mem":"33.3m"},"uvc_app":{"pid":677,"cpu":17.6,"mem":"111.3m"},"memory":{"total":393,"used":211,"free":83},"disk":{"usage":"6%"},"config":{"led_current.rgb_current": 30, "led_current.ir_current": 17}}
[2025-10-22 16:25:52.774] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.772|INFO|[angstrong/ai_module.cpp:257]switch device status:ready
[2025-10-22 16:25:52.774] [warning] [2422-2378]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-22 16:25:52.773|WARN|[angstrong/ai_module.cpp:261]ignore device status callback, init_finish:0
[2025-10-22 16:25:52.774] [warning] [2422-2378]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-22 16:25:52.773|WARN|[angstrong/ai_module_palm_biz.cpp:636]change device_status:5 to working2
[2025-10-22 16:25:52.774] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.773|INFO|[angstrong/ai_module_palm_biz.cpp:652]device_status:2 now_ms:195470818 working2_start_ms:195470005 s0_timeout_ms:300000
[2025-10-22 16:25:52.775] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.775|INFO|[angstrong/ai_module.cpp:46]ai module OnInitFinish
[2025-10-22 16:25:52.775] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.775|INFO|[camera/camera_device.cpp:80]init finish, ret: 0 cost: 10789ms end_mem:142660KB
[2025-10-22 16:25:52.779] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.778|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-22 16:25:52.788] [info] [2393-2378]|[palm/yt_palm_service.cpp:527]Init PalmManager ret  0 palmManagerVersion: v1.2.2.2025091812-88118d5, firmwareVersion: v1.2.1_202508262216V5.0.1.2025082619, algoSdkVersion: v1.3.0.2025082611-fe99742c, algoVersion: v1.0, palmMode: 2
[2025-10-22 16:25:52.793] [info] [2393-2378]|[palm/yt_palm_service.cpp:551]PalmManager currentPalmCount: 653
[2025-10-22 16:25:52.793] [info] [2422-2378]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-22 16:25:52.792|INFO|[angstrong/ai_module_palm_feat.cpp:179]GetPalmFeatureCount:{"mem_count": 653, "db_count": 653}
[2025-10-22 16:25:52.794] [info] [2393-2378]|[palm/yt_palm_context.h:586]NotifyPalmAuthStatusChangedfrom 0 to 1
[2025-10-22 16:25:52.794] [info] [2378-2378]|[palm/yt_palm_context.h:154]DoOnPalmAuthorizeSucceed
[2025-10-22 16:25:52.795] [info] [QML] onPalmAuthorizeSucceed
[2025-10-22 16:25:52.810] [info] [2393-2378]|[http/yt_qt_http_client.cpp:57]SSL supported, version: OpenSSL 1.1.1h  22 Sep 2020
[2025-10-22 16:25:52.971] [info] [2393-2378]|[http/yt_qt_http_client.cpp:293]SSL error ignore policy set to: false
[2025-10-22 16:25:52.972] [warning] [2393-2378]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-10-22 16:25:54.994: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-10-22 16:25:54.994: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-10-22 16:25:54.994: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-10-22 16:25:54.995] [info] [2393-2378]|[storage/yt_database_manager.cpp:101]CreateOrOpenDatabase Database opened with new key directly
[2025-10-22 16:25:57.016] [info] [2393-2378]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-10-22 16:25:57.017] [info] [2393-2378]|[storage/yt_database_manager.cpp:129]CheckVersion current: 3, config: 3
[2025-10-22 16:25:57.040] [info] [2393-2378]|[devicectl/yt_devicectl_manager.cpp:141]SyncPalmsFromDB begin
[2025-10-22 16:25:57.041] [error] [2393-2378]|[storage/yt_database_manager.h:48]ExecSQL SQL execution failed, rc: 1, error: no such table: palm_entity_new, SQL: SELECT COUNT(*) FROM palm_entity_new;
[2025-10-22 16:25:57.042] [error] [2393-2378]|[storage/yt_palm_db_manager.cpp:160]QueryAllPalms QueryCount failed, ret: 161
[2025-10-22 16:25:57.042] [info] [2393-2378]|[devicectl/yt_devicectl_manager.cpp:160]SyncPalmsFromDB total : 0 done : 0, to sync : 0, invalid count : 0
[2025-10-22 16:25:57.042] [info] [2393-2378]|[devicectl/yt_devicectl_manager.cpp:172]SyncPalmsFromDB end
[2025-10-22 16:25:57.044] [warning] [2393-2378]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2