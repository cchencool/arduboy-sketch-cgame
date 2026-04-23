
#ifndef Btn_ctrl_hpp
#define Btn_ctrl_hpp

#include "Arduino.h"

class Btn_ctrl
{
public:
    Btn_ctrl();
    boolean up_click();
    boolean down_click();
    boolean right_click();
    boolean left_click();
    boolean a_click();
    boolean b_click();

private:
    boolean lock_up;
    boolean lock_down;
    boolean lock_left;
    boolean lock_right;
    boolean lock_a;
    boolean lock_b;

    boolean check_press(boolean &btn_locker, uint8_t ard_button);
    boolean check_release(boolean &btn_locker, uint8_t ard_button);

};

#endif // Btn_ctrl_hpp
