/**
 * Func_snake.hpp - 贪吃蛇功能模块
 *
 * 继承 Base_func，实现：
 *   - 游戏边框绘制
 *   - 食物生成与绘制
 *   - 碰撞检测（吃食物）
 *   - 暂停/恢复控制
 *   - 彩蛋提示
 */

#if !defined(Func_snake_hpp)
#define Func_snake_hpp

#include <arduino.h>
#include "Arduboy2.h"
#include "Snaker.hpp"
#include "Base_func.hpp"
#include "Func_settings.hpp"

// ==================== 外部全局变量声明 ====================

extern Arduboy2 *arduboy;      // Arduboy2 实例
extern Btn_ctrl *btn_ctrl;     // 按键控制器
extern Base_func *func_settings; // 设置模块指针（预留：获取蛇初始长度配置）

/** 彩蛋提示显示帧计数器 */
uint8_t time_counter = 0;

/**
 * Func_snake - 贪吃蛇功能模块
 *
 * 每帧执行流程：
 *   1. 绘制游戏边框
 *   2. 绘制食物
 *   3. 检测是否吃到食物
 *   4. 处理方向键输入（转弯）
 *   5. 处理 A 键输入（暂停/恢复）
 *   6. 移动蛇体或显示暂停提示
 *   7. 彩蛋提示显示
 *   8. 渲染蛇体
 */
class Func_snake : public Base_func
{

  private:
    Snaker *snaker;     // 贪吃蛇引擎实例
    Point *food_pt;     // 食物坐标
    boolean is_eaten;   // 食物是否被吃掉（需要重新生成）

  public:
    /**
     * 构造函数：初始化贪吃蛇模块
     *
     * - 创建 Snaker 引擎实例（使用默认初始长度）
     * - 在屏幕有效区域内随机生成食物坐标
     *
     * 预留：从设置模块获取自定义蛇初始长度（已注释）
     */
    Func_snake()
    {
        // 预留：从设置模块获取自定义长度
        // if (func_settings != NULL)
        // {
        //     this->snaker = new Snaker(func_settings->get_config());
        // }

        this->snaker = new Snaker(INITIAL_SNAKE_LENGTH);

        // 随机生成食物坐标（有效范围：x: 1~126, y: 1~62）
        this->food_pt = new Point;
        this->food_pt->x = random(126) + 1;
        this->food_pt->y = random(62) + 1;
    }

    /**
     * 析构函数：释放蛇引擎和食物坐标的内存
     */
    ~Func_snake()
    {
        delete snaker;
        delete food_pt;
        snaker = NULL;
        food_pt = NULL;
    }

    /**
     * play - 模块主逻辑，每帧由 Navigator 调用
     *
     * 包含：
     *   - 渲染（边框、食物、蛇体、暂停提示、彩蛋）
     *   - 输入处理（方向键转弯、A 键暂停）
     *   - 游戏逻辑（吃食物检测、蛇体移动、撞墙重置）
     */
    void play()
    {
        draw_square();   // 绘制游戏边框

        draw_food();     // 绘制食物

        if_eat();        // 检测蛇头是否吃到食物

        // 方向键控制蛇转弯
        if (btn_ctrl->up_click())
        {
            snaker->turn_to(UP);
        }
        else if (btn_ctrl->down_click())
        {
            snaker->turn_to(DOWN);
        }
        else if (btn_ctrl->left_click())
        {
            snaker->turn_to(LEFT);
        }
        else if (btn_ctrl->right_click())
        {
            snaker->turn_to(RIGHT);
        }

        // A 键切换暂停/运行状态
        if (btn_ctrl->a_click())
        {
            if (snaker->get_status() == ACTIVE)
            {
                snaker->stop(false);  // 暂停
            }
            else if (snaker->get_status() == INACTIVE)
            {
                snaker->start();      // 恢复
            }
        }

        if (snaker->get_status() == ACTIVE)
        {
            // 运行中：移动蛇体一格
            boolean move_result = snaker->move(1);
            if (!move_result) {
                // 撞墙：重置蛇体（重新开始）
                snaker->reset_body();
            }
        }
        else
        {
            // 暂停中：显示 "Pause" 文字
            arduboy->setCursor(95, 55);
            arduboy->print(F("Pause"));
        }

        // 彩蛋提示
        if (snaker->if_find_egg())
        {
            arduboy->setCursor(0, 0);
            arduboy->print(F("Found Egg!"));

            // 显示 15 帧后自动清除彩蛋提示
            if (++time_counter % 15 == 0)
            {
                snaker->reset_egg();
            }
        }

        // 渲染蛇体到屏幕
        snaker->show(arduboy);
    }

