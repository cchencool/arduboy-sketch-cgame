/**
 * Func_counter.hpp - 计数器功能模块
 *
 * 继承 Base_func，实现：
 *   - 显示计数器界面与当前值
 *   - 上/下键增减数值
 *   - 左/右键改变步进倍率（×10 / ÷10）
 *   - A 键重置计数器
 *   - 按键锁定与连续按处理
 */

#if !defined(Func_counter_hpp)
#define Func_counter_hpp

#include <Arduino.h>
#include "Arduboy2.h"
#include "arduino.h"
#include "Base_func.hpp"

// ==================== 外部全局变量声明 ====================

extern Btn_ctrl *btn_ctrl;  // 按键控制器
extern Arduboy2 *arduboy;   // Arduboy2 实例

/**
 * Func_counter - 计数器功能模块
 *
 * 操作说明：
 *   - 上键：数值 + count_times
 *   - 下键：数值 - count_times
 *   - 右键：倍率 ×10
 *   - 左键：倍率 ÷10
 *   - A 键：重置计数器
 *
 * 防连按机制：
 *   按键被按下后会锁定，松开时才解锁。
 *   但如果持续按住超过 15 帧，则进入「连续按」模式，
 *   允许数值持续变化（模拟长按加速）。
 */
class Func_counter: public Base_func
{

  private:
    long count;           // 当前计数值（初始为 5211314，谐音「我爱你一生一世」）
    long count_times;     // 当前步进倍率（初始为 1）

    const int lock_free_frame_limit;  // 连续按检测帧数阈值（15 帧）
    int lock_free_frame_count;        // 当前连续按键帧计数

    boolean lock;        // 按键锁定标志（防止连按）
    boolean lock_free;   // 连续按模式标志（长按超过阈值后解锁）

  public:
    /**
     * 默认构造函数（成员变量使用类内初始化）
     */
    Func_counter() : count(5211314l), count_times(1l),
                     lock_free_frame_limit(15), lock_free_frame_count(0),
                     lock(false), lock_free(false) {}

    /** 获取当前计数值 */
    long get_count()
    {
        return count;
    }

    /** 获取当前步进倍率 */
    int get_count_times()
    {
        return count_times;
    }

    /** 设置计数值 */
    void set_count(long count)
    {
        this->count = count;
    }

    /**
     * play - 模块主逻辑，每帧由 Navigator 调用
     *
     * 执行顺序：
     *   1. 绘制界面边框
     *   2. 处理计数输入（按键）
     *   3. 更新连续按标志
     *   4. 更新锁定标志
     *   5. 渲染文字（问候语、计数值、倍率）
     */
    void play()
    {
        this->draw_square();           // 绘制界面边框

        this->do_count();              // 处理按键输入并更新计数

        this->check_lock_free_flag();  // 检测是否进入连续按模式

        this->check_lock_flag();       // 检测是否应重置锁定

        // 渲染界面文字
        arduboy->setCursor(10, 20);
        arduboy->print(F("hello, my darling."));

        arduboy->setCursor(10, 30);
        arduboy->print(String("count:  ") + String(this->get_count()));

        arduboy->setCursor(10, 40);
        arduboy->print(String("count_times:  ") + String(this->get_count_times()));
    }

    /**
     * exit - 退出计数器模块
     *
     * 先重置计数器，再调用基类的 exit 释放模块实例。
     *
     * @param p 指向模块指针的指针
     */
    void exit(Base_func **p)
    {
        this->reset_counter();  // 重置计数值
        Base_func::exit(p);     // 释放模块实例
    }

    /**
     * draw_square - 绘制计数器界面边框
     *
     * 绘制梯形装饰边框（顶部较窄，底部较宽，角部斜线过渡）
     */
    void draw_square()
    {
        arduboy->drawLine(0, 0, 5, 5, WHITE);          // 左上角斜线
        arduboy->drawLine(5, 5, 122, 5, WHITE);        // 上边
        arduboy->drawLine(5, 5, 5, 58, WHITE);         // 左边
        arduboy->drawLine(122, 5, 127, 0, WHITE);      // 右上角斜线
        arduboy->drawLine(122, 5, 122, 58, WHITE);     // 右边
        arduboy->drawLine(122, 58, 127, 63, WHITE);    // 右下角斜线
        arduboy->drawLine(5, 58, 122, 58, WHITE);      // 下边
        arduboy->drawLine(5, 58, 0, 63, WHITE);        // 左下角斜线
    }

