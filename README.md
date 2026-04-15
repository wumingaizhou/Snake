## 程序介绍
项目基于 SFML 图形库实现，是一个带有菜单、暂停、帮助、关于、设置等界面的贪吃蛇小游戏。
当前版本还额外加入了历史前三高分记录，以及会随机刷新的限时高分小球玩法。

原版代码作者：[jhpy1024 / sfSnake](https://github.com/jhpy1024/sfSnake)
改进版代码作者：[JingYiJun / sfSnakePro](https://github.com/JingYiJun/sfSnakePro)

[查看修改日志](doc/ChangeLog.md)

[查看设计说明](doc/贪吃蛇游戏设计说明.md)

## 当前构建环境

当前项目已经迁移到 `SFML 3.0.2`，推荐在 Windows 下使用 `MSYS2 UCRT64` 环境构建和运行。

已验证可用的工具链组合：

- Windows
- MSYS2 UCRT64
- mingw-w64-ucrt-x86_64-gcc
- CMake
- MinGW Make
- SFML 3.0.2

说明：

- 项目现在使用动态链接的 SFML 运行时
- 构建完成后会自动把运行所需的 DLL 复制到 `build` 目录

## 如何安装依赖

### 1. 安装 MSYS2

先到官网下载安装：

[https://www.msys2.org/](https://www.msys2.org/)

安装完成后，按照官网文档完成初始化，并确保已经安装 `mingw-w64-ucrt-x86_64-gcc`。

### 2. 打开正确的终端

请使用 `MSYS2 UCRT64` 终端，不要使用普通的 `MSYS`、`CLANG64` 或其他不匹配的环境。

### 3. 安装项目依赖

在 `MSYS2 UCRT64` 终端中执行：

```bash
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-sfml
```

## 如何编译

进入项目根目录后，在 `MSYS2 UCRT64` 终端中执行：

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

编译成功后，会在 `build` 目录下生成：

- `main.exe`
- `assets/`
- 运行所需的 `SFML` 与相关依赖 `dll`

说明：

- `build` 更适合作为开发构建目录
- 真正用于分发时，推荐单独导出一个干净的 `release` 目录

## 如何导出 Release

推荐使用单独的 Release 构建目录和安装目录，不要直接把整个 `build` 目录发出去。

在 `MSYS2 UCRT64` 终端中执行：

```bash
cmake -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
cmake --install build-release --prefix release
```

执行完成后，`release` 目录中会包含：

- `sfSnakePro.exe`
- `app/`

其中：

- `sfSnakePro.exe` 是根目录启动器
- `app/main.exe` 是真正的游戏程序
- `app/assets/` 和 `app/*.dll` 是运行所需资源与依赖

启动方式：

```bash
./release/sfSnakePro.exe
```

如果是在 Windows 资源管理器中操作，也可以直接双击：

```text
release\sfSnakePro.exe
```

如果需要发给别人，通常直接压缩整个 `release` 目录即可，不要单独移动 `app` 里的文件。

## 如何启动

开发阶段最简单的启动方式，仍然是直接运行 `build/main.exe`。

如果你当前就在 `MSYS2 UCRT64` 终端的项目根目录，可以执行：

```bash
./build/main.exe
```

如果你是在 Windows 资源管理器中操作，也可以直接双击：

```text
build\main.exe
```

注意：

- 请从项目根目录完成编译，确保 `build` 目录中的资源和 DLL 被正确复制
- 如果修改了代码，通常只需要重新执行 `cmake --build build`
- 如果更换了构建目录或工具链，建议重新执行一次 `cmake -B build -G "MinGW Makefiles"`
- 正式发布时，建议使用 `release\sfSnakePro.exe`，不要直接分发开发用的 `build` 目录
- 游戏运行后会在当前运行目录生成或更新 `highscores.txt`，用于保存历史前三高分

## 操作方式

- 点击或长按任意位置，改变蛇的方向
- 按 `W/A/S/D` 或方向键控制转向
- 蛇碰到边界后会从另一侧穿出
- 按住空格可以加速
- 场上会随机出现限时高分小球，吃到后可获得比普通小球更高的分数

## 特点

相比原版和大部分走直线的贪吃蛇，界面更完整，动画更流畅，操作也更直观。

- 支持菜单、暂停、帮助、关于、设置等完整界面流程
- 支持历史前三高分记录，重新启动游戏后依然保留
- 保留多种普通果子得分机制，并新增限时高分小球玩法
- 高分小球拥有更醒目的特殊样式，方便玩家快速识别和追逐

## 截图

![MenuScreenShot](doc/image/MenuScreenShot.png "游戏彩蛋截图")

![GameScreenShot](doc/image/GameScreenShot.png "游戏主界面截图")

![OptionScreenShot](doc/image/OptionScreenShot.png)

## 下载

如果使用 Release 版本，下载后解压并直接运行即可。

## License

The MIT License (MIT)  
Copyright © 2022 <JingYiJun>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
