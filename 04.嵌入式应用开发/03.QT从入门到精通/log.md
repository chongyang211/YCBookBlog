Last login: Thu Oct 30 16:38:41 on ttys009

The default interactive shell is now zsh.
To update your account to use zsh, please run `chsh -s /bin/zsh`.
For more details, please visit https://support.apple.com/kb/HT208050.
CHONGYYANG-MB1:~ yangchong$ adb push /Users/yangchong/PalmLinuxApp-v1.4.0-std-2025101315-m3-62-ga4e8dbf4-rv1126-20251030085001-a4e8dbf4/. /oem/yt-palm/
/Users/yangchong/PalmLinuxApp-v1.4.0-s.... 2.7 MB/s (45754088 bytes in 16.222s)
CHONGYYANG-MB1:~ yangchong$ adb shell
[root@RV1126_RV1109:/]# sh /oem/yt-palm/run.sh
LD_LIBRARY_PATH=/oem/yt-palm/libs:
[root@RV1126_RV1109:/]# qt.qpa.wayland: qtvirtualkeyboard currently is not supported at client-side, use QT_IM_MODULE=qtvirtualkeyboard at compositor-side.
QStandardPaths: wrong permissions on runtime directory /var/run, 7755 instead of 7700
[I] <MMKV.cpp:172::initialize> version v2.1.0, page size 4096, arch armeabi-v7a/NEON (hard-float)
[I] <MMKV.cpp:229::initializeMMKV> root dir: /oem/yt-palm/preference
[I] <MMKV.cpp:259::mmkvWithID> prepare to load mmkv.default (id 731bea0aecca1fdc1986674f2bb78e64) from rootPath /oem/yt-palm/preference
[I] <MemoryFile.cpp:103::open> open fd[8], flag 0x7, /oem/yt-palm/preference/mmkv.default
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6f30000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default]
[I] <MemoryFile.cpp:103::open> open fd[9], flag 0x7, /oem/yt-palm/preference/mmkv.default.crc
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa3c95000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default.crc]
[I] <MMKV_IO.cpp:244::loadMetaInfoAndCheck> meta file [mmkv.default] has flag [0]
[I] <MMKV_IO.cpp:84::loadFromFile> loading [mmkv.default] with 1877 actual size, file size 4096, InterProcess 0, meta info version:4
[I] <MMKV_IO.cpp:89::loadFromFile> loading [mmkv.default] with crc 414611570 sequence 6 version 4
[I] <MMKV_IO.cpp:133::loadFromFile> loaded [mmkv.default] with 27 key-values
[2025-10-30 16:51:26.930] [info] [1000-1000]|[src/main.cpp:71]Application started with log level: 2
[2025-10-30 16:51:26.947] [info] [1000-1000]|[register/yt_palm_register.cpp:42]switch palm register mode to: StdDeviceRegister
[2025-10-30 16:51:27.014] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:211]GetWifiState wifiState : 4
[2025-10-30 16:51:27.028] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:225]IsEthernetConnected isConnected : 0
[2025-10-30 16:51:27.055] [info] [1000-1000]|[hal/yt_device_hal_manager.cpp:175]RefreshNetworkStatus network connected changed: 1 network connected: 1 network type: 0 wifi state: CONNECTED ethernet connected: 0
[2025-10-30 16:51:27.055] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:243]StartObserveNetworkStatus
[2025-10-30 16:51:27.058] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:269]Network status monitoring started
[2025-10-30 16:51:27.058] [info] [1000-1000]|[update/yt_update_manager.cpp:65]Detect Xike device to config OTA mode :1
[2025-10-30 16:51:27.058] [warning] [1000-1000]|[access/yt_access_manager.cpp:236]YtAccessManager Open door not supported
[2025-10-30 16:51:27.058] [error] [1000-1000]|[access/yt_access_manager.cpp:22]Failed to initialize strategy
[2025-10-30 16:51:27.058] [info] [1000-1000]|[access/yt_access_manager.cpp:24]YtAccessManager created
[2025-10-30 16:51:27.058] [info] [1000-1000]|[app/yt_app_customize_manager.cpp:8]AppCustomizeManager constructor called
[2025-10-30 16:51:27.059] [info] [1000-1000]|[audio/yt_audio_manager.cpp:402]Initializing audio priorities
[2025-10-30 16:51:27.059] [info] [1000-1000]|[audio/yt_audio_manager.cpp:142]Using standard audio path: qrc:/audio/
[2025-10-30 16:51:27.059] [info] [1000-1000]|[audio/yt_audio_manager.cpp:49]Created new instance
[2025-10-30 16:51:27.059] [info] [1000-1000]|[audio/yt_audio_manager.cpp:70]Initializing audio manager
[2025-10-30 16:51:27.137] [info] [1000-1000]|[preference/yt_preference_manager.cpp:210]GetPlayerVolume volume : 0
[2025-10-30 16:51:27.138] [info] [1000-1000]|[audio/yt_audio_manager.cpp:87]Get system volume: 0
[2025-10-30 16:51:27.138] [info] [1000-1000]|[audio/yt_audio_manager.cpp:91]Synced audio volume with system: 0
[2025-10-30 16:51:27.138] [info] [1000-1000]|[audio/yt_audio_manager.cpp:106]Audio manager initialized successfully
[2025-10-30 16:51:27.138] [info] [1000-1000]|[src/main.cpp:175]Detect Xike device to config app:1
[2025-10-30 16:51:27.449] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:77]GetSerialNumber sn : CK0A3KP02526000185
[2025-10-30 16:51:27.449] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:81]GetSerialNumber ota version : 110
qml: GifPlayer GIF is already qrc:/images/gif_palm_bg1.gif
qml: GifPlayer GIF is already qrc:/images/gif_splash_1.gif
qml: SplashPage First GIF (gif_spalsh_1) animation started
qml: GifPlayer GIF start palying
qrc:/qml/main.qml:220: TypeError: Cannot call method 'debug' of undefined
[2025-10-30 16:51:27.695] [warning] [Qt-Warning] qrc:/qml/main.qml:220: TypeError: Cannot call method 'debug' of undefined (qrc:/qml/main.qml:220)
qt.qpa.wayland: No shell integration named "xdg-shell" found
[2025-10-30 16:51:27.755] [warning] [Qt-Warning] No shell integration named "xdg-shell" found
qml: LogService singleton initialized
qml: YTLoggerImpl可用，发送测试日志
[2025-10-30 16:51:27.802] [info] [QML] LogService初始化成功
qml: 已发送测试信号
[2025-10-30 16:51:27.805] [info] [QML] Application started
[2025-10-30 16:51:27.805] [info] [QML] Log service available: true
[2025-10-30 16:51:27.807] [info] [QML] Keyboard height configured: 480
[2025-10-30 16:51:27.810] [info] [1000-1000]|[preference/yt_preference_manager.cpp:316]GetAnimationEnabled Retrieved animation enabled: 1
[2025-10-30 16:51:27.812] [info] [QML] FullscreenGifOverlay: Component onCompleted , _isAnimEnable true
[2025-10-30 16:51:27.812] [info] [QML] SplashPage Splash page onCompleted
[2025-10-30 16:51:27.813] [info] [QML] WifiStatusIndicator initialized
[2025-10-30 16:51:27.821] [info] [1008-1000]|[palm/yt_palm_service.cpp:397]PalmService _InitImpl Start
[2025-10-30 16:51:27.821] [info] [1008-1000]|[palm/yt_palm_service.cpp:406]device to init impl :1, isNetworkAvailable: 1
[2025-10-30 16:51:27.821] [info] [1008-1000]|[palm/yt_palm_service.cpp:490]InitPalm start app version: v1.4.0.2025103008-a4e8dbf4, SN : CK0A3KP02526000185
[2025-10-30 16:51:27.824] [error] [1028-1000]|[palm/yt_palm_service.cpp:1133][Palmmgr]2025-10-30 16:51:27.822|ERROR|[src/yt_config_mgr.cpp:38]ConfigMgr::LoadFromFile parse failed:unexpected end of input
[2025-10-30 16:51:27.824] [info] [1000-1000]|[xike/yt_xike_device_hal_impl.cpp:384]StartWatchdog feed_divide : 30 seconds
[2025-10-30 16:51:27.824] [info] [1008-1000]|[palm/yt_palm_service.cpp:1124]PalmManager log level set to 2
[2025-10-30 16:51:27.824] [info] [1000-1000]|[app/yt_app_watchdog_looper.cpp:35]Watchdog interval calculation - system timeout: 30s, feed interval: 10s, actual interval: 20s
[2025-10-30 16:51:27.824] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:27.823|INFO|[camera/camera_device.cpp:196]set log level: 2
[2025-10-30 16:51:27.825] [error] [1028-1000]|[palm/yt_palm_service.cpp:1133][Palmmgr]2025-10-30 16:51:27.824|ERROR|[src/yt_config_mgr.cpp:38]ConfigMgr::LoadFromFile parse failed:unexpected end of input
[2025-10-30 16:51:27.825] [info] [1000-1000]|[utils/yt_future_task.cpp:22]Starting future task name WatchdogLooper, interval ms 20000, single_shot 0, auto_stop 1
[2025-10-30 16:51:27.825] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:27.824|INFO|[src/yt_palm_manager.cpp:30]init palm manager
[2025-10-30 16:51:27.825] [info] [1000-1000]|[utils/yt_future_task.cpp:83]Stop future task WatchdogLooper
[2025-10-30 16:51:27.825] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:27.825|INFO|[camera/camera_device.cpp:20]init begin, memory usage: 69292KB
[2025-10-30 16:51:27.825] [info] [1000-1000]|[src/main.cpp:232]device is m3 :0
[2025-10-30 16:51:27.826] [info] [1000-1000]|[src/main.cpp:244]Core file cleanup result: failed
[2025-10-30 16:51:27.827] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:27.827|INFO|[angstrong/ai_module.cpp:20]ai module init
[2025-10-30 16:51:27.827] [info] [1029-1000]|[utils/yt_future_task.cpp:55]handleThreadStarted WatchdogLooper in thread 0x23a2638
[2025-10-30 16:51:27.828] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:27.828|INFO|[camera/fsm_event_queue.cpp:19]FsmEventQueue starting...
[2025-10-30 16:51:27.830] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:27.830|INFO| [ ANG_SDK ] start the device listener
[2025-10-30 16:51:29.015] [info] [QML] GifPlayer durationTimer onTriggered stop
[2025-10-30 16:51:29.016] [info] [QML] SplashPage First GIF completed, switching to second GIF
[2025-10-30 16:51:29.017] [info] [QML] GifPlayer GIF start stop qrc:/images/gif_splash_1.gif
qml: GifPlayer GIF is already qrc:/images/gif_splash_2.gif
qml: SplashPage Second GIF (gif_spalsh_2) animation started - looping
qml: GifPlayer GIF start palying
[2025-10-30 16:51:29.111] [info] [QML] GifPlayer GIF start stop qrc:/images/gif_splash_2.gif
[2025-10-30 16:51:29.398] [info] [QML] SplashPage Timer triggered, reloading second GIF
[2025-10-30 16:51:29.398] [info] [QML] GifPlayer Reloading GIF: qrc:/images/gif_splash_2.gif
[2025-10-30 16:51:29.399] [info] [QML] GifPlayer GIF start stop qrc:/images/gif_splash_2.gif
[2025-10-30 16:51:29.399] [info] [QML] GifPlayer Load time statistics reset
[2025-10-30 16:51:29.401] [info] [QML] GifPlayer GIF load failed, this is Null status
qml: GifPlayer GIF is already qrc:/images/gif_splash_2.gif
qml: SplashPage Second GIF (gif_spalsh_2) animation started - looping
qml: GifPlayer GIF start palying
[2025-10-30 16:51:29.491] [info] [QML] GifPlayer Attempting to play GIF
[2025-10-30 16:51:29.492] [info] [QML] GifPlayer GIF is already playing
[2025-10-30 16:51:29.493] [info] [QML] GifPlayer GIF play result: true
[2025-10-30 16:51:29.833] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:29.833|INFO| [ ANG_SDK ] attached
[2025-10-30 16:51:37.708] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.708|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-30 16:51:37.710] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.710|INFO| [ ANG_SDK ] config uvc process cmd success, ret 0
[2025-10-30 16:51:37.738] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.738|INFO| [ ANG_SDK ] open camera successful
[2025-10-30 16:51:37.739] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.739|INFO| [ ANG_SDK ] start the device listener
[2025-10-30 16:51:37.739] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.739|INFO|[angstrong/common_module.cpp:239]common module connected
[2025-10-30 16:51:37.739] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.739|INFO|[angstrong/common_module.cpp:250]camera open ret:0
[2025-10-30 16:51:37.741] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.739|INFO|[angstrong/common_module.cpp:259]wait module connected...
[2025-10-30 16:51:37.741] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.741|INFO|[angstrong/common_module.cpp:265]wait module connected success
[2025-10-30 16:51:37.746] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.746|INFO|[angstrong/common_module.cpp:212]set algo log level success, level:2
[2025-10-30 16:51:37.750] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.750|INFO|[angstrong/common_module.cpp:223]set ufms log level success, level:2
[2025-10-30 16:51:37.789] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.789|INFO|[angstrong/ai_module.cpp:570]sync time: 2025-10-30 08:51:37 ret:0
[2025-10-30 16:51:37.792] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.792|INFO|[angstrong/ai_module.cpp:287]switch device status:working2 success, cost: 2ms
[2025-10-30 16:51:37.792] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:51:37.792|WARN|[angstrong/ai_module.cpp:291]ignore device status callback, init_finish:0
[2025-10-30 16:51:37.834] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.834|INFO|[angstrong/ai_module.cpp:84]=== Module SN: YTA8000F20000047
[2025-10-30 16:51:37.835] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.834|INFO|[angstrong/ai_module.cpp:86]=== PalmManager Version: v1.2.2.2025091812-88118d5
[2025-10-30 16:51:37.835] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.835|INFO|[angstrong/ai_module.cpp:88]=== AngstrongSDK Version: 5.0.2.2025091812
[2025-10-30 16:51:37.837] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.837|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-30 16:51:37.837] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.837|INFO|[angstrong/ai_module.cpp:91]=== Firmware Version: v1.2.1_202508262216V5.0.1.2025082619
[2025-10-30 16:51:37.841] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.841|INFO|[angstrong/ai_module.cpp:94]=== AlgoSDK Version: v1.3.0.2025082611-fe99742c
[2025-10-30 16:51:37.848] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.847|INFO|[angstrong/ai_module.cpp:96]=== Algo Version: v1.0
[2025-10-30 16:51:37.851] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.851|INFO|[angstrong/ai_module.cpp:98]=== Blacklist Version: v1.0
[2025-10-30 16:51:37.852] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.851|INFO|[angstrong/ai_module.cpp:99]=== config wait_s0_hibernate_timeout_ms: 300000
[2025-10-30 16:51:37.852] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.852|INFO|[angstrong/ai_module.cpp:100]=== config wait_s1_hibernate_timeout_ms: 600000
[2025-10-30 16:51:37.852] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.852|INFO|[angstrong/ai_module.cpp:323]switch mode: 1
[2025-10-30 16:51:37.853] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.853|INFO|[angstrong/ai_module.cpp:446]=== config solution: 3
[2025-10-30 16:51:37.853] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.853|INFO|[angstrong/ai_module.cpp:447]=== config mode: 1
[2025-10-30 16:51:37.854] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.853|INFO|[angstrong/ai_module.cpp:448]=== config register_mode: 3
[2025-10-30 16:51:37.855] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.854|INFO|[angstrong/ai_module.cpp:449]=== config recognize_mode: 4
[2025-10-30 16:51:37.863] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.862|INFO|[angstrong/ai_module.cpp:452]=== config prefer quality: high, ret: 0
[2025-10-30 16:51:37.873] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.873|INFO|[angstrong/ai_module.cpp:457]=== config prefer disable_feature: 0, ret: 0
[2025-10-30 16:51:37.879] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.878|INFO|[angstrong/ai_module.cpp:463]=== config prefer disable_retrieval: 0, ret: 0
[2025-10-30 16:51:37.883] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.883|INFO|[angstrong/ai_module.cpp:470]=== config prefer retrieval_level: high, ret: 0
[2025-10-30 16:51:37.889] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.889|INFO|[angstrong/ai_module.cpp:478]=== config prefer disable_preferred_image: 0, ret: 0
[2025-10-30 16:51:37.893] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.893|INFO|[angstrong/ai_module.cpp:485]=== config recognize disable_preferred_image: 0, ret: 0
[2025-10-30 16:51:37.898] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.897|INFO|[angstrong/ai_module.cpp:494]=== config recognize export_image_after_retrieval_empty: 1, ret: 0
[2025-10-30 16:51:37.900] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.899|INFO|[angstrong/ai_module.cpp:119]=== config stop_session: true, ret: 0
[2025-10-30 16:51:37.916] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.916|INFO|[angstrong/ai_module.cpp:127]=== config report image: 1, ret: 0
[2025-10-30 16:51:37.921] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.921|INFO|[angstrong/ai_module.cpp:138]=== config enable session: 1, ret: 0
[2025-10-30 16:51:37.928] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.928|INFO|[angstrong/ai_module.cpp:144]=== config report threshold: 1, ret: 0
[2025-10-30 16:51:37.932] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.932|INFO|[angstrong/ai_module.cpp:150]=== config report score: 1, ret: 0
[2025-10-30 16:51:37.935] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.935|INFO|[angstrong/ai_module.cpp:157]=== config session_cache_limit: 5, ret: 0
[2025-10-30 16:51:37.943] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.942|INFO|[angstrong/ai_module.cpp:167]=== config live blacklist: 0, ret: 0
[2025-10-30 16:51:37.943] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.943|INFO| [ ANG_SDK ] setEventCallback PREFER
[2025-10-30 16:51:37.943] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.943|INFO|[angstrong/ai_module.cpp:176]=== config prefer event callback, ret: 0
[2025-10-30 16:51:37.944] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.943|INFO| [ ANG_SDK ] setEventCallback RECOGNIZEPRO
[2025-10-30 16:51:37.944] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.943|INFO|[angstrong/ai_module.cpp:187]=== config recognize event callback, ret: 0
[2025-10-30 16:51:37.944] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.944|INFO|[angstrong/ai_module.cpp:193]=== config local metrics: 0
[2025-10-30 16:51:37.951] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.950|INFO|[angstrong/ai_module.cpp:205]=== config report last image: 0
[2025-10-30 16:51:37.951] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:37.951|INFO|[angstrong/common_module.cpp:274]start test device...
[2025-10-30 16:51:38.613] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.613|INFO|[angstrong/common_module.cpp:377]module stream state: 1, psensor calibration file state: 1, psensor value: 253, extra info: {"ispserver":{"pid":550,"cpu":33.3,"mem":"33.4m"},"uvc_app":{"pid":675,"cpu":17.6,"mem":"97.2m"},"memory":{"total":393,"used":196,"free":111},"disk":{"usage":"6%"},"config":{"led_current.rgb_current": 30, "led_current.ir_current": 17}}
[2025-10-30 16:51:38.614] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.613|INFO|[angstrong/ai_module.cpp:257]switch device status:ready
[2025-10-30 16:51:38.614] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:51:38.614|WARN|[angstrong/ai_module.cpp:261]ignore device status callback, init_finish:0
[2025-10-30 16:51:38.614] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.614|INFO|[angstrong/ai_module.cpp:46]ai module OnInitFinish
[2025-10-30 16:51:38.615] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:51:38.614|WARN|[angstrong/ai_module_palm_biz.cpp:636]change device_status:5 to working2
[2025-10-30 16:51:38.615] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.614|INFO|[camera/camera_device.cpp:80]init finish, ret: 0 cost: 10789ms end_mem:809064KB
[2025-10-30 16:51:38.615] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.615|INFO|[angstrong/ai_module_palm_biz.cpp:652]device_status:2 now_ms:14666781 working2_start_ms:14665955 s0_timeout_ms:300000
[2025-10-30 16:51:38.617] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.617|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-30 16:51:38.637] [info] [1008-1000]|[palm/yt_palm_service.cpp:527]Init PalmManager ret  0 palmManagerVersion: v1.2.2.2025091812-88118d5, firmwareVersion: v1.2.1_202508262216V5.0.1.2025082619, algoSdkVersion: v1.3.0.2025082611-fe99742c, algoVersion: v1.0, palmMode: 1
[2025-10-30 16:51:38.645] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:38.645|INFO|[angstrong/ai_module_palm_feat.cpp:179]GetPalmFeatureCount:{"mem_count": 0, "db_count": 0}
[2025-10-30 16:51:38.646] [info] [1008-1000]|[palm/yt_palm_service.cpp:551]PalmManager currentPalmCount: 0
[2025-10-30 16:51:38.647] [info] [1008-1000]|[palm/yt_palm_context.h:586]NotifyPalmAuthStatusChangedfrom 0 to 1
[2025-10-30 16:51:38.657] [info] [1008-1000]|[http/yt_qt_http_client.cpp:57]SSL supported, version: OpenSSL 1.1.1h  22 Sep 2020
[2025-10-30 16:51:38.682] [info] [1000-1000]|[palm/yt_palm_context.h:154]DoOnPalmAuthorizeSucceed
[2025-10-30 16:51:38.683] [info] [QML] onPalmAuthorizeSucceed
[2025-10-30 16:51:38.804] [info] [1008-1000]|[http/yt_qt_http_client.cpp:293]SSL error ignore policy set to: true
[2025-10-30 16:51:38.804] [warning] [1008-1000]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-10-30 16:51:40.955: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-10-30 16:51:40.955: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-10-30 16:51:40.955: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-10-30 16:51:40.957] [info] [1008-1000]|[storage/yt_database_manager.cpp:101]CreateOrOpenDatabase Database opened with new key directly
[2025-10-30 16:51:41.792] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 273
[2025-10-30 16:51:43.166] [info] [1008-1000]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-10-30 16:51:43.167] [info] [1008-1000]|[storage/yt_database_manager.cpp:129]CheckVersion current: 3, config: 3
[2025-10-30 16:51:43.181] [info] [1008-1000]|[devicectl/yt_devicectl_manager.cpp:141]SyncPalmsFromDB begin
[2025-10-30 16:51:43.181] [error] [1008-1000]|[storage/yt_database_manager.h:48]ExecSQL SQL execution failed, rc: 1, error: no such table: palm_entity_new, SQL: SELECT COUNT(*) FROM palm_entity_new;
[2025-10-30 16:51:43.182] [error] [1008-1000]|[storage/yt_palm_db_manager.cpp:160]QueryAllPalms QueryCount failed, ret: 161
[2025-10-30 16:51:43.182] [info] [1008-1000]|[devicectl/yt_devicectl_manager.cpp:160]SyncPalmsFromDB total : 0 done : 0, to sync : 0, invalid count : 0
[2025-10-30 16:51:43.182] [info] [1008-1000]|[devicectl/yt_devicectl_manager.cpp:172]SyncPalmsFromDB end
[2025-10-30 16:51:43.182] [warning] [1008-1000]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-10-30 16:51:45.198: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-10-30 16:51:45.198: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-10-30 16:51:45.198: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-10-30 16:51:45.200] [info] [1008-1000]|[storage/yt_database_manager.cpp:101]CreateOrOpenDatabase Database opened with new key directly
[2025-10-30 16:51:47.213] [info] [1008-1000]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-10-30 16:51:47.213] [info] [1008-1000]|[storage/yt_database_manager.cpp:129]CheckVersion current: 1, config: 1
[2025-10-30 16:51:47.214] [error] [1008-1000]|[storage/yt_database_manager.h:48]ExecSQL SQL execution failed, rc: 21, error: Unknown error, SQL: SELECT COUNT(*) FROM verify_rule;
[2025-10-30 16:51:47.214] [error] [1008-1000]|[storage/yt_verify_rule_db_manager.cpp:129]QueryVerifyRuleCount Failed to query verify_rule count, ret: 161
[2025-10-30 16:51:47.215] [warning] [1008-1000]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-10-30 16:51:49.223: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-10-30 16:51:49.223: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-10-30 16:51:49.223: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-10-30 16:51:49.224] [info] [1008-1000]|[storage/yt_database_manager.cpp:101]CreateOrOpenDatabase Database opened with new key directly
[2025-10-30 16:51:51.254] [info] [1008-1000]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-10-30 16:51:51.255] [info] [1008-1000]|[storage/yt_database_manager.cpp:129]CheckVersion current: 2, config: 1
[2025-10-30 16:51:51.255] [error] [1008-1000]|[storage/yt_database_manager.cpp:42]CheckVersion ret :165 in Initialize
[2025-10-30 16:51:51.256] [warning] [1008-1000]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
[2025-10-30 16:51:52.688] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 273
2025-10-30 16:51:53.267: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-10-30 16:51:53.267: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-10-30 16:51:53.267: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-10-30 16:51:53.268] [info] [1008-1000]|[storage/yt_database_manager.cpp:101]CreateOrOpenDatabase Database opened with new key directly
[2025-10-30 16:51:55.284] [info] [1008-1000]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-10-30 16:51:55.285] [info] [1008-1000]|[storage/yt_database_manager.cpp:129]CheckVersion current: 2, config: 1
[2025-10-30 16:51:55.286] [error] [1008-1000]|[storage/yt_database_manager.cpp:42]CheckVersion ret :165 in Initialize
[2025-10-30 16:51:55.286] [info] [1008-1000]|[palm/yt_palm_service.cpp:393]Init DevicectlManager RemoteDeviceCtlManager :0
[2025-10-30 16:51:55.286] [info] [1008-1000]|[palm/yt_palm_service.cpp:435]Network observer callback - type: 0, connected_changed: 1, has_network: 1, wifi_state: 4, ethernet_connected: 0
[2025-10-30 16:51:55.286] [info] [1008-1000]|[palm/yt_palm_service.cpp:445]SyncTime ret: 0
[2025-10-30 16:51:55.286] [info] [1008-1000]|[app/yt_app_customize_manager.cpp:24]AppCustomizeManager initializing...
[2025-10-30 16:51:55.286] [info] [1008-1000]|[app/yt_app_customize_manager.cpp:28]AppCustomizeManager initialization completed successfully
[2025-10-30 16:51:55.286] [info] [1008-1000]|[card/yt_card_manager.cpp:34]Init CardManager: unsupported vendorType 1
[2025-10-30 16:51:55.286] [info] [1008-1000]|[card/yt_card_manager.cpp:77]SetCardNoCallback SetCardNoCallback
[2025-10-30 16:51:55.286] [info] [1008-1000]|[palm/yt_palm_service.cpp:1252]Old App Version: v1.4.0.2025103008-a4e8dbf4, Solution: 2
[2025-10-30 16:51:55.287] [info] [1008-1000]|[palm/yt_palm_service.cpp:1265]Updated App Version: v1.4.0.2025103008-a4e8dbf4, Solution: 2
[2025-10-30 16:51:55.287] [info] [1008-1000]|[palm/yt_palm_service.cpp:1390]NotifyModuleReadyStatus ready: 1, current: 0
[2025-10-30 16:51:55.330] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.330|INFO|[angstrong/ai_module.cpp:570]sync time: 2025-10-30 08:51:55 ret:0
[2025-10-30 16:51:55.340] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.340|INFO| [ ANG_SDK ] CmdAndes api version: 202508262216, firmware version: 1002001
[2025-10-30 16:51:55.341] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.341|INFO|[angstrong/common_module.cpp:434]sdk_version:v1.3.0.2025082611-fe99742c firm_version:v1.2.1_202508262216V5.0.1.2025082619
[2025-10-30 16:51:55.341] [info] [1008-1000]|[palm/yt_palm_service.cpp:1402]module ready, ParseOTAVersion ret: 0, firmwareVersion: 1.2.1.202508262216, algo version: v1.3.0, algo build: 2025082611
[2025-10-30 16:51:55.342] [error] [1008-1000]|[palm/yt_palm_service.cpp:1212]not init in CheckAndUpdateAlgoVersion
[2025-10-30 16:51:55.342] [info] [1008-1000]|[palm/yt_palm_service.cpp:190]EnablePalmWorking current 0
[2025-10-30 16:51:55.342] [error] [1008-1000]|[palm/yt_palm_service.cpp:192]not init in EnablePalmWorking
[2025-10-30 16:51:55.342] [info] [1008-1000]|[palm/yt_palm_service.cpp:1423]InitSoftRebootParam
[2025-10-30 16:51:55.342] [info] [1008-1000]|[palm/yt_palm_service.cpp:466]PalmService init succeed
[2025-10-30 16:51:55.344] [info] [1000-1000]|[palm/yt_palm_context.h:130]DoOnServiceInitSucceed
[2025-10-30 16:51:55.345] [info] [QML] Palm service initialized successfully
[2025-10-30 16:51:55.346] [info] [QML] SplashPage Palm service initialized successfully
[2025-10-30 16:51:55.346] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:115]UnInit react InitSucceedEvent
[2025-10-30 16:51:55.347] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:144]NeedPalmAuth entryGuard, isPalmAuthorized 1
[2025-10-30 16:51:55.347] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:146]NeedPalmAuth entryGuard, already authorized, change to next status
[2025-10-30 16:51:55.347] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:178]NeedIoTAuth enter
[2025-10-30 16:51:55.348] [info] [1000-1000]|[auth/yt_auth_manager.cpp:66]StartCheckActivated Local key_id: f6d6c9d7-2d9b-4bac-a24a-5041490c58e2
[2025-10-30 16:51:55.348] [info] [1000-1000]|[auth/yt_auth_manager.cpp:69]Device has been activated
[2025-10-30 16:51:55.348] [info] [1000-1000]|[utils/yt_future_task.cpp:22]Starting future task name ExchangeKeyLooper, interval ms 600000, single_shot 0, auto_stop 1
[2025-10-30 16:51:55.348] [info] [1000-1000]|[utils/yt_future_task.cpp:83]Stop future task ExchangeKeyLooper
[2025-10-30 16:51:55.349] [info] [1000-1000]|[auth/yt_auth_manager.cpp:198]NotifyActivationSucceed
[2025-10-30 16:51:55.349] [info] [1000-1000]|[devicectl/yt_devicectl_manager.cpp:91]AuthManager activation succeed callback
[2025-10-30 16:51:55.349] [info] [1150-1000]|[utils/yt_future_task.cpp:55]handleThreadStarted ExchangeKeyLooper in thread 0x252f310
[2025-10-30 16:51:55.349] [info] [1000-1000]|[palm/yt_palm_context.h:216]DoOnIoTRegistrationSucceed
[2025-10-30 16:51:55.350] [info] [QML] onIoTRegistrationSucceed
[2025-10-30 16:51:55.351] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:184]NeedIoTAuth react IoTAuthSucceed
[2025-10-30 16:51:55.351] [info] [1000-1000]|[palm/yt_palm_service.cpp:119]CheckAndSetEnable current 0
[2025-10-30 16:51:55.354] [info] [1000-1000]|[palm/yt_palm_service.cpp:1223][Service enable]CheckAndUpdateAlgoVersion current : v1.0, stored : v1.0
[2025-10-30 16:51:55.354] [info] [1000-1000]|[utils/yt_future_task.cpp:22]Starting future task name SyncPalmLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-10-30 16:51:55.354] [info] [1000-1000]|[utils/yt_future_task.cpp:83]Stop future task SyncPalmLooper
[2025-10-30 16:51:55.355] [info] [1000-1000]|[utils/yt_future_task.cpp:22]Starting future task name SyncUserLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-10-30 16:51:55.355] [info] [1000-1000]|[utils/yt_future_task.cpp:83]Stop future task SyncUserLooper
[2025-10-30 16:51:55.355] [info] [1000-1000]|[utils/yt_future_task.cpp:22]Starting future task name StdHeartbeat, interval ms 60000, single_shot 0, auto_stop 1
[2025-10-30 16:51:55.355] [info] [1000-1000]|[utils/yt_future_task.cpp:83]Stop future task StdHeartbeat
[2025-10-30 16:51:55.356] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:204]Working enter, set service enable ret 0
[2025-10-30 16:51:55.358] [info] [QML] guardNavigateHome navigate to home
[2025-10-30 16:51:55.366] [info] [1151-1000]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncPalmLooper in thread 0x24d7db8
[2025-10-30 16:51:55.366] [info] [1152-1000]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncUserLooper in thread 0x24a99d8
[2025-10-30 16:51:55.366] [info] [1152-1000]|[looper/yt_sync_user_looper.cpp:69]RequestSyncUser next_id: 0, start_time: 1761798825, sync_type: incr
[2025-10-30 16:51:55.367] [info] [1153-1000]|[utils/yt_future_task.cpp:55]handleThreadStarted StdHeartbeat in thread 0x252f2c8
[2025-10-30 16:51:55.371] [info] [1151-1000]|[looper/yt_sync_palm_looper.cpp:163]RequestSyncPalm query_position:
[2025-10-30 16:51:55.371] [info] [1153-1000]|[pass/yt_pass_config_manager.cpp:654]StartBatchUploadPassRecords No unuploaded pass records
[2025-10-30 16:51:55.371] [info] [1153-1000]|[heartbeat/yt_heartbeat_manager.cpp:42]Init local heartbeat status from persistence: [{"desired_state": "{\"ocode\":\"1746854527\"}", "state_version": 1, "identifier": "device_bind_ocode_info"}, {"desired_state": "{\"scene_group\":\"8ed261b1-4a7b-4634-8b47-fe22430eba0a\",\"scene\":\"ycpalm1\"}", "state_version": 1, "identifier": "device_scene_info"}]
[2025-10-30 16:51:55.382] [info] [1155-1000]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://20.255.216.254/wecardpalm/device/incr-sync-user, traceId: 3164306745635271633351446c30514c, reqId: 1761814315367-1
[2025-10-30 16:51:55.388] [info] [1159-1000]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://20.255.216.254/wecardpalm/device/heartbeat, traceId: 703864495765376a5a704d6c50715161, reqId: 1761814315371-2
[2025-10-30 16:51:55.394] [info] [1157-1000]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://20.255.216.254/wecardpalm/device/synchronize_palm_feature, traceId: 51366735313148753437735071664262, reqId: 1761814315366-0
[2025-10-30 16:51:55.553] [error] [1086-1000]|[http/yt_qt_http_client.cpp:279]HandleSSLErrors The host name did not match any of the valid hosts for this certificate
[2025-10-30 16:51:55.554] [error] [1086-1000]|[http/yt_qt_http_client.cpp:279]HandleSSLErrors The certificate is self-signed, and untrusted
[2025-10-30 16:51:55.554] [info] [1086-1000]|[http/yt_qt_http_client.cpp:284]Ignoring SSL errors due to environment configuration
[2025-10-30 16:51:55.561] [error] [1086-1000]|[http/yt_qt_http_client.cpp:279]HandleSSLErrors The host name did not match any of the valid hosts for this certificate
[2025-10-30 16:51:55.561] [error] [1086-1000]|[http/yt_qt_http_client.cpp:279]HandleSSLErrors The certificate is self-signed, and untrusted
[2025-10-30 16:51:55.561] [info] [1086-1000]|[http/yt_qt_http_client.cpp:284]Ignoring SSL errors due to environment configuration
qml: [VirtualKeyboard] Performance monitoring: enabled
[2025-10-30 16:51:55.569] [error] [1086-1000]|[http/yt_qt_http_client.cpp:279]HandleSSLErrors The host name did not match any of the valid hosts for this certificate
[2025-10-30 16:51:55.569] [error] [1086-1000]|[http/yt_qt_http_client.cpp:279]HandleSSLErrors The certificate is self-signed, and untrusted
[2025-10-30 16:51:55.569] [info] [1086-1000]|[http/yt_qt_http_client.cpp:284]Ignoring SSL errors due to environment configuration
qml: GifPlayer GIF is already qrc:/images/gif_palm_home_image.gif
[2025-10-30 16:51:55.763] [info] [1086-1000]|[http/yt_qt_http_client.h:70]onResponse req_id 1761814315367-1, tag : /wecardpalm/device/incr-sync-user
[2025-10-30 16:51:55.763] [info] [1086-1000]|[looper/yt_sync_user_looper.cpp:106]Sync user succeed: empty response, next_id=0
[2025-10-30 16:51:55.837] [info] [QML] Home page initialized, check host connection state
[2025-10-30 16:51:55.837] [info] [1000-1000]|[preference/yt_preference_manager.cpp:316]GetAnimationEnabled Retrieved animation enabled: 1
[2025-10-30 16:51:55.838] [info] [QML] Home page gifPlayer isAnimEnable : true
[2025-10-30 16:51:55.840] [info] [1086-1000]|[http/yt_qt_http_client.h:70]onResponse req_id 1761814315371-2, tag : /wecardpalm/device/heartbeat
[2025-10-30 16:51:55.841] [info] [1086-1000]|[heartbeat/yt_heartbeat_manager.cpp:77]StartHeartbeat Response: 0
[2025-10-30 16:51:55.841] [warning] [1086-1000]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
qml: GifPlayer GIF start palying
[2025-10-30 16:51:55.852] [info] [1000-1000]|[device/yt_serial_port_device.cpp:21]StartListen
[2025-10-30 16:51:55.866] [info] [QML] Check host connection state: false (palmMode: 1, registryMode: 2)
[2025-10-30 16:51:55.867] [info] [QML] FullscreenGifOverlay main Found fullscreenGifOverlay property, connecting signals
[2025-10-30 16:51:55.868] [info] [QML] FullscreenGifOverlay main Successfully connected to fullscreenGifOverlay signals
qml: MainKeyboard onCompleted
[2025-10-30 16:51:55.878] [info] [QML] HomePage Background: disabling palm, pageType=Undefined
[2025-10-30 16:51:55.878] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 0
[2025-10-30 16:51:55.879] [info] [QML] GifPlayer Manual pause detected, staying paused
[2025-10-30 16:51:55.879] [info] [QML] GifPlayer GIF paused successfully , qrc:/images/gif_palm_home_image.gif
[2025-10-30 16:51:55.880] [info] [QML] HomePage Active
[2025-10-30 16:51:55.880] [info] [1000-1000]|[palm/yt_palm_service.cpp:190]EnablePalmWorking current 0
[2025-10-30 16:51:55.880] [info] [1000-1000]|[palm/yt_palm_service.cpp:232]CheckAndNotifyPalmWorkingfrom 0 to 1
[2025-10-30 16:51:55.882] [info] [QML] GifPlayer GIF resume successfully qrc:/images/gif_palm_home_image.gif
[2025-10-30 16:51:55.930] [info] [1000-1000]|[palm/yt_palm_context.h:238]DoOnPalmServiceWorkingStatus palm_working_enable: 1
[2025-10-30 16:51:55.930] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:277]Idle ServiceWorkingStatusEvent,  palm_working_enable 1
[2025-10-30 16:51:55.932] [info] [QML] Check host connection state: false (palmMode: 1, registryMode: 2)
[2025-10-30 16:51:55.933] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.932|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:2
[2025-10-30 16:51:55.933] [info] [1000-1000]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  , event : 1, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-30 16:51:55.934] [info] [QML] onPalmProcessEvent session_id , event : 1 error : 0, 0
[2025-10-30 16:51:55.935] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:312]Detecting react PalmProcessEvent  1
[2025-10-30 16:51:55.945] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.945|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:12ms
[2025-10-30 16:51:55.945] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.945|INFO|[angstrong/ai_module_palm_biz.cpp:38]start prefer...
[2025-10-30 16:51:55.946] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:51:55.945|WARN|[angstrong/ai_module.cpp:634]change device_status:5 to working2
[2025-10-30 16:51:55.946] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:51:55.946|INFO|[angstrong/ai_module.cpp:652]device_status:2 now_ms:14684112 working2_start_ms:14665955 s0_timeout_ms:300000
[2025-10-30 16:51:55.946] [info] [1008-1000]|[palm/yt_palm_service.cpp:269]_StartProcess palmMode: register StartPrefer ret: 0
2025-10-30 16:51:57.898: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-10-30 16:51:57.899: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-10-30 16:51:57.899: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-10-30 16:51:57.900] [info] [1086-1000]|[storage/yt_database_manager.cpp:101]CreateOrOpenDatabase Database opened with new key directly
[2025-10-30 16:51:58.701] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:51:59.513] [info] [QML] GifPlayer durationTimer onTriggered stop
[2025-10-30 16:51:59.514] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-30 16:51:59.515] [info] [QML] GifPlayer GIF start stop qrc:/images/gif_splash_2.gif
[2025-10-30 16:51:59.916] [info] [1086-1000]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-10-30 16:51:59.916] [info] [1086-1000]|[storage/yt_database_manager.cpp:129]CheckVersion current: 2, config: 1
[2025-10-30 16:51:59.916] [error] [1086-1000]|[storage/yt_database_manager.cpp:42]CheckVersion ret :165 in Initialize
[I] <MMKV.cpp:1184::sync> MMKV::sync, SyncFlag = 1
[2025-10-30 16:51:59.921] [info] [1086-1000]|[http/yt_qt_http_client.h:70]onResponse req_id 1761814315366-0, tag : /wecardpalm/device/synchronize_palm_feature
[2025-10-30 16:51:59.926] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=hello_test, palm_id=70bb06be-751b-4e45-b355-8850259097ea, name=, status=1, direction=2, palm_data_version=1, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.927] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111222, palm_id=7533e560-c06b-43e9-b53f-0fc1577b0f9a, name=, status=2, direction=2, palm_data_version=16, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.927] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111222, palm_id=8c0ee7ef-aaa4-47ff-aef0-f8611b9f0311, name=, status=2, direction=1, palm_data_version=19, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.927] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=00002, palm_id=8d3e2cf5-07d3-4502-902d-b281adeb3f9b, name=, status=2, direction=1, palm_data_version=4, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.927] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=00001, palm_id=c6d91fa6-c507-4829-86f9-4ed0d30727a1, name=, status=1, direction=2, palm_data_version=10, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.927] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111333, palm_id=ce3bb1bf-e61a-48e9-89b0-b344f222f63f, name=, status=2, direction=1, palm_data_version=14, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.927] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111333, palm_id=f9d77ea9-c804-42df-9685-238bba5a88f7, name=, status=2, direction=2, palm_data_version=18, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814319, user_tags=}
[2025-10-30 16:51:59.928] [error] [1086-1000]|[storage/yt_database_manager.h:111]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-10-30 16:51:59.928] [error] [1086-1000]|[storage/yt_palm_db_manager.cpp:241]InsertOrUpdatePalms Failed to insert/update palms, ret: 161
[2025-10-30 16:51:59.928] [error] [1086-1000]|[devicectl/yt_devicectl_manager.cpp:220]update palms to db ret :161 in UpdatePalms
[2025-10-30 16:51:59.928] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:186]SyncPalmLooper response: hasMore=2, query_position=eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MTksInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NjE4MTQzMTV9LHsibmV4dF92ZXJzaW9uIjoxOCwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc2MTgxNDMxNX1dfQ==, count=7, updateResult=161
[2025-10-30 16:52:02.131] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:05.543] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:09.000] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:12.491] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:13.210] [info] [QML] Settings button clicked, navigating to Password page
qml: Page onCompleted
qml: Password Page get password (sensitive data redacted)
[2025-10-30 16:52:13.276] [info] [1000-1000]|[palm/yt_palm_service.cpp:1104]GetDevicePwd called (sensitive data redacted)
[2025-10-30 16:52:13.277] [info] [QML] Password Page get password (sensitive data redacted)
qml: inputMethod hide
[2025-10-30 16:52:13.280] [info] [QML] Dialog is now visible
[2025-10-30 16:52:13.282] [info] [QML] Password Page dialog page onCompleted
qml: MainKeyboard onCompleted
[2025-10-30 16:52:13.295] [info] [QML] HomePage Background: disabling palm, pageType=RootPwdPage
[2025-10-30 16:52:13.295] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 1
[2025-10-30 16:52:13.295] [info] [1000-1000]|[palm/yt_palm_service.cpp:232]CheckAndNotifyPalmWorkingfrom 1 to 0
[2025-10-30 16:52:13.295] [info] [1008-1000]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: UNKNOWN
[2025-10-30 16:52:13.295] [info] [1008-1000]|[palm/yt_palm_service.cpp:848]ResetPalmParams
[2025-10-30 16:52:13.296] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:13.296|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-30 16:52:13.296] [info] [QML] GifPlayer Manual pause detected, staying paused
[2025-10-30 16:52:13.297] [info] [QML] GifPlayer GIF paused successfully , qrc:/images/gif_palm_home_image.gif
[2025-10-30 16:52:13.305] [info] [QML] HomePage Background: disabling palm, pageType=RootPwdPage
[2025-10-30 16:52:13.306] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 0
[2025-10-30 16:52:13.306] [info] [QML] GifPlayer GIF is not playing, cannot pause
[2025-10-30 16:52:13.309] [info] [QML] Home.qml NavigationUtils page PasswordDialog_QMLTYPE_51(0x2714e80, "PasswordDialog") , navigationLock true
[2025-10-30 16:52:13.331] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:52:13.330|WARN|[angstrong/ai_module.cpp:825]wait psensor stopped
[2025-10-30 16:52:13.331] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:13.331|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:34ms
[2025-10-30 16:52:13.331] [info] [1000-1000]|[palm/yt_palm_context.h:238]DoOnPalmServiceWorkingStatus palm_working_enable: 0
[2025-10-30 16:52:13.331] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:419]Detecting ServiceWorkingStatusEvent,  palm_working_enable 0
[2025-10-30 16:52:13.332] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:52:13.331|WARN|[angstrong/ai_module_palm_biz.cpp:636]change device_status:5 to working2
[2025-10-30 16:52:13.332] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:13.332|INFO|[angstrong/ai_module_palm_biz.cpp:652]device_status:2 now_ms:14701498 working2_start_ms:14665955 s0_timeout_ms:300000
[2025-10-30 16:52:13.332] [info] [1008-1000]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: UNKNOWN
[2025-10-30 16:52:13.332] [info] [1008-1000]|[palm/yt_palm_service.cpp:848]ResetPalmParams
[2025-10-30 16:52:13.332] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:13.332|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-30 16:52:13.365] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:13.365|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:32ms
qml: Dialog opened: true
qml: Dialog opened
[2025-10-30 16:52:13.537] [info] [QML] Password Page dialog opened
qml: EnglishKeyboard onCompleted
qml: custom keyboard shown
qml: EnglishKeyboard onDestruction
qml: Symbol1Keyboard onCompleted
qml: onTextChanged Password : 0
[2025-10-30 16:52:16.821] [info] [QML] Password Page onTextChanged Password :0
qml: onTextChanged Password : 00
[2025-10-30 16:52:17.011] [info] [QML] Password Page onTextChanged Password :00
qml: onTextChanged Password : 000
[2025-10-30 16:52:17.152] [info] [QML] Password Page onTextChanged Password :000
qml: onTextChanged Password : 0000
[2025-10-30 16:52:17.326] [info] [QML] Password Page onTextChanged Password :0000
qml: onTextChanged Password : 00000
[2025-10-30 16:52:17.461] [info] [QML] Password Page onTextChanged Password :00000
qml: onTextChanged Password : 000000
[2025-10-30 16:52:17.616] [info] [QML] Password Page onTextChanged Password :000000
qml: click Password entered: 000000
qml: click Password entered: 000000
qml: click Password entered: ok
[2025-10-30 16:52:18.462] [info] [QML] Password Page entered: ok
qml: setupPasswordCallbacks pwdSureClicked stackView.pop
[2025-10-30 16:52:18.463] [info] [QML] Home.qml NavigationUtils The user inputs the correct password and navigates to the settings page
qml: Password Page keyboard hidden
[2025-10-30 16:52:18.466] [info] [QML] Password Page keyboard hidden
qml: MainKeyboard onDestruction
qml: Symbol1Keyboard onDestruction
qml: Page onDestruction
[2025-10-30 16:52:18.472] [info] [QML] Home.qml NavigationUtils page finish
[2025-10-30 16:52:18.473] [info] [QML] Password Page dialog page onDestruction
[2025-10-30 16:52:18.489] [info] [QML] HomePage Background: disabling palm, pageType=RootPwdPage
[2025-10-30 16:52:18.489] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 0
[2025-10-30 16:52:18.490] [info] [QML] GifPlayer GIF is not playing, cannot pause
[2025-10-30 16:52:18.491] [info] [QML] HomePage Active
[2025-10-30 16:52:18.491] [info] [1000-1000]|[palm/yt_palm_service.cpp:190]EnablePalmWorking current 0
[2025-10-30 16:52:18.491] [info] [1000-1000]|[palm/yt_palm_service.cpp:232]CheckAndNotifyPalmWorkingfrom 0 to 1
[2025-10-30 16:52:18.494] [info] [QML] GifPlayer GIF resume successfully qrc:/images/gif_palm_home_image.gif
qml: setupPasswordCallbacks onSuccess
[2025-10-30 16:52:18.496] [info] [QML] Home page , NavigationUtils onSuccess
[2025-10-30 16:52:18.583] [info] [1000-1000]|[preference/yt_preference_manager.cpp:210]GetPlayerVolume volume : 0
[2025-10-30 16:52:18.711] [info] [QML] SettingsPage onCompleted
[2025-10-30 16:52:18.715] [info] [QML] current palmMode 3
[2025-10-30 16:52:18.716] [info] [QML] setPalmMode called with currentPalmMode: 3
[2025-10-30 16:52:18.718] [info] [QML] current output mode 2
[2025-10-30 16:52:18.718] [info] [QML] setOutputMode called with current output mode : 2
[2025-10-30 16:52:18.720] [info] [1000-1000]|[preference/yt_preference_manager.cpp:210]GetPlayerVolume volume : 0
[2025-10-30 16:52:18.720] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 1
[2025-10-30 16:52:18.720] [info] [1000-1000]|[palm/yt_palm_service.cpp:232]CheckAndNotifyPalmWorkingfrom 1 to 0
[2025-10-30 16:52:18.720] [info] [1008-1000]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: UNKNOWN
[2025-10-30 16:52:18.721] [info] [1008-1000]|[palm/yt_palm_service.cpp:848]ResetPalmParams
[2025-10-30 16:52:18.721] [info] [QML] SettingsPage onCompleted, disabling palm recognition
[2025-10-30 16:52:18.721] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:18.721|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-30 16:52:18.721] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:18.721|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:0ms
[2025-10-30 16:52:18.722] [info] [QML] Mobile palm register option initialized, currentPalmMode: 3, isSelected: false
[2025-10-30 16:52:18.723] [info] [QML] SettingsPage activated - refreshing version information
[2025-10-30 16:52:18.727] [info] [QML] Sync mode state on page activation: 3
[2025-10-30 16:52:18.727] [info] [QML] setPalmMode called with currentPalmMode: 3
[2025-10-30 16:52:18.727] [info] [1000-1000]|[preference/yt_preference_manager.cpp:210]GetPlayerVolume volume : 0
[2025-10-30 16:52:18.728] [info] [QML] Sync volume settings on page activation: 0
[2025-10-30 16:52:18.738] [info] [QML] HomePage Background: disabling palm, pageType=Settings
[2025-10-30 16:52:18.738] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 0
[2025-10-30 16:52:18.739] [info] [QML] GifPlayer Manual pause detected, staying paused
[2025-10-30 16:52:18.740] [info] [QML] GifPlayer GIF paused successfully , qrc:/images/gif_palm_home_image.gif
[2025-10-30 16:52:18.750] [info] [QML] HomePage Background: disabling palm, pageType=Settings
[2025-10-30 16:52:18.750] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 0
[2025-10-30 16:52:18.751] [info] [QML] GifPlayer GIF is not playing, cannot pause
[2025-10-30 16:52:18.803] [info] [1000-1000]|[palm/yt_palm_context.h:238]DoOnPalmServiceWorkingStatus palm_working_enable: 1
[2025-10-30 16:52:18.803] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:277]Idle ServiceWorkingStatusEvent,  palm_working_enable 1
[2025-10-30 16:52:18.803] [error] [1000-1000]|[palm/yt_palm_service.cpp:238]palm not working StartProcess
[2025-10-30 16:52:18.805] [info] [QML] Check host connection state: false (palmMode: 1, registryMode: 2)
[2025-10-30 16:52:18.806] [info] [1000-1000]|[palm/yt_palm_context.h:238]DoOnPalmServiceWorkingStatus palm_working_enable: 0
[2025-10-30 16:52:18.806] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:419]Detecting ServiceWorkingStatusEvent,  palm_working_enable 0
[2025-10-30 16:52:18.808] [info] [1008-1000]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: UNKNOWN
[2025-10-30 16:52:18.808] [info] [1008-1000]|[palm/yt_palm_service.cpp:848]ResetPalmParams
[2025-10-30 16:52:18.811] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:18.810|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-30 16:52:18.811] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:18.811|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:0ms
[2025-10-30 16:52:25.855] [info] [QML] Open mode selection dialog, current mode: 3
[2025-10-30 16:52:25.857] [info] [QML]   - palmService.GetCurrentPalmMode(): 1
[2025-10-30 16:52:25.858] [info] [QML]   - palmRegister.GetRegistryMode(): 2
[2025-10-30 16:52:25.862] [info] [QML] Mode dialog opened, force sync state
[2025-10-30 16:52:25.862] [info] [QML]   - currentPalmMode before sync: 3
[2025-10-30 16:52:25.863] [info] [1152-1000]|[looper/yt_sync_user_looper.cpp:69]RequestSyncUser next_id: 0, start_time: 1761798825, sync_type: incr
[2025-10-30 16:52:25.865] [info] [QML]   - currentPalmMode after sync: 3
[2025-10-30 16:52:25.867] [info] [QML]   - palmService.GetCurrentPalmMode(): 1
[2025-10-30 16:52:25.867] [info] [1151-1000]|[looper/yt_sync_palm_looper.cpp:163]RequestSyncPalm query_position:
[2025-10-30 16:52:25.868] [info] [QML]   - palmRegister.GetRegistryMode(): 2
[2025-10-30 16:52:25.871] [info] [1280-1000]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://20.255.216.254/wecardpalm/device/incr-sync-user, traceId: 64693458415655704b7756345969744c, reqId: 1761814345863-4
[2025-10-30 16:52:25.877] [info] [1282-1000]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://20.255.216.254/wecardpalm/device/synchronize_palm_feature, traceId: 4443484c756d4b497944774239747464, reqId: 1761814345862-3
[2025-10-30 16:52:26.113] [info] [1086-1000]|[http/yt_qt_http_client.h:70]onResponse req_id 1761814345863-4, tag : /wecardpalm/device/incr-sync-user
[2025-10-30 16:52:26.113] [info] [1086-1000]|[looper/yt_sync_user_looper.cpp:106]Sync user succeed: empty response, next_id=0
[2025-10-30 16:52:26.217] [info] [1086-1000]|[http/yt_qt_http_client.h:70]onResponse req_id 1761814345862-3, tag : /wecardpalm/device/synchronize_palm_feature
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=hello_test, palm_id=70bb06be-751b-4e45-b355-8850259097ea, name=, status=1, direction=2, palm_data_version=1, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111222, palm_id=7533e560-c06b-43e9-b53f-0fc1577b0f9a, name=, status=2, direction=2, palm_data_version=16, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111222, palm_id=8c0ee7ef-aaa4-47ff-aef0-f8611b9f0311, name=, status=2, direction=1, palm_data_version=19, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=00002, palm_id=8d3e2cf5-07d3-4502-902d-b281adeb3f9b, name=, status=2, direction=1, palm_data_version=4, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=00001, palm_id=c6d91fa6-c507-4829-86f9-4ed0d30727a1, name=, status=1, direction=2, palm_data_version=10, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111333, palm_id=ce3bb1bf-e61a-48e9-89b0-b344f222f63f, name=, status=2, direction=1, palm_data_version=14, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.227] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=111333, palm_id=f9d77ea9-c804-42df-9685-238bba5a88f7, name=, status=2, direction=2, palm_data_version=18, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761814346, user_tags=}
[2025-10-30 16:52:26.228] [error] [1086-1000]|[storage/yt_database_manager.h:111]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-10-30 16:52:26.228] [error] [1086-1000]|[storage/yt_palm_db_manager.cpp:241]InsertOrUpdatePalms Failed to insert/update palms, ret: 161
[2025-10-30 16:52:26.228] [error] [1086-1000]|[devicectl/yt_devicectl_manager.cpp:220]update palms to db ret :161 in UpdatePalms
[2025-10-30 16:52:26.228] [info] [1086-1000]|[looper/yt_sync_palm_looper.cpp:186]SyncPalmLooper response: hasMore=2, query_position=eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MTksInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NjE4MTQzNDZ9LHsibmV4dF92ZXJzaW9uIjoxOCwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc2MTgxNDM0Nn1dfQ==, count=7, updateResult=161
[2025-10-30 16:52:26.781] [info] [QML] Recognition mode clicked, current mode: 3
[2025-10-30 16:52:26.785] [info] [QML] Set mode to recognition mode(1)
[2025-10-30 16:52:26.786] [info] [1000-1000]|[palm/yt_palm_service.cpp:892]SetCurrentPalmMode: recognize, current: register
[2025-10-30 16:52:26.787] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.786|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:2
[2025-10-30 16:52:26.788] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.786|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:0ms
[2025-10-30 16:52:26.788] [info] [1008-1000]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: UNKNOWN
[2025-10-30 16:52:26.789] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.788|INFO|[angstrong/ai_module.cpp:323]switch mode: 2
[2025-10-30 16:52:26.789] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.789|INFO|[angstrong/ai_module.cpp:446]=== config solution: 3
[2025-10-30 16:52:26.790] [info] [1008-1000]|[palm/yt_palm_service.cpp:848]ResetPalmParams
[2025-10-30 16:52:26.790] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.789|INFO|[angstrong/ai_module.cpp:447]=== config mode: 2
[2025-10-30 16:52:26.790] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.790|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-30 16:52:26.790] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.790|INFO|[angstrong/ai_module.cpp:448]=== config register_mode: 3
[2025-10-30 16:52:26.791] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.790|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:0ms
[2025-10-30 16:52:26.791] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.790|INFO|[angstrong/ai_module.cpp:449]=== config recognize_mode: 4
[2025-10-30 16:52:26.797] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.797|INFO|[angstrong/ai_module.cpp:452]=== config prefer quality: normal, ret: 0
[2025-10-30 16:52:26.803] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.803|INFO|[angstrong/ai_module.cpp:457]=== config prefer disable_feature: 0, ret: 0
[2025-10-30 16:52:26.808] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.808|INFO|[angstrong/ai_module.cpp:463]=== config prefer disable_retrieval: 0, ret: 0
[2025-10-30 16:52:26.815] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.815|INFO|[angstrong/ai_module.cpp:470]=== config prefer retrieval_level: normal, ret: 0
[2025-10-30 16:52:26.820] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.819|INFO|[angstrong/ai_module.cpp:478]=== config prefer disable_preferred_image: 0, ret: 0
[2025-10-30 16:52:26.826] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.825|INFO|[angstrong/ai_module.cpp:485]=== config recognize disable_preferred_image: 0, ret: 0
[2025-10-30 16:52:26.833] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:26.832|INFO|[angstrong/ai_module.cpp:494]=== config recognize export_image_after_retrieval_empty: 1, ret: 0
[2025-10-30 16:52:26.834] [info] [QML] onPalmModeChanged: 2
[2025-10-30 16:52:26.837] [info] [QML] setPalmMode called with currentPalmMode: 1
[2025-10-30 16:52:29.906] [info] [QML] SettingsPage onDestruction
[2025-10-30 16:52:29.919] [info] [QML] HomePage Background: disabling palm, pageType=Settings
[2025-10-30 16:52:29.919] [info] [1000-1000]|[palm/yt_palm_service.cpp:207]DisablePalmWorking current 0
[2025-10-30 16:52:29.920] [info] [QML] GifPlayer GIF is not playing, cannot pause
[2025-10-30 16:52:29.920] [info] [QML] HomePage Active
[2025-10-30 16:52:29.921] [info] [1000-1000]|[palm/yt_palm_service.cpp:190]EnablePalmWorking current 0
[2025-10-30 16:52:29.921] [info] [1000-1000]|[palm/yt_palm_service.cpp:232]CheckAndNotifyPalmWorkingfrom 0 to 1
[2025-10-30 16:52:29.923] [info] [QML] GifPlayer GIF resume successfully qrc:/images/gif_palm_home_image.gif
[2025-10-30 16:52:29.975] [info] [1000-1000]|[palm/yt_palm_context.h:238]DoOnPalmServiceWorkingStatus palm_working_enable: 1
[2025-10-30 16:52:29.975] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:277]Idle ServiceWorkingStatusEvent,  palm_working_enable 1
[2025-10-30 16:52:29.976] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:29.975|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:2
[2025-10-30 16:52:29.976] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:29.976|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:0ms
[2025-10-30 16:52:29.977] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:29.977|INFO|[angstrong/ai_module_palm_biz.cpp:197]start recognize...
[2025-10-30 16:52:29.977] [info] [1008-1000]|[palm/yt_palm_service.cpp:278]_StartProcess palmMode: recognize StartRecognize ret: 0
[2025-10-30 16:52:29.977] [warning] [1028-1000]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-30 16:52:29.977|WARN|[angstrong/ai_module.cpp:634]change device_status:5 to working2
[2025-10-30 16:52:29.978] [info] [1028-1000]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-30 16:52:29.977|INFO|[angstrong/ai_module.cpp:652]device_status:2 now_ms:14718143 working2_start_ms:14665955 s0_timeout_ms:300000
[2025-10-30 16:52:29.979] [info] [QML] Check host connection state: false (palmMode: 2, registryMode: 2)
[2025-10-30 16:52:29.979] [info] [1000-1000]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  , event : 1, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-30 16:52:29.980] [info] [QML] onPalmProcessEvent session_id , event : 1 error : 0, 0
[2025-10-30 16:52:29.981] [info] [1000-1000]|[fsm/yt_palm_state_machine.h:312]Detecting react PalmProcessEvent  1
[2025-10-30 16:52:32.784] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:36.282] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:39.679] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:43.040] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-30 16:52:46.417] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39