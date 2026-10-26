# 🎮 retro-go_chaeng（ESP32-S3 复古掌机）

<p align="center">
  <img width="4000" height="2252" alt="20260614_092928" src="https://github.com/user-attachments/assets/87ce1f19-227a-4a08-91c1-b1887e7af833" />
</p>

<p align="center">
基于 Retro-Go（ducalex）移植的 ESP32-S3 复古掌机项目<br/>
Retro-Go 中文硬件扩展版本（chaeng 定制）
</p>

---

<p align="center">

![Base](https://img.shields.io/badge/Base-Retro--Go%20fork-orange)
![Platform](https://img.shields.io/badge/MCU-ESP32--S3-blue)
![Display](https://img.shields.io/badge/LCD-3.2inch%20TFT-green)
![Audio](https://img.shields.io/badge/I2S-Audio-yellow)
![RGB](https://img.shields.io/badge/WS2812-RGB-red)
![Language](https://img.shields.io/badge/Code-中文化-orange)
![License](https://img.shields.io/badge/GPL-v3.0-blue)

</p>

---

## 🆕 最新更新 · Chaeng 2.0.7（2026-07-22）

<p align="center">
  <img width="2273" height="1279" alt="微信图片_20260722063755_10_2" src="https://github.com/user-attachments/assets/59c66b15-5d0b-4dad-9b80-26df3de1b48b" />
</p>


- **电子书**：新增 TXT 阅读应用，启动器中作为独立机种，每本书相当于一个 ROM（`roms/txt/*.txt`）
- **语音 AI**：启动器和 TXT 阅读器中同时按住 A+B 录音，松开任一键停止；AI 回复以文字显示，阅读器请求附带当前页内容
- **编码与字体**：UTF-8 / GBK 自动识别，霞鹜文楷多字号阅读
- **书目菜单**：与游戏一致 — 继续游戏 / 新游戏 / 删除存档
- **阅读操作**：菜单键、设置键与模拟器相同（存档槽 / 系统选项）
- **翻页**：A 下一页、B 上一页；上/下方向键按约 5% 进度跳转，左/右切换前后章节；选择/开始调节字号
- **存档**：中文书名路径兼容（rename 失败时回退直接写入）
- **界面字库**：FusionPixel 扩至完整 GB2312，修复中文书名缺字
- **翻译**：补充存档槽预览、超频等中文文案

更多历史见 [CHANGELOG.md](CHANGELOG.md)

### 语音 AI 配置（二次开发）

启动器和 TXT 阅读器支持 OpenAI 兼容接口：同时按住 A+B 录音，松开任一键后上传（最长 15 秒），屏幕显示文字回复；较长回复可用方向键滚动，按 A 选择“确定”，或按 B 退出。TXT 阅读器会将当前页文本一并作为上下文。麦克风使用 ICS-43434，LR 接地时按左声道采集。

在 SD 卡创建 `/sd/retro-go/config/ai.conf`：

**OpenAI 兼容接口**（默认）：

```ini
base_url=https://api.openai.com/v1
api_key=在此填写服务商API密钥
transcription_model=whisper-1
chat_model=gpt-4o-mini
```

**智谱（BigModel）接口**：

```ini
base_url=https://open.bigmodel.cn/api/paas/v4
api_key=在此填写智谱API密钥
transcription_model=glm-asr-2512
chat_model=glm-4.5-flash
```

字段说明：

- `base_url`：接口根地址，必须 HTTPS，末尾不要带路径（如不要写 `/chat/completions`）。
- `api_key`：必填。
- `transcription_model`：转写模型。OpenAI 用 `whisper-1`；智谱用 `glm-asr-2512`。
- `chat_model`：聊天模型。OpenAI 用 `gpt-4o-mini`；智谱可用 `glm-4.5-flash`（低价）等。

两者都要求服务兼容 `/audio/transcriptions`（multipart 上传音频）与 `/chat/completions` 两个接口——智谱新版 ASR 接口与 OpenAI 格式一致。配置文件中的密钥以明文保存在 SD 卡，请保护 SD 卡并使用可撤销、有限额的 API 密钥；设备只接受 HTTPS 接口。


## ✨ 项目特性

- 基于 Retro-Go（ducalex）二次开发
- ESP32-S3 双核高性能主控
- 3.2 寸 TFT LCD（SPI，320×240）
- SD 卡游戏加载
- TXT 电子书阅读（`roms/txt/*.txt`，UTF-8/GBK，书签存档与游戏逻辑一致）
- I2S 数字音频输出
- 完整按键控制系统
- 可编程 WS2812 RGB 状态灯（默认蓝色）
- 语音 AI：板载麦克风（ICS-43434）录音，OpenAI 兼容接口，文字回复

---

## 🔧 编译与烧录

本项目使用 ESP-IDF 5.5.5，目标芯片为 ESP32-S3。Windows 用户请在 ESP-IDF 终端中，从仓库根目录执行：

```bat
python rg_tool.py --target=t320-s3 build-img
```

编译完成后会生成 `retro-go_chaeng_t320-s3.img`。连接设备并确认串口号后烧录：

```bat
python rg_tool.py --target=t320-s3 --port=COM23 install
```

将 `COM3` 替换为实际串口号。需要干净重编译时使用：

```bat
python rg_tool.py --target=t320-s3 release
```

详细说明见 [BUILDING.md](BUILDING.md) 和 [t320-s3 目标说明](components/retro-go/targets/t320-s3/docs/README.md)。

---

## 🧩 功能模块与代码位置

本项目采用“共享运行时组件 + 多个独立 ESP-IDF 应用”的结构。各应用分别编译为 ESP32 应用分区，由启动器负责选择和切换。

| 功能 | 代码位置 | 主要入口 |
|---|---|---|
| 系统初始化、应用切换、启动参数 | `components/retro-go` | `rg_system.c` |
| 显示、绘图、Surface | `components/retro-go` | `rg_display.c`、`rg_surface.c` |
| 按键和电池检测 | `components/retro-go` | `rg_input.c` |
| 音频 | `components/retro-go/drivers/audio` | `rg_audio.c`、音频驱动文件 |
| GUI、菜单、对话框 | `components/retro-go` | `rg_gui.c` |
| 存储、存档、设置 | `components/retro-go` | `rg_storage.c`、`rg_settings.c` |
| 启动器、ROM 浏览、应用注册 | `launcher/main` | `main.c`、`applications.c`、`browser.c` |
| NES、SNES、GB、GBC、PCE、SMS、G&W、Lynx | `retro-core` | `retro-core/main/main.c` |
| DOOM | `prboom-go` | `prboom-go/main/main.c` |
| Mega Drive | `gwenesis` | `gwenesis/main/main.c` |
| MSX | `fmsx` | `fmsx/main/main.c` |
| TXT 电子书 | `txt-reader` | `txt-reader/main/main.c` |
| 语音 AI、麦克风录音 | `components/retro-go` | `rg_ai.c`、`rg_ai.h` |
| 主题、图片和字体 | `themes`、`components/retro-go/fonts`、`tools` | `gen_images.py`、字体生成脚本 |
| 翻译 | `components/retro-go` | `translations.h`、`rg_localization.c` |

### 应用切换流程

启动器在 `launcher/main/applications.c` 中注册 ROM 类型和目标应用：

- `nes`、`snes`、`gb`、`gbc`、`pce`、`sms` 等进入 `retro-core`
- `md` 进入 `gwenesis`
- `doom` 进入 `prboom-go`
- `msx` 进入 `fmsx`
- `txt` 进入 `txt-reader`

用户选择 ROM 后，启动器调用 `rg_system_switch_app()` 保存目标分区、系统名称和 ROM 路径，然后重启设备。目标应用启动后读取这些参数，再进入对应的模拟器主函数。因此，应用之间是通过 ESP32 应用分区和重启机制连接的，而不是普通的函数调用。

### t320-s3 硬件配置

当前硬件目标的配置集中在 `components/retro-go/targets/t320-s3/`：

- `config.h`：屏幕、GPIO、按键、音频和目标功能配置
- `env.py`：芯片类型、镜像格式和项目名称
- `sdkconfig`：ESP-IDF 和 ESP32-S3 编译配置
- `docs/README.md`：硬件引脚和使用说明

其中包括 ST7789 屏幕、320×240 分辨率、SD 卡、I2S 音频、麦克风（ICS-43434）、电池 ADC、WS2812 状态灯以及全部按键 GPIO 配置。

### 硬件驱动位置

| 硬件功能 | 实现文件 |
|---|---|
| 板级 GPIO、屏幕、按键、音频和电池配置 | `components/retro-go/targets/t320-s3/config.h` |
| 芯片型号、镜像名称和目标参数 | `components/retro-go/targets/t320-s3/env.py` |
| ESP32-S3、Flash、PSRAM 和烧录配置 | `components/retro-go/targets/t320-s3/sdkconfig` |
| 屏幕底层驱动 | `components/retro-go/drivers/display/ili9341.h` |
| 屏幕刷新、旋转、缩放和背光管理 | `components/retro-go/rg_display.c` |
| SD 卡和文件系统 | `components/retro-go/rg_storage.c` |
| GPIO 按键和 ADC 电池检测 | `components/retro-go/rg_input.c` |
| I2S 音频底层驱动 | `components/retro-go/drivers/audio/i2s.c` |
| 音频设备选择、音量和滤波 | `components/retro-go/rg_audio.c` |
| 麦克风 I2S RX 采集与语音 AI | `components/retro-go/rg_ai.c` |
| WS2812 RGB 灯 | `components/retro-go/drivers/led/ws2812.c` |
| 系统初始化、SPI 总线和硬件切换 | `components/retro-go/rg_system.c` |

修改硬件时，通常先修改 `targets/t320-s3/config.h` 中的 GPIO 和功能宏；只有通信协议或驱动行为改变时，才需要继续修改 `drivers` 或对应的 `rg_*.c` 文件。应用目录一般通过这些公共接口使用硬件，不直接操作 GPIO。

麦克风 GPIO1（WS）、GPIO2（SCK）、GPIO21（SD）现由语音 AI 功能使用，LR 接地按左声道采集。

### 当前项目限制记录

以下内容是当前项目结构中的已知现状，暂不作为问题修复：

- `rg_tool.py` 默认目标仍是 `odroid-go`，编译本项目时应显式使用 `--target=t320-s3`。
- 各应用的分区大小在 `rg_tool.py` 中固定定义，增大应用或字体时需要关注空间是否足够。
- 构建脚本会生成临时的 `partitions.csv`，它不是当前镜像的实际应用分区布局文件。
- 启动器通过检测应用分区是否存在来决定是否显示对应项目，因此代码中注册的系统数量可能多于当前实际构建的应用数量。
- GPIO0 同时承担启动/恢复相关功能和按键功能，具体行为依赖硬件设计和启动时序。
- 网络更新相关配置包含跳过 TLS 证书验证的选项，使用不可信网络时需要注意风险。

## 🔌 GPIO 引脚定义

SD_CMD - GPIO11  
SD_CLK - GPIO13  
SD_DATA - GPIO9  
SD_CD - GPIO10  

SPK_DOUT - GPIO40  
SPK_BCLK - GPIO41  
SPK_LRCK - GPIO42  

KEY_BOOT - GPIO0  
KEY_MENU - GPIO18  
KEY_OPTION - GPIO8  
KEY_SELECT - GPIO16  
KEY_START - GPIO17  
KEY_A - GPIO15  
KEY_B - GPIO5  
KEY_LEFT - GPIO19  
KEY_RIGHT - GPIO6  
KEY_UP - GPIO7  
KEY_DOWN - GPIO20  

BAT - GPIO4  
TFT_BLK - GPIO39  
TFT_MOSI - GPIO12  
TFT_CLK - GPIO48  
TFT_DC - GPIO47  
TFT_RST - GPIO3  
TFT_CS - GPIO14  

MIC_WS - GPIO1  
MIC_SCK - GPIO2  
MIC_SD - GPIO21  

STATUS_LED - GPIO38  

---

## 🧠 系统架构

### 硬件架构

ESP32-S3 主控  
├── 3.2寸 TFT 屏幕  
├── SD 卡模块  
├── I2S 音频输出  
├── 麦克风（ICS-43434）  
├── WS2812 RGB 灯  
├── 按键输入系统  
└── 电池供电系统  


---

### 软件架构

retro-go base  
↓  
retro-go_chaeng port  
├── driver（驱动层）  ESP-IDF 5.5.5
ESP32-S3
launcher
├── ui（界面系统）  
├── input（输入系统）  
├── audio（音频系统）  
└── game（游戏运行层）  

---

## 🎯 项目目标

- 构建 ESP32-S3 复古掌机系统  
- 基于 Retro-Go 实现硬件移植  
- 提供完整开源软硬件方案  
- 支持教学 / DIY / 二次开发  
- 可扩展 AI(已拓展) / IoT / 联机能力  

---

## 📜 开源协议

GNU General Public License v3.0 (GPLv3)

✔ 允许使用 / 修改 / 分发  
❗ 必须开源衍生代码  
❗ 必须保留原始版权信息  
