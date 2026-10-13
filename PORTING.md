# 目录

- [前提条件](#prerequisites)
- [目标](#targets)
- [移植](#porting)
- [打补丁](#patching)


# 简介

本文档描述了为新的 ESP32 设备添加支持的过程（retro-go 将其称为*目标*）。


# 前提条件

## 硬件前提条件
Retro-Go 可以运行在任何使用 esp32 芯片（包括 s2、s3、p4 等变体）的设备上，只要该设备具有 PSRAM（也称为 SPIRAM 或外部 RAM）。你还需要一根串口线（大多数 esp32 设备通过 USB 暴露串口接口）。


## 软件前提条件
你需要一个可正常工作的 esp-idf 安装。Retro-Go 支持许多版本，更多信息请参考 [BUILDING.md](BUILDING.md)。如果你是 esp-idf/esp32 开发的新手，在尝试像 retro-go 这样复杂的项目之前，你可能想先向你的设备烧录几个示例程序，以确保一切正常。


# 目标
retro-go 中的目标是一组描述硬件设备的文件（要使用的驱动、GPIO、按键映射等）。目标是以文件夹形式存放在 `components/retro-go/targets/` 中，通常包含以下文件：

| 名称          | 描述 |
|---------------|-------------|
| config.h      | 这是 retro-go 配置文件，它向 retro-go 描述你的硬件 |
| env.py        | 这个文件由 rg_tool.py 导入，用于设置工具所需的环境变量，例如 esp32 芯片类型、波特率或二进制格式 |
| sdkconfig     | 这是你的板子/设备的 esp-idf 配置文件 |


# 移植
Retro-Go 使用可复用的硬件组件，因此很容易为你的硬件进行设置。

首先，找到一个与你的设备最相似的目标，并将其作为起点。克隆该目标的文件夹并给它起一个新名字，然后将你的新目标添加到 `components/retro-go/config.h` 中。


## 目标文件

### config.h

此文件包含你的大部分配置。目前没有关于此文件中可定义的所有选项的详尽文档，但你可以参考 odroid-go 的 config.h，其中包含了大部分选项。

不要忘记将 `RG_TARGET_NAME` 改为与你的目标文件夹名称一致。

#### 显示

Retro-Go 只有一个输出驱动：ILI9341/ST7789。幸运的是，大多数屏幕都使用这个控制器！

你需要定义正确的引脚排列（RG_GPIO_LCD_*）和正确的初始化序列（RG_SCREEN_INIT）。要找到你显示器/设备所需的命令序列，可以参考为你的显示器/设备编写的其他示例代码。


如果你没有使用 ILI9341/ST7789 屏幕驱动，你需要编写自己的驱动。

你可以克隆 `components/retro-go/drivers/ili9341.h` 作为起点。不要忘记将它添加到 `rg_display.c` 的顶部：

```c
#if RG_SCREEN_DRIVER == 0 /* ILI9341/ST7789 */
#include "drivers/display/ili9341.h"
#elif RG_SCREEN_DRIVER == 2             // <--
#include "drivers/display/my-driver.h"  // <--
#elif RG_SCREEN_DRIVER == 99
#include "drivers/display/sdl2.h"
#else
#include "drivers/display/dummy.h"
#endif
```

#### 输入

Retro-Go 有五种输入驱动：
- GPIO：这是连接到 esp32 某个引脚的按键
- ADC：通常与分压器一起使用，将多个按键连接到单个 esp32 引脚
- I2C：也称为 GPIO 扩展器，retro-go 支持许多芯片（AW9523、PCF9539、MCP23017、PCF8575）
- 移位寄存器：类似于 SNES 手柄或 74HC165
- 虚拟：如果你的设备按键不够，你可以将组合键映射到缺失的按键。例如 start+select = menu

你可以通过在 `config.h` 中定义适当的 `RG_GAMEPAD_*_MAP` 来组合使用它们中的任何一种。参考 `rg_input.h` 或其他目标以了解如何进行配置。


### sdkconfig

此文件由 ESP-IDF 用于为 ESP32 本身构建代码。

Retro-Go 在大多数情况下不关心 sdkconfig，其内容完全取决于所使用的芯片/板子。但有一些事情你需要记住：
- 主任务堆栈大小必须至少为 8KB，以满足 retro-go 的需求（[CONFIG_ESP_MAIN_TASK_STACK_SIZE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig-reference.html#config-esp-main-task-stack-size)）
- CPU 频率应设置为最大可能值，并且应禁用电源管理（[CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#config-esp-default-cpu-freq-mhz)、[CONFIG_PM_ENABLE](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#config-pm-enable)）
- 应启用 SPIRAM 并针对你的设备正确配置（[CONFIG_SPIRAM_\*](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#esp-psram)）
- Flash 和 SPIRAM 速度应设置为你的设备支持的最大值（[CONFIG_ESPTOOLPY_FLASHFREQ](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#config-esptoolpy-flashfreq)、[CONFIG_SPIRAM_SPEED](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#config-spiram-speed)）
- 应在 FATFS 中启用长文件名支持（[CONFIG_FATFS_LFN_HEAP](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#config-fatfs-long-filenames)）
- 应在 FATFS 中启用 UTF-8 支持（[CONFIG_FATFS_API_ENCODING_UTF_8](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/kconfig-reference.html#config-fatfs-api-encoding)）

ESP-IDF 提供了一个编辑它的工具，即 `idf.py menuconfig`，但要在 retro-go 中使用它，你必须遵循以下步骤：

1. 为你的目标构建启动器（这将确保你选择了正确的 ESP32 板子并生成默认的 sdkconfig）
    - `./rg_tool.py clean`
    - `./rg_tool.py --target=my-target build launcher`
2. 进入启动器目录：`cd launcher`
3. 运行 `idf.py menuconfig` 并进行你需要的更改。确保保存更改并退出。
4. 可选地使用新配置测试应用（但此时不要运行 `rg_tool.py clean`，否则你的新配置将被删除）
    - `cd ..`
    - `./rg_tool.py --target=my-target run launcher`
5. 当你满意后，将 `sdkconfig` 文件从 launcher 复制到目标文件夹，以便所有应用都使用它
    - `cd ..`
    - `mv  -f launcher/sdkconfig components/retro-go/targets/my-target/sdkconfig`

请注意，每当你修改目标的 `sdkconfig` 文件时，都必须运行 `./rg_tool.py clean` 以确保它会被应用。


### env.py

将 `env.py` 中的目标板子改为与你的一致


完成所有这些步骤后，你应该能够使用 `rg_tool` 构建你的应用。有关此操作和烧录的更多信息，请参阅 [BUILDING.md](BUILDING.md#flashing-an-image-for-the-first-time)。**确保使用 `.img` 的步骤，而不是 `.fw`**


## 打补丁

并非所有事情都能通过 `config.h` 完成。有时你必须为你的硬件修补 retro-go 以添加特殊代码。任何此类修改如果以合入上游为目标的话都会更加困难，但它们很容易执行。

构建系统会自动为正在使用的目标定义一个常量，例如，如果你的目标名为 `my-target`，则会定义 `RG_TARGET_MY_TARGET`（名称大写，并将 `-` 替换为 `_`）。

在 retro-go 的任何地方，你都可以添加包裹在 #ifdef 块中的代码，这些代码将仅适用于你的目标：

````c
#ifdef RG_TARGET_MY_TARGET
// do stuff
#endif
````