[2026-03-05 17:59:13.052] [iot] [info] [3989,3995] [action_code_processor.cpp:119,ParseRawData] ActionCodeProcessor: parsed action_code=10001, sign_type=1, valid_type=1
[2026-03-05 17:59:13.052] [iot] [info] [3989,3995] [action_code_processor.cpp:270,ExecuteAction] ActionCodeProcessor: execute action_code=10001, params_count=0
[2026-03-05 17:59:13.052] [iot] [info] [3989,3995] [device_operation_manager.cpp:432,ExecuteViewDeviceInfo] DeviceOperationManager: execute view device info
[2026-03-05 17:59:13.052] [iot] [info] [3989,3995] [base_ui.cpp:23,Init] BaseUi::Init - start
[2026-03-05 17:59:13.053] [iot] [info] [3989,3995] [display_service.cpp:94,Init] read screen info width: 240 height: 280
[2026-03-05 17:59:13.054] [iot] [info] [3989,3995] [display_service.cpp:28,operator()] [Warn]	(2571.091, +2571091)	 lv_init: Style sanity checks are enabled that uses more RAM 	(in lv_obj.c line #181)

[2026-03-05 17:59:13.059] [iot] [info] [3989,3995] [operation_ui.cpp:20,CreateScreen] OperationUi::CreateScreen
[2026-03-05 17:59:13.060] [iot] [info] [3989,3995] [base_ui.cpp:42,Init] BaseUi::Init - complete
[2026-03-05 17:59:13.060] [iot] [info] [3989,3995] [base_ui.cpp:83,Show] BaseUi::Show - loading screen
[2026-03-05 17:59:13.061] [iot] [info] [3989,3995] [display_service.cpp:145,Start] DisplayService: Starting UI Looper
rga_api version 1.3.0_[11] (RGA is compiling with meson base: $PRODUCT_BASE)
[2026-03-05 17:59:13.063] [iot] [info] [3989,3995] [display_service.cpp:150,Start] DisplayService: UI Looper started
[2026-03-05 17:59:13.065] [iot] [info] [3989,3995] [operation_ui.cpp:52,ShowExecuting] OperationUi::ShowExecuting - message: Loading...
[2026-03-05 17:59:13.103] [iot] [info] [3989,3995] [system_interface.cpp:474,GetSystemTimeZone] System timezone: Asia/Shanghai
[2026-03-05 17:59:13.106] [iot] [info] [3989,3995] [device_opcode_manager.cpp:139,GetFirmwareInfo] DeviceOpcodeManager GetFirmwareInfo started: {"brightness":0,"iccid":"UNAVAILABLE","imei":"UNAVAILABLE","ip_address":"192.168.9.109","memory_info":"420M/734M","sn":"GKO201WPE220000353RT3T4","timezone":"Asia/Shanghai","volume":0}
DBus :: Received an error from message serial #29 : Service is null.
[2026-03-05 17:59:13.113] [iot] [error] [3989,4003] [dbus_client.h:158,operator()] dbus async call error:
DBus :: Error :
[2026-03-05 17:59:13.115] [iot] [warning] [3989,3989] [device_opcode_manager.cpp:156,operator()] DeviceOpcodeManager GetCellInfo FAILED: Bad message (74)
[2026-03-05 17:59:13.116] [iot] [info] [3989,3995] [device_opcode_manager.cpp:209,operator()] DeviceOpcodeManager GetVolume: 100
[2026-03-05 17:59:13.119] [iot] [info] [3989,3995] [device_opcode_manager.cpp:225,operator()] DeviceOpcodeManager GetBrightness: 80
[2026-03-05 17:59:13.122] [iot] [info] [3989,3995] [device_opcode_manager.cpp:144,operator()] DeviceOpcodeManager GetFirmwareInfo completed: {"brightness":80,"iccid":"UNAVAILABLE","imei":"UNAVAILABLE","ip_address":"192.168.9.109","memory_info":"420M/734M","sn":"GKO201WPE220000353RT3T4","timezone":"Asia/Shanghai","volume":100}
[2026-03-05 17:59:13.124] [iot] [info] [3989,3989] [operation_ui.cpp:148,ShowListView] OperationUi::ShowListView - items: 8
[2026-03-05 17:59:13.179] [iot] [info] [3989,3989] [list_view_widget.cpp:125,Show] ListViewWidget::Show - 8 items
[2026-03-05 17:59:23.125] [iot] [info] [3989,3989] [device_operation_manager.cpp:423,operator()] DeviceOperationManager: auto dismiss
[2026-03-05 17:59:23.125] [iot] [info] [3989,3989] [base_ui.cpp:96,Hide] BaseUi::Hide - hiding screen
[2026-03-05 17:59:23.126] [iot] [info] [3989,3989] [display_service.cpp:158,Stop] DisplayService: Stopping UI Looper
[2026-03-05 17:59:23.126] [iot] [info] [3989,3989] [display_service.cpp:161,Stop] DisplayService: UI Looper stopped
[2026-03-05 17:59:23.127] [iot] [info] [3989,3989] [action_code_processor.cpp:274,operator()] ActionCodeProcessor: action 10001 executed successfully

扫码重启的日志

[2026-03-05 18:02:33.430] [iot] [info] [549,555] [palm_interface.cpp:446,operator()] PalmInterface: URI code scanned, scheme=qcloud
[2026-03-05 18:02:33.432] [iot] [info] [549,555] [qrcode_operation_handler.cpp:104,OnUriCodeScanned] QRCodeOperationHandler: UriCodeScanned, uri len=373
[2026-03-05 18:02:33.432] [iot] [info] [549,555] [action_code_processor.cpp:63,ProcessActionCode] ActionCodeProcessor: process action code, len=373
[2026-03-05 18:02:33.433] [iot] [info] [549,555] [action_code_processor.cpp:119,ParseRawData] ActionCodeProcessor: parsed action_code=20001, sign_type=4, valid_type=1
[2026-03-05 18:02:33.433] [iot] [error] [549,555] [action_code_processor.cpp:356,OnError] ActionCodeProcessor: error=7201, msg=no cached common key, device needs network at least once
[2026-03-05 18:02:33.434] [iot] [info] [549,549] [base_ui.cpp:23,Init] BaseUi::Init - start
[2026-03-05 18:02:33.435] [iot] [info] [549,549] [display_service.cpp:94,Init] read screen info width: 240 height: 280
[2026-03-05 18:02:33.436] [iot] [info] [549,549] [display_service.cpp:28,operator()] [Warn]	(2771.473, +2771473)	 lv_init: Style sanity checks are enabled that uses more RAM 	(in lv_obj.c line #181)

[2026-03-05 18:02:33.440] [iot] [info] [549,549] [operation_ui.cpp:20,CreateScreen] OperationUi::CreateScreen
[2026-03-05 18:02:33.441] [iot] [info] [549,549] [base_ui.cpp:42,Init] BaseUi::Init - complete
[2026-03-05 18:02:33.441] [iot] [info] [549,549] [base_ui.cpp:83,Show] BaseUi::Show - loading screen
[2026-03-05 18:02:33.441] [iot] [info] [549,549] [display_service.cpp:145,Start] DisplayService: Starting UI Looper
[2026-03-05 18:02:33.442] [iot] [info] [549,549] [display_service.cpp:150,Start] DisplayService: UI Looper started
[2026-03-05 18:02:33.443] [iot] [info] [549,549] [operation_ui.cpp:85,ShowFailed] OperationUi::ShowFailed - message: Action Code Error, error_code: 7201
rga_api version 1.3.0_[11] (RGA is compiling with meson base: $PRODUCT_BASE)
[2026-03-05 18:02:33.448] [iot] [info] [549,549] [two_line_text_widget.cpp:63,Show] TwoLineTextWidget::Show - title: Action Code Error, tip: code: 7201


出现白板问题：

[2026-03-05 19:56:40.727] [iot] [info] [2685,2691] [qrcode_operation_handler.cpp:104,OnUriCodeScanned] QRCodeOperationHandler: UriCodeScanned, uri len=29
[2026-03-05 19:56:40.728] [iot] [info] [2685,2691] [action_code_processor.cpp:63,ProcessActionCode] ActionCodeProcessor: process action code, len=29
[2026-03-05 19:56:40.729] [iot] [info] [2685,2691] [action_code_processor.cpp:119,ParseRawData] ActionCodeProcessor: parsed action_code=10001, sign_type=1, valid_type=1
[2026-03-05 19:56:40.741] [iot] [info] [2685,2691] [action_code_processor.cpp:270,ExecuteAction] ActionCodeProcessor: execute action_code=10001, params_count=0
[2026-03-05 19:56:40.746] [iot] [info] [2685,2691] [device_operation_manager.cpp:511,ExecuteViewDeviceInfo] DeviceOperationManager: execute view device info
[2026-03-05 19:56:40.747] [iot] [info] [2685,2689] [palm_interface.cpp:206,Pause] PalmInterface::Pause - reason: action_code_operation
[2026-03-05 19:56:40.782] [iot] [info] [2685,2691] [palm_interface.cpp:222,operator()] PalmInterface::Pause success - reason: action_code_operation, result: 0
[2026-03-05 19:56:40.783] [iot] [info] [2685,2689] [device_operation_manager.cpp:250,PausePalmApp] PausePalmApp success
[2026-03-05 19:56:40.783] [iot] [info] [2685,2685] [base_ui.cpp:23,Init] BaseUi::Init
[2026-03-05 19:56:40.784] [iot] [info] [2685,2685] [display_service.cpp:97,Init] read screen info width: 240 height: 280
[2026-03-05 19:56:40.784] [iot] [info] [2685,2685] [display_service.cpp:28,operator()] [Warn]	(1159.941, +1159941)	 lv_init: Style sanity checks are enabled that uses more RAM 	(in lv_obj.c line #181)

[2026-03-05 19:56:40.790] [iot] [info] [2685,2685] [operation_ui.cpp:20,CreateScreen] OperationUi::CreateScreen
[2026-03-05 19:56:40.792] [iot] [info] [2685,2685] [display_service.cpp:152,Start] DisplayService: Starting UI Looper
[2026-03-05 19:56:40.793] [iot] [info] [2685,2685] [display_service.cpp:158,Start] DisplayService: UI Looper started
[2026-03-05 19:56:40.793] [iot] [info] [2685,2685] [operation_ui.cpp:51,ShowExecuting] OperationUi::ShowExecuting - message: Loading...
[2026-03-05 19:56:40.834] [iot] [info] [2685,2685] [system_interface.cpp:474,GetSystemTimeZone] System timezone: Asia/Shanghai
[2026-03-05 19:56:40.837] [iot] [info] [2685,2685] [device_opcode_manager.cpp:139,GetFirmwareInfo] DeviceOpcodeManager GetFirmwareInfo started: {"brightness":0,"iccid":"UNAVAILABLE","imei":"UNAVAILABLE","ip_address":"192.168.9.109","memory_info":"404M/734M","sn":"GKO201WPE220000353RT3T4","timezone":"Asia/Shanghai","volume":0}
rga_api version 1.3.0_[11] (RGA is compiling with meson base: $PRODUCT_BASE)
DBus :: Received an error from message serial #23 : Service is null.
[2026-03-05 19:56:40.845] [iot] [error] [2685,2699] [dbus_client.h:158,operator()] dbus async call error:
DBus :: Error :
[2026-03-05 19:56:40.852] [iot] [warning] [2685,2685] [device_opcode_manager.cpp:156,operator()] DeviceOpcodeManager GetCellInfo FAILED: Bad message (74)
[2026-03-05 19:56:40.853] [iot] [info] [2685,2685] [device_opcode_manager.cpp:209,operator()] DeviceOpcodeManager GetVolume: 100
[2026-03-05 19:56:40.856] [iot] [info] [2685,2685] [device_opcode_manager.cpp:225,operator()] DeviceOpcodeManager GetBrightness: 80
[2026-03-05 19:56:40.858] [iot] [info] [2685,2685] [device_opcode_manager.cpp:144,operator()] DeviceOpcodeManager GetFirmwareInfo completed: {"brightness":80,"iccid":"UNAVAILABLE","imei":"UNAVAILABLE","ip_address":"192.168.9.109","memory_info":"404M/734M","sn":"GKO201WPE220000353RT3T4","timezone":"Asia/Shanghai","volume":100}
[2026-03-05 19:56:40.860] [iot] [info] [2685,2685] [operation_ui.cpp:147,ShowListView] OperationUi::ShowListView - items: 8
[2026-03-05 19:56:40.894] [iot] [info] [2685,2685] [list_view_widget.cpp:125,Show] ListViewWidget::Show - 8 items
[2026-03-05 19:56:50.861] [iot] [info] [2685,2685] [device_operation_manager.cpp:498,operator()] DeviceOperationManager: auto dismiss
[2026-03-05 19:56:50.862] [iot] [info] [2685,2685] [base_ui.cpp:94,Hide] BaseUi::Hide - hiding screen
[2026-03-05 19:56:50.862] [iot] [info] [2685,2685] [display_service.cpp:166,Stop] DisplayService: Stopping UI Looper
[2026-03-05 19:56:50.863] [iot] [info] [2685,2685] [display_service.cpp:170,Stop] DisplayService: UI Looper stopped
[2026-03-05 19:56:50.864] [iot] [info] [2685,2685] [action_code_processor.cpp:274,operator()] ActionCodeProcessor: action 10001 executed successfully
[2026-03-05 19:56:50.865] [iot] [info] [2685,2689] [palm_interface.cpp:241,Resume] PalmInterface::Resume - reason: action_code_operation
[2026-03-05 19:56:50.903] [iot] [info] [2685,2691] [palm_interface.cpp:257,operator()] PalmInterface::Resume success - reason: action_code_operation, result: 0
[2026-03-05 19:56:50.903] [iot] [info] [2685,2689] [device_operation_manager.cpp:263,ResumePalmApp] ResumePalmApp success
