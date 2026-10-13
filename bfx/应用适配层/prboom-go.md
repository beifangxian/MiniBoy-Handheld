# prboom-go

[返回知识点清单](../知识点清单.md) | [查看 launcher](launcher.md) | [查看 retro-core](retro-core.md) | [查看 gwenesis](gwenesis.md) | [查看 fmsx](fmsx.md) | [查看 txt-reader](txt-reader.md)

## 1. 功能定位

`prboom-go` 是 Doom / Doom II 兼容应用，基于 PrBoom 引擎运行 WAD 文件。

主要功能：

- 加载 IWAD 和 PWAD
- 选择 Doom 基础数据文件
- 将 Doom 画面输出到 Retro-Go 屏幕
- 将掌机按键转换为 Doom 输入事件
- 混合音效和 OPL 音乐
- 使用统一游戏菜单、Gamma Boost 和系统设置
- 使用 Doom 自己的存档目录

## 2. 启动流程

入口是 [`prboom-go/main/main.c`](../../prboom-go/main/main.c)。

1. 创建 `rg_handlers_t`，注册重绘、输入、选项和截图等处理器。
2. 调用 `rg_system_init(AUDIO_SAMPLE_RATE, &handlers, NULL)`。
3. 设置 Doom 的 `TICRATE` 系统节拍。
4. 创建 PAL565 framebuffer，并根据屏幕尺寸设置 `SCREENWIDTH` 和 `SCREENHEIGHT`。
5. 使用 `is_iwad()` 判断当前 ROM 是 IWAD 还是 PWAD。
6. 如果当前文件不是 IWAD，通过文件选择器寻找 `/sd/roms/doom` 下的 IWAD。
7. 组装 `-save`、`-iwad` 和 `-file` 参数。
8. 调用 `Z_Init()` 和 `D_DoomMain()` 进入 PrBoom 主流程。

## 3. 平台适配接口

Doom 引擎通过 `I_*` 接口连接 Retro-Go：

| 接口 | 功能 |
|---|---|
| `I_FinishUpdate()` | 提交 Doom 视频帧 |
| `I_SetPalette()` | 设置 Doom 调色板 |
| `I_StartTic()` | 读取掌机按键并生成 `event_t` |
| `I_InitSound()` | 创建 Doom 音频任务 |
| `I_StartSound()` | 播放音效 |
| `I_PlaySong()` | 播放 OPL 音乐 |

关键实现集中在 [`prboom-go/main/main.c`](../../prboom-go/main/main.c)，Doom 引擎源码位于 [`prboom-go/components/prboom`](../../prboom-go/components/prboom)。

## 4. 文件和存档

- ROM 目录：`/sd/roms/doom`
- 存档目录：`/sd/saves/doom`
- IWAD：通常为 `doom.wad`、`doom2.wad` 等
- PWAD：由启动器选择或通过 Doom 参数加载

启动器把选中的 WAD 路径写入启动参数，`prboom-go` 再通过 `app->romPath` 读取。

## 5. 与 launcher 的连接

启动器中的应用注册类似：

```c
application("DOOM", "doom", "wad zip", "prboom-go", 0);
```

启动流程：

1. launcher 扫描 `/sd/roms/doom`。
2. 用户选择 WAD 文件。
3. launcher 调用 `rg_system_switch_app()`。
4. ESP32 切换到 `prboom-go` 分区并重启。
5. `prboom-go` 读取 WAD 路径并启动 Doom。
6. 退出时调用 `rg_system_exit()` 返回 launcher。

## 6. 存档和限制

Doom 通过 `-save` 参数使用自己的存档目录。当前统一的 `rg_handlers_t` 存档处理器不是 Doom 的主要存档路径，实际游戏存档由 PrBoom 管理。

## 7. 构建配置

顶层 [`prboom-go/CMakeLists.txt`](../../prboom-go/CMakeLists.txt)：

```cmake
set(COMPONENTS "main retro-go prboom")
set(RG_ENABLE_NETWORKING 0)
```

网络功能关闭，应用依赖：

- `retro-go`：显示、输入、音频、GUI、存储和系统生命周期
- `prboom`：Doom 引擎

## 8. 相关文档

- [返回知识点清单](../知识点清单.md)
- [启动器](launcher.md)
- [编译和烧录说明](../../BUILDING.md)