    /**
     * exit - 退出贪吃蛇模块
     *
     * 暂停蛇引擎，但不删除对象。
     * 原因：在 2.5KB SRAM 上频繁 new/delete 容易造成内存碎片，
     * 反复进出贪吃蛇模块可能导致内存不足。
     *
     * @param p 指向模块指针的指针（保留参数以匹配基类接口）
     */
    void exit(Base_func **p)
    {
        this->snaker->stop(false);
        // 不删除对象，保留实例以备下次进入时复用
        // Base_func::exit(p);
    }

    /**
     * if_eat - 检测蛇头是否吃到食物
     *
     * 碰撞判定：蛇头与食物坐标的距离在 1 像素以内即视为吃到。
     * 吃到后蛇体增长，标记食物已消耗，等待下次重新生成。
     *
     * @return 始终返回 void（通过 is_eaten 标志传递状态）
     */
    boolean if_eat()
    {
        Point *head = this->snaker->getHead();
        if (head != NULL
        && head->x - this->food_pt->x <= 1   // 蛇头在食物右侧 1 格内
        && head->x - this->food_pt->x >= 0   // 蛇头在食物左侧或正上方
        && head->y - this->food_pt->y <= 1   // 蛇头在食物下方 1 格内
        && head->y - this->food_pt->y >= 0)  // 蛇头在食物上方或正左方
        {
            this->snaker->grow();  // 蛇体增长
            is_eaten = true;       // 标记食物已消耗
        }
    }

    /**
     * draw_food - 绘制食物
     *
     * 食物为 2x2 实心方块（4 个白色像素点）。
     * 如果食物已被消耗（is_eaten），则在有效区域内重新生成。
     * 避开右下角无效区域（x: 0~35, y: 50~64，即调试信息显示区）。
     */
    void draw_food()
    {
        boolean is_valid = false;
        while (is_eaten && !is_valid)
        {
            // 随机生成新食物坐标
            this->food_pt->x = random(126) + 1;
            this->food_pt->y = random(62) + 1;

            // 排除右下角调试信息显示区域
            if (this->food_pt->x > 0
            && this->food_pt->x < 35
            && this->food_pt->y > 50
            && this->food_pt->y < 64)
            {
                is_valid = false;  // 落在无效区域，重新生成
            }
            else
            {
                is_valid = true;   // 有效位置，接受
            }
            is_eaten = false;      // 清除已消耗标志
        }

        // 绘制 2x2 实心方块
        arduboy->drawPixel(this->food_pt->x, this->food_pt->y, WHITE);
        arduboy->drawPixel(this->food_pt->x + 1, this->food_pt->y, WHITE);
        arduboy->drawPixel(this->food_pt->x, this->food_pt->y + 1, WHITE);
        arduboy->drawPixel(this->food_pt->x + 1, this->food_pt->y + 1, WHITE);
    }

    /**
     * draw_square - 绘制游戏区域边框
     *
     * 绘制单线矩形边框，包围整个 128x64 屏幕。
     */
    void draw_square()
    {
        arduboy->drawLine(0, 0, WIDTH - BODER_WIDTH, 0, WHITE);             // 上边框
        arduboy->drawLine(0, 0, 0, HEIGHT - BODER_WIDTH, WHITE);            // 左边框
        arduboy->drawLine(WIDTH - BODER_WIDTH, 0, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);  // 右边框
        arduboy->drawLine(0, HEIGHT - BODER_WIDTH, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE); // 下边框
    }
};

#endif // Func_snake_hpp
