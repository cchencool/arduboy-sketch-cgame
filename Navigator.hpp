/**
 * Navigator.hpp - 导航器（主菜单与功能调度器）
 *
 * 职责：
 *   1. 显示主菜单，等待用户选择功能
 *   2. 延迟创建功能模块实例（首次选中时 new）
 *   3. 将每帧的渲染委托给当前功能模块的 play() 方法
 *   4. 退出时释放功能模块实例
 */

#if !defined(Navigator_hpp)
#define Navigator_hpp

#include <avr/pgmspace.h>
#include "Arduboy2.h"
#include "Btn_ctrl.hpp"
#include "Base_func.hpp"
#include "Func_snake.hpp"
#include "Func_counter.hpp"
#include "Func_bunny.hpp"
#include "Snaker.hpp"

/** 菜单文字起始 X 坐标 */
#define STR_START_X 6

// ==================== 外部全局变量声明 ====================

extern Btn_ctrl *btn_ctrl;       // 按键控制器
extern Arduboy2 *arduboy;        // Arduboy2 实例
extern Base_func *func_snake;    // 贪吃蛇模块指针
extern Base_func *func_counter;  // 计数器模块指针
extern Base_func *func_settings; // 设置模块指针
extern Base_func *func_bunny;    // 小兔子游戏模块指针
extern boolean is_error;         // 全局错误标志

// ==================== 功能枚举 ====================

/** 支持的功能模块编号 */
enum Support_func
{
    SNAKE,    // 贪吃蛇
    COUNTER,  // 计数器（已隐藏）
    SETTINGS, // 设置
    BUNNY     // 小兔子跳跃游戏
};

/**
 * Navigator - 导航器类
 *
 * 状态机：
 *   has_made_choice == false → 显示主菜单
 *   has_made_choice == true  → 运行对应功能模块
 */
class Navigator
{
  private:
    boolean has_made_choice;  // 用户是否已选择功能
    Support_func func_choice; // 当前选择的功能编号

  public:
    /**
     * 构造函数：默认选择 COUNTER（但 COUNTER 在菜单中已隐藏）
     */
    Navigator()
    {
        this->func_choice = COUNTER;
    }

    /** 用户是否已选择功能 */
    boolean get_has_make_choice()
    {
        return this->has_made_choice;
    }

    /** 设置用户选择状态 */
    void set_has_made_choice(boolean has_or_not)
    {
        this->has_made_choice = has_or_not;
    }

    /** 获取当前选择的功能编号 */
    Support_func get_func_choice()
    {
        return this->func_choice;
    }

    /** 设置当前选择的功能编号 */
    void set_func_choice(Support_func choice)
    {
        this->func_choice = choice;
    }

    /**
     * show_navigator - 显示主菜单
     *
     * 绘制双线边框与菜单文字，监听方向键和 A 键选择功能。
     * 当前可用的选择：
     *   - 上键：贪吃蛇
     *   - A 键：设置
     *   （计数器功能已在菜单中隐藏）
     */
    void show_navigator()
    {
        draw_square();  // 绘制菜单边框

        // 菜单标题
        arduboy->setCursor(STR_START_X + 15, 10);
        arduboy->print(F("Please choose: "));

        // 选项 A：贪吃蛇（附带 Snaker 类大小信息）
        arduboy->setCursor(STR_START_X, 30);
        arduboy->print(F("Press Up: snake."));
        arduboy->print(String(sizeof(Snaker)));

        // 选项 B：小兔子跳跃游戏
        arduboy->setCursor(STR_START_X, 40);
        arduboy->print(F("Press  D: bunny."));

        // 选项 C：设置
        arduboy->setCursor(STR_START_X, 50);
        arduboy->print(F("Press  A: settings."));

        // 检测上键选择贪吃蛇
        if (btn_ctrl->up_click() && !has_made_choice)
        {
            func_choice = SNAKE;
            has_made_choice = true;
        }

        // 检测右键选择小兔子跳跃游戏
        if (btn_ctrl->right_click() && !has_made_choice)
        {
            func_choice = BUNNY;
            has_made_choice = true;
        }

        // 检测 A 键选择设置
        if (btn_ctrl->a_click() && !has_made_choice)
        {
            func_choice = SETTINGS;
            has_made_choice = true;
        }
    }

