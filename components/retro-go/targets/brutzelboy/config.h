
// REF: https://wiki.odroid.com/odroid_go/odroid_go

// Target definition
#define RG_TARGET_NAME             "brutzelboy"

// Storage
#define RG_STORAGE_ROOT             "/sd"
#define RG_STORAGE_SDSPI_HOST       SPI3_HOST
#define RG_STORAGE_SDSPI_SPEED      SDMMC_FREQ_DEFAULT
// #define RG_STORAGE_SDMMC_HOST       SDMMC_HOST_SLOT_1
// #define RG_STORAGE_SDMMC_SPEED      SDMMC_FREQ_DEFAULT
// #define RG_STORAGE_FLASH_PARTITION  "vfs"

// Audio
#define RG_AUDIO_USE_INT_DAC        0   // 0 = Disable, 1 = GPIO25, 2 = GPIO26, 3 = Both
#define RG_AUDIO_USE_EXT_DAC        1   // 0 = Disable, 1 = Enable

// Video
#define RG_SCREEN_DRIVER            0   // 0 = ILI9341 驱动选择
#define RG_SCREEN_HOST              SPI2_HOST //SPI2_HOST：宏的值，来自 ESP-IDF 的 driver/spi_master.h，代表 ESP32 芯片内部的 SPI2 控制器。

/*
    芯片极限：ILI9341 控制器本身支持高速 SPI（有开发板在良好条件下可达 80MHz ），但这并非“保底”能力。
    驱动与线路限制：在 ESP32 上，如果 SPI 使用了 GPIO 矩阵（非专用 IOMUX 引脚），高速通信的可靠性会大幅下降。
    官方文档指出，这种情况最高只支持 40MHz ，强行拉高到 80MHz 极易出现随机花屏或黑线 。
    治标不治本：某些面板对时序敏感，高速率可能诱发显示异常。
    而屏幕供应商已通过调整时序参数来专门修复这类硬件兼容性问题，其参数设定也维持在 60Hz 的常规范围 。 
*/
#define RG_SCREEN_SPEED             SPI_MASTER_FREQ_40M // SPI_MASTER_FREQ_80M
#define RG_SCREEN_BACKLIGHT         1                   //有背光控制
//分辨率
#define RG_SCREEN_WIDTH             320                 
#define RG_SCREEN_HEIGHT            240
#define RG_SCREEN_ROTATE            0                   //旋转角度
//边距
#define RG_SCREEN_MARGIN_TOP        0                   
#define RG_SCREEN_MARGIN_BOTTOM     0
#define RG_SCREEN_MARGIN_LEFT       0
#define RG_SCREEN_MARGIN_RIGHT      36

/*
    ILI9341 屏幕的初始化命令序列宏，
    把一块 ILI9341 驱动 IC 的寄存器配置打包成一个大宏，
    方便在代码里一次性调用完成屏幕初始化。
*/ 
#define RG_SCREEN_INIT()                                                                                         \
    ILI9341_CMD(0xCF, 0x00, 0xc3, 0x30);                                                                         \
    ILI9341_CMD(0xED, 0x64, 0x03, 0x12, 0x81);                                                                   \
    ILI9341_CMD(0xE8, 0x85, 0x00, 0x78);                                                                         \
    ILI9341_CMD(0xCB, 0x39, 0x2c, 0x00, 0x34, 0x02);                                                             \
    ILI9341_CMD(0xF7, 0x20);                                                                                     \
    ILI9341_CMD(0xEA, 0x00, 0x00);                                                                               \
    ILI9341_CMD(0xC0, 0x1B);                 /* Power control电源控制1   //VRH[5:0] */                                    \
    ILI9341_CMD(0xC1, 0x12);                 /* Power control电源控制2  //SAP[2:0];BT[3:0] */                            \
    ILI9341_CMD(0xC5, 0x32, 0x3C);           /* VCM  控制 1 control */                                                   \
    ILI9341_CMD(0xC7, 0x91);                 /* VCM  控制 2 control2 */                                                  \
    ILI9341_CMD(0x36, (0x20 | 0x80 | 0x08)); /* Memory Access Control 存储器访问控制 thebrutzler hat ((0x60|0x08)^0xC0))*/     \
    ILI9341_CMD(0xB1, 0x00, 0x10);           /* Frame Rate Control 帧率控制(1B=70, 1F=61, 10=119) */                     \
    ILI9341_CMD(0xB6, 0x0A, 0xA2); /* ILI9341 的 0xB6 寄存器 用于配置屏幕的扫描模式、驱动行数以及帧率相关的参数，直接影响画面的刷新范围和刷新方向。*/\
    ILI9341_CMD(0xF6, 0x01, 0x30);                                                                               \
    ILI9341_CMD(0xF2, 0x00); /* 3Gamma Function Disable关闭 3-Gamma 功能 */                                                       \
    ILI9341_CMD(0x26, 0x01); /* Gamma curve selected 用于选择显示控制器内部预置的 Gamma 校正曲线。*/              \
    ILI9341_CMD(0xE0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00); \
    ILI9341_CMD(0xE1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);


