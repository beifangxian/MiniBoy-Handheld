# gwenesis

[返回知识点清单](../知识点清单.md) | [查看 launcher](launcher.md) | [查看 retro-core](retro-core.md) | [查看 prboom-go](prboom-go.md) | [查看 fmsx](fmsx.md) | [查看 txt-reader](txt-reader.md)

## 1. 功能定位

`gwenesis` 是 Sega Mega Drive / Genesis 模拟器应用。

主要功能：

- 模拟 68000 主 CPU
- 模拟 Z80 音频 CPU
- 模拟 VDP 视频系统
- 模拟 YM2612 和 SN76489 音频
- 加载普通 ROM 和 ZIP ROM
- 处理手柄输入、画面和音频
- 保存、恢复和截图
- 提供 YM2612、SN76489、Z80 开关

## 2. 启动流程

入口是 [`gwenesis/main/main.c`](../../gwenesis/main/main.c)。

1. 注册状态、重置、截图、重绘、事件和选项处理器。
2. 调用 `rg_system_init(AUDIO_SAMPLE_RATE / 2, &handlers, NULL)`。
3. 从 `NS_APP` 读取 YM2612、SN76489 和 Z80 开关。
4. 创建 320×241 的 PAL565 Surface，为模拟器保留 overdraw 空间。
5. 分配 VRAM。
6. 读取普通 ROM 或通过 `rg_storage_unzip_file()` 读取 ZIP ROM。
7. 调用 `load_cartridge()`、`power_on()` 和 `reset_emulation()`。
8. 如果是恢复启动，调用 `rg_emu_load_state()`。
9. 设置 60 Hz tick rate，进入 CPU、VDP、输入和音频主循环。

## 3. 模拟器内部模块

代码位于 [`gwenesis/components/gwenesis`](../../gwenesis/components/gwenesis)：

| 模块目录 | 功能 |
|---|---|
| `src/cpus/M68K` | Motorola 68000 主 CPU |
| `src/cpus/Z80` | Z80 音频 CPU |
| `src/vdp` | VDP、VRAM、扫描线和画面渲染 |
| `src/sound` | YM2612、SN76489 和音频混合 |
| `src/savestate` | 模拟器状态序列化 |
| `src/io` | 手柄和外围输入 |
| `src/bus` | 系统总线和设备连接 |

应用适配层在 [`gwenesis/main/main.c`](../../gwenesis/main/main.c)：

- `gwenesis_io_get_buttons()`：把 Retro-Go 按键转换为模拟器输入。
- `gwenesis_vdp_render_line()`：逐行渲染视频。
- `saveGwenesisStateGetBuffer()`：导出模拟器状态。
- `saveGwenesisStateSetBuffer()`：恢复模拟器状态。

## 4. 显示、音频和输入

- 显示使用 `rg_surface_t` 和 `rg_display_submit()`。
- 音频使用 `rg_audio_submit()`。
- 输入使用 `rg_input_read_gamepad()`。
- `RG_KEY_MENU` 打开统一游戏菜单。
- `RG_KEY_OPTION` 打开 Mega Drive 模拟器选项。
- 调色板使用 CRAM565 转换到屏幕格式。

## 5. 与 launcher 的连接

启动器注册 Mega Drive：

```c
application("Sega Mega Drive", "md", "md gen bin zip", "gwenesis", 0);
```

启动流程：

1. launcher 扫描 `/sd/roms/md`。
2. 用户选择 ROM。
3. launcher 通过 `rg_system_switch_app()` 写入 `gwenesis` 分区和 ROM 路径。
4. `gwenesis` 通过 `rg_system_init()` 读取 `app->romPath`。
5. 模拟器加载卡带并开始运行。
6. 退出时通过 `rg_system_exit()` 返回 launcher。

## 6. 存档机制

应用同时支持两层状态接口：

- `rg_emu_load_state()` / `rg_emu_save_state()`：接入公共游戏存档菜单。
- `saveGwenesisStateGetBuffer()` / `saveGwenesisStateSetBuffer()`：负责 gwenesis 内部状态序列化。

## 7. 构建配置

顶层 [`gwenesis/CMakeLists.txt`](../../gwenesis/CMakeLists.txt)：

```cmake
set(COMPONENTS "main retro-go gwenesis")
set(RG_ENABLE_NETWORKING 0)
```

`gwenesis` 组件依赖 `retro-go`，并在 [`gwenesis/components/gwenesis/CMakeLists.txt`](../../gwenesis/components/gwenesis/CMakeLists.txt) 中加入 CPU、VDP、IO、声音和存档源码目录。

## 8. 相关文档

- [返回知识点清单](../知识点清单.md)
- [启动器](launcher.md)
- [编译和烧录说明](../../BUILDING.md)
