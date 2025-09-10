Last login: Wed Aug 27 16:04:59 on ttys001

The default interactive shell is now zsh.
To update your account to use zsh, please run `chsh -s /bin/zsh`.
For more details, please visit https://support.apple.com/kb/HT208050.
CHONGYYANG-MB1:~ yangchong$ adb push /Users/yangchong/yt-palm /data/yt-palm/
/Users/yangchong/yt-palm: 1 file pushe...d. 0.7 MB/s (8456452 bytes in 11.507s)
CHONGYYANG-MB1:~ yangchong$ adb shell
[root@RV1126_RV1109:/]# /data/palmApp/watchdog_palmapp.sh
LD_LIBRARY_PATH=/userdata/yt-palm/libs::/oem/usrlibs:/oem/usr/lib:/oem/lib:/oem/usrlibs:/oem/usr/lib:/oem/lib:/oem/usrlibs:/oem/usr/lib:/oem/lib
QStandardPaths: XDG_RUNTIME_DIR not set, defaulting to '/tmp/runtime-root'
[I] <MMKV.cpp:172::initialize> version v2.1.0, page size 4096, arch armeabi-v7a/NEON (hard-float)
[I] <MMKV.cpp:229::initializeMMKV> root dir: /userdata/yt-palm/preference
[I] <MMKV.cpp:259::mmkvWithID> prepare to load mmkv.default (id 36b43d9009bc3bc5b82e82d707da300a) from rootPath /userdata/yt-palm/preference
[I] <MemoryFile.cpp:103::open> open fd[18], flag 0x7, /userdata/yt-palm/preference/mmkv.default
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6f2d000], oldPtr [(nil)], [/userdata/yt-palm/preference/mmkv.default]
[I] <MemoryFile.cpp:103::open> open fd[19], flag 0x7, /userdata/yt-palm/preference/mmkv.default.crc
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa38a5000], oldPtr [(nil)], [/userdata/yt-palm/preference/mmkv.default.crc]
[I] <MMKV_IO.cpp:244::loadMetaInfoAndCheck> meta file [mmkv.default] has flag [0]
[I] <MMKV_IO.cpp:84::loadFromFile> loading [mmkv.default] with 3597 actual size, file size 4096, InterProcess 0, meta info version:4
[I] <MMKV_IO.cpp:89::loadFromFile> loading [mmkv.default] with crc 2788907230 sequence 1174 version 4
[I] <MMKV_IO.cpp:133::loadFromFile> loaded [mmkv.default] with 23 key-values
[2025-08-27 16:12:57.581] [info] [2614-2614]|[src/main.cpp:69]Application started with log level: 2
[2025-08-27 16:12:57.600] [info] [2614-2614]|[register/yt_palm_register.cpp:42]switch palm register mode to: StdDeviceRegister
[2025-08-27 16:12:57.600] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:86]SXDDeviceHalImpl: constructor begin
[2025-08-27 16:12:57.601] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:92]dlopen librkwifibt.so success: 0xa6f392f8
[2025-08-27 16:12:57.601] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:97]dlsym RK_wifi_running_getConnectionInfo success: 0xa5b708a0
[2025-08-27 16:12:57.601] [info] [2614-2614]|[shengsida/yt_network_status_looper.cpp:12]NetworkStatusLooper created, interval: 5000ms
[2025-08-27 16:12:57.646] [info] [2614-2614]|[shengsida/yt_network_status_looper.cpp:22]Initial WiFi state: CONNECTED
[2025-08-27 16:12:59.685] [info] [2614-2614]|[shengsida/yt_network_status_looper.cpp:30]Initial Ethernet state: disconnected
[2025-08-27 16:12:59.685] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:103]NetworkStatusLooper created
[2025-08-27 16:12:59.685] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:105]SXDDeviceHalImpl: constructor done, this=0x88570c
[2025-08-27 16:12:59.718] [info] [2614-2614]|[hal/yt_device_hal_manager.cpp:165]RefreshNetworkStatus network connected changed: 1 network connected: 1 network type: 0 wifi state: CONNECTED ethernet connected: 0
[2025-08-27 16:12:59.718] [info] [2614-2614]|[shengsida/yt_network_status_looper.cpp:59]AddNetworkObserver: observer added, total count: 1
[2025-08-27 16:12:59.718] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:355]StartObserveNetworkStatus Start observe network status
[2025-08-27 16:12:59.718] [info] [2614-2614]|[utils/yt_future_task.cpp:22]Starting future task name NetworkStatusLooper, interval ms 5000, single_shot 0, auto_stop 1
[2025-08-27 16:12:59.718] [info] [2614-2614]|[utils/yt_future_task.cpp:83]Stop future task NetworkStatusLooper
[2025-08-27 16:12:59.718] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:359]NetworkStatusLooper started
[2025-08-27 16:12:59.718] [info] [2614-2614]|[update/yt_update_manager.cpp:71]Detect Shengsida device to config app : 2
[2025-08-27 16:12:59.719] [info] [2614-2614]|[relay/yt_relay_strategy.cpp:74]YtRelayStrategy initialize
[2025-08-27 16:12:59.719] [info] [2614-2614]|[access/yt_access_manager.cpp:60]Strategy set as Relay
[2025-08-27 16:12:59.719] [info] [2614-2614]|[relay/yt_relay_strategy.cpp:74]YtRelayStrategy initialize
[2025-08-27 16:12:59.719] [info] [2614-2614]|[access/yt_access_manager.cpp:85]Switched to Relay strategy
[2025-08-27 16:12:59.719] [info] [2614-2614]|[access/yt_access_manager.cpp:226]Detact K-V config access mode is 0, use Relay
[2025-08-27 16:12:59.719] [info] [2614-2614]|[relay/yt_relay_strategy.cpp:74]YtRelayStrategy initialize
[2025-08-27 16:12:59.719] [info] [2614-2614]|[access/yt_access_manager.cpp:238]Strategy initialized successfully
[2025-08-27 16:12:59.719] [info] [2614-2614]|[access/yt_access_manager.cpp:24]YtAccessManager created
[2025-08-27 16:12:59.720] [info] [2614-2614]|[audio/yt_audio_manager.cpp:29]AudioManager constructor called
[2025-08-27 16:12:59.720] [info] [2614-2614]|[audio/yt_audio_manager.cpp:369]Initializing audio priorities
[2025-08-27 16:12:59.720] [info] [2614-2614]|[audio/yt_audio_manager.cpp:131]Initializing audio resources
[2025-08-27 16:12:59.720] [info] [2614-2614]|[audio/yt_audio_manager.cpp:132]Audio resources initialized
[2025-08-27 16:12:59.720] [info] [2614-2614]|[audio/yt_audio_manager.cpp:48]Created new AudioManager instance
[2025-08-27 16:12:59.720] [info] [2614-2614]|[audio/yt_audio_manager.cpp:69]Initializing audio manager
[2025-08-27 16:12:59.721] [info] [2645-2614]|[utils/yt_future_task.cpp:55]handleThreadStarted NetworkStatusLooper in thread 0x8be908
[2025-08-27 16:12:59.827] [info] [2614-2614]|[preference/yt_preference_manager.cpp:189]GetPlayerVolume volume : 99
[2025-08-27 16:12:59.827] [info] [2614-2614]|[audio/yt_audio_manager.cpp:86]Get system volume: 99
[2025-08-27 16:12:59.827] [info] [2614-2614]|[audio/yt_audio_manager.cpp:90]Synced audio volume with system: 99
[2025-08-27 16:12:59.827] [info] [2614-2614]|[audio/yt_audio_manager.cpp:105]Audio manager initialized successfully
[2025-08-27 16:12:59.828] [info] [2614-2614]|[src/main.cpp:175]Detect Shengsida device to config app:2
qrc:/qml_m3/main.qml:186: TypeError: Cannot call method 'info' of undefined
[2025-08-27 16:13:00.259] [warning] [Qt-Warning] qrc:/qml_m3/main.qml:186: TypeError: Cannot call method 'info' of undefined (qrc:/qml_m3/main.qml:186)
qml: LogService singleton initialized
qml: YTLoggerImpl可用，发送测试日志
[2025-08-27 16:13:00.303] [info] [QML] LogService初始化成功
qml: 已发送测试信号
[2025-08-27 16:13:00.304] [info] [QML] Application started
[2025-08-27 16:13:00.305] [info] [QML] Log service available: true
[2025-08-27 16:13:00.305] [info] [QML] Global mask registered
[2025-08-27 16:13:00.306] [info] [QML] Keyboard height configured: 192
[2025-08-27 16:13:00.309] [info] [2619-2614]|[palm/yt_palm_service.cpp:406]PalmService _InitImpl Start
[2025-08-27 16:13:00.309] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:533]StartWatchdog status: 32768
[2025-08-27 16:13:00.309] [info] [2619-2614]|[palm/yt_palm_service.cpp:413]device to init impl :2
[2025-08-27 16:13:00.309] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:542]StartWatchdog original timeout: 21
[2025-08-27 16:13:00.309] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:545]StartWatchdog new timeout set before 30
[2025-08-27 16:13:00.309] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:552]StartWatchdog timeout set after 21
[2025-08-27 16:13:00.309] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:561]StartWatchdog timeout after setting: 21
[2025-08-27 16:13:00.309] [info] [2614-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:565]First Fed watchdog ret = 1
[2025-08-27 16:13:00.309] [info] [2614-2614]|[app/yt_app_watchdog_looper.cpp:36]Watchdog interval calculation - system timeout: 21s, feed interval: 5s, actual interval: 16s
[2025-08-27 16:13:00.309] [info] [2614-2614]|[app/yt_app_watchdog_looper.cpp:36]Watchdog interval calculation - system timeout: 21s, feed interval: 5s, actual interval: 16s
[2025-08-27 16:13:00.309] [info] [2614-2614]|[app/yt_app_watchdog_looper.cpp:12]WatchdogLooper initialized with interval: 16000ms
[2025-08-27 16:13:00.309] [info] [2614-2614]|[utils/yt_future_task.cpp:22]Starting future task name WatchdogLooper, interval ms 16000, single_shot 0, auto_stop 1
[2025-08-27 16:13:00.309] [info] [2614-2614]|[utils/yt_future_task.cpp:83]Stop future task WatchdogLooper
[2025-08-27 16:13:00.310] [info] [2619-2614]|[palm/yt_palm_service.cpp:491]InitPalm start app version: v1.2.2.2025082707-612b1fe, SN : SSDA3GP02511000108
[2025-08-27 16:13:00.313] [info] [2619-2614]|[palm/yt_palm_service.cpp:1097]PalmManager log level set to 2
[2025-08-27 16:13:00.316] [info] [2671-2614]|[hal/yt_device_hal_manager.cpp:189]DeviceHalManager::OpenApp: Open app: /usr/bin/Settings
[2025-08-27 16:13:00.317] [info] [2671-2614]|[hal/yt_device_hal_manager.cpp:190]DeviceHalManager::OpenApp: m_device_hal.get() = 0x885714
[2025-08-27 16:13:00.318] [error] [2673-2614]|[palm/yt_palm_service.cpp:1106][Palmmgr]2025-08-27 16:13:00.310|ERROR|[src/yt_config_mgr.cpp:35]ConfigMgr::LoadFromFile parse failed:unexpected end of input
[2025-08-27 16:13:00.319] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:00.313|INFO|[camera/camera_device.cpp:187]set log level: 2
[2025-08-27 16:13:00.319] [info] [2671-2614]|[hal/yt_device_hal_manager.cpp:196]DeviceHalManager::OpenApp: detect ssdHal = 0x88570c
[2025-08-27 16:13:00.320] [info] [2671-2614]|[hal/yt_device_hal_manager.cpp:200]DeviceHalManager::OpenApp: connect signalsuccessfully
[2025-08-27 16:13:00.321] [error] [2673-2614]|[palm/yt_palm_service.cpp:1106][Palmmgr]2025-08-27 16:13:00.313|ERROR|[src/yt_config_mgr.cpp:35]ConfigMgr::LoadFromFile parse failed:unexpected end of input
[2025-08-27 16:13:00.322] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:00.313|INFO|[src/yt_palm_manager.cpp:32]init palm manager
[2025-08-27 16:13:00.322] [info] [2671-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:377]OpenApp Open app: /usr/bin/Settings, args:
[2025-08-27 16:13:00.323] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:00.314|INFO|[camera/camera_device.cpp:20]init begin, memory usage: 49908KB
[2025-08-27 16:13:00.323] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:00.315|INFO|[angstrong/ai_module.cpp:20]ai module init
[2025-08-27 16:13:00.324] [warning] [Qt-Warning] QObject: Cannot create children for a parent that is in a different thread.
(Parent is yt_palm_app::SXDDeviceHalImpl(0x88570c), parent's thread is QThread(0x873088), current thread is QThread(0xab4a10)
[2025-08-27 16:13:00.325] [info] [2672-2614]|[utils/yt_future_task.cpp:55]handleThreadStarted WatchdogLooper in thread 0xab9700
QObject: Cannot create children for a parent that is in a different thread.
(Parent is yt_palm_app::SXDDeviceHalImpl(0x88570c), parent's thread is QThread(0x873088), current thread is QThread(0xab4a10)
[2025-08-27 16:13:00.325] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:13:00.334] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:00.333|INFO|[camera/fsm_event_queue.cpp:17]FsmEventQueue starting...
[2025-08-27 16:13:00.338] [info] [2671-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:430]OpenApp App started successfully: /usr/bin/Settings
[2025-08-27 16:13:00.338] [info] [2671-2614]|[palm/yt_palm_service.cpp:378]Shengsida device jump to external application :0
[2025-08-27 16:13:00.338] [info] [2614-2614]|[hal/yt_device_hal_manager.cpp:490]DeviceHalManager::onAppProcessStateChanged: Received app process state changed signal: true
[2025-08-27 16:13:00.338] [info] [2614-2614]|[hal/yt_device_hal_manager.cpp:492]DeviceHalManager::onAppProcessStateChanged: refresh network status
[2025-08-27 16:13:00.379] [info] [QML] Network state changed: type=0, hasNetwork=true, wifiState=4 (已连接)
[2025-08-27 16:13:00.388] [info] [QML] Wifi Image loaded successfully : qrc:/images/icons/wifi_status.svg
[2025-08-27 16:13:00.389] [info] [QML] Network state changed: getWifiStatusIconFromParams icon qrc:/images/icons/wifi_status.svg
[2025-08-27 16:13:00.390] [info] [2614-2614]|[hal/yt_device_hal_manager.cpp:165]RefreshNetworkStatus network connected changed: 0 network connected: 1 network type: 0 wifi state: CONNECTED ethernet connected: 0
[2025-08-27 16:13:00.391] [info] [QML] WifiStatusIndicator initialized
[2025-08-27 16:13:00.392] [info] [QML] WifiStatusIndicator initialized currentNetworkType0, currentHasNetwork=true, currentWifiState=4, currentIsEthernetConnected=false
[2025-08-27 16:13:00.395] [info] [QML] Wifi Image loaded successfully : qrc:/images/icons/wifi_status.svg
[2025-08-27 16:13:02.338] [info] [2671-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:437]CloseApp Close app: /usr/bin/Settings
[2025-08-27 16:13:02.338] [info] [2671-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:447]Attempting to terminate process gracefully
[2025-08-27 16:13:02.342] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:02.338|INFO| [ ANG_SDK ] attached
QObject::connect: Cannot queue arguments of type 'QProcess::ExitStatus'
(Make sure 'QProcess::ExitStatus' is registered using qRegisterMetaType().)
[2025-08-27 16:13:02.405] [warning] [Qt-Warning] QObject::connect: Cannot queue arguments of type 'QProcess::ExitStatus'
(Make sure 'QProcess::ExitStatus' is registered using qRegisterMetaType().)
[2025-08-27 16:13:02.405] [info] [2671-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:466]Remove process from map: /usr/bin/Settings
[2025-08-27 16:13:02.405] [info] [2671-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:469]App process closed successfully: /usr/bin/Settings
[2025-08-27 16:13:02.405] [info] [2671-2614]|[palm/yt_palm_service.cpp:386]Shengsida device close to external application :0
[2025-08-27 16:13:02.407] [info] [QML] main Palm service onCloseM3SettingsApp success
[2025-08-27 16:13:02.410] [info] [QML] WifiStatusIndicator.qml onVisibleChanged false
[2025-08-27 16:13:02.410] [info] [QML] SplashPage.qml onVisibleChanged false
[2025-08-27 16:13:02.914] [info] [QML] WifiStatusIndicator.qml onVisibleChanged true
[2025-08-27 16:13:02.915] [info] [QML] SplashPage.qml onVisibleChanged true
[2025-08-27 16:13:10.120] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.119|INFO| [ ANG_SDK ] CmdAndes api version: 202507291457
[2025-08-27 16:13:10.123] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.123|INFO| [ ANG_SDK ] config uvc process cmd success, ret 0
[2025-08-27 16:13:10.151] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.151|INFO| [ ANG_SDK ] open camera successful
[2025-08-27 16:13:10.152] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.152|INFO| [ ANG_SDK ] start the device listener
[2025-08-27 16:13:10.152] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.152|INFO|[angstrong/common_module.cpp:237]common module connected
[2025-08-27 16:13:10.152] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.152|INFO|[angstrong/common_module.cpp:248]camera open ret:0
[2025-08-27 16:13:10.153] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.153|INFO|[angstrong/common_module.cpp:257]wait module connected...
[2025-08-27 16:13:10.153] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.153|INFO|[angstrong/common_module.cpp:263]wait module connected success
[2025-08-27 16:13:10.156] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.156|INFO|[angstrong/common_module.cpp:210]set algo log level success, level:2
[2025-08-27 16:13:10.161] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.160|INFO|[angstrong/common_module.cpp:221]set ufms log level success, level:2
[2025-08-27 16:13:10.205] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.205|INFO|[angstrong/ai_module.cpp:530]sync time: 2025-08-27 08:13:10 ret:0
[2025-08-27 16:13:10.208] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.207|INFO|[angstrong/ai_module.cpp:261]switch device status:working2 success, cost: 2ms
[2025-08-27 16:13:10.208] [warning] [2673-2614]|[palm/yt_palm_service.cpp:1110][Palmmgr]2025-08-27 16:13:10.207|WARN|[angstrong/ai_module.cpp:265]ignore device status callback, init_finish:0
[2025-08-27 16:13:10.241] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.240|INFO|[angstrong/ai_module.cpp:82]=== Module SN: YTA8000F11000144
[2025-08-27 16:13:10.241] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.240|INFO|[angstrong/ai_module.cpp:84]=== PalmManager Version: v1.1.2.2025080107-248c8e3d
[2025-08-27 16:13:10.241] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.241|INFO|[angstrong/ai_module.cpp:86]=== AngstrongSDK Version: 4.9.18.2025072117
[2025-08-27 16:13:10.243] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.243|INFO| [ ANG_SDK ] CmdAndes api version: 202507291457
[2025-08-27 16:13:10.244] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.243|INFO|[angstrong/ai_module.cpp:89]=== Firmware Version: v1.1.1_202507291457V4.9.16.2025072914
[2025-08-27 16:13:10.247] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.247|INFO|[angstrong/ai_module.cpp:92]=== AlgoSDK Version: v1.2.8.2025072903-d0d1a21d
[2025-08-27 16:13:10.256] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.256|INFO|[angstrong/ai_module.cpp:94]=== Algo Version: v1.0
[2025-08-27 16:13:10.256] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.256|INFO|[angstrong/ai_module.cpp:96]=== config wait_s0_hibernate_timeout_ms: 300000
[2025-08-27 16:13:10.257] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.256|INFO|[angstrong/ai_module.cpp:97]=== config wait_s1_hibernate_timeout_ms: 600000
[2025-08-27 16:13:10.257] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.257|INFO|[angstrong/ai_module.cpp:297]switch mode: 2
[2025-08-27 16:13:10.257] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.257|INFO|[angstrong/ai_module.cpp:409]=== config solution: 3
[2025-08-27 16:13:10.257] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.257|INFO|[angstrong/ai_module.cpp:410]=== config mode: 2
[2025-08-27 16:13:10.257] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.257|INFO|[angstrong/ai_module.cpp:411]=== config register_mode: 3
[2025-08-27 16:13:10.258] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.257|INFO|[angstrong/ai_module.cpp:412]=== config recognize_mode: 4
[2025-08-27 16:13:10.261] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.261|INFO|[angstrong/ai_module.cpp:415]=== config prefer quality: normal, ret: 0
[2025-08-27 16:13:10.265] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.264|INFO|[angstrong/ai_module.cpp:420]=== config prefer disable_feature: 0, ret: 0
[2025-08-27 16:13:10.269] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.269|INFO|[angstrong/ai_module.cpp:426]=== config prefer disable_retrieval: 0, ret: 0
[2025-08-27 16:13:10.276] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.276|INFO|[angstrong/ai_module.cpp:433]=== config prefer retrieval_level: normal, ret: 0
[2025-08-27 16:13:10.281] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.281|INFO|[angstrong/ai_module.cpp:441]=== config prefer disable_preferred_image: 0, ret: 0
[2025-08-27 16:13:10.284] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.284|INFO|[angstrong/ai_module.cpp:448]=== config recognize disable_preferred_image: 0, ret: 0
[2025-08-27 16:13:10.290] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.289|INFO|[angstrong/ai_module.cpp:457]=== config recognize export_image_after_retrieval_empty: 1, ret: 0
[2025-08-27 16:13:10.294] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.294|INFO|[angstrong/ai_module.cpp:116]=== config report image: 1, ret: 0
[2025-08-27 16:13:10.298] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.298|INFO|[angstrong/ai_module.cpp:127]=== config enable session: 1, ret: 0
[2025-08-27 16:13:10.304] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.304|INFO|[angstrong/ai_module.cpp:133]=== config report threshold: 1, ret: 0
[2025-08-27 16:13:10.309] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.309|INFO|[angstrong/ai_module.cpp:139]=== config report score: 1, ret: 0
[2025-08-27 16:13:10.309] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.309|INFO| [ ANG_SDK ] setEventCallback PREFER
[2025-08-27 16:13:10.309] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.309|INFO|[angstrong/ai_module.cpp:150]=== config prefer event callback, ret: 0
[2025-08-27 16:13:10.310] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.310|INFO| [ ANG_SDK ] setEventCallback RECOGNIZEPRO
[2025-08-27 16:13:10.310] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.310|INFO|[angstrong/ai_module.cpp:161]=== config recognize event callback, ret: 0
[2025-08-27 16:13:10.310] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.310|INFO|[angstrong/ai_module.cpp:167]=== config local metrics: 0
[2025-08-27 16:13:10.313] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.313|INFO|[angstrong/ai_module.cpp:179]=== config report last image: 0
[2025-08-27 16:13:10.313] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:10.313|INFO|[angstrong/common_module.cpp:272]start test device...
[2025-08-27 16:13:11.594] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:11.594|INFO|[angstrong/common_module.cpp:375]module stream state: 1, psensor calibration file state: 1, psensor value: 253, extra info: {"ispserver":{"pid":551,"cpu":30.0,"mem":"33.4m"},"uvc_app":{"pid":677,"cpu":23.5,"mem":"59.4m"},"memory":{"total":393,"used":147,"free":185},"disk":{"usage":"5%"}}
[2025-08-27 16:13:11.596] [error] [2673-2614]|[palm/yt_palm_service.cpp:1106][Palmmgr]2025-08-27 16:13:11.594|ERROR|[angstrong/ai_module.cpp:231]switch device status:ready
[2025-08-27 16:13:11.597] [warning] [2673-2614]|[palm/yt_palm_service.cpp:1110][Palmmgr]2025-08-27 16:13:11.596|WARN|[angstrong/ai_module.cpp:235]ignore device status callback, init_finish:0
[2025-08-27 16:13:11.598] [warning] [2673-2614]|[palm/yt_palm_service.cpp:1110][Palmmgr]2025-08-27 16:13:11.597|WARN|[angstrong/ai_module_palm_biz.cpp:636]change device_status:5 to working2
[2025-08-27 16:13:11.598] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:11.598|INFO|[angstrong/ai_module_palm_biz.cpp:652]device_status:2 now_ms:218987 working2_start_ms:217594 s0_timeout_ms:300000
[2025-08-27 16:13:11.600] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:11.598|INFO|[angstrong/ai_module.cpp:46]ai module OnInitFinish
[2025-08-27 16:13:11.600] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:11.599|INFO|[camera/camera_device.cpp:76]init finish, ret: 0 cost: 11285ms end_mem:54584KB
[2025-08-27 16:13:11.604] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:11.603|INFO| [ ANG_SDK ] CmdAndes api version: 202507291457
[2025-08-27 16:13:11.622] [info] [2619-2614]|[palm/yt_palm_service.cpp:527]Init PalmManager ret  0 palmManagerVersion: v1.1.2.2025080107-248c8e3d, firmwareVersion: v1.1.1_202507291457V4.9.16.2025072914, algoSdkVersion: v1.2.8.2025072903-d0d1a21d, algoVersion: v1.0, palmMode: 2
[2025-08-27 16:13:11.627] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:11.627|INFO|[angstrong/ai_module_palm_feat.cpp:187]GetPalmFeatureCount:{"mem_count": 167, "db_count": 167}
[2025-08-27 16:13:11.628] [info] [2619-2614]|[palm/yt_palm_service.cpp:551]PalmManager currentPalmCount: 167
[2025-08-27 16:13:11.629] [info] [2619-2614]|[palm/yt_palm_context.h:544][Success] Module return code: 0
[2025-08-27 16:13:11.630] [info] [2619-2614]|[palm/yt_palm_context.h:556]NotifyPalmAuthStatusChangedfrom 0 to 1
[2025-08-27 16:13:11.630] [info] [2614-2614]|[palm/yt_palm_context.h:154]DoOnPalmAuthorizeSucceed
[2025-08-27 16:13:11.631] [info] [QML] onPalmAuthorizeSucceed
[2025-08-27 16:13:11.632] [info] [2619-2614]|[palm/yt_palm_service.cpp:430]PalmService _InitImpl M3 wait_for 3 seconds
[2025-08-27 16:13:11.632] [info] [2619-2614]|[palm/yt_palm_service.cpp:434]almService _InitImpl M3 _DelayedM3Task is finish status
[2025-08-27 16:13:11.646] [info] [2619-2614]|[http/yt_qt_http_client.cpp:57]SSL supported, version: OpenSSL 1.1.1h  22 Sep 2020
[2025-08-27 16:13:11.788] [info] [2619-2614]|[http/yt_qt_http_client.cpp:291]SSL error ignore policy set to: false
[2025-08-27 16:13:11.788] [info] [2619-2614]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-27 16:13:11.789] [warning] [2619-2614]|[utils/yt_file_util.cpp:34]Directory already exists: /userdata/yt-palm/data
2025-08-27 16:13:13.757: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-08-27 16:13:13.758: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-08-27 16:13:13.758: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-08-27 16:13:13.759] [info] [2619-2614]|[storage/yt_database_manager.cpp:101]Database opened with new key directly
[2025-08-27 16:13:15.624] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:13:15.727] [info] [2619-2614]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-08-27 16:13:15.728] [info] [2619-2614]|[storage/yt_database_manager.cpp:129]CheckVersion current :3 config :2
[2025-08-27 16:13:15.728] [error] [2619-2614]|[storage/yt_database_manager.cpp:42]CheckVersion ret :165 in Initialize
[2025-08-27 16:13:15.728] [info] [2619-2614]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-27 16:13:15.728] [warning] [2619-2614]|[utils/yt_file_util.cpp:34]Directory already exists: /userdata/yt-palm/data
2025-08-27 16:13:17.682: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-08-27 16:13:17.682: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-08-27 16:13:17.682: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-08-27 16:13:17.683] [info] [2619-2614]|[storage/yt_database_manager.cpp:101]Database opened with new key directly
[2025-08-27 16:13:19.676] [info] [2619-2614]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-08-27 16:13:19.677] [info] [2619-2614]|[storage/yt_database_manager.cpp:129]CheckVersion current :3 config :2
[2025-08-27 16:13:19.677] [error] [2619-2614]|[storage/yt_database_manager.cpp:42]CheckVersion ret :165 in Initialize
[2025-08-27 16:13:19.677] [error] [2619-2614]|[devicectl/yt_devicectl_manager.cpp:74]Init Palm DBManager ret :165 in Init
[2025-08-27 16:13:19.677] [info] [2619-2614]|[palm/yt_palm_service.cpp:402]Init DevicectlManager RemoteDeviceCtlManager :165
[2025-08-27 16:13:19.677] [info] [2619-2614]|[palm/yt_palm_service.cpp:446]Network status changed: 1, has_network: 1
Configuring serial port
[2025-08-27 16:13:19.678] [info] [2619-2614]|[palm/yt_palm_service.cpp:451]SyncTime ret: 0
[2025-08-27 16:13:19.701] [info] [2619-2614]|[card/yt_card_manager.cpp:45]Init CardManager initialized with scanner Ssdmh1903Scanner
[2025-08-27 16:13:19.701] [info] [2619-2614]|[card/yt_card_manager.cpp:77]SetCardNoCallback SetCardNoCallback
[2025-08-27 16:13:19.701] [info] [2619-2614]|[palm/yt_palm_service.cpp:1224]Old App Version: v1.2.0.2025081805-8755822, Solution: 2
[2025-08-27 16:13:19.702] [info] [2619-2614]|[palm/yt_palm_service.cpp:1237]Updated App Version: v1.2.2.2025082707-612b1fe, Solution: 2
[2025-08-27 16:13:19.702] [info] [2619-2614]|[palm/yt_palm_service.cpp:1285]NotifyModuleReadyStatus ready: 1, current: 0
[2025-08-27 16:13:19.717] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:19.716|INFO|[angstrong/ai_module.cpp:530]sync time: 2025-08-27 08:13:19 ret:0
[2025-08-27 16:13:19.726] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:19.726|INFO| [ ANG_SDK ] CmdAndes api version: 202507291457
[2025-08-27 16:13:19.727] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:13:19.726|INFO|[angstrong/common_module.cpp:432]sdk_version:v1.2.8.2025072903-d0d1a21d firm_version:v1.1.1_202507291457V4.9.16.2025072914
[2025-08-27 16:13:19.727] [info] [2619-2614]|[palm/yt_palm_service.cpp:1302]module ready, ParseOTAVersion ret: 0, firmwareVersion: 1.1.1.202507291457, algo version: v1.2.8, algo build: 2025072903
[2025-08-27 16:13:19.727] [error] [2619-2614]|[palm/yt_palm_service.cpp:1185]not init in CheckAndUpdateAlgoVersion
[2025-08-27 16:13:19.727] [info] [2619-2614]|[palm/yt_palm_service.cpp:186]EnablePalmWorking current working 0
[2025-08-27 16:13:19.728] [error] [2619-2614]|[palm/yt_palm_service.cpp:188]not init in EnablePalmWorking
[2025-08-27 16:13:19.728] [info] [2619-2614]|[palm/yt_palm_service.cpp:468]PalmService init succeed
[2025-08-27 16:13:19.728] [info] [2614-2614]|[palm/yt_palm_context.h:130]DoOnServiceInitSucceed
[2025-08-27 16:13:19.729] [info] [QML] Palm service initialized successfully
[2025-08-27 16:13:19.730] [info] [QML] Palm service initialized successfully
[2025-08-27 16:13:19.730] [info] [2614-2614]|[fsm/yt_palm_state_machine.h:115]UnInit react InitSucceedEvent
[2025-08-27 16:13:19.730] [info] [2614-2614]|[fsm/yt_palm_state_machine.h:144]NeedPalmAuth entryGuard, isPalmAuthorized 1
[2025-08-27 16:13:19.730] [info] [2614-2614]|[fsm/yt_palm_state_machine.h:146]NeedPalmAuth entryGuard, already authorized, change to next status
[2025-08-27 16:13:19.731] [info] [2614-2614]|[fsm/yt_palm_state_machine.h:178]NeedIoTAuth enter
[2025-08-27 16:13:19.731] [info] [2614-2614]|[auth/yt_auth_manager.cpp:66]StartCheckActivated Local key_id: 331d45e0-297e-44a3-a45a-c98f5fd06f58
[2025-08-27 16:13:19.731] [info] [2614-2614]|[auth/yt_auth_manager.cpp:69]Device has been activated
[2025-08-27 16:13:19.731] [info] [2614-2614]|[utils/yt_future_task.cpp:22]Starting future task name ExchangeKeyLooper, interval ms 600000, single_shot 0, auto_stop 1
[2025-08-27 16:13:19.731] [info] [2614-2614]|[utils/yt_future_task.cpp:83]Stop future task ExchangeKeyLooper
[2025-08-27 16:13:19.732] [info] [2614-2614]|[auth/yt_auth_manager.cpp:174]NotifyActivationSucceed
[2025-08-27 16:13:19.733] [info] [2868-2614]|[utils/yt_future_task.cpp:55]handleThreadStarted ExchangeKeyLooper in thread 0xabce38
[2025-08-27 16:13:31.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:13:47.627] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:14:03.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:14:19.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:14:35.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:14:51.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:15:07.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:15:23.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:15:39.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:15:55.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:16:11.614] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:16:27.624] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:16:43.617] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:16:59.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:17:15.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:17:31.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:17:47.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:18:03.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:18:10.239] [info] [2673-2614]|[palm/yt_palm_service.cpp:1114][Palmmgr]2025-08-27 16:18:10.238|INFO|[angstrong/ai_module.cpp:261]switch device status:s0 hibernate success, cost: 3ms
[2025-08-27 16:18:10.239] [info] [2676-2614]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Unknown to S0Hibernate
[2025-08-27 16:18:10.241] [info] [2620-2614]|[palm/yt_palm_service.cpp:566]_HandlePalmDeviceStatusmodule status changed from Unknown to S0Hibernate
[2025-08-27 16:18:10.241] [info] [2620-2614]|[palm/yt_palm_service.cpp:576]module enter hibernate, need close screen? should keep screen on is 1
[2025-08-27 16:18:19.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:18:35.618] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:18:51.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:19:07.621] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:19:23.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:19:39.614] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:19:55.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:20:11.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:20:27.617] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:20:43.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:20:59.624] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:21:15.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:21:31.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:21:47.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:22:03.623] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:22:19.628] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:22:35.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:22:51.624] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1
[2025-08-27 16:23:07.625] [info] [2672-2614]|[shengsida/yt_ssd_device_hal_impl.cpp:587]feed watchdog succeed 1


