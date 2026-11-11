# ======================================================================
# native/scripts/package-native.ps1 —— 独立分发打包（Windows）
#
# 用法（PowerShell）：
#   .\scripts\package-native.ps1                # Release 纯净版
#   .\scripts\package-native.ps1 -TagLib        # 链 TagLib（需 VCPKG_ROOT）
#   .\scripts\package-native.ps1 -SysMedia      # 开系统媒体控制（SMTC）
#
# 产物：
#   native\dist\include\mp_native.h
#   native\dist\bin\mp_native.dll
#   native\dist\lib\mp_native.lib
#   native\dist\lib\cmake\mp-native\*           # find_package 支持
#   native\dist\mp-native-<版本>-windows-x64.zip
# ======================================================================
param(
    [switch]$TagLib,
    [switch]$SysMedia
)

$ErrorActionPreference = "Stop"
Set-Location (Join-Path $PSScriptRoot "..")     # → native\

# ---- 版本号 ----
$version = (Select-String -Path CMakeLists.txt -Pattern 'VERSION ([0-9.]+)').Matches[0].Groups[1].Value

# ---- 清理 ----
if (Test-Path build-dist) { Remove-Item -Recurse -Force build-dist }
if (Test-Path dist)       { Remove-Item -Recurse -Force dist }

# ---- 配置 ----
$cmakeArgs = @("-S", ".", "-B", "build-dist",
               "-DCMAKE_BUILD_TYPE=Release",
               "-DMP_EMBED_OUTPUT=OFF",
               "-DCMAKE_INSTALL_PREFIX=dist",
               "-A", "x64")
if ($TagLib) {
    if (-not $env:VCPKG_ROOT) { throw "TagLib 需要 VCPKG_ROOT 环境变量（vcpkg install taglib:x64-windows）" }
    $cmakeArgs += @("-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake",
                    "-DMP_WITH_TAGLIB=ON")
}
if ($SysMedia) { $cmakeArgs += "-DMP_WITH_SYSMEDIA=ON" }

Write-Host ">> cmake $($cmakeArgs -join ' ')" -ForegroundColor Cyan
cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) { throw "configure failed" }

# ---- 构建 + 安装 ----
cmake --build build-dist --config Release -j
if ($LASTEXITCODE -ne 0) { throw "build failed" }
cmake --install build-dist
if ($LASTEXITCODE -ne 0) { throw "install failed" }

# ---- 冒烟验证 ----
$dll = "dist\bin\mp_native.dll"
if (-not (Test-Path $dll)) { throw "✗ $dll not found" }
$dumpbin = Get-Command dumpbin -ErrorAction SilentlyContinue
if ($dumpbin) {
    $exports = & dumpbin /EXPORTS $dll | Select-String "mp_version"
    if (-not $exports) { throw "✗ mp_version not exported" }
}

# ---- zip ----
$zipName = "mp-native-$version-windows-x64.zip"
Compress-Archive -Path "dist\*" -DestinationPath $zipName -Force

Write-Host ""
Write-Host "✓ packaged: native\$zipName" -ForegroundColor Green
Get-ChildItem -Recurse dist | ForEach-Object { Write-Host "    $($_.FullName.Replace((Get-Location).Path + '\dist\', ''))" }