// Input
#define RG_GAMEPAD_ADC_FILTER_WINDOW 150 // this is a filter value, to be sure, the adc is in stable situation (not falling or rizing)
#define RG_GAMEPAD_ADC_MAP {\
    {RG_KEY_UP,    ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_11, 2000, 4096},\
    {RG_KEY_RIGHT, ADC_UNIT_1, ADC_CHANNEL_5, ADC_ATTEN_DB_11, 1500, 1900},\
    {RG_KEY_DOWN,  ADC_UNIT_1, ADC_CHANNEL_6, ADC_ATTEN_DB_11, 1500, 1900},\
    {RG_KEY_LEFT,  ADC_UNIT_1, ADC_CHANNEL_5, ADC_ATTEN_DB_11, 2000, 4096},\
}

#define RG_GAMEPAD_GPIO_MAP {\
    {RG_KEY_SELECT, GPIO_NUM_16, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_START,  GPIO_NUM_17, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_MENU,   GPIO_NUM_18, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_OPTION, GPIO_NUM_8,  GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_A,      GPIO_NUM_15, GPIO_PULLUP_ONLY, 0},\
    {RG_KEY_B,      GPIO_NUM_5,  GPIO_PULLUP_ONLY, 0},\
}

// Battery
#define RG_BATTERY_DRIVER           1
#define RG_BATTERY_ADC_UNIT         ADC_UNIT_1
#define RG_BATTERY_ADC_CHANNEL      ADC1_CHANNEL_3
#define RG_BATTERY_CALC_PERCENT(raw) (((raw) * 2.7f - 2000.f) / (2700.f - 2000.f) * 100.f)
#define RG_BATTERY_CALC_VOLTAGE(raw) ((raw) * 2.7f * 0.001f)

// Status LED
#define RG_GPIO_LED                 GPIO_NUM_38

// SPI Display (back up working)
#define RG_GPIO_LCD_MISO            GPIO_NUM_NC
#define RG_GPIO_LCD_MOSI            GPIO_NUM_12
#define RG_GPIO_LCD_CLK             GPIO_NUM_48
#define RG_GPIO_LCD_CS              GPIO_NUM_NC
#define RG_GPIO_LCD_DC              GPIO_NUM_47
#define RG_GPIO_LCD_BCKL            GPIO_NUM_39
#define RG_GPIO_LCD_RST             GPIO_NUM_3

#define RG_GPIO_SDSPI_MISO          GPIO_NUM_9
#define RG_GPIO_SDSPI_MOSI          GPIO_NUM_11
#define RG_GPIO_SDSPI_CLK           GPIO_NUM_13
#define RG_GPIO_SDSPI_CS            GPIO_NUM_10

// External I2S DAC
#define RG_GPIO_SND_I2S_BCK         41
#define RG_GPIO_SND_I2S_WS          42
#define RG_GPIO_SND_I2S_DATA        40
// #define RG_GPIO_SND_AMP_ENABLE      18
