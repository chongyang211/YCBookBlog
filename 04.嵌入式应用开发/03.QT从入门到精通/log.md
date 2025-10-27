[root@RV1126_RV1109:/]# [2025-10-27 14:16:10.098] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:10.744] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.744|INFO|[angstrong/ai_module.cpp:721]get psensor:114 count:198
[2025-10-27 14:16:10.749] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.749|INFO|[angstrong/ai_module.cpp:835]will enter target status:1
[2025-10-27 14:16:10.751] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.751|INFO|[angstrong/ai_module.cpp:287]switch device status:working success, cost: 1ms
[2025-10-27 14:16:10.751] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Unknown to Working1
[2025-10-27 14:16:10.751] [info] [2434-2381]|[palm/yt_palm_service.cpp:622]handleSessionStatus session_id :  stage: 1
[2025-10-27 14:16:10.751] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.751|INFO|[angstrong/ai_module_palm_biz.cpp:219]wait psensor valid, ret:0
[2025-10-27 14:16:10.752] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.751|INFO|[angstrong/ai_module_palm_biz.cpp:244]recognize once start
[2025-10-27 14:16:10.835] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.835|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1100 detail_code:110003 msg:手掌请位于画面中心 psensor:116 is_best:0 cost:83ms
[2025-10-27 14:16:10.837] [info] [2434-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: , error_code: 1100
[2025-10-27 14:16:10.840] [info] [QML] showFrameMessage: Align your palm with camera center
[2025-10-27 14:16:10.842] [info] [QML] showFrameMessage update message: Align your palm with camera center
[2025-10-27 14:16:10.843] [info] [QML] FullscreenGifOverlay: showAndPlay called with source:
[2025-10-27 14:16:10.845] [info] [QML] FullscreenGifOverlay: Current state - visible:
[2025-10-27 14:16:10.845] [info] [QML] FullscreenGifOverlay: Showing overlay with source:
rga_api version 1.3.0_[11] (RGA is compiling with meson base: $PRODUCT_BASE)
[2025-10-27 14:16:10.910] [info] [QML] FullscreenGifOverlay: Show animation completed
[2025-10-27 14:16:10.911] [info] [QML] FullscreenGifOverlay: Starting GIF playback after show animation , source: qrc:/images/gif_palm_bg1.gifvisible:true
[2025-10-27 14:16:10.912] [info] [QML] FullscreenGifOverlay onGifOverlayShown
[2025-10-27 14:16:10.913] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:10.914] [info] [QML] FullscreenGifOverlay: About to call gifPlayer.play() - isPlaying: falsehasError:falseisLoading:false
[2025-10-27 14:16:10.914] [info] [QML] GifPlayer Debug - source: qrc:/images/gif_palm_bg1.gif
[2025-10-27 14:16:10.915] [info] [QML] GifPlayer Debug - _isPlaying: false
[2025-10-27 14:16:10.915] [info] [QML] GifPlayer Debug - _hasError: false
[2025-10-27 14:16:10.915] [info] [QML] GifPlayer Debug - _isLoading: false
[2025-10-27 14:16:10.917] [info] [QML] GifPlayer Debug - AnimatedImage.playing: false
[2025-10-27 14:16:10.918] [info] [QML] GifPlayer Debug - AnimatedImage.status: 1
[2025-10-27 14:16:10.918] [info] [QML] GifPlayer Debug - AnimatedImage.frameCount: 20
[2025-10-27 14:16:10.918] [info] [QML] GifPlayer Debug - AnimatedImage.currentFrame: 0
[2025-10-27 14:16:10.919] [info] [QML] GifPlayer Debug - Load duration: 0ms
[2025-10-27 14:16:10.931] [warning] [2431-2381]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-27 14:16:10.931|WARN|[angstrong/ai_module.cpp:915]frame callback, stop by too far: 131
[2025-10-27 14:16:10.931] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.931|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1011 detail_code:110053 msg:手掌请位于画面中心 psensor:131 is_best:0 cost:95ms
[2025-10-27 14:16:10.931] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.931|INFO|[angstrong/ai_module_palm_biz.cpp:251]recognize once over, ret:1100
[2025-10-27 14:16:10.931] [info] [2434-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: , error_code: 1011
[2025-10-27 14:16:10.931] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.931|INFO|[angstrong/ai_module_palm_biz.cpp:271]recognize end by too far, retry!!
[2025-10-27 14:16:10.931] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.931|INFO|[angstrong/ai_module.cpp:652]device_status:1 now_ms:11538941 working2_start_ms:11517659 s0_timeout_ms:300000
[2025-10-27 14:16:10.934] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.934|INFO|[angstrong/ai_module.cpp:721]get psensor:129 count:1
[2025-10-27 14:16:10.934] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.934|INFO|[angstrong/ai_module.cpp:770]enter working2 by too far
[2025-10-27 14:16:10.938] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.937|INFO|[angstrong/ai_module.cpp:287]switch device status:working2 success, cost: 3ms
[2025-10-27 14:16:10.938] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-10-27 14:16:10.960] [info] [QML] FullscreenGifOverlay: GIF play started
[2025-10-27 14:16:10.961] [info] [QML] FullscreenGifOverlay: Play result:true
[2025-10-27 14:16:10.973] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:10.972|INFO|[angstrong/ai_module.cpp:721]get psensor:126 count:1
[2025-10-27 14:16:11.000] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.001] [info] [QML] showFrameMessage update message: Move your palm closer
[2025-10-27 14:16:11.002] [info] [QML] showFrameMessage: Move your palm closer
qml: FullscreenGifOverlay: After play() call:
qml: FullscreenGifOverlay: AnimatedImage.playing: true
qml: FullscreenGifOverlay: AnimatedImage.visible: true
qml: FullscreenGifOverlay: AnimatedImage.width: 720 height: 1280
[2025-10-27 14:16:11.006] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.007] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.007|INFO|[angstrong/ai_module.cpp:721]get psensor:126 count:1
[2025-10-27 14:16:11.008] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.018] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:11.019] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:11.042] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.041|INFO|[angstrong/ai_module.cpp:721]get psensor:124 count:1
[2025-10-27 14:16:11.076] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.076|INFO|[angstrong/ai_module.cpp:721]get psensor:124 count:1
[2025-10-27 14:16:11.111] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.111|INFO|[angstrong/ai_module.cpp:721]get psensor:121 count:1
[2025-10-27 14:16:11.146] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.146|INFO|[angstrong/ai_module.cpp:721]get psensor:120 count:1
[2025-10-27 14:16:11.156] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.155|INFO|[angstrong/ai_module.cpp:835]will enter target status:1
[2025-10-27 14:16:11.158] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.158|INFO|[angstrong/ai_module.cpp:287]switch device status:working success, cost: 1ms
[2025-10-27 14:16:11.158] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-10-27 14:16:11.158] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.158|INFO|[angstrong/ai_module_palm_biz.cpp:219]wait psensor valid, ret:0
[2025-10-27 14:16:11.158] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.158|INFO|[angstrong/ai_module_palm_biz.cpp:244]recognize once start
[2025-10-27 14:16:11.172] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.183] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.184] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.256] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.256|INFO|[angstrong/ai_module.cpp:606]session started, session_id:20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:11.256] [info] [2434-2381]|[palm/yt_palm_service.cpp:622]handleSessionStatus session_id : 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c stage: 2
[2025-10-27 14:16:11.257] [info] [2404-2381]|[palm/yt_palm_service.cpp:636] session start : 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:11.357] [info] [2434-2381]|[palm/yt_palm_service.cpp:661]handleSessionFrame Previous error_code: 1011 occurred 7 times, new error_code: 1206
[2025-10-27 14:16:11.357] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.357|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1206 detail_code:120037 msg:请保持手掌静止 psensor:119 is_best:1 cost:198ms
[2025-10-27 14:16:11.434] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:11.436] [info] [QML] showFrameMessage update message: Keep your palm still
[2025-10-27 14:16:11.438] [info] [QML] showPlaySound soundType 17, lastTtsMessage: , curTtsMessage TTS1206
[2025-10-27 14:16:11.501] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.501|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1210 detail_code:120014 msg:画面过暗，请调整手掌角度 psensor:120 is_best:0 cost:144ms
[2025-10-27 14:16:11.502] [info] [2434-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, error_code: 1210
[2025-10-27 14:16:11.620] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:11.622] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:11.622] [info] [QML] showPlaySound soundType 17, lastTtsMessage: TTS1206, curTtsMessage TTS1206
[2025-10-27 14:16:11.640] [info] [2434-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, error_code: 1011
[2025-10-27 14:16:11.641] [warning] [2431-2381]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-27 14:16:11.639|WARN|[angstrong/ai_module.cpp:915]frame callback, stop by too far: 121
[2025-10-27 14:16:11.641] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.640|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1011 detail_code:120014 msg:画面过亮，请调整手掌角度 psensor:121 is_best:0 cost:137ms
[2025-10-27 14:16:11.641] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.640|INFO|[angstrong/ai_module_palm_biz.cpp:251]recognize once over, ret:1211
[2025-10-27 14:16:11.641] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.640|INFO|[angstrong/ai_module_palm_biz.cpp:271]recognize end by too far, retry!!
[2025-10-27 14:16:11.641] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.640|INFO|[angstrong/ai_module.cpp:652]device_status:1 now_ms:11539650 working2_start_ms:11538944 s0_timeout_ms:300000
[2025-10-27 14:16:11.644] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.644|INFO|[angstrong/ai_module.cpp:721]get psensor:127 count:1
[2025-10-27 14:16:11.645] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.644|INFO|[angstrong/ai_module.cpp:770]enter working2 by too far
[2025-10-27 14:16:11.647] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-10-27 14:16:11.647] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.647|INFO|[angstrong/ai_module.cpp:287]switch device status:working2 success, cost: 2ms
[2025-10-27 14:16:11.682] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.681|INFO|[angstrong/ai_module.cpp:721]get psensor:127 count:1
[2025-10-27 14:16:11.717] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.716|INFO|[angstrong/ai_module.cpp:721]get psensor:127 count:1
[2025-10-27 14:16:11.751] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.751|INFO|[angstrong/ai_module.cpp:721]get psensor:130 count:1
[2025-10-27 14:16:11.785] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.785|INFO|[angstrong/ai_module.cpp:721]get psensor:138 count:1
[2025-10-27 14:16:11.792] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.793] [info] [QML] showFrameMessage update message: Move your palm closer
[2025-10-27 14:16:11.793] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.795] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.796] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.797] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.798] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.820] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.820|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-27 14:16:11.854] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.854|INFO|[angstrong/ai_module.cpp:721]get psensor:144 count:1
[2025-10-27 14:16:11.889] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.888|INFO|[angstrong/ai_module.cpp:721]get psensor:114 count:1
[2025-10-27 14:16:11.896] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.896|INFO|[angstrong/ai_module.cpp:835]will enter target status:1
[2025-10-27 14:16:11.898] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-10-27 14:16:11.899] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.898|INFO|[angstrong/ai_module.cpp:287]switch device status:working success, cost: 1ms
[2025-10-27 14:16:11.899] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.898|INFO|[angstrong/ai_module_palm_biz.cpp:219]wait psensor valid, ret:0
[2025-10-27 14:16:11.899] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:11.898|INFO|[angstrong/ai_module_palm_biz.cpp:244]recognize once start
[2025-10-27 14:16:11.948] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:11.950] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:12.018] [info] [2434-2381]|[palm/yt_palm_service.cpp:661]handleSessionFrame Previous error_code: 1011 occurred 8 times, new error_code: 1210
[2025-10-27 14:16:12.018] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.018|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1210 detail_code:120010 msg:画面过暗，请调整手掌角度 psensor:114 is_best:0 cost:119ms
[2025-10-27 14:16:12.099] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:12.100] [info] [QML] showFrameMessage update message: Keep your palm still
[2025-10-27 14:16:12.101] [info] [QML] showPlaySound soundType 17, lastTtsMessage: TTS1206, curTtsMessage TTS1206
[2025-10-27 14:16:12.124] [info] [2434-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, error_code: 1102
[2025-10-27 14:16:12.124] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.123|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1102 detail_code:120008 msg:请保持手掌静止 psensor:114 is_best:0 cost:104ms
[2025-10-27 14:16:12.197] [info] [2434-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, error_code: 1100
[2025-10-27 14:16:12.197] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.196|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1100 detail_code:110003 msg:手掌请位于画面中心 psensor:114 is_best:0 cost:72ms
[2025-10-27 14:16:12.248] [info] [QML] showFrameMessage: Align your palm with camera center
[2025-10-27 14:16:12.250] [info] [QML] showFrameMessage update message: Align your palm with camera center
[2025-10-27 14:16:12.272] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.272|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1100 detail_code:110003 msg:手掌请位于画面中心 psensor:114 is_best:0 cost:75ms
[2025-10-27 14:16:12.354] [warning] [2431-2381]|[palm/yt_palm_service.cpp:1137][Palmmgr]2025-10-27 14:16:12.354|WARN|[angstrong/ai_module.cpp:890]frame callback, stop by invalid psensor: 0
[2025-10-27 14:16:12.355] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.354|INFO|[angstrong/ai_module_palm_biz.cpp:251]recognize once over, ret:1100
[2025-10-27 14:16:12.355] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.354|INFO|[angstrong/ai_module.cpp:1069]enter working2 by stop session
[2025-10-27 14:16:12.357] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.357|INFO|[angstrong/ai_module.cpp:287]switch device status:working2 success, cost: 1ms
[2025-10-27 14:16:12.357] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-10-27 14:16:12.368] [info] [2434-2381]|[palm/yt_palm_service.cpp:771]Recognize finished: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c error: 1206 palm_id:
[2025-10-27 14:16:12.368] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.368|INFO|[angstrong/ai_module_palm_biz.cpp:308]recognize finish with session code:1206 palm_id:
[2025-10-27 14:16:12.372] [info] [2404-2381]|[palm/yt_palm_service.cpp:848]ResetPalmParams 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:12.398] [info] [QML] showFrameMessage: Align your palm with camera center
[2025-10-27 14:16:12.398] [info] [2381-2381]|[palm/yt_palm_context.h:297]DoOnReceivePalmRecognizeFinishedEvent session:  20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, palm_id : , code : 104, msg : , palm_manager code : 1206, msg : 请保持手掌静止
qml: FullscreenGifOverlay: Hiding immediately
[2025-10-27 14:16:12.399] [info] [QML] onPalmProcessEvent session_id 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, event : 6 error : 104, 1206
[2025-10-27 14:16:12.400] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:12.401] [info] [QML] openPass user_name:  pass_result_code: 104 subtitleText:
[2025-10-27 14:16:12.405] [info] [2711-2381]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/report-log, traceId: 6b6651514f5558573634464d3061576f, reqId: 1761545772369-3
[2025-10-27 14:16:12.438] [info] [QML] Pass onCompleted user_name :  passResultCode: 104, session_id: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:12.458] [info] [QML] HomePage Background: keeping palm, pageType=Pass
[2025-10-27 14:16:12.459] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:12.469] [info] [QML] HomePage Background: keeping palm, pageType=Pass
[2025-10-27 14:16:12.473] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:377]Detecting react PalmReqRecognizeFinishEvent  104 , palmId : , userId :
[2025-10-27 14:16:12.473] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:768]ShowVerifyResult entryGuard business_result : 0, light_up : 1, user_id : , card_no : . card_no_decimal: 0
[2025-10-27 14:16:12.473] [info] [2381-2381]|[palm/yt_palm_service.cpp:337]LightupForProcessResult success 0 lightupRet 0 config timeout 3000 ms
[2025-10-27 14:16:12.474] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.473|INFO|[angstrong/ai_module_palm_biz.cpp:465]light up, success:0 keep_ms:3000
[2025-10-27 14:16:12.474] [info] [2403-2381]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: UNKNOWN
[2025-10-27 14:16:12.474] [info] [2403-2381]|[palm/yt_palm_service.cpp:848]ResetPalmParams 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:12.474] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.474|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-27 14:16:12.494] [info] [QML] FullscreenGifOverlay onGifOverlayHidden
[2025-10-27 14:16:12.495] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:12.545] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:12.546] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:12.550] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.550|INFO|[angstrong/ai_module.cpp:987]get best image finish, cost:180ms
[2025-10-27 14:16:12.552] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.552|INFO|[angstrong/ai_module_palm_biz.cpp:476]light up ret:0 cost:78ms
[2025-10-27 14:16:12.555] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.555|INFO|[angstrong/ai_module_palm_biz.cpp:507]hide palm result light, ret:0 ret_fill_rgb:0 ret_fill_ir:0 cost:2ms
[2025-10-27 14:16:12.556] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.555|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:81ms
[2025-10-27 14:16:12.556] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:12.556|INFO|[angstrong/ai_module_palm_biz.cpp:652]device_status:2 now_ms:11540565 working2_start_ms:11540365 s0_timeout_ms:300000
[2025-10-27 14:16:12.560] [info] [2435-2381]|[palm/yt_palm_service.cpp:817]sync get backtracking info: 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:12.798] [info] [2709-2381]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/create-image, traceId: 514d79574355524b4f38365043544e68, reqId: 1761545772663-4
[2025-10-27 14:16:12.821] [info] [2567-2381]|[http/yt_qt_http_client.h:70]onResponse req_id 1761545772369-3, tag : /wecardpalm/device/report-log
qml: FullscreenGifOverlay: Hiding immediately
[2025-10-27 14:16:13.408] [info] [QML] FullscreenGifOverlay onGifOverlayHidden
[2025-10-27 14:16:13.408] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:13.879] [info] [2567-2381]|[http/yt_qt_http_client.h:70]onResponse req_id 1761545772663-4, tag : /wecardpalm/device/create-image
[2025-10-27 14:16:14.794] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:15.018] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:15.463] [info] [QML] Pass goBack
[2025-10-27 14:16:15.479] [info] [QML] HomePage Background: keeping palm, pageType=Pass
[2025-10-27 14:16:15.480] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:15.481] [info] [QML] HomePage Active
[2025-10-27 14:16:15.481] [info] [2381-2381]|[palm/yt_palm_service.cpp:190]EnablePalmWorking current 1
[2025-10-27 14:16:15.483] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:15.517] [info] [2381-2381]|[palm/yt_palm_context.h:490]DoOnPassVerifyShowFinished session_id : 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:15.517] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:784]ShowVerifyResult react PalmVerifyResultShowFinishEvent  20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c
[2025-10-27 14:16:15.518] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.517|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-27 14:16:15.544] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.544|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:25ms
[2025-10-27 14:16:15.549] [info] [QML] Check host connection state: false (palmMode: 2, registryMode: 1)
[2025-10-27 14:16:15.551] [info] [2381-2381]|[palm/yt_palm_context.h:310]DoOnReceivePalmProcessFinishedEvent session:  20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, code : 0
[2025-10-27 14:16:15.551] [info] [QML] onPalmProcessEvent session_id 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, event : 9 error : 0, 0
[2025-10-27 14:16:15.551] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:254]Working PalmProcessFinishEvent,  session_id : 20251027061611-9ccede12-2472-44b7-af73-106a93d0e59c, finish_reason : -1529475072
[2025-10-27 14:16:15.552] [info] [2381-2381]|[palm/yt_palm_service.cpp:343]WaitLightupFinished waiting for lightup finished
[2025-10-27 14:16:15.552] [info] [2381-2381]|[palm/yt_palm_service.cpp:350]WaitLightupFinished lightup finished 30
[2025-10-27 14:16:15.552] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:269]Idle entryGuard change to Detecting status
[2025-10-27 14:16:15.553] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.553|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:2
[2025-10-27 14:16:15.554] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.553|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:0ms
[2025-10-27 14:16:15.554] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.554|INFO|[angstrong/ai_module_palm_biz.cpp:197]start recognize...
[2025-10-27 14:16:15.554] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.554|INFO|[angstrong/ai_module.cpp:652]device_status:2 now_ms:11543564 working2_start_ms:11540365 s0_timeout_ms:300000
[2025-10-27 14:16:15.555] [info] [2403-2381]|[palm/yt_palm_service.cpp:278]_StartProcess palmMode: recognize StartRecognize ret: 0
[2025-10-27 14:16:15.571] [info] [2381-2381]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  , event : 1, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-27 14:16:15.573] [info] [QML] onPalmProcessEvent session_id , event : 1 error : 0, 0
[2025-10-27 14:16:15.574] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:312]Detecting react PalmProcessEvent  1
[2025-10-27 14:16:15.600] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:15.602] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:15.976] [info] [2433-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: , error_code: 1011
[2025-10-27 14:16:15.976] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:15.976|INFO|[angstrong/ai_module.cpp:721]get psensor:122 count:13
[2025-10-27 14:16:15.978] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:15.980] [info] [QML] showFrameMessage update message: Move your palm closer
[2025-10-27 14:16:15.980] [info] [QML] FullscreenGifOverlay: showAndPlay called with source:
[2025-10-27 14:16:15.980] [info] [QML] FullscreenGifOverlay: Current state - visible:
[2025-10-27 14:16:15.981] [info] [QML] FullscreenGifOverlay: Showing overlay with source:
[2025-10-27 14:16:16.012] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.011|INFO|[angstrong/ai_module.cpp:721]get psensor:133 count:1
[2025-10-27 14:16:16.021] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.038] [info] [QML] FullscreenGifOverlay: Show animation completed
[2025-10-27 14:16:16.038] [info] [QML] FullscreenGifOverlay: Starting GIF playback after show animation , source: qrc:/images/gif_palm_bg1.gifvisible:true
[2025-10-27 14:16:16.038] [info] [QML] FullscreenGifOverlay onGifOverlayShown
[2025-10-27 14:16:16.039] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:16.040] [info] [QML] FullscreenGifOverlay: About to call gifPlayer.play() - isPlaying: falsehasError:falseisLoading:false
[2025-10-27 14:16:16.040] [info] [QML] GifPlayer Debug - source: qrc:/images/gif_palm_bg1.gif
[2025-10-27 14:16:16.041] [info] [QML] GifPlayer Debug - _isPlaying: false
[2025-10-27 14:16:16.041] [info] [QML] GifPlayer Debug - _hasError: false
[2025-10-27 14:16:16.042] [info] [QML] GifPlayer Debug - _isLoading: false
[2025-10-27 14:16:16.042] [info] [QML] GifPlayer Debug - AnimatedImage.playing: false
[2025-10-27 14:16:16.043] [info] [QML] GifPlayer Debug - AnimatedImage.status: 1
[2025-10-27 14:16:16.045] [info] [QML] GifPlayer Debug - AnimatedImage.frameCount: 20
qml: FullscreenGifOverlay: After play() call:
qml: FullscreenGifOverlay: AnimatedImage.playing: true
qml: FullscreenGifOverlay: AnimatedImage.visible: true
[2025-10-27 14:16:16.046] [info] [QML] GifPlayer Debug - AnimatedImage.currentFrame: 10
qml: FullscreenGifOverlay: AnimatedImage.width: 720 height: 1280
[2025-10-27 14:16:16.046] [info] [QML] GifPlayer Debug - Load duration: 0ms
[2025-10-27 14:16:16.046] [info] [QML] FullscreenGifOverlay: GIF play started
[2025-10-27 14:16:16.047] [info] [QML] FullscreenGifOverlay: Play result:true
[2025-10-27 14:16:16.051] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.051|INFO|[angstrong/ai_module.cpp:721]get psensor:138 count:1
[2025-10-27 14:16:16.052] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.086] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.086|INFO|[angstrong/ai_module.cpp:721]get psensor:134 count:1
[2025-10-27 14:16:16.091] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.099] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:16.100] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:16.121] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.121|INFO|[angstrong/ai_module.cpp:721]get psensor:133 count:1
[2025-10-27 14:16:16.156] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.156|INFO|[angstrong/ai_module.cpp:721]get psensor:123 count:1
[2025-10-27 14:16:16.188] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.191] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.191|INFO|[angstrong/ai_module.cpp:721]get psensor:121 count:1
[2025-10-27 14:16:16.200] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.201] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.226] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.226|INFO|[angstrong/ai_module.cpp:721]get psensor:121 count:1
[2025-10-27 14:16:16.228] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:16.261] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.261|INFO|[angstrong/ai_module.cpp:721]get psensor:114 count:1
[2025-10-27 14:16:16.270] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.269|INFO|[angstrong/ai_module.cpp:835]will enter target status:1
[2025-10-27 14:16:16.274] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.273|INFO|[angstrong/ai_module.cpp:287]switch device status:working success, cost: 3ms
[2025-10-27 14:16:16.274] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working2 to Working1
[2025-10-27 14:16:16.275] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.274|INFO|[angstrong/ai_module_palm_biz.cpp:219]wait psensor valid, ret:0
[2025-10-27 14:16:16.275] [info] [2434-2381]|[palm/yt_palm_service.cpp:622]handleSessionStatus session_id :  stage: 1
[2025-10-27 14:16:16.275] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.275|INFO|[angstrong/ai_module_palm_biz.cpp:244]recognize once start
[2025-10-27 14:16:16.361] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.360|INFO|[angstrong/ai_module.cpp:606]session started, session_id:20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:16.361] [info] [2434-2381]|[palm/yt_palm_service.cpp:622]handleSessionStatus session_id : 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03 stage: 2
[2025-10-27 14:16:16.361] [info] [2404-2381]|[palm/yt_palm_service.cpp:636] session start : 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:16.419] [info] [2434-2381]|[palm/yt_palm_service.cpp:661]handleSessionFrame Previous error_code: 1011 occurred 8 times, new error_code: 1210
[2025-10-27 14:16:16.419] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.419|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1210 detail_code:120014 msg:画面过暗，请调整手掌角度 psensor:115 is_best:1 cost:143ms
[2025-10-27 14:16:16.421] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:16.422] [info] [QML] showFrameMessage update message: Keep your palm still
[2025-10-27 14:16:16.424] [info] [QML] showPlaySound soundType 17, lastTtsMessage: TTS1206, curTtsMessage TTS1206
[2025-10-27 14:16:16.557] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.556|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1210 detail_code:120014 msg:画面过暗，请调整手掌角度 psensor:109 is_best:0 cost:136ms
[2025-10-27 14:16:16.561] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:16.561] [info] [QML] showPlaySound soundType 17, lastTtsMessage: TTS1206, curTtsMessage TTS1206
[2025-10-27 14:16:16.681] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.681|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1211 detail_code:120014 msg:画面过亮，请调整手掌角度 psensor:109 is_best:0 cost:123ms
[2025-10-27 14:16:16.682] [info] [2434-2381]|[palm/yt_palm_service.cpp:661]handleSessionFrame Previous error_code: 1210 occurred 2 times, new error_code: 1211
[2025-10-27 14:16:16.683] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:16.683] [info] [QML] showPlaySound soundType 17, lastTtsMessage: TTS1206, curTtsMessage TTS1206
[2025-10-27 14:16:16.801] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:16.800|INFO|[angstrong/ai_module.cpp:937]frame callback, code:1211 detail_code:120010 msg:画面过亮，请调整手掌角度 psensor:107 is_best:0 cost:118ms
[2025-10-27 14:16:16.810] [info] [QML] showFrameMessage: Keep your palm still
[2025-10-27 14:16:16.811] [info] [QML] showPlaySound soundType 17, lastTtsMessage: TTS1206, curTtsMessage TTS1206
[2025-10-27 14:16:17.008] [info] [2434-2381]|[palm/yt_palm_service.cpp:622]handleSessionStatus session_id : 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03 stage: 3
[2025-10-27 14:16:17.009] [info] [2404-2381]|[palm/yt_palm_service.cpp:645] prefer succeed : 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:17.008] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.008|INFO|[angstrong/ai_module.cpp:614]prefer succeed, session_id:20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:17.019] [info] [2381-2381]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 2, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-27 14:16:17.019] [info] [QML] onPalmProcessEvent session_id 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 2 error : 0, 0
[2025-10-27 14:16:17.051] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:17.062] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:312]Detecting react PalmProcessEvent  2
[2025-10-27 14:16:17.063] [info] [2403-2381]|[palm/yt_palm_service.cpp:300]_StopProcess session_id:  invokde_medium: PALM
[2025-10-27 14:16:17.219] [info] [2434-2381]|[palm/yt_palm_service.cpp:661]handleSessionFrame Previous error_code: 1211 occurred 2 times, new error_code: 0
[2025-10-27 14:16:17.219] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.219|INFO|[angstrong/ai_module.cpp:937]frame callback, code:0 detail_code:0 msg: psensor:108 is_best:1 cost:417ms
[2025-10-27 14:16:17.221] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.219|INFO|[angstrong/ai_module_palm_biz.cpp:251]recognize once over, ret:0
[2025-10-27 14:16:17.227] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.227|INFO|[angstrong/ai_module_palm_biz.cpp:308]recognize finish with session code:0 palm_id:ac5970d9-6c41-4233-8ca3-66b03fc11cce
[2025-10-27 14:16:17.227] [info] [2434-2381]|[palm/yt_palm_service.cpp:771]Recognize finished: 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03 error: 0 palm_id: ac5970d9-6c41-4233-8ca3-66b03fc11cce
[2025-10-27 14:16:17.231] [info] [2404-2381]|[palm/yt_palm_service.cpp:848]ResetPalmParams 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:17.245] [info] [2381-2381]|[palm/yt_palm_context.h:297]DoOnReceivePalmRecognizeFinishedEvent session:  20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, palm_id : ac5970d9-6c41-4233-8ca3-66b03fc11cce, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-27 14:16:17.246] [info] [QML] onPalmProcessEvent session_id 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 6 error : 0, 0
[2025-10-27 14:16:17.246] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:513]LocalRecognize react PalmReqRecognizeFinishEvent  0 , palmId : ac5970d9-6c41-4233-8ca3-66b03fc11cce, userId :
[2025-10-27 14:16:17.247] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:701]VerifyUser entryGuard payload: 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, palm_id : ac5970d9-6c41-4233-8ca3-66b03fc11cce, user_id :
[2025-10-27 14:16:17.247] [info] [2381-2381]|[devicectl/yt_devicectl_manager.cpp:373]VerifyPassByPalmId session_id: 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, palm_id: ac5970d9-6c41-4233-8ca3-66b03fc11cce, user_id:
[2025-10-27 14:16:17.248] [error] [2405-2381]|[storage/yt_database_manager.h:76]Failed to prepare SQL statement: SELECT p.user_id, p.palm_id, p.rgb_digest, p.ir_digest, p.status, u.name, u.card_no, p.direction, p.date_version, p.feature_type, u.user_tags FROM palm_entity_new p LEFT JOIN user_info u ON p.user_id = u.user_id WHERE p.palm_id = "ac5970d9-6c41-4233-8ca3-66b03fc11cce" LIMIT 1; , error: no such table: palm_entity_new
[2025-10-27 14:16:17.248] [error] [2405-2381]|[storage/yt_palm_db_manager.cpp:489]QueryPalmWithUserInfo Query failed, error: 161
[2025-10-27 14:16:17.248] [error] [2405-2381]|[devicectl/yt_devicectl_manager.cpp:400]_VerifyPassByPalmId RetrievePalmById failed, user_id: , ret: 161
[2025-10-27 14:16:17.265] [info] [2381-2381]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 7, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-27 14:16:17.266] [info] [QML] onPalmProcessEvent session_id 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 7 error : 0, 0
[2025-10-27 14:16:17.266] [info] [2381-2381]|[palm/yt_palm_context.h:441]DoOnPassVerifyUserFailed error_code : 161
[2025-10-27 14:16:17.266] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:738]VerifyUser react PalmPassVerifyUserFailedEvent  161
[2025-10-27 14:16:17.266] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:768]ShowVerifyResult entryGuard business_result : 0, light_up : 1, user_id : , card_no : . card_no_decimal: 0
[2025-10-27 14:16:17.266] [info] [2381-2381]|[palm/yt_palm_service.cpp:337]LightupForProcessResult success 0 lightupRet 0 config timeout 3000 ms
[2025-10-27 14:16:17.267] [info] [QML] Palm recognition verify failed, error code: 161, session_id 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
qml: FullscreenGifOverlay: Hiding immediately
[2025-10-27 14:16:17.268] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.267|INFO|[angstrong/ai_module_palm_biz.cpp:465]light up, success:0 keep_ms:3000
[2025-10-27 14:16:17.268] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:17.270] [info] [QML] openPass user_name:  pass_result_code: 161 subtitleText:
[2025-10-27 14:16:17.270] [info] [2713-2381]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/report-log, traceId: 466e4e4b475861354248343630446744, reqId: 1761545777228-5
[2025-10-27 14:16:17.285] [info] [QML] Pass onCompleted user_name :  passResultCode: 161, session_id: 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:17.302] [info] [QML] HomePage Background: keeping palm, pageType=Pass
[2025-10-27 14:16:17.303] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:17.311] [info] [QML] HomePage Background: keeping palm, pageType=Pass
[2025-10-27 14:16:17.320] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:17.332] [info] [2381-2381]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 8, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-27 14:16:17.333] [info] [QML] onPalmProcessEvent session_id 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 8 error : 0, 0
[2025-10-27 14:16:17.353] [info] [QML] FullscreenGifOverlay onGifOverlayHidden
[2025-10-27 14:16:17.353] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:17.403] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:17.404] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:17.423] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.423|INFO|[angstrong/ai_module.cpp:987]get best image finish, cost:194ms
[2025-10-27 14:16:17.425] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:17.424|INFO|[angstrong/ai_module_palm_biz.cpp:476]light up ret:0 cost:152ms
[2025-10-27 14:16:17.432] [info] [2435-2381]|[palm/yt_palm_service.cpp:817]sync get backtracking info: 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
qml: FullscreenGifOverlay: Hiding immediately
[2025-10-27 14:16:17.479] [info] [QML] FullscreenGifOverlay onGifOverlayHidden
[2025-10-27 14:16:17.480] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:17.653] [info] [2711-2381]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/create-image, traceId: 55774669464b6b7a3572373051625668, reqId: 1761545777529-6
[2025-10-27 14:16:17.694] [info] [2567-2381]|[http/yt_qt_http_client.h:70]onResponse req_id 1761545777228-5, tag : /wecardpalm/device/report-log
[2025-10-27 14:16:18.589] [info] [2567-2381]|[http/yt_qt_http_client.h:70]onResponse req_id 1761545777529-6, tag : /wecardpalm/device/create-image
[2025-10-27 14:16:19.863] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:20.263] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:20.307] [info] [QML] Pass goBack
[2025-10-27 14:16:20.324] [info] [QML] HomePage Background: keeping palm, pageType=Pass
[2025-10-27 14:16:20.325] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:20.325] [info] [QML] HomePage Active
[2025-10-27 14:16:20.325] [info] [2381-2381]|[palm/yt_palm_service.cpp:190]EnablePalmWorking current 1
[2025-10-27 14:16:20.327] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:20.362] [info] [2381-2381]|[palm/yt_palm_context.h:490]DoOnPassVerifyShowFinished session_id : 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:20.362] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:784]ShowVerifyResult react PalmVerifyResultShowFinishEvent  20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03
[2025-10-27 14:16:20.362] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.362|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:1
[2025-10-27 14:16:20.392] [info] [2434-2381]|[palm/yt_palm_service.cpp:559]HandlePalmDeviceStatusmodule status changed from Working1 to Working2
[2025-10-27 14:16:20.392] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.392|INFO|[angstrong/ai_module.cpp:287]switch device status:working2 success, cost: 3ms
[2025-10-27 14:16:20.393] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.392|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:29ms
[2025-10-27 14:16:20.393] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.392|INFO|[angstrong/ai_module_palm_biz.cpp:652]device_status:2 now_ms:11548402 working2_start_ms:11548397 s0_timeout_ms:300000
[2025-10-27 14:16:20.398] [info] [QML] Check host connection state: false (palmMode: 2, registryMode: 1)
[2025-10-27 14:16:20.399] [info] [2381-2381]|[palm/yt_palm_context.h:310]DoOnReceivePalmProcessFinishedEvent session:  20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, code : 0
[2025-10-27 14:16:20.400] [info] [QML] onPalmProcessEvent session_id 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, event : 9 error : 0, 0
[2025-10-27 14:16:20.400] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:254]Working PalmProcessFinishEvent,  session_id : 20251027061616-5d64acb0-178a-491d-bd28-6681ce27db03, finish_reason : -1529475072
[2025-10-27 14:16:20.400] [info] [2381-2381]|[palm/yt_palm_service.cpp:343]WaitLightupFinished waiting for lightup finished
[2025-10-27 14:16:20.400] [info] [2381-2381]|[palm/yt_palm_service.cpp:350]WaitLightupFinished lightup finished 30
[2025-10-27 14:16:20.400] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:269]Idle entryGuard change to Detecting status
[2025-10-27 14:16:20.402] [info] [2381-2381]|[palm/yt_palm_context.h:259]DoOnReceivePalmProcessEvent session:  , event : 1, code : 0, msg : , palm_manager code : 0, msg :
[2025-10-27 14:16:20.402] [info] [QML] onPalmProcessEvent session_id , event : 1 error : 0, 0
[2025-10-27 14:16:20.403] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.402|INFO|[angstrong/ai_module.cpp:1223]stop all with reason:2
[2025-10-27 14:16:20.404] [info] [2381-2381]|[fsm/yt_palm_state_machine.h:312]Detecting react PalmProcessEvent  1
[2025-10-27 14:16:20.427] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.427|INFO|[angstrong/ai_module.cpp:1254]stop all success, cost:24ms
[2025-10-27 14:16:20.428] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.427|INFO|[angstrong/ai_module_palm_biz.cpp:197]start recognize...
[2025-10-27 14:16:20.428] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:20.427|INFO|[angstrong/ai_module.cpp:652]device_status:2 now_ms:11548437 working2_start_ms:11548397 s0_timeout_ms:300000
[2025-10-27 14:16:20.428] [info] [2403-2381]|[palm/yt_palm_service.cpp:278]_StartProcess palmMode: recognize StartRecognize ret: 0
[2025-10-27 14:16:20.448] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:20.451] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:21.196] [info] [2433-2381]|[palm/yt_palm_service.cpp:663]handleSessionFrame session_id: , error_code: 1011
[2025-10-27 14:16:21.196] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.196|INFO|[angstrong/ai_module.cpp:721]get psensor:219 count:23
[2025-10-27 14:16:21.209] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.211] [info] [QML] showFrameMessage update message: Move your palm closer
[2025-10-27 14:16:21.211] [info] [QML] FullscreenGifOverlay: showAndPlay called with source:
[2025-10-27 14:16:21.211] [info] [QML] FullscreenGifOverlay: Current state - visible:
[2025-10-27 14:16:21.212] [info] [QML] FullscreenGifOverlay: Showing overlay with source:
[2025-10-27 14:16:21.231] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.230|INFO|[angstrong/ai_module.cpp:721]get psensor:249 count:1
[2025-10-27 14:16:21.250] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.270] [info] [QML] FullscreenGifOverlay: Show animation completed
[2025-10-27 14:16:21.271] [info] [QML] FullscreenGifOverlay: Starting GIF playback after show animation , source: qrc:/images/gif_palm_bg1.gifvisible:true
[2025-10-27 14:16:21.305] [info] [QML] FullscreenGifOverlay onGifOverlayShown
[2025-10-27 14:16:21.305] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:21.306] [info] [QML] FullscreenGifOverlay: About to call gifPlayer.play() - isPlaying: falsehasError:falseisLoading:false
[2025-10-27 14:16:21.307] [info] [QML] GifPlayer Debug - source: qrc:/images/gif_palm_bg1.gif
[2025-10-27 14:16:21.307] [info] [QML] GifPlayer Debug - _isPlaying: false
[2025-10-27 14:16:21.308] [info] [QML] GifPlayer Debug - _hasError: false
[2025-10-27 14:16:21.308] [info] [QML] GifPlayer Debug - _isLoading: false
[2025-10-27 14:16:21.309] [info] [QML] GifPlayer Debug - AnimatedImage.playing: false
[2025-10-27 14:16:21.310] [info] [QML] GifPlayer Debug - AnimatedImage.status: 1
[2025-10-27 14:16:21.310] [info] [QML] GifPlayer Debug - AnimatedImage.frameCount: 20
[2025-10-27 14:16:21.311] [info] [QML] GifPlayer Debug - AnimatedImage.currentFrame: 9
[2025-10-27 14:16:21.311] [info] [QML] GifPlayer Debug - Load duration: 0ms
[2025-10-27 14:16:21.312] [info] [QML] FullscreenGifOverlay: GIF play started
[2025-10-27 14:16:21.312] [info] [QML] FullscreenGifOverlay: Play result:true
qml: FullscreenGifOverlay: After play() call:
qml: FullscreenGifOverlay: AnimatedImage.playing: true
qml: FullscreenGifOverlay: AnimatedImage.visible: true
qml: FullscreenGifOverlay: AnimatedImage.width: 720 height: 1280
[2025-10-27 14:16:21.335] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.335|INFO|[angstrong/ai_module.cpp:721]get psensor:247 count:3
[2025-10-27 14:16:21.337] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.370] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.370|INFO|[angstrong/ai_module.cpp:721]get psensor:247 count:1
[2025-10-27 14:16:21.381] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.381] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:21.382] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:21.407] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.406|INFO|[angstrong/ai_module.cpp:721]get psensor:225 count:1
[2025-10-27 14:16:21.441] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.441|INFO|[angstrong/ai_module.cpp:721]get psensor:207 count:1
[2025-10-27 14:16:21.466] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.476] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.476|INFO|[angstrong/ai_module.cpp:721]get psensor:200 count:1
[2025-10-27 14:16:21.478] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.479] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.511] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.511|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:21.548] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.547|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-27 14:16:21.548] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.560] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.583] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.582|INFO|[angstrong/ai_module.cpp:721]get psensor:180 count:1
[2025-10-27 14:16:21.614] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.617] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.617|INFO|[angstrong/ai_module.cpp:721]get psensor:181 count:1
[2025-10-27 14:16:21.627] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.652] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.652|INFO|[angstrong/ai_module.cpp:721]get psensor:182 count:1
[2025-10-27 14:16:21.671] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.687] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.687|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-27 14:16:21.688] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.721] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.721|INFO|[angstrong/ai_module.cpp:721]get psensor:183 count:1
[2025-10-27 14:16:21.744] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.758] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.759] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.756|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-27 14:16:21.791] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.791|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:21.817] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.825] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.825|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:21.831] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.860] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.860|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-27 14:16:21.883] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.894] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.894|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:21.896] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.929] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.928|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:21.953] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.963] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.963|INFO|[angstrong/ai_module.cpp:721]get psensor:184 count:1
[2025-10-27 14:16:21.967] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:21.998] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:21.997|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-27 14:16:22.024] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.033] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.032|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-27 14:16:22.039] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.069] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.069|INFO|[angstrong/ai_module.cpp:721]get psensor:187 count:1
[2025-10-27 14:16:22.096] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.104] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.104|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:22.113] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.142] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.141|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:22.165] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.177] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.176|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:22.182] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.211] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.211|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.235] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.246] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.246|INFO|[angstrong/ai_module.cpp:721]get psensor:187 count:1
[2025-10-27 14:16:22.252] [info] [QML] showFrameMessage: Move your palm closer
qml: FullscreenGifOverlay: Hiding immediately
[2025-10-27 14:16:22.253] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:22.254] [info] [QML] FullscreenGifOverlay onGifOverlayHidden
[2025-10-27 14:16:22.255] [info] [QML] GifPlayer GIF is already playing
[2025-10-27 14:16:22.280] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.280|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:22.298] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.298] [info] [QML] FullscreenGifOverlay: showAndPlay called with source:
[2025-10-27 14:16:22.299] [info] [QML] FullscreenGifOverlay: Current state - visible:
[2025-10-27 14:16:22.300] [info] [QML] FullscreenGifOverlay: Showing overlay with source:
[2025-10-27 14:16:22.315] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.315|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:22.350] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.350|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.357] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.358] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.385] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.385|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.387] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.420] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.419|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.427] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.446] [info] [QML] FullscreenGifOverlay: Show animation completed
[2025-10-27 14:16:22.447] [info] [QML] FullscreenGifOverlay: Starting GIF playback after show animation , source: qrc:/images/gif_palm_bg1.gifvisible:true
[2025-10-27 14:16:22.448] [info] [QML] FullscreenGifOverlay onGifOverlayShown
[2025-10-27 14:16:22.448] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:22.450] [info] [QML] FullscreenGifOverlay: About to call gifPlayer.play() - isPlaying: falsehasError:falseisLoading:false
[2025-10-27 14:16:22.451] [info] [QML] GifPlayer Debug - source: qrc:/images/gif_palm_bg1.gif
[2025-10-27 14:16:22.451] [info] [QML] GifPlayer Debug - _isPlaying: false
[2025-10-27 14:16:22.452] [info] [QML] GifPlayer Debug - _hasError: false
[2025-10-27 14:16:22.452] [info] [QML] GifPlayer Debug - _isLoading: false
[2025-10-27 14:16:22.453] [info] [QML] GifPlayer Debug - AnimatedImage.playing: false
[2025-10-27 14:16:22.453] [info] [QML] GifPlayer Debug - AnimatedImage.status: 1
[2025-10-27 14:16:22.455] [info] [QML] GifPlayer Debug - AnimatedImage.frameCount: 20
[2025-10-27 14:16:22.455] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.454|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.456] [info] [QML] GifPlayer Debug - AnimatedImage.currentFrame: 7
[2025-10-27 14:16:22.456] [info] [QML] GifPlayer Debug - Load duration: 0ms
[2025-10-27 14:16:22.457] [info] [QML] FullscreenGifOverlay: GIF play started
[2025-10-27 14:16:22.458] [info] [QML] FullscreenGifOverlay: Play result:true
[2025-10-27 14:16:22.490] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.490|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.496] [info] [QML] showFrameMessage: Move your palm closer
qml: FullscreenGifOverlay: After play() call:
qml: FullscreenGifOverlay: AnimatedImage.playing: true
qml: FullscreenGifOverlay: AnimatedImage.visible: true
qml: FullscreenGifOverlay: AnimatedImage.width: 720 height: 1280
[2025-10-27 14:16:22.498] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.525] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.525|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:22.528] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.548] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:22.551] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:22.562] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.562|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:22.596] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.596|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.620] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.621] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.632] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.631|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.667] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.666|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.672] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.673] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.703] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.703|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:22.705] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.738] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.738|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:22.741] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.777] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.777|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.812] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.811|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:22.817] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.818] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.847] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.847|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.882] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.882|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:22.882] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.895] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.917] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.917|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.952] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.952|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:22.955] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.956] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:22.987] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:22.987|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:23.023] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.024] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.023|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:23.036] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.058] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.058|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:23.092] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.094] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.093|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.107] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.130] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.129|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.164] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.164|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:23.167] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.168] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.201] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.200|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:23.234] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.235] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.234|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:23.251] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.270] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.270|INFO|[angstrong/ai_module.cpp:721]get psensor:196 count:1
[2025-10-27 14:16:23.304] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.305] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.305|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:23.322] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.341] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.341|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:23.370] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.376] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.375|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.387] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.411] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.411|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:23.442] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.446] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.446|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:23.462] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.481] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.481|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.511] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.516] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.515|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.551] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.551|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:23.584] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.585] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.586] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.585|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.605] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.622] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.621|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:23.650] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.657] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.657|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.671] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.692] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.691|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:23.724] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.727] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.726|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:23.745] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.762] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.762|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:23.785] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:23.796] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.796|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:23.828] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.829] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.831] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.831|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:23.850] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.867] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.866|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:23.868] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.901] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.901|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:23.911] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.937] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.936|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:23.967] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:23.973] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:23.973|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:24.007] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.007|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:24.041] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.041|INFO|[angstrong/ai_module.cpp:721]get psensor:196 count:1
[2025-10-27 14:16:24.077] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.076|INFO|[angstrong/ai_module.cpp:721]get psensor:186 count:1
[2025-10-27 14:16:24.113] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.113|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:24.121] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.122] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.138] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.140] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.141] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.147] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.147|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:24.182] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.182|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:24.216] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.216|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:24.251] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.251|INFO|[angstrong/ai_module.cpp:721]get psensor:196 count:1
[2025-10-27 14:16:24.287] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.286|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:24.288] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.289] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.289] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.290] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.321] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.321|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:24.357] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.357|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:24.392] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.392|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:24.427] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.426|INFO|[angstrong/ai_module.cpp:721]get psensor:188 count:1
[2025-10-27 14:16:24.440] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.442] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.443] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.445] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.445] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.461] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.461|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:24.496] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.496|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:24.531] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.530|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:24.566] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.566|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:24.596] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.597] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.598] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.599] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.607] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.607|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:24.642] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.642|INFO|[angstrong/ai_module.cpp:721]get psensor:190 count:1
[2025-10-27 14:16:24.677] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.677|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:24.712] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.712|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:24.747] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.747|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:24.760] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.761] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.763] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.764] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.765] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.782] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.781|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:24.816] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.816|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:24.851] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.851|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:24.885] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.885|INFO|[angstrong/ai_module.cpp:721]get psensor:189 count:1
[2025-10-27 14:16:24.914] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.915] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.917] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.918] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:24.921] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.920|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:24.956] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.956|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:24.990] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:24.990|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:25.026] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.026|INFO|[angstrong/ai_module.cpp:721]get psensor:191 count:1
[2025-10-27 14:16:25.061] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.061|INFO|[angstrong/ai_module.cpp:721]get psensor:186 count:1
[2025-10-27 14:16:25.070] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.071] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.072] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.073] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.074] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.097] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.096|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:25.131] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.131|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:25.165] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.165|INFO|[angstrong/ai_module.cpp:721]get psensor:192 count:1
[2025-10-27 14:16:25.200] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.200|INFO|[angstrong/ai_module.cpp:721]get psensor:194 count:1
[2025-10-27 14:16:25.227] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.228] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.229] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.230] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.235] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.235|INFO|[angstrong/ai_module.cpp:721]get psensor:193 count:1
[2025-10-27 14:16:25.270] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.270|INFO|[angstrong/ai_module.cpp:721]get psensor:195 count:1
[2025-10-27 14:16:25.304] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.304|INFO|[angstrong/ai_module.cpp:721]get psensor:201 count:1
[2025-10-27 14:16:25.339] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.339|INFO|[angstrong/ai_module.cpp:721]get psensor:201 count:1
[2025-10-27 14:16:25.342] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:25.374] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.374|INFO|[angstrong/ai_module.cpp:721]get psensor:227 count:1
[2025-10-27 14:16:25.381] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.381] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.382] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.384] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.385] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.404] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:25.410] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:25.409|INFO|[angstrong/ai_module.cpp:721]get psensor:227 count:1
[2025-10-27 14:16:25.439] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:25.490] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:25.491] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:27.646] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:27.935] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:28.288] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:28.331] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:28.395] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:28.396] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:30.980] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:31.018] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:31.076] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:31.078] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:33.004] [info] [2705-2381]|[looper/yt_sync_palm_looper.cpp:163]RequestSyncPalm query_position:
[2025-10-27 14:16:33.014] [info] [2709-2381]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/synchronize_palm_feature, traceId: 394d7a4a57673861566b744e304a4771, reqId: 1761545792999-7
[2025-10-27 14:16:33.019] [info] [2706-2381]|[looper/yt_sync_user_looper.cpp:69]RequestSyncUser next_id: 0, start_time: 1761536304, sync_type: incr
[2025-10-27 14:16:33.026] [info] [2713-2381]|[http/yt_qt_http_client.cpp:179]Post Request URL: https://device.palmoa.youtu.qq.com/wecardpalm/device/incr-sync-user, traceId: 6b3756775673444845486b646842304e, reqId: 1761545793019-8
[2025-10-27 14:16:33.436] [info] [2567-2381]|[http/yt_qt_http_client.h:70]onResponse req_id 1761545793019-8, tag : /wecardpalm/device/incr-sync-user
[2025-10-27 14:16:33.437] [info] [2567-2381]|[looper/yt_sync_user_looper.cpp:106]Sync user succeed: empty response, next_id=0
[2025-10-27 14:16:33.660] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:33.699] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:33.771] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:33.772] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:33.950] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:35.332] [info] [2567-2381]|[http/yt_qt_http_client.h:70]onResponse req_id 1761545792999-7, tag : /wecardpalm/device/synchronize_palm_feature
[2025-10-27 14:16:35.352] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=abcabc, palm_id=03109d3c-395a-4a29-a1b3-6ed70920c502, name=, status=1, direction=2, palm_data_version=7, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.352] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8615201806426, palm_id=1fb325e8-3ab3-4324-9487-448351a5b0ae, name=, status=2, direction=2, palm_data_version=15, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.352] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=aTnv9B6V, palm_id=219392e4-c06d-4f83-bd38-0e57791811da, name=, status=1, direction=1, palm_data_version=9, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=12345678, palm_id=4c4c8b9e-dd36-4c17-b594-044b27e2fc2b, name=, status=2, direction=1, palm_data_version=22, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=86188888889, palm_id=720aecca-acda-4974-9e1d-1c6ceb858b82, name=, status=2, direction=2, palm_data_version=13, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new_upgrade_1, palm_id=73035d09-8e04-4e01-be3c-a5b10b55e60f, name=, status=1, direction=1, palm_data_version=18, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_same_upgrade, palm_id=7c09d068-5de4-405b-b39c-30f5c395ed25, name=, status=1, direction=1, palm_data_version=16, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_same, palm_id=7fa8bb29-a51c-49ae-8c94-632e6b442696, name=, status=1, direction=1, palm_data_version=14, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new, palm_id=80006db0-7463-4f6e-bce6-2e423df5c18a, name=, status=2, direction=1, palm_data_version=12, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=vISX7nQL, palm_id=98d2e90a-442c-4fff-a212-8acaf8c4206d, name=, status=1, direction=1, palm_data_version=10, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8613012890285, palm_id=a021d71e-30f3-4373-b108-263d9946ed85, name=, status=2, direction=2, palm_data_version=14, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=EXMDBcDS, palm_id=bb0189a5-a4d3-4d35-b483-7ce318f72cec, name=, status=2, direction=1, palm_data_version=17, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.353] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_1_upgrade, palm_id=bc717db5-78bf-4217-9d42-dbba754c41d4, name=, status=2, direction=2, palm_data_version=6, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_user, palm_id=c077ecdc-dc61-4fe1-bbcf-0a9a4284f07c, name=, status=2, direction=2, palm_data_version=25, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=WC4KKQBv, palm_id=c874de3c-b9dc-437d-a699-9a735181f419, name=, status=1, direction=1, palm_data_version=8, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=test_new_upgrade, palm_id=c97cf563-a36e-40d9-bd61-384890c01653, name=, status=1, direction=1, palm_data_version=13, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8617349756267, palm_id=d1c9f315-14c7-40d4-a461-54f430ee0964, name=, status=1, direction=2, palm_data_version=19, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=delete_1, palm_id=e181b8b0-85d2-46c1-8319-a2332e96fc9f, name=, status=2, direction=2, palm_data_version=4, feature_type=0, rgb_digest_length=0, ir_digest_length=0, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=8613770320839, palm_id=f62fe416-6fd2-4909-acf4-36917d3f1f32, name=, status=1, direction=2, palm_data_version=20, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.354] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:181]SyncPalmLooper sync palm: PalmEntity{user_id=13012890285, palm_id=fd6c532c-df31-4eaa-b3d2-56a529553c25, name=, status=1, direction=2, palm_data_version=8, feature_type=2, rgb_digest_length=684, ir_digest_length=684, card_no=, timestamp=1761545795, user_tags=}
[2025-10-27 14:16:35.355] [error] [2567-2381]|[storage/yt_database_manager.h:111]Failed to prepare SQL statement: INSERT OR REPLACE INTO palm_entity_new (user_id, palm_id, rgb_digest, ir_digest, status, direction, date_version, feature_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?); , error: no such table: palm_entity_new
[2025-10-27 14:16:35.355] [error] [2567-2381]|[storage/yt_palm_db_manager.cpp:241]InsertOrUpdatePalms Failed to insert/update palms, ret: 161
[2025-10-27 14:16:35.355] [error] [2567-2381]|[devicectl/yt_devicectl_manager.cpp:220]update palms to db ret :161 in UpdatePalms
[2025-10-27 14:16:35.355] [info] [2567-2381]|[looper/yt_sync_palm_looper.cpp:186]SyncPalmLooper response: hasMore=2, query_position=eyJxdWVyeV9jdHhfaW5mb19sc3QiOlt7Im5leHRfdmVyc2lvbiI6MjIsInBhbG1fZGlyZWN0aW9uIjoxLCJsYXN0X3RpbWUiOjE3NjE1NDU3OTN9LHsibmV4dF92ZXJzaW9uIjoyNSwicGFsbV9kaXJlY3Rpb24iOjIsImxhc3RfdGltZSI6MTc2MTU0NTc5M31dfQ==, count=20, updateResult=161
[2025-10-27 14:16:35.499] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.498|INFO|[angstrong/ai_module.cpp:721]get psensor:125 count:290
[2025-10-27 14:16:35.533] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.533|INFO|[angstrong/ai_module.cpp:721]get psensor:132 count:1
[2025-10-27 14:16:35.568] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.568|INFO|[angstrong/ai_module.cpp:721]get psensor:136 count:1
[2025-10-27 14:16:35.591] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:35.592] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:35.592] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:35.604] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.603|INFO|[angstrong/ai_module.cpp:721]get psensor:144 count:1
[2025-10-27 14:16:35.638] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.638|INFO|[angstrong/ai_module.cpp:721]get psensor:146 count:1
[2025-10-27 14:16:35.673] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.672|INFO|[angstrong/ai_module.cpp:721]get psensor:145 count:1
[2025-10-27 14:16:35.708] [info] [2431-2381]|[palm/yt_palm_service.cpp:1141][Palmmgr]2025-10-27 14:16:35.708|INFO|[angstrong/ai_module.cpp:721]get psensor:151 count:1
[2025-10-27 14:16:35.744] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:35.744] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:35.745] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:35.745] [info] [QML] showFrameMessage: Move your palm closer
[2025-10-27 14:16:36.363] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:36.401] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:36.467] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:36.469] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:39.051] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:39.089] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:39.142] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:39.144] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:39.419] [info] [2381-2381]|[audio/yt_audio_manager.cpp:441]Processing audio queue
[2025-10-27 14:16:40.307] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:41.755] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:41.793] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:41.867] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:41.868] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:44.427] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:44.466] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:44.544] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:44.545] [info] [QML] GifPlayer GIF load failed, this is Null status
[2025-10-27 14:16:46.747] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 39
[2025-10-27 14:16:47.099] [info] [QML] GifPlayer Reached last frame, preparing to loop... total frame count 20
[2025-10-27 14:16:47.137] [info] [QML] GifPlayer Loop completed, restarting...
[2025-10-27 14:16:47.190] [info] [QML] GifPlayer Restarting GIF from beginning
[2025-10-27 14:16:47.192] [info] [QML] GifPlayer GIF load failed, this is Null status
