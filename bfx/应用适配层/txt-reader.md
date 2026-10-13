# txt-reader

[返回知识点清单](../知识点清单.md) | [查看 launcher](launcher.md) | [查看 retro-core](retro-core.md) | [查看 prboom-go](prboom-go.md) | [查看 gwenesis](gwenesis.md) | [查看 fmsx](fmsx.md)

## 1. 功能定位

`txt-reader` 是当前项目新增的 TXT 电子书应用。它把每本 TXT 文件当作一个可启动项目，使用与游戏相似的启动、存档和菜单机制。

主要功能：

- 支持 UTF-8 和 GBK/GB2312 文本
- 支持中文书名和 UTF-8 文件名
- 使用 16、20、24 像素原生字体
- 翻页、百分比进度和历史返回
- 4 个固定阅读进度/存档槽
- 兼容 launcher 的 Resume、New Game 和删除存档流程
- 支持选项菜单调节字体大小和跳转阅读位置

## 2. 启动流程

入口是 [`txt-reader/main/main.c`](../../txt-reader/main/main.c)。

1. 初始化公共系统、显示、输入、存储和 GUI。
2. 通过 `rg_system_init()` 读取启动参数和 TXT 路径。
3. 打开 `app->romPath` 指向的文本文件。
4. 读取文件头并自动判断 UTF-8 或 GBK 编码。
5. 根据当前字号计算每页可显示的行数和字数。
6. 读取当前页文本并绘制到 320×240 屏幕。
7. 响应翻页、进度跳转、字体调节、存档和返回操作。
8. 关机或睡眠时保存阅读进度。

## 3. 文本编码

应用定义了两种编码：

```c
BOOK_ENC_UTF8
BOOK_ENC_GBK
```

GBK 到 Unicode 的转换使用：

- [`txt-reader/main/gbk_table.c`](../../txt-reader/main/gbk_table.c)
- [`tools/gen_gbk_table.py`](../../tools/gen_gbk_table.py)

读取文本时，应用会保留文本内容和换行，根据字符编码逐字计算显示宽度，避免中文换行错位。

## 4. 字体和绘制

应用使用三套原生栅格字体：

- `font_ebook`：16 像素
- `font_ebook_20`：20 像素
- `font_ebook_24`：24 像素

字体定义来自 `txt-reader/main` 的生成资源。应用通过 `rg_gui_set_font_ptr()` 切换当前字体，不对位图进行放大拉伸。

页面绘制主要由以下函数完成：

- `apply_reader_font()`：切换字号。
- `text_line_height()`：计算文字行高。
- `draw_page()`：绘制当前页面和底部百分比。
- `jump_percent()`：按百分比跳转。
- `jump_offset()`：按文件偏移跳转。

## 5. 翻页和操作

默认操作：

| 按键 | 功能 |
|---|---|
| 上 / A | 上一页 |
| 下 / B | 下一页 |
| 左 / 右 | 按约 5% 进度跳转 |
| Select | 增大字号 |
| Start | 减小字号 |
| Menu | 游戏式主菜单 |
| Option / Settings | 阅读器选项 |

应用还维护 `HISTORY_MAX` 历史偏移，用于页面跳转后的返回逻辑。

## 6. 阅读进度和存档

应用定义 4 个固定槽位，并使用公共 Retro-Go 存档路径：

- `rg_emu_get_path(RG_PATH_SAVE_STATE + slot, app->romPath)`：生成标准存档槽路径。
- `load_state_handler()`：读取文件偏移并恢复页面。
- `save_state_handler()`：保存当前文件偏移。
- `reset_handler()`：回到文件开头。
- `save_progress()`：保存阅读进度。
- `refresh_slot_cache()`：刷新存档槽预览。

旧版本使用 CRC 文件名保存书签，当前代码包含旧路径迁移逻辑。

## 7. 与 launcher 的连接

启动器注册 TXT 阅读器：

```c
application("E-Book", "txt", "txt", "txt-reader", 0);
```

启动流程：

1. launcher 扫描 `/sd/roms/txt` 下的 `.txt` 文件。
2. 用户选择书籍。
3. launcher 通过 `rg_system_switch_app()` 写入 `txt-reader` 分区和文件路径。
4. `txt-reader` 从 `app->romPath` 打开书籍。
5. 阅读器使用公共菜单和存档接口。
6. 退出后返回 launcher。

## 8. 资源生成

相关资源工具：

- [`tools/gen_ebook_font.py`](../../tools/gen_ebook_font.py)：生成电子书多字号字体。
- [`tools/gen_gbk_table.py`](../../tools/gen_gbk_table.py)：生成 GBK 对照表。
- [`tools/gen_txt_theme.py`](../../tools/gen_txt_theme.py)：生成 TXT 阅读器主题资源。

## 9. 构建配置

顶层 [`txt-reader/CMakeLists.txt`](../../txt-reader/CMakeLists.txt) 引入 `main` 和公共 `retro-go` 组件。应用依赖公共层提供的显示、输入、存储、GUI、设置和系统切换功能。

## 10. 相关文档

- [返回知识点清单](../知识点清单.md)
- [启动器](launcher.md)
- [编译和烧录说明](../../BUILDING.md)
