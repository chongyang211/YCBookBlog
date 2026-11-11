# ======================================================================
# native/scripts/package-native.ps1 —— 独立分发打包（Windows）
#
# 用法（PowerShell）：
#   .\scripts\package-native.ps1                # Release 纯净版
#   .\scripts\package-native.ps1 -TagLib        # 链 TagLib（需 VCPKG_ROOT）
#   .\scripts\package-native.ps1 -SysMedia      # 开系统媒体控制（SMTC）
#
# 产物（统一在 out/ 下）：
#   out\dist\include\mp_native.h
#   out\dist\bin\mp_native.dll
#   out\dist\lib\mp_native.lib
#   out\dist\lib\cmake\mp-native\*              # find_package 支持
# ======================================================================
param(
    [switch]$TagLib,
    [switch]$SysMedia
)

$ErrorActionPreference = "Stop"
Set-Location (Join-Path $PSScriptRoot "..")     # → mp-native\

# ---- 清理 ----
if (Test-Path out\build-dist) { Remove-Item -Recurse -Force out\build-dist }
if (Test-Path out\dist)       { Remove-Item -Recurse -Force out\dist }

# ---- 配置 ----
$cmakeArgs = @("-S", ".", "-B", "out\build-dist",
               "-DCMAKE_BUILD_TYPE=Release",
               "-DMP_EMBED_OUTPUT=OFF",
               "-DCMAKE_INSTALL_PREFIX=out/dist",
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
cmake --build out\build-dist --config Release -j
if ($LASTEXITCODE -ne 0) { throw "build failed" }
cmake --install out\build-dist
if ($LASTEXITCODE -ne 0) { throw "install failed" }

# ---- 冒烟验证 ----
$dll = "out\dist\bin\mp_native.dll"
if (-not (Test-Path $dll)) { throw "✗ $dll not found" }
$dumpbin = Get-Command dumpbin -ErrorAction SilentlyContinue
if ($dumpbin) {
    $exports = & dumpbin /EXPORTS $dll | Select-String "mp_version"
    if (-not $exports) { throw "✗ mp_version not exported" }
}

Write-Host ""
Write-Host "✓ packaged: mp-native\out\dist\" -ForegroundColor Green
Get-ChildItem -Recurse out\dist | ForEach-Object {
    Write-Host "    $($_.FullName.Replace((Get-Location).Path + '\out\dist\', ''))"
}
