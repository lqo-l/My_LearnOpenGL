# 编译单个目标的脚本
param(
    [Parameter(Mandatory=$true)]
    [string]$Target
)

# 如果build目录不存在，先配置CMake
if (!(Test-Path "build")) {
    Write-Host "首次构建，正在配置CMake..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "build"
    Set-Location build
    cmake -G "MinGW Makefiles" ..
    Set-Location ..
}

# 进入构建目录并编译指定目标
Set-Location build
Write-Host "正在编译 $Target ..." -ForegroundColor Green
mingw32-make $Target

Set-Location ..

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ 编译成功！" -ForegroundColor Green
} else {
    Write-Host "✗ 编译失败" -ForegroundColor Red
}


