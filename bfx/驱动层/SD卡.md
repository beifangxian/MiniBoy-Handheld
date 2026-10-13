# SD 卡

[返回知识点清单](../知识点清单.md) | [硬件模块索引](硬件模块索引.md) | [ST7789 屏幕](ST7789屏幕.md) | [扬声器](扬声器.md)

## 1. 硬件功能

SD 卡用于保存 ROM、TXT 书籍、BIOS、封面、存档、主题和缓存，是 Retro-Go 的主要外部存储设备。

## 2. GPIO 和接口

当前使用 SPI SD 卡模式：

| 信号 | GPIO |
|---|---:|
| MOSI / CMD | 11 |
| CLK | 13 |
| MISO / DATA | 9 |
| CS / CD | 10 |

配置位于 [`components/retro-go/targets/t320-s3/config.h`](../../components/retro-go/targets/t320-s3/config.h)：

```c
#define RG_STORAGE_ROOT "/sd"
#define RG_STORAGE_SDSPI_HOST SPI3_HOST
#define RG_GPIO_SDSPI_MISO GPIO_NUM_9
#define RG_GPIO_SDSPI_MOSI GPIO_NUM_11
#define RG_GPIO_SDSPI_CLK GPIO_NUM_13
#define RG_GPIO_SDSPI_CS GPIO_NUM_10
```

## 3. 软件实现

主要实现位于 [`components/retro-go/rg_storage.c`](../../components/retro-go/rg_storage.c)：

- 初始化 SDSPI 总线
- 挂载 FAT 文件系统
- 创建和扫描目录
- 读取和写入文件
- 处理 ROM、存档、封面和缓存
- 提供 ZIP 解压和路径操作支持

ESP-IDF 底层接口包括 `sdspi_host`、SPI bus 和 FATFS。

## 4. 主要目录

```text
/sd/roms/       ROM 和 TXT 书籍
/sd/bios/       模拟器 BIOS
/sd/saves/      游戏和阅读器存档
/sd/covers/     ROM 封面
/sd/themes/     主题资源
/sd/cache/      CRC 和其他缓存
```

## 5. 应用使用

- launcher 扫描 ROM、存档和封面。
- 模拟器从 `app->romPath` 读取当前 ROM。
- TXT 阅读器从 `/sd/roms/txt` 读取书籍。
- 公共 `rg_storage_*()` 接口屏蔽 FATFS 细节。

## 6. 修改位置

- 修改 SPI 引脚：`targets/t320-s3/config.h`。
- 修改挂载、文件读写或目录扫描：`components/retro-go/rg_storage.c`。
- 修改 ROM 目录映射：`launcher/main/applications.c`。

相关：[存储、存档和设置](../知识点清单.md#驱动和公共抽象层)。