    /**
     * play_snake - 运行贪吃蛇模块
     *
     * 延迟实例化：首次进入时创建 Func_snake 实例。
     * 如果内存分配失败（new 返回 NULL），设置全局错误标志。
     */
    void play_snake()
    {
        if (func_snake == NULL)
        {
            func_snake = new Func_snake();
        }
        if (func_snake == NULL)
        {
            is_error = true;  // 内存不足，无法创建模块
        }
        else
        {
            func_snake->play();  // 委托给贪吃蛇模块渲染
        }
    }

    /**
     * play_count - 运行计数器模块
     *
     * 同 play_snake 的延迟实例化逻辑
     */
    void play_count()
    {
        if (func_counter == NULL)
        {
            func_counter = new Func_counter();
        }

        if (func_counter == NULL)
        {
            is_error = true;
        }
        else
        {
            func_counter->play();
        }
    }

    /**
     * play_settings - 运行设置模块
     *
     * 同 play_snake 的延迟实例化逻辑
     */
    void play_settings()
    {
        if (func_settings == NULL)
        {
            func_settings = new Func_settings();
        }

        if (func_settings == NULL)
        {
            is_error = true;
        }
        else
        {
            func_settings->play();
        }
    }

    /**
     * play_bunny - 运行小兔子跳跃游戏模块
     *
     * 同 play_snake 的延迟实例化逻辑
     */
    void play_bunny()
    {
        if (func_bunny == NULL)
        {
            func_bunny = new Func_bunny();
        }

        if (func_bunny == NULL)
        {
            is_error = true;
        }
        else
        {
            func_bunny->play();
        }
    }

    /**
     * stop_game - 停止当前功能模块并释放资源
     *
     * 根据当前选择的功能编号，调用对应模块的 exit() 方法。
     * exit() 会删除模块实例并清空指针。
     *
     * 注意：Func_snake 的 exit() 重写了基类行为，不实际删除对象，
     * 因为频繁 new/delete 在 2.5KB SRAM 上容易造成内存碎片。
     */
    void stop_game()
    {
        if (has_made_choice)
        {
            if (func_choice == SNAKE)
            {
                if (func_snake != NULL)
                {
                    func_snake->exit(&func_snake);
                }
            }
            else if (func_choice == COUNTER)
            {
                if (func_counter != NULL)
                {
                    func_counter->exit(&func_counter);
                }
            }
            else if (func_choice == SETTINGS)
            {
                if (func_settings != NULL)
                {
                    func_settings->exit(&func_settings);
                }
            }
            else if (func_choice == BUNNY)
            {
                if (func_bunny != NULL)
                {
                    func_bunny->exit(&func_bunny);
                }
            }
        }
    }

    /**
     * draw_square - 绘制菜单界面的双线粗边框
     *
     * 通过绘制两条相邻平行线来实现加粗效果，
     * 边框覆盖屏幕四周，中间区域留给菜单文字。
     */
    void draw_square()
    {
        // 上边框（双线加粗）
        arduboy->drawLine(0, 0, WIDTH - BODER_WIDTH, 0, WHITE);
        arduboy->drawLine(0, 1, WIDTH - BODER_WIDTH, 1, WHITE);
        // 左边框（双线加粗）
        arduboy->drawLine(0, 0, 0, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(1, 0, 1, HEIGHT - BODER_WIDTH, WHITE);
        // 右边框（双线加粗）
        arduboy->drawLine(WIDTH - BODER_WIDTH, 0, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(WIDTH - BODER_WIDTH - 1, 0, WIDTH - BODER_WIDTH - 1, HEIGHT - BODER_WIDTH, WHITE);
        // 下边框（双线加粗）
        arduboy->drawLine(0, HEIGHT - BODER_WIDTH, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(0, HEIGHT - BODER_WIDTH - 1, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH - 1, WHITE);
    }
};

#endif // Navigator_hpp
