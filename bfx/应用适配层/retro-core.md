# retro-core

[返回知识点清单](../知识点清单.md) | [查看 launcher](launcher.md) | [查看 gwenesis](gwenesis.md) | [查看 prboom-go](prboom-go.md) | [查看 fmsx](fmsx.md) | [查看 txt-reader](txt-reader.md)

## 1. 功能定位

`retro-core` 是多个主机模拟器共用的一个 ESP32 应用分区。它不是单一模拟器，而是由一个 `app_main()` 根据启动参数选择具体模拟器入口。

包含的系统：

- Game Boy / Game Boy Color
- NES / Famicom / NSF
- PC Engine
- Sega Master System / Game Gear / ColecoVision
- Game & Watch
- Super Nintendo
- Atari Lynx

将多个核心合并到一个应用中，可以减少独立 App 分区和公共运行时带来的空间开销。

## 2. 启动和分派流程

入口是 [`retro-core/main/main.c`](../../retro-core/main/main.c)。

1. 调用 `rg_system_init(AUDIO_SAMPLE_RATE, NULL, NULL)`。
2. 读取 `app->configNs`，它由启动器写入，例如 `nes`、`snes` 或 `gbc`。
3. 根据名称进入对应的 `*_main()` 函数。
4. 具体模拟器使用 `rg_system_reinit()` 重新设置采样率、事件处理器和模拟器配置。
5. 模拟器运行主循环，处理显示、输入、音频和存档。
6. 退出时通过 `rg_system_exit()` 返回 launcher。

主要分派关系：

```text
 gb / gbc       -> gbc_main()
 nes            -> nes_main()
 pce            -> pce_main()
 sms / gg / col -> sms_main()
 gw             -> gw_main()
 snes           -> snes_main()
 lnx            -> lynx_main()
```

## 3. 模拟器入口文件

| 系统 | 入口文件 | 内部核心 |
|---|---|---|
| GB / GBC | [`retro-core/main/main_gbc.c`](../../retro-core/main/main_gbc.c) | `gnuboy` |
| NES | [`retro-core/main/main_nes.c`](../../retro-core/main/main_nes.c) | `nofrendo` |
| SMS / GG / ColecoVision | [`retro-core/main/main_sms.c`](../../retro-core/main/main_sms.c) | `smsplus` |
| PC Engine | [`retro-core/main/main_pce.c`](../../retro-core/main/main_pce.c) | `pce-go`、PSG |
| Game & Watch | [`retro-core/main/main_gw.c`](../../retro-core/main/main_gw.c) | `gw-emulator` |
| SNES | [`retro-core/main/main_snes.c`](../../retro-core/main/main_snes.c) | `snes9x` |
| Atari Lynx | [`retro-core/main/main_lynx.cpp`](../../retro-core/main/main_lynx.cpp) | `handy` |
| 公共声明 | [`retro-core/main/shared.h`](../../retro-core/main/shared.h) | 入口、音频和共享内存声明 |

各平台适配层负责把模拟器核心的回调接到 Retro-Go：

- 视频回调提交到 `rg_surface_t`。
- 音频回调提交到 `rg_audio_submit()`。
- 输入读取使用 `rg_input_read_gamepad()`。
- 存档使用统一的 `rg_emu_*` 接口。
- 菜单和选项使用公共 GUI 接口。

## 4. 各模拟器功能

### GB / GBC

[`main_gbc.c`](../../retro-core/main/main_gbc.c) 负责：

- ROM 或 ZIP ROM 加载
- BIOS、SRAM 和 RTC 处理
- Game Boy / Game Boy Color 调色板
- 视频和音频回调
- 状态保存和恢复

### NES

[`main_nes.c`](../../retro-core/main/main_nes.c) 负责：

- NES、Famicom、FDS 和 NSF 文件
- FDS BIOS 和卡带加载
- Overscan、自动裁剪和画面显示
- 按键映射和音频输出
- NES 存档状态

### SMS / GG / ColecoVision

[`main_sms.c`](../../retro-core/main/main_sms.c) 根据启动名称区分 SMS、Game Gear 和 ColecoVision，并处理：

- PAL/NTSC 刷新率
- 不同系统的输入映射
- 视频帧输出
- 音频混合
- 游戏状态

### PC Engine

[`main_pce.c`](../../retro-core/main/main_pce.c) 通过 `pce-go` 和 PSG 核心实现：

- ROM 加载
- 帧缓冲和 VSync
- 手柄输入
- PSG 音频
- 存档状态

### Game & Watch

[`main_gw.c`](../../retro-core/main/main_gw.c) 负责：

- ROM 加载
- RTC 同步
- Game & Watch 配置
- 画面和输入
- 状态保存和恢复

### SNES

[`main_snes.c`](../../retro-core/main/main_snes.c) 负责：

- ROM 加载和初始化
- 图形、音频和输入回调
- Type A、Type B、Type C 按键布局
- 游戏菜单和状态保存

### Atari Lynx

[`main_lynx.cpp`](../../retro-core/main/main_lynx.cpp) 使用 Handy 的 `CSystem`，负责：

- ROM 加载
- 旋转和方向键重映射
- 帧缓冲显示
- 输入和音频
- `ContextSave()` / `ContextLoad()` 状态保存

## 5. 构建配置

顶层 [`retro-core/CMakeLists.txt`](../../retro-core/CMakeLists.txt) 将所有核心加入同一个应用：

```cmake
set(COMPONENTS "main retro-go nofrendo gnuboy pce-go gw-emulator handy smsplus snes9x")
set(RG_ENABLE_NETWORKING 0)
```

网络功能在这个应用中关闭，以节省空间。各模拟器组件位于 `retro-core/components/`，通常通过 `COMPONENT_REQUIRES "retro-go"` 使用公共平台层。

## 6. 与 launcher 的连接

启动器将多个 ROM 类型映射到 `retro-core` 分区，并把模拟器名称写入启动参数。`retro-core` 不需要再次扫描 ROM 目录，只读取 `app->romPath` 和 `app->configNs`。

相关公共代码：

- [`components/retro-go/rg_system.c`](../../components/retro-go/rg_system.c)
- [`components/retro-go/rg_input.c`](../../components/retro-go/rg_input.c)
- [`components/retro-go/rg_display.c`](../../components/retro-go/rg_display.c)
- [`components/retro-go/rg_audio.c`](../../components/retro-go/rg_audio.c)
- [`launcher/main/applications.c`](../../launcher/main/applications.c)

## 7. 相关文档

- [返回知识点清单](../知识点清单.md)
- [启动器](launcher.md)
- [编译和烧录说明](../../BUILDING.md)
