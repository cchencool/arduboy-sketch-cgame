# Arduboy 游戏合集

这是一个为 Arduboy 掌上游戏机开发的游戏合集项目，使用 C++ 编写。

## 硬件平台

- **主控芯片**：ATmega32u4
- **SRAM**：2.5KB（内存极其受限，代码做了大量优化）
- **屏幕**：128x64 单色 OLED
- **开发板**：Arduboy (Leonardo 兼容)

## 包含功能

### 1. 贪吃蛇游戏
经典贪吃蛇游戏，具有以下特点：
- 使用转折点算法实现蛇体，大幅节省内存
- 暂停/恢复功能
- 撞墙自动重新开始
- 食物随机生成（避开调试信息区域）

### 2. 计数器工具
一个实用的计数器功能：
- 上/下键增减数值
- 左/右键调整步进倍率（×10 / ÷10）
- A 键重置
- 长按连续增减模式

### 3. 设置界面
可配置项：
- 蛇的初始长度（默认为 15，最大约 26-27）
- 预留 EEPROM 持久化接口（未实现）

## 构建与烧录

### 依赖项
- Arduino IDE
- Arduboy2 库（需安装到 `~/Development/Arduino/libraries/`）

### 烧录步骤
1. 在 Arduino IDE 中打开 `sketch_cgame.ino`
2. 选择开发板：**Arduboy (Leonardo)**
3. 连接 Arduboy 到电脑
4. 点击「上传」按钮进行烧录

## 项目架构

项目采用模块化插件系统设计：
- `Base_func` - 功能模块抽象接口
- `Navigator` - 主菜单与功能调度器
- `Func_snake` / `Func_counter` / `Func_settings` - 具体功能模块

### 内存优化亮点

由于 ATmega32u4 只有 2.5KB SRAM，项目做了大量优化：

1. **自定义 `Vec<T>` 模板**：替代 `std::vector`，避免链接冲突和内存开销
2. **转折点算法**：蛇体仅存头、尾和转折点，通过线段连接渲染，相比逐像素存储节省 >60% 内存
3. **延迟实例化**：功能模块仅在用户选择时才创建
4. **PROGMEM 字符串**：所有静态文字存储在 flash 而非 RAM

## 目录结构

```
sketch_cgame/
├── sketch_cgame.ino      # Arduino 入口文件
├── Base_func.hpp         # 功能模块抽象接口
├── Navigator.hpp         # 主菜单与调度器
├── Btn_ctrl.hpp/cpp      # 按键检测（消抖与锁定）
├── Snaker.hpp/cpp        # 贪吃蛇引擎
├── Func_snake.hpp        # 贪吃蛇功能模块
├── Func_counter.hpp      # 计数器功能模块
├── Func_settings.hpp     # 设置功能模块
├── CLAUDE.md             # Claude Code 项目文档
└── README.md             # 本文件
```

## 开发说明

### 内存约束
- `INITIAL_SNAKE_LENGTH` 默认 15，最大约 26-27（超过会内存溢出）
- 所有动态分配未检查返回值（嵌入式设备内存不足无法恢复）

### 未实现功能
- 设置的 EEPROM 持久化（`store()` / `load()` 为空桩）
- 最高分记录
- 彩蛋功能

## 许可证

本项目仅供学习和个人使用。
