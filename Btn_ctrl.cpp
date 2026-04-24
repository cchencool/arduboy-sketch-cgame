/**
 * Btn_ctrl.cpp - 按键消抖与点击检测实现
 *
 * 核心逻辑：
 *   按下时锁定标志 → 释放时解锁并返回 true
 *   这样每次按键只有释放的那一帧算作一次「点击」
 */

#include "Arduboy2.h"
#include "arduino.h"
#include "Btn_ctrl.hpp"

/** Arduboy2 全局实例（在 sketch_cgame.ino 中创建） */
extern Arduboy2 *arduboy;

/** 按键编号（与 Arduboy2 的物理按键对应） */
enum Buttons
{
    B_UP,
    B_DOWN,
    B_LEFT,
    B_RIGHT,
    B_A,
    B_B
};

/**
 * 构造函数：初始化所有按键锁定标志为 false（未按下）
 */
Btn_ctrl::Btn_ctrl()
{
    this->lock_up    = false;
    this->lock_down  = false;
    this->lock_left  = false;
    this->lock_right = false;
    this->lock_a     = false;
    this->lock_b     = false;
}

/** 上键点击检测 */
boolean Btn_ctrl::up_click()
{
   return this->check_press(this->lock_up, UP_BUTTON);
}

/** 下键点击检测 */
boolean Btn_ctrl::down_click()
{
    return this->check_press(this->lock_down, DOWN_BUTTON);
}

/** 右键点击检测 */
boolean Btn_ctrl::right_click()
{
    return this->check_press(this->lock_right, RIGHT_BUTTON);
}

/** 左键点击检测 */
boolean Btn_ctrl::left_click()
{
    return this->check_press(this->lock_left, LEFT_BUTTON);
}

/** A 键点击检测 */
boolean Btn_ctrl::a_click()
{
    return this->check_press(this->lock_a, A_BUTTON);
}

/** B 键点击检测 */
boolean Btn_ctrl::b_click()
{
    return this->check_press(this->lock_b, B_BUTTON);
}

/**
 * check_press - 检查按键按下并锁定
 *
 * 如果按键被按下且当前未锁定：
 *   1. 锁定标志设为 true（防止同一轮再次触发）
 *   2. 交由 check_release 等待释放
 *
 * @param btn_locker  按键锁定标志的引用
 * @param ard_button  Arduboy2 按键常量
 * @return 按键已锁定且已释放时返回 true（即一次完整的点击）
 */
boolean Btn_ctrl::check_press(boolean &btn_locker, uint8_t ard_button)
{
    // 按键被按下且尚未锁定 → 锁定
    if (arduboy->pressed(ard_button) == true && !btn_locker)
    {
        btn_locker = true;
    }

    // 根据按键类型调用对应的释放检测
    switch (ard_button)
    {
    case UP_BUTTON:
        return check_release(lock_up, UP_BUTTON);
    case DOWN_BUTTON:
        return check_release(lock_down, DOWN_BUTTON);
    case LEFT_BUTTON:
        return check_release(lock_left, LEFT_BUTTON);
    case RIGHT_BUTTON:
        return check_release(lock_right, RIGHT_BUTTON);
    case A_BUTTON:
        return check_release(lock_a, A_BUTTON);
    case B_BUTTON:
        return check_release(lock_b, B_BUTTON);
    default:
        break;
    }
    return false;
}

/**
 * check_release - 检查已锁定的按键是否已释放
 *
 * 当按键处于锁定状态且检测到已释放时：
 *   1. 解锁标志
 *   2. 返回 true（表示一次完整的点击完成）
 *
 * @param btn_locker  按键锁定标志的引用
 * @param ard_button  Arduboy2 按键常量
 * @return 锁定已解除时返回 true
 */
boolean Btn_ctrl::check_release(boolean &btn_locker, uint8_t ard_button)
{
    if (btn_locker && arduboy->notPressed(ard_button) == true)
    {
        btn_locker = false;  // 解锁，允许下次点击
        return true;         // 一次点击完成
    }
    else
    {
        return false;
    }
}
