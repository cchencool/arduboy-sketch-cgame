
/**
 * sketch_cgame.ino - Arduboy 游戏合集入口
 *
 * 包含三个功能模块：贪吃蛇、计数器、设置界面
 * 通过 Navigator 在主菜单中选择不同功能
 */

#include <avr/pgmspace.h>
#include "Arduboy2.h"
#include "arduino.h"
#include "Navigator.hpp"
#include "Btn_ctrl.hpp"
#include "Func_snake.hpp"
#include "Func_counter.hpp"
#include "Func_settings.hpp"
#include "Func_bunny.hpp"

// ==================== 全局对象 ====================

/** Arduboy2 显示与输入控制实例 */
Arduboy2 *arduboy = new Arduboy2;

/** 按键消抖与点击检测实例 */
Btn_ctrl *btn_ctrl = new Btn_ctrl;

/** 导航器，负责主菜单与功能切换 */
Navigator *navigator = new Navigator;

/** 功能模块指针（延迟实例化，选定时才创建） */
Base_func *func_snake;    // 贪吃蛇
Base_func *func_counter;  // 计数器
Base_func *func_settings; // 设置
Base_func *func_bunny;    // 小兔子跳跃游戏

/** 错误提示信息（使用 F() 宏从 flash 读取，节省 RAM） */
#define error_str F("error occur!")

/** 全局错误标志 */
boolean is_error = false;

/**
 * setup() - Arduino 初始化函数，上电时仅执行一次
 *
 * 初始化 Arduboy 硬件（OLED、按键），设置帧率为 15 FPS
 */
void setup()
{
  arduboy->begin();              // 初始化 Arduboy 硬件
  // arduboy->boot();            // 可节省代码空间的替代方案

  arduboy->setFrameRate(15);     // 设置帧率 15 FPS
}

/**
 * loop() - Arduino 主循环，以 15 FPS 反复执行
 *
 * 流程：
 *   1. 帧率控制（nextFrame 跳过不需要重绘的帧）
 *   2. 清屏
 *   3. 错误时显示错误信息，否则根据导航器状态显示主菜单或运行功能模块
 *   4. 按 B 键返回主菜单
 *   5. 将帧缓冲输出到 OLED 屏幕
 */
void loop()
{
  // 帧率控制：如果未到下一帧则直接返回
  if (!(arduboy->nextFrame()))
  {
    return;
  }

  arduboy->clear();  // 清除帧缓冲

  if (is_error)
  {
    // 错误状态：显示错误信息
    arduboy->setCursor(50, 50);
    arduboy->print(error_str);
  }
  else
  {
    if (!navigator->get_has_make_choice())
    {
      // 未选择功能：显示主菜单
      navigator->show_navigator();
    }
    else
    {
      // 已选择功能：根据选择运行对应模块
      if (navigator->get_func_choice() == SNAKE)
      {
        navigator->play_snake();
      }
      else if (navigator->get_func_choice() == COUNTER)
      {
        navigator->play_count();
      }
      else if (navigator->get_func_choice() == SETTINGS)
      {
        navigator->play_settings();
      }
      else if (navigator->get_func_choice() == BUNNY)
      {
        navigator->play_bunny();
      }
    }
  }

  // 按 B 键返回主菜单（需处于功能模块或错误状态）
  if ((navigator->get_has_make_choice() || is_error) && btn_ctrl->b_click())
  {
    navigator->stop_game();                 // 释放当前功能模块资源
    navigator->set_has_made_choice(false);  // 重置选择状态
  }

  arduboy->display();  // 将帧缓冲刷新到 OLED 屏幕
}
