# CLAUDE.md

此文件为 Claude Code (claude.ai/code) 在此仓库中工作时提供指导。

## 项目概述

一个 Arduboy 游戏合集，使用 C++ 为 Arduboy 掌上游戏机编写（ATmega32u4，2.5KB SRAM，128x64 单色 OLED）。使用 Arduino IDE 构建。

## 构建与烧录

- 在 Arduino IDE 中打开 `sketch_cgame.ino`
- 依赖库：`Arduboy2`
- 库路径：`~/Development/Arduino/libraries/`
- `Snaker` 类使用自定义 `Vec<T>` 模板（替代 `std::vector`），避免 ArduinoSTL 的链接冲突和内存开销
- 目标板：Arduboy（Leonardo 兼容）
- 通过 Arduino IDE 经 USB 烧录

## 代码架构

### 入口

- `sketch_cgame.ino` - Arduino sketch 入口（`setup()` / `loop()`），创建全局单例指针：`Arduboy2`、`Btn_ctrl`、`Navigator`，以及三个 `Base_func` 函数指针
- 游戏循环以 15 FPS 运行，通过 `arduboy->setFrameRate(15)` 和 `arduboy->nextFrame()` 控制帧率

### 核心组件

| 文件 | 用途 |
|---|---|
| `Navigator.hpp` | 主菜单 - 让用户选择贪吃蛇、计数器或设置。首次选择时延迟创建功能对象，退出时通过 `stop_game()` 销毁 |
| `Base_func.hpp` | 功能模块的抽象接口。定义了 `play()`、`exit(Base_func**)`、`get_config()` |
| `Btn_ctrl.hpp/cpp` | 围绕 Arduboy2 按键 API 的消抖/锁定封装。提供 `*_click()` 方法，在按键释放时返回 true |
| `Snaker.hpp/cpp` | 贪吃蛇游戏引擎。使用 head/tail + turning_points 向量代替存储每个身体像素，通过转折点之间绘制线段。针对 2.5KB SRAM 做了内存优化 |
| `Func_snake.hpp` | 贪吃蛇功能模块（继承 `Base_func`）。处理食物生成、碰撞检测、暂停状态 |
| `Func_counter.hpp` | 计数器工具。上/下增减数值，左/右改变步进倍率，A 键重置 |
| `Func_settings.hpp` | 设置界面。显示/调整蛇的初始长度。`store()` 和 `load()` 是未实现的空桩函数 |

### 关键设计模式

- **延迟实例化**：功能对象（`Func_snake`、`Func_counter`、`Func_settings`）在用户选择时才创建，而非启动时。指针类型为全局 `Base_func*`，实际指向具体子类。
- **头尾蛇体渲染**：`Snaker` 不将每个身体像素存入向量（内存消耗大），而是仅存储头、尾和转折点为 `Turn_Point` 结构体，然后在转折点之间绘制线段，大幅降低内存占用。
- **自定义 `Vec` 容器**：项目使用 `Vec<T>` 模板（~30 行）替代 `std::vector`，提供 `size()/push_back()/operator[]/erase_first()/clear()` 等必要方法，避免 ArduinoSTL 的 `std::nothrow` 链接冲突。
- **按键锁定模式**：`Btn_ctrl` 使用每个按键的锁定标志，将"按下→释放"检测为单次"点击"事件，防止按住时重复触发。
- **内存约束**：`INITIAL_SNAKE_LENGTH` 设为 15，因为长度超过 26/27 会导致 2.5KB SRAM 设备内存溢出。`Snaker.cpp` 中大量注释掉的向量实现记录了从逐像素存储到当前转折点方案的演进过程。

### 常量

- 屏幕：128x64 像素（来自 Arduboy2 `WIDTH`/`HEIGHT`）
- `BODER_WIDTH`：1（边框宽度）
- `INITIAL_SNAKE_LENGTH`：15
- 帧率：15 FPS
