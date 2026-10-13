# launcher

[返回知识点清单](../知识点清单.md) | [查看 retro-core](retro-core.md) | [查看 gwenesis](gwenesis.md) | [查看 prboom-go](prboom-go.md) | [查看 fmsx](fmsx.md) | [查看 txt-reader](txt-reader.md)

## 1. 功能定位

`launcher` 是 Retro-Go 的主启动器，负责把 SD 卡上的 ROM、电子书和其他资源展示给用户，并根据用户选择切换到对应应用。

主要功能：

- 初始化主界面和主题
- 扫描 ROM、存档、封面和缓存
- 管理应用 Tab、文件列表、最近使用和收藏
- 显示应用是否已编译并存在对应分区
- 启动模拟器或 TXT 阅读器
- 管理系统设置、游戏菜单和恢复启动
- 在启用网络时提供 WebUI 和更新功能

## 2. 启动流程

入口是 [`launcher/main/main.c`](../../launcher/main/main.c) 的 `app_main()`。

基本流程：

1. 创建 `rg_handlers_t`，注册事件、选项和关于页面处理函数。
2. 调用 `rg_system_init(32000, &handlers, NULL)` 初始化公共运行时。
3. 将 `configNs` 设置为 `launcher`，并标记为启动器。
4. 检查 SD 卡是否准备完成。
5. 创建缓存和配置目录，执行旧版本数据迁移。
6. 初始化 GUI、应用列表、书签、网络和 WebUI。
7. 进入 `retro_loop()`，持续读取按键、处理菜单并刷新界面。

## 3. 内部功能文件

| 文件 | 功能 |
|---|---|
| [`launcher/main/main.c`](../../launcher/main/main.c) | 启动器入口、主循环、系统事件和输入处理 |
| [`launcher/main/applications.c`](../../launcher/main/applications.c) | 应用注册、ROM 扫描、存档扫描、封面和 CRC 缓存、启动应用 |
| [`launcher/main/applications.h`](../../launcher/main/applications.h) | `retro_app_t`、`retro_file_t` 等应用数据结构 |
| [`launcher/main/gui.c`](../../launcher/main/gui.c) | Tab、列表、预览、主题和主界面绘制 |
| [`launcher/main/browser.c`](../../launcher/main/browser.c) | 文件浏览器和目录扫描 |
| [`launcher/main/bookmarks.c`](../../launcher/main/bookmarks.c) | 收藏和最近使用记录 |
| [`launcher/main/webui.c`](../../launcher/main/webui.c) | HTTP 文件服务和远程操作界面 |
| [`launcher/main/updater.c`](../../launcher/main/updater.c) | 在线更新和固件更新逻辑 |
| [`launcher/main/images.c`](../../launcher/main/images.c) | 由主题图片生成的 C 资源 |

## 4. 应用注册与 ROM 映射

应用注册主要在 `applications_init()` 中完成。每个应用会定义显示名、短名称、ROM 扩展名、目标分区和资源路径。

| 短名称 | 目标分区 | ROM 目录 | 常见扩展名 |
|---|---|---|---|
| `nes` | `retro-core` | `/sd/roms/nes` | `nes`、`fc`、`fds`、`nsf`、`zip` |
| `snes` | `retro-core` | `/sd/roms/snes` | `smc`、`sfc`、`zip` |
| `gb`、`gbc` | `retro-core` | `/sd/roms/gb`、`/sd/roms/gbc` | `gb`、`gbc`、`zip` |
| `gw` | `retro-core` | `/sd/roms/gw` | `gw` |
| `sms`、`gg` | `retro-core` | 对应 ROM 目录 | `sms`、`sg`、`gg`、`zip` |
| `pce` | `retro-core` | `/sd/roms/pce` | `pce`、`zip` |
| `lnx` | `retro-core` | `/sd/roms/lnx` | `lnx` |
| `md` | `gwenesis` | `/sd/roms/md` | `md`、`gen`、`bin`、`zip` |
| `doom` | `prboom-go` | `/sd/roms/doom` | `wad`、`zip` |
| `msx` | `fmsx` | `/sd/roms/msx` | `rom`、`mx1`、`mx2`、`dsk` |
| `txt` | `txt-reader` | `/sd/roms/txt` | `txt` |

## 5. 启动其他应用

用户选择文件后，`application_start()` 会：

1. 取得目标分区名、应用短名称和 ROM 路径。
2. 根据启动设置决定是否恢复存档槽。
3. 把当前文件加入最近使用记录。
4. 调用 `rg_system_switch_app(part, name, path, flags)`。
5. 公共系统保存启动参数并重启 ESP32。

目标应用启动后通过 `rg_system_init()` 读取这些参数。应用退出时通常调用 `rg_system_exit()` 返回启动器。

## 6. 构建依赖

顶层 [`launcher/CMakeLists.txt`](../../launcher/CMakeLists.txt) 使用：

```cmake
set(COMPONENTS "main retro-go wifi esp_http_server")
```

启动器依赖公共 `retro-go`、Wi-Fi 和 `esp_http_server`。图片资源修改后，需要运行 [`tools/gen_images.py`](../../tools/gen_images.py) 更新 `launcher/main/images.c`。

## 7. 相关文档

- [硬件和公共驱动](../知识点清单.md#硬件驱动位置)
- [编译和烧录说明](../../BUILDING.md)
- [主题说明](../../THEMING.md)
- [翻译说明](../../LOCALIZATION.md)
