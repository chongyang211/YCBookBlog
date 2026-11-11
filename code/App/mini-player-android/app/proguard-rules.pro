# MiniPlayer Android · ProGuard 规则

# Room 生成的实现类（运行时反射调用）
-keep class * extends androidx.room.RoomDatabase { <init>(); }
-keep @androidx.room.Entity class * { *; }

# Media3（官方自带 consumer rules，这里兜底）
-dontwarn androidx.media3.**

# MediaMetadataRetriever 常量内联保护
-keepclassmembers class android.media.MediaMetadataRetriever { *; }
