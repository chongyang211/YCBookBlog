Last login: Tue Aug  5 17:10:42 on ttys007

The default interactive shell is now zsh.
To update your account to use zsh, please run `chsh -s /bin/zsh`.
For more details, please visit https://support.apple.com/kb/HT208050.
CHONGYYANG-MB1:~ yangchong$ adb push /Users/yangchong/yt-palm /oem/yt-palm/
/Users/yangchong/yt-palm: 1 file pushe...ed. 2.1 MB/s (8279988 bytes in 3.804s)
CHONGYYANG-MB1:~ yangchong$ adb shell
[root@RV1126_RV1109:/]# /oem/yt-palm/run.sh
/bin/bash: warning: setlocale: LC_ALL: cannot change locale (zh_CN.utf8)
LD_LIBRARY_PATH=/oem/yt-palm/libs:
qt.qpa.wayland: qtvirtualkeyboard currently is not supported at client-side, use QT_IM_MODULE=qtvirtualkeyboard at compositor-side.
QStandardPaths: wrong permissions on runtime directory /var/run, 7755 instead of 7700
[I] <MMKV.cpp:172::initialize> version v2.1.0, page size 4096, arch armeabi-v7a/NEON (hard-float)
[I] <MMKV.cpp:229::initializeMMKV> root dir: /oem/yt-palm/preference
[I] <MMKV.cpp:259::mmkvWithID> prepare to load mmkv.default (id 731bea0aecca1fdc1986674f2bb78e64) from rootPath /oem/yt-palm/preference
[I] <MemoryFile.cpp:103::open> open fd[8], flag 0x7, /oem/yt-palm/preference/mmkv.default
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6f40000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default]
[I] <MemoryFile.cpp:103::open> open fd[9], flag 0x7, /oem/yt-palm/preference/mmkv.default.crc
[I] <MemoryFile.cpp:217::mmap> mmap to address [0xa6f3f000], oldPtr [(nil)], [/oem/yt-palm/preference/mmkv.default.crc]
[I] <MMKV_IO.cpp:244::loadMetaInfoAndCheck> meta file [mmkv.default] has flag [0]
[I] <MMKV_IO.cpp:84::loadFromFile> loading [mmkv.default] with 2166 actual size, file size 4096, InterProcess 0, meta info version:4
[I] <MMKV_IO.cpp:89::loadFromFile> loading [mmkv.default] with crc 3576124931 sequence 2 version 4
[I] <MMKV_IO.cpp:133::loadFromFile> loaded [mmkv.default] with 23 key-values
[2025-08-05 17:15:06.542] [info] [627-627]|[src/main.cpp:69]Application started with log level: 2
[2025-08-05 17:15:06.560] [info] [627-627]|[register/yt_palm_register.cpp:42]switch palm register mode to: StdQRCodeRegister
[2025-08-05 17:15:06.626] [info] [627-627]|[xike/yt_xike_device_hal_impl.cpp:172]GetWifiState wifiState : 4
[2025-08-05 17:15:06.640] [info] [627-627]|[xike/yt_xike_device_hal_impl.cpp:186]IsEthernetConnected isConnected : 0
[2025-08-05 17:15:06.640] [info] [627-627]|[hal/yt_device_hal_manager.cpp:156]RefreshNetworkStatus network connected changed: 1 network connected: 1 network type: 0 wifi state: CONNECTED ethernet connected: 0
[2025-08-05 17:15:06.640] [info] [627-627]|[xike/yt_xike_device_hal_impl.cpp:204]StartObserveNetworkStatus
[2025-08-05 17:15:06.642] [info] [627-627]|[xike/yt_xike_device_hal_impl.cpp:230]Network status monitoring started
[2025-08-05 17:15:06.643] [info] [627-627]|[update/yt_update_manager.cpp:65]Detect Xike device to config OTA mode :1
[2025-08-05 17:15:06.643] [warning] [627-627]|[access/yt_access_manager.cpp:187]YtAccessManager Open door not supported
[2025-08-05 17:15:06.643] [error] [627-627]|[access/yt_access_manager.cpp:21]Failed to initialize strategy
[2025-08-05 17:15:06.643] [info] [627-627]|[access/yt_access_manager.cpp:23]YtAccessManager created
[2025-08-05 17:15:06.644] [info] [627-627]|[audio/yt_audio_manager.cpp:29]AudioManager constructor called
[2025-08-05 17:15:06.644] [info] [627-627]|[audio/yt_audio_manager.cpp:359]Initializing audio priorities
[2025-08-05 17:15:06.644] [info] [627-627]|[audio/yt_audio_manager.cpp:121]Initializing audio resources
[2025-08-05 17:15:06.644] [info] [627-627]|[audio/yt_audio_manager.cpp:122]Audio resources initialized
[2025-08-05 17:15:06.645] [info] [627-627]|[audio/yt_audio_manager.cpp:48]Created new AudioManager instance
[2025-08-05 17:15:06.645] [info] [627-627]|[audio/yt_audio_manager.cpp:69]Initializing audio manager
[2025-08-05 17:15:06.728] [info] [627-627]|[xike/yt_xike_device_hal_impl.cpp:106]GetCurrentVolume volume : 41
[2025-08-05 17:15:06.728] [info] [627-627]|[audio/yt_audio_manager.cpp:88]Synced audio volume with system: 41
[2025-08-05 17:15:06.728] [info] [627-627]|[audio/yt_audio_manager.cpp:95]Audio manager initialized successfully
[2025-08-05 17:15:06.729] [info] [627-627]|[src/main.cpp:171]Detect Xike device to config app:1
qrc:/qml/main.qml:203: TypeError: Cannot call method 'info' of undefined
[2025-08-05 17:15:06.988] [warning] [Qt-Warning] qrc:/qml/main.qml:203: TypeError: Cannot call method 'info' of undefined (qrc:/qml/main.qml:203)
qt.qpa.wayland: No shell integration named "xdg-shell" found
[2025-08-05 17:15:07.045] [warning] [Qt-Warning] No shell integration named "xdg-shell" found
qml: LogService singleton initialized
qml: YTLoggerImpl可用，发送测试日志
[2025-08-05 17:15:07.094] [info] [QML] LogService初始化成功
qml: 已发送测试信号
[2025-08-05 17:15:07.096] [info] [QML] Application started
[2025-08-05 17:15:07.097] [info] [QML] Log service available: true
[2025-08-05 17:15:07.099] [info] [QML] Keyboard height configured: 480
[2025-08-05 17:15:07.103] [info] [QML] WifiStatusIndicator initialized
[2025-08-05 17:15:07.113] [info] [627-627]|[xike/yt_xike_device_hal_impl.cpp:345]StartWatchdog feed_divide : 30 seconds
[2025-08-05 17:15:07.113] [info] [627-627]|[app/yt_app_watchdog_looper.cpp:36]Watchdog interval calculation - system timeout: 30s, feed interval: 10s, actual interval: 20s
[2025-08-05 17:15:07.113] [info] [635-627]|[xike/yt_xike_device_hal_impl.cpp:75]GetSerialNumber sn : CK0A3KP02526000185
[2025-08-05 17:15:07.113] [info] [627-627]|[app/yt_app_watchdog_looper.cpp:36]Watchdog interval calculation - system timeout: 30s, feed interval: 10s, actual interval: 20s
[2025-08-05 17:15:07.113] [info] [627-627]|[app/yt_app_watchdog_looper.cpp:12]WatchdogLooper initialized with interval: 20000ms
[2025-08-05 17:15:07.113] [info] [627-627]|[utils/yt_future_task.cpp:22]Starting future task name WatchdogLooper, interval ms 20000, single_shot 0, auto_stop 1
[2025-08-05 17:15:07.113] [info] [635-627]|[xike/yt_xike_device_hal_impl.cpp:79]GetSerialNumber ota version : 110
[2025-08-05 17:15:07.113] [info] [627-627]|[utils/yt_future_task.cpp:83]Stop future task WatchdogLooper
[2025-08-05 17:15:07.113] [info] [635-627]|[palm/yt_palm_service.cpp:440]InitPalm start app version: v1.2.0.2025080508-7fe91a6, SN : CK0A3KP02526000185
[2025-08-05 17:15:07.114] [info] [635-627]|[palm/yt_palm_service.cpp:991]PalmManager log level set to 2
[2025-08-05 17:15:07.117] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385307115225|INFO|[src/yt_palm_manager_impl.cpp:68]init palm manager
[2025-08-05 17:15:07.117] [info] [650-627]|[utils/yt_future_task.cpp:55]handleThreadStarted WatchdogLooper in thread 0x9357f8
[2025-08-05 17:15:07.120] [info] [650-627]|[xike/yt_xike_device_hal_impl.cpp:367]FeedWatchdog ret : 0
2025-08-05 17:15:09.122 [info]  [ ANG_SDK ] attached
2025-08-05 17:15:16.981 [info]  [ ANG_SDK ] open camera successful
2025-08-05 17:15:16.982 [info]  [ ANG_SDK ] start the device listener
[2025-08-05 17:15:16.983] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385316982824|INFO|[src/yt_palm_manager_impl.cpp:90]module connected
[2025-08-05 17:15:16.985] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385316983164|INFO|[src/yt_palm_manager_impl.cpp:112]open camera ret:0
[2025-08-05 17:15:17.050] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317050010|INFO|[src/yt_palm_manager_impl.cpp:1578]sync time: 2025-08-05 09:15:16 ret:0
[2025-08-05 17:15:17.055] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317054449|INFO|[src/yt_palm_manager_impl.cpp:635]switch device status:working2 success, cost: 4ms
[2025-08-05 17:15:17.055] [info] [653-627]|[palm/yt_palm_service.cpp:508]HandlePalmDeviceStatusmodule status changed from Unknown to Working2
[2025-08-05 17:15:17.057] [info] [636-627]|[palm/yt_palm_service.cpp:515]_HandlePalmDeviceStatusmodule status changed from Unknown to Working2
[2025-08-05 17:15:17.116] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317116601|INFO|[src/yt_palm_manager_impl.cpp:410]=== Module SN: YTA8000F20000047
[2025-08-05 17:15:17.119] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317116936|INFO|[src/yt_palm_manager_impl.cpp:412]=== PalmManager Version: v1.1.13.2025053006-153994ad
[2025-08-05 17:15:17.119] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317118834|INFO|[src/yt_palm_manager_impl.cpp:414]=== AngstrongSDK Version: v4.9.7.20250516
[2025-08-05 17:15:17.123] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317121565|INFO|[src/yt_palm_manager_impl.cpp:417]=== Firmware Version: V02.0.6_202506242120V4.9.5.2025061921
[2025-08-05 17:15:17.138] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317137617|INFO|[src/yt_palm_manager_impl.cpp:420]=== AlgoSDK Version: v1.2.8.2025061912-60e8ea92
[2025-08-05 17:15:17.142] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385317142669|INFO|[src/yt_palm_manager_impl.cpp:422]=== Algo Version: v1.0
2025-08-05 17:15:19.572 [error]  [ ANG_SDK ] exec DHCP fail! ret 255
[2025-08-05 17:15:19.573] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319572565|INFO|[src/yt_palm_manager_impl.cpp:426]=== Module Run DHCP, ret:-1
2025-08-05 17:15:19.711 [error]  [ ANG_SDK ] Network not working!!!
[2025-08-05 17:15:19.713] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319712158|INFO|[src/yt_palm_manager_impl.cpp:428]=== verifyNetwork, ret:-1
[2025-08-05 17:15:19.713] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319712299|INFO|[src/yt_palm_manager_impl.cpp:212]switch mode: 2
[2025-08-05 17:15:19.713] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319712378|INFO|[src/yt_palm_manager_impl.cpp:321]=== setConfig solution: 3
[2025-08-05 17:15:19.714] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319712462|INFO|[src/yt_palm_manager_impl.cpp:322]=== setConfig mode: 2
[2025-08-05 17:15:19.714] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319712531|INFO|[src/yt_palm_manager_impl.cpp:323]=== setConfig register_mode: 3
[2025-08-05 17:15:19.714] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319712660|INFO|[src/yt_palm_manager_impl.cpp:324]=== setConfig recognize_mode: 4
[2025-08-05 17:15:19.726] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319726262|INFO|[src/yt_palm_manager_impl.cpp:327]=== setConfig for prefer quality: normal, ret: 0
[2025-08-05 17:15:19.749] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319749176|INFO|[src/yt_palm_manager_impl.cpp:332]=== setConfig for prefer disable_feature: 0, ret: 0
[2025-08-05 17:15:19.763] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319763212|INFO|[src/yt_palm_manager_impl.cpp:338]=== setConfig for prefer disable_retrieval: 0, ret: 0
[2025-08-05 17:15:19.797] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319796803|INFO|[src/yt_palm_manager_impl.cpp:345]=== setConfig for prefer retrieval_level: normal, ret: 0
[2025-08-05 17:15:19.811] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319811122|INFO|[src/yt_palm_manager_impl.cpp:353]=== setConfig for prefer disable_preferred_image: 0, ret: 0
[2025-08-05 17:15:19.833] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319833318|INFO|[src/yt_palm_manager_impl.cpp:361]=== setConfig for recognize disable_preferred_image: 0, ret: 0
[2025-08-05 17:15:19.852] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319852058|INFO|[src/yt_palm_manager_impl.cpp:370]=== setConfig for recognize export_image_after_retrieval_empty: 1, ret: 0
[2025-08-05 17:15:19.880] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319879742|INFO|[src/yt_palm_manager_impl.cpp:445]=== setConfig for report image: 0, ret: 0
[2025-08-05 17:15:19.886] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319885514|INFO|[src/yt_palm_manager_impl.cpp:456]=== setConfig for enable session: 1, ret: 0
[2025-08-05 17:15:19.914] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319913970|INFO|[src/yt_palm_manager_impl.cpp:462]=== setConfig for report threshold: 1, ret: 0
2025-08-05 17:15:19.921 [info]  [ ANG_SDK ] setEventCallback PREFER
[2025-08-05 17:15:19.921] [info2025-08-05 17:15:19.922 [] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319921632|INFO|[src/yt_palm_manager_impl.cpp:468]=== setConfig for report score: 1, ret: 0
info]  [ ANG_SDK ] setEventCallback RECOGNIZEPRO
[2025-08-05 17:15:19.922] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319921963|INFO|[src/yt_palm_manager_impl.cpp:479]=== setConfig for prefer event callback, ret: 0
[2025-08-05 17:15:19.923] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319922639|INFO|[src/yt_palm_manager_impl.cpp:490]=== setConfig for recognize event callback, ret: 0
[2025-08-05 17:15:19.965] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319965316|INFO|[src/yt_palm_manager_impl.cpp:503]=== setConfig for int8 feature: 1
[2025-08-05 17:15:19.965] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319965493|INFO|[src/yt_palm_manager_impl.cpp:509]=== setConfig for local metrics: 10
[2025-08-05 17:15:19.967] [warning] [651-627]|[palm/yt_palm_service.cpp:1004][Palmmgr]1754385319965808|WARN|[src/yt_local_metric_mgr.cpp:173]LocalMetricMgr::LoadMetrics not exist: /userdata/palm_metrics/v1.2.8.2025061912-60e8ea92/metrics.json
[2025-08-05 17:15:19.967] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385319966919|INFO|[src/yt_palm_manager_impl.cpp:138]Test device status...
2025-08-05 17:15:19.968 [debug]  [ ANG_SDK ] go to get psensor value
2025-08-05 17:15:20.273 [debug]  [ ANG_SDK ] go to get psensor value finish
[2025-08-05 17:15:20.323] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385320323477|INFO|[src/yt_palm_manager_impl.cpp:1578]sync time: 2025-08-05 09:15:20 ret:0
[2025-08-05 17:15:20.328] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385320328105|INFO|[src/yt_palm_manager_impl.cpp:635]switch device status:working success, cost: 4ms
[2025-08-05 17:15:20.328] [info] [653-627]|[palm/yt_palm_service.cpp:508]HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-08-05 17:15:20.329] [info] [636-627]|[palm/yt_palm_service.cpp:515]_HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-08-05 17:15:20.645] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385320644843|INFO|[src/yt_palm_manager_impl.cpp:1984]stop all with reason:1
[2025-08-05 17:15:20.668] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385320667650|INFO|[src/yt_palm_manager_impl.cpp:2015]stop all success, cost:22ms
[2025-08-05 17:15:20.670] [warning] [651-627]|[palm/yt_palm_service.cpp:1004][Palmmgr]1754385320667876|WARN|[src/yt_palm_manager_impl_verify.cpp:36]ignore ret 1100
[2025-08-05 17:15:20.671] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385320670546|INFO|[src/yt_palm_manager_impl.cpp:140]Test device status finish, ret: 0
[2025-08-05 17:15:20.671] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385320670712|INFO|[src/yt_palm_manager_impl.cpp:146]init finish, ret: 0 cost: 13553ms open:9866ms
[2025-08-05 17:15:20.692] [info] [635-627]|[palm/yt_palm_service.cpp:476]Init PalmManager ret  0 palmManagerVersion: v1.1.13.2025053006-153994ad, firmwareVersion: V02.0.6_202506242120V4.9.5.2025061921, algoSdkVersion: v1.2.8.2025061912-60e8ea92, algoVersion: v1.0, palmMode: 2
[2025-08-05 17:15:20.703] [info] [635-627]|[palm/yt_palm_service.cpp:500]PalmManager currentPalmCount: 0
[2025-08-05 17:15:20.703] [info] [635-627]|[palm/yt_palm_context.h:539][Success] Module return code: 0
[2025-08-05 17:15:20.703] [info] [635-627]|[palm/yt_palm_context.h:551]NotifyPalmAuthStatusChangedfrom 0 to 1
[2025-08-05 17:15:20.703] [info] [627-627]|[palm/yt_palm_context.h:149]DoOnPalmAuthorizeSucceed
[2025-08-05 17:15:20.704] [info] [QML] onPalmAuthorizeSucceed
[2025-08-05 17:15:20.717] [info] [635-627]|[http/yt_qt_http_client.cpp:57]SSL supported, version: OpenSSL 1.1.1h  22 Sep 2020
[2025-08-05 17:15:20.869] [info] [635-627]|[http/yt_qt_http_client.cpp:291]SSL error ignore policy set to: false
[2025-08-05 17:15:20.869] [info] [635-627]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-05 17:15:20.869] [warning] [635-627]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-08-05 17:15:22.861: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-08-05 17:15:22.861: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-08-05 17:15:22.861: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-08-05 17:15:22.863] [info] [635-627]|[storage/yt_database_manager.cpp:101]Database opened with new key directly
[2025-08-05 17:15:24.857] [info] [635-627]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-08-05 17:15:24.857] [info] [635-627]|[storage/yt_database_manager.cpp:129]CheckVersion current :2 config :2
[2025-08-05 17:15:24.858] [info] [635-627]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-05 17:15:24.858] [error] [635-627]|[storage/yt_database_manager.cpp:33]duplicate init in Initialize
[2025-08-05 17:15:24.867] [info] [635-627]|[devicectl/yt_devicectl_manager.cpp:135]SyncPalmsFromDB begin
[2025-08-05 17:15:24.868] [info] [635-627]|[storage/yt_palm_db_manager.cpp:141]QueryAllPalms page size  1000
[2025-08-05 17:15:24.868] [info] [635-627]|[storage/yt_database_manager.cpp:181]QueryCount count:0 ret:161
[2025-08-05 17:15:24.868] [error] [635-627]|[storage/yt_palm_db_manager.cpp:147]QueryAllPalms QueryCount failed
[2025-08-05 17:15:24.869] [info] [635-627]|[devicectl/yt_devicectl_manager.cpp:154]SyncPalmsFromDB total : 0 done : 0, to sync : 0, invalid count : 0
[2025-08-05 17:15:24.869] [info] [635-627]|[devicectl/yt_devicectl_manager.cpp:166]SyncPalmsFromDB end
[2025-08-05 17:15:24.869] [info] [635-627]|[storage/yt_database_manager.cpp:29]Initialize
[2025-08-05 17:15:24.869] [warning] [635-627]|[utils/yt_file_util.cpp:34]Directory already exists: /oem/yt-palm/data
2025-08-05 17:15:26.872: ERROR CORE sqlcipher_page_cipher: hmac check failed for pgno=1
2025-08-05 17:15:26.872: ERROR CORE sqlite3Codec: error decrypting page 1 data: 1
2025-08-05 17:15:26.872: ERROR CORE sqlcipher_codec_ctx_set_error 1
[2025-08-05 17:15:26.874] [info] [635-627]|[storage/yt_database_manager.cpp:101]Database opened with new key directly
[2025-08-05 17:15:27.424] [info] [650-627]|[xike/yt_xike_device_hal_impl.cpp:367]FeedWatchdog ret : 0
[2025-08-05 17:15:28.862] [info] [635-627]|[utils/yt_sqlite_util.cpp:48]CheckIntegrity success
[2025-08-05 17:15:28.862] [info] [635-627]|[storage/yt_database_manager.cpp:129]CheckVersion current :1 config :1
[2025-08-05 17:15:28.863] [info] [635-627]|[palm/yt_palm_service.cpp:395]Network status changed: 1, has_network: 1
[2025-08-05 17:15:28.863] [info] [635-627]|[palm/yt_palm_service.cpp:400]SyncTime ret: 0
[2025-08-05 17:15:28.863] [info] [635-627]|[card/yt_card_manager.cpp:34]Init CardManager: unsupported vendorType 1
[2025-08-05 17:15:28.863] [info] [635-627]|[card/yt_card_manager.cpp:77]SetCardNoCallback SetCardNoCallback
[2025-08-05 17:15:28.863] [info] [635-627]|[palm/yt_palm_service.cpp:1141]Old App Version: v1.2.0.2025080508-7fe91a6, Solution: 2
[2025-08-05 17:15:28.864] [info] [635-627]|[palm/yt_palm_service.cpp:1154]Updated App Version: v1.2.0.2025080508-7fe91a6, Solution: 2
[2025-08-05 17:15:28.864] [info] [635-627]|[palm/yt_palm_service.cpp:1202]NotifyModuleReadyStatus ready: 1, current: 0
[2025-08-05 17:15:28.870] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385328870199|INFO|[src/yt_palm_manager_impl_ota.cpp:67]sdk_version:v1.2.8.2025061912-60e8ea92 firm_version:V02.0.6_202506242120V4.9.5.2025061921
[2025-08-05 17:15:28.870] [info] [635-627]|[palm/yt_palm_service.cpp:1219]module ready, ParseOTAVersion ret: 0, firmwareVersion: 02.0.6.202506242120, algo version: v1.2.8, algo build: 2025061912
[2025-08-05 17:15:28.870] [error] [635-627]|[palm/yt_palm_service.cpp:1079]not init in CheckAndUpdateAlgoVersion
[2025-08-05 17:15:28.871] [info] [635-627]|[palm/yt_palm_service.cpp:186]EnablePalmWorking current working 0
[2025-08-05 17:15:28.871] [error] [635-627]|[palm/yt_palm_service.cpp:188]not init in EnablePalmWorking
[2025-08-05 17:15:28.871] [info] [635-627]|[palm/yt_palm_service.cpp:417]PalmService init succeed
[2025-08-05 17:15:28.871] [info] [627-627]|[palm/yt_palm_context.h:125]DoOnServiceInitSucceed
[2025-08-05 17:15:28.872] [info] [QML] Palm service initialized successfully
[2025-08-05 17:15:28.873] [info] [QML] Palm service initialized successfully
[2025-08-05 17:15:28.873] [info] [627-627]|[fsm/yt_palm_state_machine.h:115]UnInit react InitSucceedEvent
[2025-08-05 17:15:28.873] [info] [627-627]|[fsm/yt_palm_state_machine.h:144]NeedPalmAuth entryGuard, isPalmAuthorized 1
[2025-08-05 17:15:28.873] [info] [627-627]|[fsm/yt_palm_state_machine.h:146]NeedPalmAuth entryGuard, already authorized, change to next status
[2025-08-05 17:15:28.874] [info] [627-627]|[fsm/yt_palm_state_machine.h:178]NeedIoTAuth enter
[2025-08-05 17:15:28.874] [info] [627-627]|[auth/yt_auth_manager.cpp:66]StartCheckActivated Local key_id: 0f00636f-bb1d-45fe-a438-5000c1526725
[2025-08-05 17:15:28.874] [info] [627-627]|[auth/yt_auth_manager.cpp:69]Device has been activated
[2025-08-05 17:15:28.874] [info] [627-627]|[utils/yt_future_task.cpp:22]Starting future task name ExchangeKeyLooper, interval ms 600000, single_shot 0, auto_stop 1
[2025-08-05 17:15:28.874] [info] [627-627]|[utils/yt_future_task.cpp:83]Stop future task ExchangeKeyLooper
[2025-08-05 17:15:28.875] [info] [627-627]|[auth/yt_auth_manager.cpp:174]NotifyActivationSucceed
[2025-08-05 17:15:28.875] [info] [627-627]|[devicectl/yt_devicectl_manager.cpp:84]AuthManager activation succeed callback
[2025-08-05 17:15:28.875] [info] [750-627]|[utils/yt_future_task.cpp:55]handleThreadStarted ExchangeKeyLooper in thread 0xa51780
[2025-08-05 17:15:28.877] [info] [627-627]|[palm/yt_palm_context.h:211]DoOnIoTRegistrationSucceed
[2025-08-05 17:15:28.878] [info] [QML] onIoTRegistrationSucceed
[2025-08-05 17:15:28.879] [info] [627-627]|[fsm/yt_palm_state_machine.h:184]NeedIoTAuth react IoTAuthSucceed
[2025-08-05 17:15:28.879] [info] [627-627]|[fsm/yt_palm_state_machine.h:267]Idle entryGuard, isPalmWorking 0
[2025-08-05 17:15:28.880] [info] [627-627]|[palm/yt_palm_service.cpp:113]CheckAndSetEnable current enable 0
[2025-08-05 17:15:28.885] [info] [627-627]|[palm/yt_palm_service.cpp:1089][Service enable]CheckAndUpdateAlgoVersion current : v1.0, stored : v1.0
[2025-08-05 17:15:28.885] [info] [627-627]|[utils/yt_future_task.cpp:22]Starting future task name SyncPalmLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-08-05 17:15:28.885] [info] [627-627]|[utils/yt_future_task.cpp:83]Stop future task SyncPalmLooper
[2025-08-05 17:15:28.885] [info] [627-627]|[utils/yt_future_task.cpp:22]Starting future task name SyncUserLooper, interval ms 30000, single_shot 0, auto_stop 1
[2025-08-05 17:15:28.885] [info] [627-627]|[utils/yt_future_task.cpp:83]Stop future task SyncUserLooper
[2025-08-05 17:15:28.886] [info] [627-627]|[utils/yt_future_task.cpp:22]Starting future task name StdHeartbeat, interval ms 60000, single_shot 0, auto_stop 1
[2025-08-05 17:15:28.886] [info] [627-627]|[utils/yt_future_task.cpp:83]Stop future task StdHeartbeat
[2025-08-05 17:15:28.886] [info] [751-627]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncPalmLooper in thread 0xa68940
[2025-08-05 17:15:28.886] [info] [627-627]|[fsm/yt_palm_state_machine.h:204]Working enter, set service enable ret 0
[2025-08-05 17:15:28.887] [info] [752-627]|[utils/yt_future_task.cpp:55]handleThreadStarted SyncUserLooper in thread 0xa70568
[2025-08-05 17:15:28.888] [info] [753-627]|[utils/yt_future_task.cpp:55]handleThreadStarted StdHeartbeat in thread 0xa6f678
[2025-08-05 17:15:28.888] [info] [QML] guardNavigateHome navigate to home
[2025-08-05 17:15:28.889] [info] [QML] StackView page changed: null
[2025-08-05 17:15:28.889] [info] [753-627]|[pass/yt_pass_config_manager.cpp:516]CheckAndUploadPassRecords No unuploaded pass records
[2025-08-05 17:15:28.890] [info] [QML] Page type: null
[2025-08-05 17:15:28.890] [info] [753-627]|[heartbeat/yt_heartbeat_manager.cpp:38]Init local heartbeat status from persistence: [{"desired_state": "{\"ocode\":\"1746854527\"}", "state_version": 3, "identifier": "device_bind_ocode_info"}]
qrc:/qml/main.qml:205: TypeError: Cannot read property 'statusBarColor' of null
[2025-08-05 17:15:28.890] [warning] [Qt-Warning] qrc:/qml/main.qml:205: TypeError: Cannot read property 'statusBarColor' of null (qrc:/qml/main.qml:205)
[2025-08-05 17:15:28.900] [info] [754-627]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 696275433759717339416c697a487644, reqId: 1754385328887-1
QQmlComponent: Component is not ready
[2025-08-05 17:15:28.905] [warning] [Qt-Warning] QQmlComponent: Component is not ready
qrc:/qml/main.qml:182:5: QML StackView: push: qrc:/qml/pages/Home.qml:852 Expected token `,'
[2025-08-05 17:15:28.906] [warning] [Qt-Warning] qrc:/qml/main.qml:182:5: QML StackView: push: qrc:/qml/pages/Home.qml:852 Expected token `,' (qrc:/qml/main.qml:182)
[2025-08-05 17:15:28.906] [info] [755-627]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/heartbeat, traceId: 4b3955575858714a6c33563174714f59, reqId: 1754385328890-2
[2025-08-05 17:15:28.913] [info] [756-627]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 74667a4d374352734958333242464a4a, reqId: 1754385328886-0
[2025-08-05 17:15:29.636] [info] [718-627]|[http/yt_qt_http_client.h:46]onResponse req_id 1754385328890-2, tag : /wecardpalm/device/heartbeat
[2025-08-05 17:15:29.637] [info] [718-627]|[heartbeat/yt_heartbeat_manager.cpp:73]StartHeartbeat Response: 0
[2025-08-05 17:15:29.637] [info] [718-627]|[heartbeat/yt_heartbeat_manager.cpp:100]HandleNewDesiredState size = 0
[2025-08-05 17:15:29.637] [info] [718-627]|[devicectl/yt_devicectl_manager.cpp:548]OnHeartbeatSucceed
[2025-08-05 17:15:29.638] [info] [718-627]|[http/yt_qt_http_client.h:46]onResponse req_id 1754385328887-1, tag : /wecardpalm/device/incr-sync-user
[2025-08-05 17:15:29.639] [info] [718-627]|[looper/yt_sync_user_looper.cpp:76]Sync user succeed: 0
[2025-08-05 17:15:30.701] [info] [653-627]|[palm/yt_palm_service.cpp:508]HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-08-05 17:15:30.701] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385330700850|INFO|[src/yt_palm_manager_impl.cpp:635]switch device status:working2 success, cost: 3ms
[2025-08-05 17:15:30.705] [info] [636-627]|[palm/yt_palm_service.cpp:515]_HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-08-05 17:15:30.762] [info] [651-627]|[palm/yt_palm_service.cpp:1008][Palmmgr]1754385330762612|INFO|[src/yt_palm_manager_impl.cpp:1578]sync time: 2025-08-05 09:15:30 ret:0
[2025-08-05 17:15:31.636] [info] [718-627]|[http/yt_qt_http_client.h:46]onResponse req_id 1754385328886-0, tag : /wecardpalm/device/synchronize_palm_feature
[2025-08-05 17:15:31.660] [info] [718-627]|[looper/yt_sync_palm_looper.cpp:174]SyncPalmLooper RequestSyncPalmStandard response: hasMore: 2, query_position: eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MjIsInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NTQzODUzMjl9LHsibmV4dF92ZXJzaW9uIjoyNSwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc1NDM4NTMyOX1dfQ==, datas size: 20
[2025-08-05 17:15:31.660] [info] [718-627]|[devicectl/yt_devicectl_manager.cpp:194]UpdatePalms entities.size: 20
[2025-08-05 17:15:31.661] [info] [718-627]|[storage/yt_palm_db_manager.cpp:205]InsertOrUpdatePalms sql: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?);
[2025-08-05 17:15:31.661] [error] [718-627]|[storage/yt_database_manager.h:105]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-08-05 17:15:31.662] [error] [718-627]|[storage/yt_palm_db_manager.cpp:227]InsertOrUpdatePalms Failed to insert/update palms, error code: 161
[2025-08-05 17:15:31.662] [error] [718-627]|[devicectl/yt_devicectl_manager.cpp:217]update palms to db ret :161 in UpdatePalms
[2025-08-05 17:15:31.662] [info] [718-627]|[looper/yt_sync_palm_looper.cpp:183]SyncPalmLooper RequestSyncPalmStandard -> RemoteDeviceCtlManager UpdatePalms ret: 161
[2025-08-05 17:15:47.427] [info] [650-627]|[xike/yt_xike_device_hal_impl.cpp:367]FeedWatchdog ret : 0
[2025-08-05 17:15:58.414] [info] [754-627]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 7a76657939634363516c495977533649, reqId: 1754385358402-3
[2025-08-05 17:15:58.461] [info] [755-627]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 39523857776f497973386b53736f3069, reqId: 1754385358430-4
[2025-08-05 17:15:58.744] [info] [718-627]|[http/yt_qt_http_client.h:46]onResponse req_id 1754385358402-3, tag : /wecardpalm/device/incr-sync-user
[2025-08-05 17:15:58.744] [info] [718-627]|[looper/yt_sync_user_looper.cpp:76]Sync user succeed: 0
[2025-08-05 17:16:01.124] [info] [718-627]|[http/yt_qt_http_client.h:46]onResponse req_id 1754385358430-4, tag : /wecardpalm/device/synchronize_palm_feature
[2025-08-05 17:16:01.147] [info] [718-627]|[looper/yt_sync_palm_looper.cpp:174]SyncPalmLooper RequestSyncPalmStandard response: hasMore: 2, query_position: eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MjIsInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NTQzODUzNTh9LHsibmV4dF92ZXJzaW9uIjoyNSwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc1NDM4NTM1OH1dfQ==, datas size: 20
[2025-08-05 17:16:01.147] [info] [718-627]|[devicectl/yt_devicectl_manager.cpp:194]UpdatePalms entities.size: 20
[2025-08-05 17:16:01.148] [info] [718-627]|[storage/yt_palm_db_manager.cpp:205]InsertOrUpdatePalms sql: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?);
[2025-08-05 17:16:01.148] [error] [718-627]|[storage/yt_database_manager.h:105]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-08-05 17:16:01.149] [error] [718-627]|[storage/yt_palm_db_manager.cpp:227]InsertOrUpdatePalms Failed to insert/update palms, error code: 161
[2025-08-05 17:16:01.149] [error] [718-627]|[devicectl/yt_devicectl_manager.cpp:217]update palms to db ret :161 in UpdatePalms
[2025-08-05 17:16:01.149] [info] [718-627]|[looper/yt_sync_palm_looper.cpp:183]SyncPalmLooper RequestSyncPalmStandard -> RemoteDeviceCtlManager UpdatePalms ret: 161

