# MyLearnOpenGL
> learnopengl-cn.github.io 学习实践

## 环境
- windows
- MinGW(如果用VS的MSVC，需要重新编译lib/libglfw3.a)

## vscode构建说明
1. 全部构建：在**根目录**运行build.ps1以构建所有章节项目，可以在根CMakeLists.txt中设置部分章节不构建，如`set(CHAPTER_1_beginner_ENABLE ON)`.
2. 仅编译特定可执行文件：在**根目录**运行build_single.ps1 <executable_name>, 如`build_single.ps1 01_hello`。其中<executable_name>为可执行文件名，在子目录CMakeLists.txt中设置，如`add_executable(hello hello.cpp)`。

**注**：着色器文件和纹理素材文件在构建时复制到可执行文件目录下，程序采用相对于exe文件的路径访问这些资源。