## 二、核心设计思想

### 1. 生产者-消费者模式

### 2. RAII资源管理


### 3. 双队列优先级设计




### 4. 

### 5. 线程生命周期管理



### 6. 锁粒度优化

### 7. 可扩展性设计


## 三、性能与安全平衡

1. 
2. 
3. **资源清理**

## 四、设计模式应用

1. **对象池模式**：线程作为可重用资源
2. **工厂方法**：通过submit方法创建任务
3. **观察者模式**：条件变量实现线程等待/通知
4. **策略模式**：任务调度策略可扩展

## 五、潜在改进方向

1. **动态线程调整**：根据负载自动增减线程
2. **任务窃取机制**：提高负载均衡
3. **任务超时处理**：防止任务长时间阻塞
4. **任务依赖管理**：支持有依赖关系的任务
5. **线程本地队列**：减少锁竞争

## 总结

这个线程池设计体现了现代C++并发编程的最佳实践：
- **资源管理**：RAII原则确保资源安全
- **任务调度**：双队列实现优先级控制
- **线程安全**：精心设计的锁策略
- **接口设计**：简洁易用的任务提交接口
- **扩展性**：提供必要的扩展点

通过这种设计，线程池能够在保证线程安全的前提下，高效地处理大量并发任务，同时提供灵活的任务优先级控制和结果获取机制，是构建高性能并发系统的核心组件。

