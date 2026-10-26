# 目录
- [构建 Retro-Go](#prerequisites)
- [移植 Retro-Go](#porting-retro-go)
- [IDE 支持](#ide-support)


# 构建 Retro-Go

## 前提条件
你需要一个可正常工作的 [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.5.5/esp32/get-started/index.html) 5.5.5 安装。本仓库当前目标为 ESP32-S3 的 `t320-s3`。

在 Windows 上请先打开 ESP-IDF 5.5.5 命令行，或在当前终端执行对应的 `export.bat`，确保 `IDF_PATH` 已设置。

### ESP-IDF 补丁
为了实现完整功能，可能需要给 esp-idf 打补丁。补丁位于 `tools/patches` 中，可以应用到你的全局 esp-idf 安装上，它们不会破坏你的其他项目/设备。
- `sdcard-fix`：此补丁对于 ODROID-GO（及其克隆版）是必需的。
- `panic-hook`：这是为了帮助用户报告 bug，详情请参见下方的 `捕获崩溃日志`。此补丁是可选的，但推荐使用。


## 获取代码

推荐使用 git，但你也可以从项目首页下载 zip 并解压（如果你愿意的话），Retro-Go 没有外部依赖。

retro-go 上通常有两个活跃的 git 分支：
- `master` 包含最近一次发布的代码，通常经过测试并已知可正常工作
- `dev` 包含开发中的代码，将在下一次发布时合并到 master，通常未经测试

`git clone -b <branch> https://github.com/ducalex/retro-go/`


## rg_tool.py
ESP-IDF 的 `idf.py` 本身无法在一个项目中管理多个应用（构建多应用镜像、烧录到正确的分区、单一构建命令等）。必须改用 `rg_tool.py`，它会向 `idf.py` 和其他工具传递正确的参数。

所有命令都应从仓库根目录执行。对于本项目，请始终指定 `--target=t320-s3`；串口操作还需要指定实际的 `--port=COMx`。

运行 `python rg_tool.py --help` 查看所有可用的标志和命令。


## 构建所有内容并生成固件镜像
在 Windows 上执行：

```bat
python rg_tool.py --target=t320-s3 build-img
```

这会生成 `retro-go_chaeng_t320-s3.img`，用于通过 USB 串口烧录。

需要干净重编译时执行：

```bat
python rg_tool.py --target=t320-s3 release
```

为了获得更小的构建，你也可以指定你想要的应用，例如仅启动器 + DOOM：
1. `python rg_tool.py --target=t320-s3 build-img launcher prboom-go`

请注意，名为 `retro-core` 的应用包含以下模拟器：NES、PCE、G&W、Lynx 和 SMS/GG/COL。因此，这些模拟器无法单独选择。将它们捆绑的原因很简单，就是为了节省体积：捆绑后它们仅占 700KB，而分开构建时几乎要占 3MB。


## 首次烧录镜像
一旦我们成功构建了镜像文件（`.img` 或 `.fw`），就必须将其烧录到设备上。

要使用 `rg_tool.py` 烧录 `.img` 文件，请运行：
```bat
python rg_tool.py --target=t320-s3 --port=COM23 install
```

要使用 `esptool.py` 烧录 `.img` 文件，请运行：
```bat
esptool.py write_flash --flash_size detect 0x0 retro-go_chaeng_t320-s3.img
```

要烧录 `.fw` 文件：

具体说明取决于你的设备，请参考 [README.md](README.md#installation)。


## 构建、烧录并监视单个应用以加快开发速度：
完整的 Retro-Go 镜像必须至少烧录一次（参考上一节），但在此之后，可以烧录并监视单个应用以加快开发速度。

1. 烧录：`python rg_tool.py --target=t320-s3 --port=COM3 flash prboom-go`
2. 监视：`python rg_tool.py --target=t320-s3 --port=COM3 monitor prboom-go`
3. 烧录后监视：`python rg_tool.py --target=t320-s3 --port=COM3 run prboom-go`


## Windows
在 Windows 上必须使用 `python rg_tool.py ...`，不要使用 `./rg_tool.py ...`。运行命令前，请先激活 ESP-IDF 环境，例如：

```bat
call C:\Espressif\frameworks\esp-idf-v5.5.5\export.bat
cd /d e:\ESP\esp_fork\retro-go_chaeng
python rg_tool.py --target=t320-s3 build-img
```


## 更改启动器的图像
启动器使用的所有图像（页眉、标志）都位于 `themes/default` 中。如果你编辑了它们，必须运行 `tools/gen_images.py` 脚本以重新生成 `launcher/main/images.c`。有关图像格式的详细信息，请参考 [THEMING.md](THEMING.md)。


## 更新翻译
在添加新的菜单项或屏幕消息时，它们应该被翻译。Retro-Go 使用类似 gettext 的系统，作为开发者，你唯一的任务是用 `_(...)` 包裹你的字符串。参考 [LOCALIZATION.md](LOCALIZATION.md) 了解如何添加实际的翻译。


## 捕获崩溃日志
当发生 panic 时，Retro-Go 有能力将调试信息保存到 `/sd/crash.log`。这为用户提供了一种简单的方式来恢复回溯（通常还有更多信息），而无需安装驱动程序和串口控制台软件。一个弱钩子被安装到 esp-idf panic 的 putchar 中，允许我们将每个字符保存到 RTC RAM 中。然后，在系统重置后，我们可以将该数据移动到 SD 卡。你可以在 tools/patches 中找到一个小的 esp-idf 补丁来启用此功能。

要解析回溯，你需要应用的 elf 文件。如果丢失了，你可以**使用相同的 esp-idf 和 retro-go 版本**重新构建应用来重新创建它。然后你可以运行 `xtensa-esp32-elf-addr2line -ifCe app-name/build/app-name.elf`。



# 移植 Retro-Go

## ESP32
移植到新 ESP32 设备的说明可以在 [PORTING.md](PORTING.md) 中找到。

## 其他架构
我不想在这个仓库中维护非 ESP32 的移植，但如果你告诉我，我可以做一些小改动来让你自己的移植更容易！Retro-Go 的绝对最低要求大致为：
- 处理器：200Mhz 32 位小端
- 内存：2MB
- 编译器：C99（以及 handy-go 所需的 C++03）

虽然所有应用都为了我们受限的需求进行了大量修改甚至重新设计，但我们特别注意将 Retro-Go 和 ESP32 特定的代码专门保留在它们的移植文件（main.c）中。这使得在你自己的代码库中重用它们非常容易！



# IDE 支持

## VS Code
Retro-Go 附带一个 VS Code 工作区文件，其中包含 C/C++ 扩展的配置。

要让智能感知（intellisense）正常工作，你必须在你的*全局*配置文件中定义一些路径：

````json
    "retro-go.sdk-path": "C:/espressif/frameworks/esp-idf-v5.0.4",
    "retro-go.tools-path": "C:/espressif/tools/xtensa-esp32-elf/esp-2021r2-patch3-8.4.0/xtensa-esp32-elf",
````

目前不支持 Clangd，因为我还没有找到一种方法让它在我们的多文件夹工作区中良好运行。