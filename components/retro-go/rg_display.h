#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    RG_DISPLAY_SCALING_OFF = 0, // No scaling, center image on screen
    RG_DISPLAY_SCALING_FIT,     // Scale and preserve aspect ratio
    RG_DISPLAY_SCALING_FULL,    // Scale and stretch to fill screen
    RG_DISPLAY_SCALING_ZOOM,  // Custom zoom and preserve aspect ratio
    RG_DISPLAY_SCALING_COUNT
} display_scaling_t;

typedef enum
{
    RG_DISPLAY_FILTER_OFF = 0,
    RG_DISPLAY_FILTER_HORIZ,
    RG_DISPLAY_FILTER_VERT,
    RG_DISPLAY_FILTER_BOTH,
    RG_DISPLAY_FILTER_COUNT,
} display_filter_t;

typedef enum
{
    RG_DISPLAY_ROTATION_OFF = 0,
    RG_DISPLAY_ROTATION_AUTO,
    RG_DISPLAY_ROTATION_LEFT,
    RG_DISPLAY_ROTATION_RIGHT,
    RG_DISPLAY_ROTATION_COUNT,
} display_rotation_t;

typedef enum
{
    RG_DISPLAY_BACKLIGHT_MIN = 1,
    RG_DISPLAY_BACKLIGHT_MAX = 100,
} display_backlight_t;

enum
{
    RG_DISPLAY_WRITE_NOSYNC = (1 << 0),
    RG_DISPLAY_WRITE_NOSWAP = (1 << 1),
};

typedef struct
{
    display_rotation_t rotation;
    display_scaling_t scaling;
    display_filter_t filter;
    display_backlight_t backlight;
    char *border_file;
    double custom_zoom;
} rg_display_config_t;

typedef struct
{
    int32_t totalFrames;
    int32_t fullFrames;
    int32_t partFrames;
    int64_t blockTime;
    int64_t busyTime;
} rg_display_counters_t;

typedef struct
{
    const char *name;                                               // 驱动名字
    bool (*init)(void);                                             // 初始化显示屏
    bool (*deinit)(void);                                           // 反初始化
    bool (*sync)(void);                                             // 同步/等待 暂停直到所有已提交的数据真正刷完。用于防止在 DMA 还在传输时修改缓冲区。
    bool (*set_backlight)(float percent);                           // 设置背光亮度，参数是 0.0 ~ 1.0 的浮点百分比。
    bool (*set_window)(int left, int top, int width, int height);   // 设置绘制窗口（即 ILI9341 的 Column/Page Address Set），限定后续像素写入的区域。
    uint16_t *(*get_buffer)(size_t length);                         // 获取一块支持 DMA 的像素缓冲区。长度单位是像素数（uint16_t 个数）。
    bool (*send_buffer)(uint16_t *buffer, size_t length);           // 把缓冲区数据发送到屏幕。必须用 get_buffer 拿到的缓冲区。
    // bool (*write)(int left, int top, int width, int height, int pitch, const uint16_t data);
} rg_display_driver_t;

//图形库中用于描述显示输出状态的结构体
typedef struct
{
    struct
    {
        int real_width, real_height; //物理分辨率。屏幕实际有多少像素，比如 320×240。
        int width, height; // 可见分辨率。扣掉边距后真正能用来显示图像的区域大小。
        //四边边距 {left, top, right, bottom}。用于适配某些屏幕边缘不可用、或有边框遮挡的情况。
        struct {int left, top, right, bottom;} margins;
        // int format;/屏幕像素格式，如 RGB565、RGB666 等。
    } screen;
    struct
    {
        int top, left;//视口在屏幕上的起始坐标（相对可见区域）。
        int width, height;//视口的目标尺寸，即图像最终显示多大。
        float step_x, step_y;//采样步长（浮点）。用于缩放：每输出一个像素，在源图上前进多少。step_x > 1 是缩小，< 1 是放大。
        bool filter_x, filter_y;//是否在该方向启用滤波（如双线性插值）。关闭时用最近邻，速度快但可能有锯齿。
    } viewport;//“源图像要画到屏幕的哪个位置、怎么缩放”。
    struct
    {
        int width, height;//待显示的源图像原始尺寸。
    } source;
    bool changed;//脏标记：避免每帧都重算重画，只有状态真正改变时才触发更新，节省 CPU。
} rg_display_t;

#include "rg_surface.h"

void rg_display_init(void);
void rg_display_deinit(void);
void rg_display_write_rect(int left, int top, int width, int height, int stride, const uint16_t *buffer, uint32_t flags);
void rg_display_clear_rect(int left, int top, int width, int height, uint16_t color_le);
void rg_display_clear_except(int left, int top, int width, int height, uint16_t color_le);
void rg_display_clear(uint16_t color_le);
bool rg_display_sync(bool block);
void rg_display_force_redraw(void);
void rg_display_submit(const rg_surface_t *update, uint32_t flags);

rg_display_counters_t rg_display_get_counters(void);
const rg_display_t *rg_display_get_info(void);
int rg_display_get_width(void);
int rg_display_get_height(void);

void rg_display_set_scaling(display_scaling_t scaling);
display_scaling_t rg_display_get_scaling(void);
void rg_display_set_filter(display_filter_t filter);
display_filter_t rg_display_get_filter(void);
void rg_display_set_rotation(display_rotation_t rotation);
display_rotation_t rg_display_get_rotation(void);
void rg_display_set_backlight(display_backlight_t percent);
display_backlight_t rg_display_get_backlight(void);
void rg_display_set_border(const char *filename);
char *rg_display_get_border(void);
void rg_display_set_custom_zoom(double factor);
double rg_display_get_custom_zoom(void);
