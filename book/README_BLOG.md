2026-06-29 18:12:43.243 23559-23735 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23735, ]:[11605K] [get][331] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/execute-list, trace = 00-4575654172714163674e594f00000000-d8fc6850ad364fb4-01
2026-06-29 18:12:44.571 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:27278, ]:[9701K] [get][331] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/instructions/execute-list}}
2026-06-29 18:13:00.091 29516-29516 UserLogUploadHandler    com.tencent.palm.wepay               I  [:29516, ]:[4479K] handle: date=20260629, eventName=uploadLog_15_mqz27l16
2026-06-29 18:13:00.098 23559-23762 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:23762, ]:[9287K] start: date = 20260629
2026-06-29 18:13:00.098 23559-23762 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:23762, ]:[9319K] createInstruction: POST /iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions
2026-06-29 18:13:00.143 23559-23762 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:23762, ]:[10677K] [post][332] url = https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions, trace = 00-4537444f383461464871344300000000-fb30b5f92781908f-01
2026-06-29 18:13:07.227 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8792K] Http: --> POST https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions http/1.1
2026-06-29 18:13:07.228 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8792K] Http: Authorization: IOTDEVICE-SHA256-SM2 device_sn="X8AZ112F2220337VHJH8EFA",nonce_str="ZhevsYq8mHC9ChI8wWzsdizAKXx2UXrl",signature="CgExEoQBCh5UWllTMVdYMTI0NDIyRjg0MzNGQTAwMDAwMDAwMDAQAhitionSBioQrblLJcOie+pQZRq7ni7ldDJGMEQCIAQmDZDz85gpgHQoT+uGkoz01e/XcIHcOIKwAfzzNxeLAiAezzcOi3zghqvurjnrLtzDZHYkD2Ahecs6jrk6a9u9aTgAGg5EZXZpY2VBdXRoU2lnbg==",timestamp="1782727980"
2026-06-29 18:13:07.228 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8792K] Http: Accept: application/json
2026-06-29 18:13:07.228 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8792K] Http: traceparent: 00-31744b4b70657445467a705500000000-1e956ab91decf562-01
2026-06-29 18:13:07.228 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8792K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: x-ms-blob-type: BlockBlob
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: Content-Type: application/json; charset=utf-8
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: Content-Length: 108
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: Host: device.gz-ty.palm.tencent.com
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: Connection: Keep-Alive
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: Accept-Encoding: gzip
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http: User-Agent: okhttp/4.10.0
2026-06-29 18:13:07.229 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[8808K] Http:
2026-06-29 18:13:07.495 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11253K] Http: <-- 200 OK https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions (265ms)
2026-06-29 18:13:07.495 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11254K] Http: Date: Mon, 29 Jun 2026 10:13:08 GMT
2026-06-29 18:13:07.496 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11254K] Http: Content-Type: application/json
2026-06-29 18:13:07.496 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11254K] Http: Content-Length: 53
2026-06-29 18:13:07.496 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11254K] Http: Connection: keep-alive
2026-06-29 18:13:07.496 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11254K] Http: grpc-metadata-content-type: application/grpc
2026-06-29 18:13:07.497 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11338K] Http: grpc-metadata-otel_call_app: devicegateway
2026-06-29 18:13:07.498 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11339K] Http: grpc-metadata-otel_call_ip: 10.105.48.39
2026-06-29 18:13:07.498 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11339K] Http: grpc-metadata-otel_call_method: CreateLogUploadInstruction
2026-06-29 18:13:07.498 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11339K] Http: grpc-metadata-otel_call_server: devicegateway
2026-06-29 18:13:07.498 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11355K] Http: grpc-metadata-otel_call_service: DeviceGateway
2026-06-29 18:13:07.498 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11356K] Http: x-envoy-upstream-service-time: 36
2026-06-29 18:13:07.499 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11357K] Http: x-zerus-env: vt-66q19zaggdknr
2026-06-29 18:13:07.499 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11357K] Http: X-WAF-UUID: bdb5b0f7af68cdf98aad23f4dd1b1708-85808137386cf3991ebec70ab27a5acc
2026-06-29 18:13:07.500 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11357K] Http:
2026-06-29 18:13:07.500 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11357K] Http: {"instruction_no":"01202606291813089824585906742185"}
2026-06-29 18:13:07.500 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     D  [:27278, ]:[11373K] Http: <-- END HTTP (53-byte body)
2026-06-29 18:13:07.501 23559-27278 CloudRequestManager     com.tencent.wxpayface.iotservice     I  [:27278, ]:[11409K] [post][332] onResponse: Response{protocol=http/1.1, code=200, message=OK, url=https://device.gz-ty.palm.tencent.com/iot/devices/X8AZ112F2220337VHJH8EFA/log-upload-instructions}
2026-06-29 18:13:07.502 23559-27278 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:27278, ]:[11442K] createInstruction onSuccess: {"instruction_no":"01202606291813089824585906742185"}
2026-06-29 18:13:07.503 23559-27278 UserLogUpl...chestrator com.tencent.wxpayface.iotservice     I  [:27278, ]:[11475K] onInstructionAccepted: 01202606291813089824585906742185
2026-06-29 18:13:07.506 29516-29516 UserLogUploadHandler    com.tencent.palm.wepay               I  [:29516, ]:[5247K] onAccepted: 01202606291813089824585906742185