/**
 * Btn_ctrl.hpp - 按键消抖与点击检测接口
 *
 * 封装 Arduboy2 的原始按键 API，提供「点击」检测：
 *   按下 → 释放 算作一次点击，防止长按时重复触发
 */

#ifndef Btn_ctrl_hpp
#define Btn_ctrl_hpp

#include "Arduino.h"

/**
 * Btn_ctrl - 按键控制器
 *
 * 内部为每个按键维护一个锁定标志（lock_*），
 * 在按下时锁定，释放时解锁并返回 true，
 * 从而实现「一次按键 = 一次点击」的语义。
 */
class Btn_ctrl
{
public:
    Btn_ctrl();

    /** 各按键的点击检测：按下后释放时返回一次 true */
    boolean up_click();    // 上键
    boolean down_click();  // 下键
    boolean right_click(); // 右键
    boolean left_click();  // 左键
    boolean a_click();     // A 键（功能键）
    boolean b_click();     // B 键（返回键）

private:
    /** 各按键的锁定标志，防止重复触发 */
    boolean lock_up;
    boolean lock_down;
    boolean lock_left;
    boolean lock_right;
    boolean lock_a;
    boolean lock_b;

    /**
     * 检查按键是否被按下并锁定
     * @param btn_locker 按键锁定标志的引用
     * @param ard_button Arduboy2 定义的按键常量
     * @return 如果刚按下则锁定并返回 true（实际点击结果由 check_release 决定）
     */
    boolean check_press(boolean &btn_locker, uint8_t ard_button);

    /**
     * 检查按键是否已释放，释放时解锁并返回 true
     * @param btn_locker 按键锁定标志的引用
     * @param ard_button Arduboy2 定义的按键常量
     * @return 如果已锁定且检测到释放，则解锁并返回 true
     */
    boolean check_release(boolean &btn_locker, uint8_t ard_button);

};

#endif // Btn_ctrl_hpp
