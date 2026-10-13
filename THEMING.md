# 为 Retro-Go 设置主题

本文档旨在说明什么是主题、它们如何构成，以及如何制作主题。

示例主题可以在本项目的 [themes](/themes/) 文件夹中找到。


## 主题结构

主题是放置在 `sd:/retro-go/themes` 中的一个文件夹，包含以下文件：

````
/retro-go/themes
└── example
    ├── background.png
    ├── background_*.png
    ├── banner_*.png
    ├── logo_*.png
    ├── preview.png
    └── theme.json
````

| 名称 | 格式 | 描述 | 是否必需 |
|--|--|--|--|
| `theme.json` | JSON | 包含主题元数据（描述、作者、颜色等） | 是 |
| `preview.png` | PNG 160x120 | 在主题选择器中显示的主题预览 | 否 |
| `background.png` | PNG 320x240 | 启动器的默认背景 | 否 |
| `background_<tab_name>.png` | PNG 320x240 | 启动器的各标签页背景 | 否 |
| `banner_<tab_name>.png` | PNG 272x24 | 启动器的各标签页横幅 | 否 |
| `logo_<tab_name>.png` | PNG 46x50 | 启动器的各标签页标志 | 否 |


### theme.json

`theme.json` 文件包含主题所使用的颜色，以及一些作者元数据。

所有字段都是可选的，但不建议省略单个值，因为所使用的回退值可能会在 retro-go 的不同版本之间发生变化。不过，你可以自由地省略整个部分。例如，如果你只想重新设置启动器的主题而不想改变对话框，或者反过来，都可以。

颜色为 RGB565 格式，可以表示为整数或十六进制字符串。在某些地方也接受特殊值 `transparent`（透明）。

<details>
  <summary>查看 theme.json 示例</summary>

````json
{
    "description": "Default Retro-Go Theme",
    "website": "https://github.com/ducalex/retro-go/",
    "author": "ducalex",
    "dialog": {
        "__comment": "This section contains global dialog colors",
        "background": "0x0010",
        "foreground": "0xFFFF",
        "border": "0x6B4D",
        "header": "0xFFFF",
        "scrollbar": "0xFFFF",
        "shadow": "none",
        "item_standard": "0xFFFF",
        "item_disabled": "0x8410",
        "item_message": "0xBDF7"
    },
    "launcher_1": {
        "__comment": "This section contains launcher colors variant 1",
        "background": "0x0000",
        "foreground": "0xFFDE",
        "list_standard_bg": "transparent",
        "list_standard_fg": "0x8410",
        "list_selected_bg": "transparent",
        "list_selected_fg": "0xFFFF"
    },
    "launcher_2": {
        "__comment": "This section contains launcher colors variant 2",
        "background": "0x0000",
        "foreground": "0xFFDE",
        "list_standard_bg": "transparent",
        "list_standard_fg": "0x8410",
        "list_selected_bg": "transparent",
        "list_selected_fg": "0x07E0"
    },
    "launcher_3": {
        "__comment": "This section contains launcher colors variant 3",
        "background": "0x0000",
        "foreground": "0xFFDE",
        "list_standard_bg": "transparent",
        "list_standard_fg": "0x8410",
        "list_selected_bg": "0xFFFF",
        "list_selected_fg": "0x0000"
    },
    "launcher_4": {
        "__comment": "This section contains launcher colors variant 4",
        "background": "0x0000",
        "foreground": "0xFFDE",
        "list_standard_bg": "transparent",
        "list_standard_fg": "0xAD55",
        "list_selected_bg": "0xFFFF",
        "list_selected_fg": "0x0000"
    }
}
````
</details>


### 图像

强烈建议尽可能保持图像文件尺寸较小，以确保良好的加载速度。这可以通过在保存 PNG 文件时使用尽可能低的位深度来实现。像 [pngquant](https://pngquant.org/) 这样的工具也可以提供帮助！

在某些情况下，洋红色（rgb(255, 0, 255) / 0xF81F）被用作透明色。