# CMake 构建脚本 - PowerShell

# 创建构建目录
if (!(Test-Path "build")) { 
    New-Item -ItemType Directory -Path "build"
}
Set-Location build # 进入构建目录

cmake -G "MinGW Makefiles" .. # 运行CMake配置 (使用MinGW Makefiles)
cmake --build . # 编译. == mingw32-make 

Set-Location ..
Write-Host "构建完成！可执行文件在 build 目录中" -ForegroundColor Green
