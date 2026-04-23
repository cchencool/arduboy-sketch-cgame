
#include "Arduboy2.h"
#include "arduino.h"
#include "Btn_ctrl.hpp"

extern Arduboy2 *arduboy;

enum Buttons
{
    B_UP,
    B_DOWN,
    B_LEFT,
    B_RIGHT,
    B_A,
    B_B
};

Btn_ctrl::Btn_ctrl()
{
    this->lock_up = false;
    this->lock_down = false;
    this->lock_left = false;
    this->lock_right = false;
    this->lock_a = false;
    this->lock_b = false;
}

boolean Btn_ctrl::up_click()
{
   return this->check_press(this->lock_up, UP_BUTTON); 
}
boolean Btn_ctrl::down_click()
{
    return this->check_press(this->lock_down, DOWN_BUTTON); 
}

boolean Btn_ctrl::right_click()
{
    return this->check_press(this->lock_right, RIGHT_BUTTON); 
}

boolean Btn_ctrl::left_click()
{
    return this->check_press(this->lock_left, LEFT_BUTTON); 
}

boolean Btn_ctrl::a_click()
{
    return this->check_press(this->lock_a, A_BUTTON); 
}

boolean Btn_ctrl::b_click()
{
    return this->check_press(this->lock_b, B_BUTTON); 
}



boolean Btn_ctrl::check_press(boolean &btn_locker, uint8_t ard_button)
{
    if (arduboy->pressed(ard_button) == true && !btn_locker)
    {
        btn_locker = true;
    }
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

boolean Btn_ctrl::check_release(boolean &btn_locker, uint8_t ard_button)
{
    if (btn_locker && arduboy->notPressed(ard_button) == true)
    {
        btn_locker = false;
        return true;
    }
    else
    {
        return false;
    }
}