    /**
     * reset_counter - 重置计数器
     *
     * 将计数值归零，倍率重置为 1，同时锁定按键。
     */
    void reset_counter()
    {
        count = 0;
        count_times = 1;
        lock = true;  // 锁定，防止重置瞬间再次触发按键
    }

    /**
     * do_count - 处理按键输入并更新计数
     *
     * 各按键功能：
     *   上键：count += count_times    （增加）
     *   下键：count -= count_times    （减少）
     *   右键：count_times *= 10       （增大步进倍率）
     *   左键：count_times *= 0.1      （缩小步进倍率，最小为 1）
     *   A 键：重置计数器
     *
     * 所有操作受 lock / lock_free 标志控制，防止误触。
     */
    void do_count()
    {
        // 上键：增加计数
        if (arduboy->pressed(UP_BUTTON) == true && (!lock || lock_free))
        {
            count += count_times;
            lock = true;              // 锁定，等待释放
            lock_free_frame_count++;  // 计入连续按帧数
        }

        // 下键：减少计数
        if (arduboy->pressed(DOWN_BUTTON) == true && (!lock || lock_free))
        {
            count -= count_times;
            lock = true;
        }

        // 右键：增大步进取 10 倍
        if (arduboy->pressed(RIGHT_BUTTON) == true && (!lock || lock_free))
        {
            count_times *= 10;
            lock = true;
        }

        // 左键：缩小步进为 1/10（最小为 1）
        if (arduboy->pressed(LEFT_BUTTON) == true && (!lock || lock_free))
        {
            count_times *= 0.1;
            if (count_times == 0)
            {
                count_times = 1;  // 保证最小步进为 1
            }
            lock = true;
        }

        // A 键：重置计数器
        if (arduboy->pressed(A_BUTTON) == true && (!lock || lock_free))
        {
            reset_counter();
        }
    }

    /**
     * check_lock_free_flag - 检测连续按模式
     *
     * 如果任意按键被按下，则递增连续按帧计数。
     * 当连续按帧数达到阈值（15 帧）时：
     *   - 解除锁定（允许持续输入）
     *   - 标记进入连续按模式
     *
     * 效果：长按超过 0.5 秒（15 帧 @ 30fps ≈ 0.5s）后，
     * 数值会持续快速变化，提升操作效率。
     */
    void check_lock_free_flag()
    {
        // 任意按键按下时，递增连续按帧计数
        if (arduboy->pressed(A_BUTTON) == true || arduboy->pressed(B_BUTTON) == true
         || arduboy->pressed(UP_BUTTON) == true || arduboy->pressed(DOWN_BUTTON) == true
         || arduboy->pressed(RIGHT_BUTTON) == true || arduboy->pressed(LEFT_BUTTON) == true)
        {
            lock_free_frame_count++;
        }

        // 达到连续按阈值，进入连续模式
        if (lock_free_frame_count >= lock_free_frame_limit)
        {
            lock = false;      // 解除锁定，允许持续输入
            lock_free = true;  // 标记连续按模式
        }
    }

    /**
     * check_lock_flag - 检测并重置锁定标志
     *
     * 当所有按键都已释放时：
     *   - 解除锁定
     *   - 退出连续按模式
     *   - 重置连续按帧计数
     *
     * 确保下次按键从单步模式重新开始。
     */
    void check_lock_flag()
    {
        // 所有按键都已释放
        if (arduboy->notPressed(A_BUTTON) == true && arduboy->notPressed(B_BUTTON) == true
         && arduboy->notPressed(UP_BUTTON) == true && arduboy->notPressed(DOWN_BUTTON) == true
         && arduboy->notPressed(RIGHT_BUTTON) == true && arduboy->notPressed(LEFT_BUTTON) == true)
        {
            lock = false;            // 解除锁定
            lock_free = false;       // 退出连续按模式
            lock_free_frame_count = 0;  // 重置帧计数
        }
    }
};

#endif // Func_counter_hpp
