# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

一个 Arduboy 掌上游戏机的游戏合集，使用 C++ 编写，运行于 ATmega32u4（2.5KB SRAM，128x64 单色 OLED）。目前包含：贪吃蛇游戏、计数器工具、设置界面。

## 构建与烧录

### 依赖
- Arduino IDE
- `Arduboy2` 库（安装路径：`~/Development/Arduino/libraries/`）

### 常用操作
```
# 编译与烧录（在 Arduino IDE 中）
1. 打开 sketch_cgame.ino
2. 选择开发板：Arduboy (Leonardo)
3. 点击"上传"通过 USB 烧录
```

本项目没有独立的单元测试框架（嵌入式资源受限），所有功能测试均在硬件上进行。

## 代码架构

### 整体架构：模块化插件系统

项目采用**抽象基类 + 延迟实例化**的插件式架构：

- `Base_func` 定义了功能模块的统一接口（`play()`/`exit()`/`get_config()`）
- `Navigator` 是主菜单和功能调度器，维护三个 `Base_func*` 全局指针
- 用户选择功能后，`Navigator` 才 `new` 出对应模块实例（延迟实例化，节省启动内存）
- 退出时通过 `exit()` 释放模块内存

```
┌─────────────────────────────────────────────┐
│  sketch_cgame.ino - 主循环 (15 FPS)         │
└───────────┬─────────────────────────────────┘
            │
            ▼
┌─────────────────────────────────────────────┐
│  Navigator - 主菜单 / 功能调度              │
│  - 未选择：显示菜单文字等待输入              │
│  - 已选择：委托 play() 给选中的功能模块     │
└───────────┬─────────────────────────────────┘
            │
            ▼
┌─────────────────────────────────────────────┐
│  Base_func - 抽象接口                        │
│  ┌──────────┐ ┌────────────┐ ┌────────────┐ │
│  │Func_snake│ │Func_counter│ │Func_settings│ │
│  └──────────┘ └────────────┘ └────────────┘ │
└─────────────────────────────────────────────┘
```

### 核心组件

| 文件 | 职责 | 关键设计 |
|---|---|---|
| `sketch_cgame.ino` | Arduino 入口 (`setup()`/`loop()`) | 全局单例模式：`Arduboy2*`、`Btn_ctrl*`、`Navigator*` 均为全局指针 |
| `Base_func.hpp` | 功能模块抽象接口 | 虚接口设计，`exit()` 默认实现 `delete *p` |
| `Navigator.hpp` | 主菜单与功能调度 | 延迟实例化：选中时才 `new` 模块；`Func_snake` 退出时不删除（避免内存碎片） |
| `Btn_ctrl.hpp/cpp` | 按键消抖与点击检测 | 按键锁定："按下→释放"才判定一次点击；防止按住重复触发 |
| `Snaker.hpp/cpp` | 贪吃蛇游戏引擎 | **转折点算法**：仅存头、尾、转折点，线段连接渲染，相比逐像素存储节省 >60% 内存 |
| `Func_snake.hpp` | 贪吃蛇功能模块 | 处理食物随机生成（避开调试区）、碰撞检测、暂停状态 |
| `Func_counter.hpp` | 计数器工具 | 长按连续模式：按住 15 帧后自动进入连续增减 |
| `Func_settings.hpp` | 设置界面 | 调整蛇初始长度；`store()`/`load()` 为 EEPROM 持久化预留桩 |

### 关键设计决策（内存受限环境）

1. **自定义 `Vec<T>` 模板替代 `std::vector`**
   - 原因：ArduinoSTL 的 `std::vector` 与核心 `new.cpp` 存在 `std::nothrow` 链接冲突
   - 实现：仅 ~30 行代码，提供 `push_back()`/`operator[]`/`erase_first()`/`clear()`
   - 扩容策略：初始容量 4，每次不足时容量翻倍

2. **Snake 转折点算法（核心优化）**
   - 原方案：存储每个身体像素 `vector<Point*>`，长度超过 ~16 就内存溢出
   - 当前方案：仅存 `head` + `tail` + `turning_points`（转折点列表）
   - 渲染：通过 `drawLine()` 在相邻点之间绘制线段形成蛇身
   - 移动逻辑：head 前进加转折点，tail 前进消耗转折点，吃食物时 tail 回退一格

3. **延迟实例化**
   - 不使用时不占用内存：三个功能模块只有选中的才会分配内存
   - `Func_snake` 特殊处理：退出时不删除实例，暂停引擎保留实例供下次复用，避免小内存频繁分配造成碎片

4. **PROGMEM 字符串优化**
   - 所有静态文字使用 `F()` 宏包装，存储在 flash 而非 RAM

### 内存约束

- ATmega32u4 总 SRAM：**2.5KB**
- `INITIAL_SNAKE_LENGTH` 上限：约 26-27（超过会内存溢出），当前默认设置为 15
- 所有动态分配均未检查返回值（嵌入式设备内存不足无法恢复，直接设置全局错误标志显示错误）

### 常量

| 常量 | 值 | 说明 |
|---|---|---|
| `WIDTH`/`HEIGHT` | 128x64 | 屏幕分辨率（来自 Arduboy2） |
| `BODER_WIDTH` | 1 | 游戏边框宽度 |
| `INITIAL_SNAKE_LENGTH` | 15 | 蛇初始长度 |
| 帧率 | 15 FPS | 通过 `arduboy->setFrameRate(15)` 控制 |

## 全局变量

以下变量在 `sketch_cgame.ino` 中定义为全局，各模块通过 `extern` 声明使用：
- `Arduboy2 *arduboy` - OLED 显示与按键硬件抽象
- `Btn_ctrl *btn_ctrl` - 按键点击检测
- `Navigator *navigator` - 功能调度
- `Base_func *func_snake / func_counter / func_settings` - 功能模块指针（延迟实例化）
- `boolean is_error` - 全局错误标志

## 未实现功能

- `Func_settings::store()` / `load()` - EEPROM 持久化存储配置（当前为空桩）
- 彩蛋功能 - 源代码中有彩蛋检测条件，但已禁用
- 最高分记录 - `Snaker::stop()` 预留位置但未实现
