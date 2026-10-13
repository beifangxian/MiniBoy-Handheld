# RGB 灯

[返回知识点清单](../知识点清单.md) | [硬件模块索引](硬件模块索引.md) | [电池检测](电池检测.md) | [扬声器](扬声器.md)

## 1. 硬件功能

当前设备使用 XL-2020RGBC-2812B / WS2812 RGB 灯，通过单线时序控制颜色和亮度。

## 2. GPIO 和配置

| 项目 | 配置 |
|---|---|
| 数据 GPIO | GPIO38 |
| 协议 | WS2812 |
| ESP32 外设 | RMT |
| 颜色顺序 | GRB |
| 默认亮度 | 48 |

配置位于 [`components/retro-go/targets/t320-s3/config.h`](../../components/retro-go/targets/t320-s3/config.h)：

```c
#define RG_LED_WS2812 1
#define RG_WS2812_BRIGHTNESS 48
#define RG_GPIO_LED GPIO_NUM_38
```

## 3. 软件实现

- [`components/retro-go/drivers/led/ws2812.c`](../../components/retro-go/drivers/led/ws2812.c)：RMT 通道、WS2812 编码器和数据发送。
- [`components/retro-go/rg_system.c`](../../components/retro-go/rg_system.c)：系统初始化和状态灯相关调用。
- [`components/retro-go/rg_gui.c`](../../components/retro-go/rg_gui.c)：部分系统状态和磁盘指示逻辑。

RMT 负责生成 WS2812 所需的精确高低电平时序，应用层不需要直接操作 GPIO 波形。

## 4. 修改位置

- 修改灯珠 GPIO、亮度或启用状态：`targets/t320-s3/config.h`。
- 修改 RMT 时序、颜色编码或发送方式：`drivers/led/ws2812.c`。
- 修改状态灯使用场景：搜索 `RG_GPIO_LED`、`RG_LED_WS2812` 和 LED 相关系统接口。

相关：[RMT 接口知识](../知识点清单.md#rmt)。
