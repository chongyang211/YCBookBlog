帮我写一份刷掌支付的技术方案，你需要尽可能多的扫描代码，相关的代码和文件都需要扫描，技术方案要求详细、完整，而且有架构图、时序图等图表，图片使用mermaid画，最终结果需要输出到一份markdown文档中。


下面是一些帮助：

1.首先，调用刷掌WcPalmManager初始化，设置多种刷掌监听，然后开启刷掌
2.开启刷掌和停止刷掌使用handler消息机制
3.在刷掌PalmClient.Callback回调中做各种业务逻辑处理
4.在onSessionFinished回调中处理AEON_PAY业务
5.在AEON_PAY业务成功后，开始调用支付的请求接口




