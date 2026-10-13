#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <rg_system.h>

typedef struct retro_app_s retro_app_t;

enum
{
    RETRO_TYPE_INVALID = 0,
    RETRO_TYPE_FOLDER,
    RETRO_TYPE_FILE,
};

//一个 ROM/游戏文件条目的结构体
typedef struct
{
    const char *name;//文件名字
    const char *folder;//rom所在的子目录名字
    uint32_t checksum;//ROM的CRC32校验值
    uint16_t missing_cover;//封面缺失标志
    uint8_t saves;//存档相关标志
    uint8_t type;//文件类型/平台类型
    retro_app_t *app;//反向指针
} retro_file_t;

//单个模拟器引用的结构体
typedef struct retro_app_s
{
    char description[64];//完整名称
    char short_name[16];//缩写
    char partition[24];//分区名
    char extensions[32];//文件拓展名
    struct {
        char covers[RG_PATH_MAX];//封面图目录
        char saves[RG_PATH_MAX];//存档目录
        char roms[RG_PATH_MAX];//rom目录
    } paths;
    size_t crc_offset;//偏移字节数
    retro_file_t *files;//指向retro_file_t的堆指针
    size_t files_capacity;//已分配槽位
    size_t files_count;//实际使用条目数
    rg_bucket_t *filenames;//文件名索引
    bool use_crc_covers;//状态标志
    bool initialized;//是否初始化
    bool available;//该应用在当前设备是否可用
} retro_app_t;

typedef struct tab_s tab_t;

void applications_init(void);
void application_show_file_menu(retro_file_t *file, bool simplified);
bool application_get_file_crc32(retro_file_t *file);
bool application_path_to_file(const char *path, retro_file_t *out_file);
void crc_cache_prebuild(void);
