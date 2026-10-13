# fmsx

[返回知识点清单](../知识点清单.md) | [查看 launcher](launcher.md) | [查看 retro-core](retro-core.md) | [查看 prboom-go](prboom-go.md) | [查看 gwenesis](gwenesis.md) | [查看 txt-reader](txt-reader.md)

## 1. 功能定位

`fmsx` 是 MSX 模拟器应用，支持 MSX ROM 和 DSK 磁盘镜像。

主要功能：

- 运行 MSX、MSX1 和 MSX2 软件
- 加载 MSX BIOS
- 加载 ROM、MX1、MX2 和 DSK 文件
- 处理掌机按键、虚拟键盘和 MSX 键盘映射
- 输出 PAL565 双缓冲画面
- 将 fMSX 音频连接到 Retro-Go 音频队列
- 支持状态保存、恢复、重置和截图

## 2. 启动流程

入口是 [`fmsx/main/main.c`](../../fmsx/main/main.c)。

1. 注册状态、截图、重置、重绘和选项处理器。
2. 调用 `rg_system_init(AUDIO_SAMPLE_RATE, &handlers, NULL)`。
3. 设置 fMSX tick rate。
4. 创建两个 256×228 的 `RG_PIXEL_565_BE` Surface。
5. 从设置中读取输入和画面裁剪选项。
6. 检查 `/sd/bios/msx` 下的 MSX BIOS 文件。
7. 根据扩展名构造 ROM 或 DSK 的 fMSX 启动参数。
8. 创建音频任务。
9. 调用 `fmsx_main(argc, argv)` 进入 MSX 模拟器。
10. fMSX 返回后调用 `rg_system_exit()` 返回 launcher。

常用启动参数包括：

```text
-ram 2
-vram 2
-skip 50
-home <BIOS 目录>
-joy 1
-diska <DSK 文件>
```

## 3. BIOS 和资源

默认 BIOS 目录：

```text
/sd/bios/msx/
```

常见 BIOS 文件：

- `MSX.ROM`
- `MSX2.ROM`
- `MSX2EXT.ROM`
- `DISK.ROM`
- `MSXDOS2.ROM`

ROM 目录由启动器注册为 `/sd/roms/msx`。

## 4. 内部功能

主要实现位于 [`fmsx/main/main.c`](../../fmsx/main/main.c)：

- `ProcessEvents()`：处理手柄、菜单、虚拟键盘和 MSX 键盘映射。
- `InitMachine()`：初始化视频、调色板、音频和状态记录器。
- 双缓冲 Surface：避免画面更新时闪烁。
- `PendingLoadSTA`：在模拟器初始化后延迟恢复存档。
- 音频任务：把 fMSX 的音频数据提交到 `rg_audio`。

fMSX 核心代码位于 [`fmsx/components/fmsx`](../../fmsx/components/fmsx)，其中 [`fmsx/components/fmsx/msxfix.c`](../../fmsx/components/fmsx/msxfix.c) 包含针对 Retro-Go/ESP-IDF 的兼容封装。

## 5. 与 launcher 的连接

启动器注册 MSX：

```c
application("MSX", "msx", "rom mx1 mx2 dsk", "fmsx", 0);
```

启动流程：

1. launcher 扫描 `/sd/roms/msx`。
2. 用户选择 ROM 或 DSK 文件。
3. launcher 写入 `fmsx` 分区、系统名称和 ROM 路径。
4. fMSX 读取 `app->romPath`。
5. 根据文件类型构造参数并启动核心。
6. 退出后返回 launcher。

## 6. 存档和按键

- 公共 Resume/Save 菜单通过 `rg_emu_get_path()` 生成存档路径。
- MSX 选项由 `options_handler()` 提供。
- 手柄按键通过 `ProcessEvents()` 转换为 MSX 输入。
- 虚拟键盘用于输入 MSX 特定按键。

## 7. 构建配置

顶层 [`fmsx/CMakeLists.txt`](../../fmsx/CMakeLists.txt) 引入 `main`、`retro-go` 和 `fmsx` 组件。fMSX 依赖公共 `retro-go` 提供显示、输入、音频、存储、GUI 和应用生命周期。

## 8. 相关文档

- [返回知识点清单](../知识点清单.md)
- [启动器](launcher.md)
- [编译和烧录说明](../../BUILDING.md)
