2026-06-29 18:41:26.162 29516-29516 UserLogUploadHandler    com.tencent.palm.wepay               I  [:29516, ]:[4488K] handle: date=20260629, eventName=uploadLog_19_mqz385g0
2026-06-29 18:41:26.174 23559-23939 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:23939, ]:[11867K] start: date = 20260629
2026-06-29 18:41:26.174 23559-23939 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:23939, ]:[11867K] createInstruction: POST /iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions
2026-06-29 18:41:26.220 23559-23939 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23939, ]:[11931K] [post][441] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions, trace = 00-51673277324c36377165633500000000-6b5572827b9c9215-01
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6397K] Http: --> POST https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions http/1.1
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6413K] Http: Authorization: IOTDEVICE-SHA256-SM2 device_sn="X8AZ112F2220337VHJH8EFA",nonce_str="vdNjsyOD5qnX1ZRrJI5wBIWWDPXyR9sq",signature="CgExEoUBCh5UWllTMVdYMTI0NDIyRjg0MzNGQTAwMDAwMDAwMDAQAhjXl4nSBioQYBYMsR4i4Tu92gPpZjHimjJHMEUCICLJyBkayySUjYEdxrHr6lRBKZbREMeL2QtxXw7kFvX4AiEA4W+p3+SsXh/26FENoOz04+RDsx8/iDe/kG6TaOGInIQ4ABoORGV2aWNlQXV0aFNpZ24=",timestamp="1782729686"
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: Accept: application/json
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: traceparent: 00-7550586c534145494253306300000000-ef652c306cbdcc10-01
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: x-ms-blob-type: BlockBlob
2026-06-29 18:41:26.444 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: Content-Type: application/json; charset=utf-8
2026-06-29 18:41:26.445 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: Content-Length: 108
2026-06-29 18:41:26.445 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: Host: device.gz-ty.palm.tencent.com
2026-06-29 18:41:26.445 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: Connection: Keep-Alive
2026-06-29 18:41:26.445 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: Accept-Encoding: gzip
2026-06-29 18:41:26.445 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http: User-Agent: okhttp/4.10.0
2026-06-29 18:41:26.445 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6429K] Http:
2026-06-29 18:41:26.671 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6479K] Http: <-- 200 OK https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions (225ms)
2026-06-29 18:41:26.671 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: Date: Mon, 29 Jun 2026 10:41:28 GMT
2026-06-29 18:41:26.671 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: Content-Type: application/json
2026-06-29 18:41:26.671 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: Content-Length: 53
2026-06-29 18:41:26.671 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: Connection: keep-alive
2026-06-29 18:41:26.671 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: grpc-metadata-content-type: application/grpc
2026-06-29 18:41:26.672 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: grpc-metadata-otel_call_app: devicegateway
2026-06-29 18:41:26.672 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: grpc-metadata-otel_call_ip: 10.105.48.53
2026-06-29 18:41:26.672 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: grpc-metadata-otel_call_method: CreateLogUploadInstruction
2026-06-29 18:41:26.672 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: grpc-metadata-otel_call_server: devicegateway
2026-06-29 18:41:26.672 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: grpc-metadata-otel_call_service: DeviceGateway
2026-06-29 18:41:26.672 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: x-envoy-upstream-service-time: 62
2026-06-29 18:41:26.673 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:41:26.673 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: X-WAF-UUID: f28b75c86418cdf7cc72d3cc1c41496a-4c362d22876fa2ad542bbb1281d116b4
2026-06-29 18:41:26.673 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http:
2026-06-29 18:41:26.673 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: {"instruction_no":"01202606291841281123063497774694"}
2026-06-29 18:41:26.674 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[6495K] Http: <-- END HTTP (53-byte body)
2026-06-29 18:41:26.674 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[6527K] [post][441] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions}
2026-06-29 18:41:26.675 23559-31525 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:31525, ]:[6528K] createInstruction onSuccess: {"instruction_no":"01202606291841281123063497774694"}
2026-06-29 18:41:26.676 23559-31525 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:31525, ]:[6528K] onInstructionAccepted: 01202606291841281123063497774694
2026-06-29 18:41:26.681 29516-29516 UserLogUploadHandler    com.tencent.palm.wepay               I  [:29516, ]:[4793K] onAccepted: 01202606291841281123063497774694
2026-06-29 18:41:40.703 23559-23559 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23559, ]:[8695K] [post][442] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/upgradedetails/batch-query, trace = 00-697a7134644f38394276425100000000-c66a60af1a81261d-01
2026-06-29 18:41:41.055 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[9181K] [post][442] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/upgradedetails/batch-query}
2026-06-29 18:41:45.488 23559-23735 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23735, ]:[9496K] [get][443] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/execute-list, trace = 00-397677383238455a3547543500000000-722bdb9fdc7eda7c-01
2026-06-29 18:41:46.175 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[8665K] [get][443] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/execute-list}}
2026-06-29 18:41:46.196 23559-23734 Uploadv2Manager         com.tencent.wxpayface.iotservice     D  [:23734, ]:[9410K] onNewCmd, id = 01202606291841281123063497774694, name = uploadLog, bsParam = {"log_file_rule":"20260629","log_target_path":"/sdcard/wxpayface/log"}
2026-06-29 18:41:46.199 23559-23734 Uploadv2Manager         com.tencent.wxpayface.iotservice     D  [:23734, ]:[9445K] use cloud upload log upload
2026-06-29 18:41:47.247 23559-23735 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23735, ]:[14562K] [post][444] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/01202606291841281123063497774694/update-exec-state, trace = 00-4a775355394f72613048447500000000-12010edeca26acfc-01
2026-06-29 18:41:48.036 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[15164K] [post][444] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/01202606291841281123063497774694/update-exec-state}
2026-06-29 18:41:49.148 23559-23559 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23559, ]:[15456K] [post][445] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/heartbeat, trace = 00-4c706a57303131316d796b3300000000-da67669d3e604fa1-01
2026-06-29 18:41:49.748 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12584K] Http: --> POST https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/heartbeat http/1.1
2026-06-29 18:41:49.748 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Authorization: IOTDEVICE-SHA256-SM2 device_sn="X8AZ112F2220337VHJH8EFA",nonce_str="uK2xw3E2PawNaa1HTC8392VYz34ImrA8",signature="CgExEoUBCh5UWllTMVdYMTI0NDIyRjg0MzNGQTAwMDAwMDAwMDAQAhjul4nSBioQ8CYlY7zIjTiROermukAJsjJHMEUCIHy1SDFd0d1+TrD8oONIfDDXGPJn9rap3Y07MJhRhTVLAiEA2NaIAj+JLf4Q9ErED5YnKjCTskxHu5cJOZbZ7j/kTMI4ABoORGV2aWNlQXV0aFNpZ24=",timestamp="1782729709"
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Accept: application/json
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: traceparent: 00-52537a354333566e5245415000000000-ff548c94960c68ae-01
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: x-ms-blob-type: BlockBlob
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Content-Type: application/json; charset=utf-8
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Content-Length: 62
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Host: device.gz-ty.palm.tencent.com
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Connection: Keep-Alive
2026-06-29 18:41:49.749 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: Accept-Encoding: gzip
2026-06-29 18:41:49.750 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12602K] Http: User-Agent: okhttp/4.10.0
2026-06-29 18:41:49.750 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[12654K] Http:
2026-06-29 18:41:50.065 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: <-- 200 OK https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/heartbeat (314ms)
2026-06-29 18:41:50.065 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: Date: Mon, 29 Jun 2026 10:41:51 GMT
2026-06-29 18:41:50.065 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: Content-Type: application/json
2026-06-29 18:41:50.065 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: Content-Length: 105
2026-06-29 18:41:50.065 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: Connection: keep-alive
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: grpc-metadata-content-type: application/grpc
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: grpc-metadata-otel_call_app: devicegateway
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18340K] Http: grpc-metadata-otel_call_ip: 10.105.48.53
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: grpc-metadata-otel_call_method: DeviceHeartbeat
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: grpc-metadata-otel_call_server: devicegateway
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: grpc-metadata-otel_call_service: DeviceGateway
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: x-biz-code: 0
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: x-envoy-upstream-service-time: 43
2026-06-29 18:41:50.066 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:41:50.067 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18356K] Http: X-WAF-UUID: a82ee01f5f876ee474179b70b8c3179a-4751ab088aaf4315f75aaef1d064c5f9
2026-06-29 18:41:50.067 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18408K] Http:
2026-06-29 18:41:50.068 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18425K] Http: {"code":0,"message":"OK","server_time":"1782729711","heartbeat_interval_sec":60,"need_config_sync":false}
2026-06-29 18:41:50.068 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[18425K] Http: <-- END HTTP (105-byte body)
2026-06-29 18:41:50.071 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[18491K] [post][445] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/heartbeat}
2026-06-29 18:41:59.704 23559-23734 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23734, ]:[15997K] [post][446] url = https://device.gz-ty.palm.tencent.com/instruction/cos/presign, trace = 00-6861696f7454694b356b4f5200000000-90f5c4b64fd1c3ac-01
2026-06-29 18:42:00.028 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16484K] Http: --> POST https://device.gz-ty.palm.tencent.com/instruction/cos/presign http/1.1
2026-06-29 18:42:00.029 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16484K] Http: Authorization: IOTDEVICE-SHA256-SM2 device_sn="X8AZ112F2220337VHJH8EFA",nonce_str="A71gziSLOincbcugVAPNPZWLSv2fWX7d",signature="CgExEoUBCh5UWllTMVdYMTI0NDIyRjg0MzNGQTAwMDAwMDAwMDAQAhj4l4nSBioQjCBzfruO2V+mSA/3rRrKbDJHMEUCIQDJdpMJMrh4Rb4ExMfN6VZoffS8VPFPaMlI9hG19HhC8AIgfg8Jz+mIrTxiqiwSemTAUzTMu0ps99bbSeLliP0viNE4ABoORGV2aWNlQXV0aFNpZ24=",timestamp="1782729719"
2026-06-29 18:42:00.029 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16484K] Http: Accept: application/json
2026-06-29 18:42:00.029 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: traceparent: 00-4a465166546c647232644a4200000000-c097b7fae51222c6-01
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: x-ms-blob-type: BlockBlob
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: Content-Type: application/json; charset=utf-8
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: Content-Length: 88
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: Host: device.gz-ty.palm.tencent.com
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: Connection: Keep-Alive
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: Accept-Encoding: gzip
2026-06-29 18:42:00.030 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http: User-Agent: okhttp/4.10.0
2026-06-29 18:42:00.031 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16500K] Http:
2026-06-29 18:42:00.141 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16550K] Http: <-- 503 Service Unavailable https://device.gz-ty.palm.tencent.com/instruction/cos/presign (109ms)
2026-06-29 18:42:00.141 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16550K] Http: Date: Mon, 29 Jun 2026 10:42:01 GMT
2026-06-29 18:42:00.141 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16550K] Http: Content-Length: 0
2026-06-29 18:42:00.141 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16550K] Http: Connection: keep-alive
2026-06-29 18:42:00.141 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16550K] Http: X-WAF-UUID: 7766f1ddc5eb6e6aca1f6d2b61901cb1-7446d169c6eee3cd85a8196bd955aef2
2026-06-29 18:42:00.141 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:31525, ]:[16550K] Http: <-- END HTTP (0-byte body)
2026-06-29 18:42:00.142 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[16566K] [post][446] onResponse: Response{protocol=http/1.1, code=503, message=Service Unavailable, url=https://device.gz-ty.palm.tencent.com/instruction/cos/presign}
2026-06-29 18:42:01.218 23559-23735 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23735, ]:[17324K] [post][447] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/01202606291841281123063497774694/update-exec-state, trace = 00-366d5358586d4f4a315a475600000000-67c794ddd16582a4-01
2026-06-29 18:42:01.904 23559-31525 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:31525, ]:[11381K] [post][447] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/01202606291841281123063497774694/update-exec-state}
