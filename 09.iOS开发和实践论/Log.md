Last login: Tue Jun  3 19:35:57 on ttys012

The default interactive shell is now zsh.
To update your account to use zsh, please run `chsh -s /bin/zsh`.
For more details, please visit https://support.apple.com/kb/HT208050.
CHONGYYANG-MB1:~ yangchong$ cd /Users/yc/ugit/WeCardPay/WecardPayPodObjc/WecardPaySdk
CHONGYYANG-MB1:WecardPaySdk yangchong$ bash xcframework_build.sh
xcodebuild device
Command line invocation:
/Applications/Xcode.app/Contents/Developer/usr/bin/xcodebuild archive -scheme AirPalmLib -configuration Release -destination generic/platform=iOS -archivePath ./build/WecardPaySdk.framework-iphoneos.xcarchive SKIP_INSTALL=NO BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

User defaults from command line:
IDEArchivePathOverride = /Users/yc/ugit/WeCardPay/WecardPayPodObjc/WecardPaySdk/build/WecardPaySdk.framework-iphoneos.xcarchive
IDEPackageSupportUseBuiltinSCM = YES

Build settings from command line:
BUILD_LIBRARIES_FOR_DISTRIBUTION = YES
SKIP_INSTALL = NO

2025-06-03 19:41:37.917 xcodebuild[56008:12600127] Writing error result bundle to /var/folders/f1/12b2l4wx1cb_js9wm1zy_8z00000gn/T/ResultBundle_2025-03-06_19-41-0037.xcresult
xcodebuild: error: The project named "WecardPaySdk" does not contain a scheme named "AirPalmLib". The "-list" option can be used to find the names of the schemes in the project.
create xcframework
error: the path does not point to a valid framework: /Users/yc/ugit/WeCardPay/WecardPayPodObjc/WecardPaySdk/build/WecardPaySdk.framework-iphoneos.xcarchive/Products/Library/Frameworks/WecardPaySdk.framework
zipping...
zip warning: name not matched: WecardPaySdk.xcframework

zip error: Nothing to do! (try: zip -r WecardPaySdk.xcframework.zip . -i WecardPaySdk.xcframework)
CHONGYYANG-MB1:WecardPaySdk yangchong$ 
