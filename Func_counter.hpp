
#if !defined(Func_counter_hpp)
#define Func_counter_hpp

#include <ArduinoSTL.h>
#include "Arduboy2.h"
#include "arduino.h"
#include "Base_func.hpp"

extern Btn_ctrl *btn_ctrl;
extern Arduboy2 *arduboy;

// for func counter.
PROGMEM const String hello_str = "hello, my darling.";

class Func_counter: public Base_func
{

  private:
    long count = 5211314l;
    long count_times = 1l;

    const int lock_free_frame_limit = 15;
    int lock_free_frame_count = 0;

    boolean lock = false;
    boolean lock_free = false;

  public:
    // Func_counter(Arduboy2 *arduboy)
    // {
    //   this->arduboy = arduboy;
    // }

    long get_count()
    {
        return count;
    }

    int get_count_times()
    {
        return count_times;
    }

    void set_count(long count)
    {
        this->count = count;
    }

    void play()
    {
        this->draw_square();

        this->do_count();

        this->check_lock_free_flag();

        this->check_lock_flag();

        arduboy->setCursor(10, 20);
        arduboy->print(hello_str);

        arduboy->setCursor(10, 30);
        arduboy->print(String("count:  ") + String(this->get_count()));

        arduboy->setCursor(10, 40);
        arduboy->print(String("count_times:  ") + String(this->get_count_times()));
    }

    void exit(Base_func **p)
    {
        this->reset_counter();
        Base_func::exit(p);
    }

    void draw_square()
    {
        arduboy->drawLine(0, 0, 5, 5, WHITE);
        arduboy->drawLine(5, 5, 122, 5, WHITE);
        arduboy->drawLine(5, 5, 5, 58, WHITE);
        arduboy->drawLine(122, 5, 127, 0, WHITE);
        arduboy->drawLine(122, 5, 122, 58, WHITE);
        arduboy->drawLine(122, 58, 127, 63, WHITE);
        arduboy->drawLine(5, 58, 122, 58, WHITE);
        arduboy->drawLine(5, 58, 0, 63, WHITE);
    }

    void reset_counter()
    {
        count = 0;
        count_times = 1;
        lock = true;
    }

    void do_count()
    {

        // up
        if (arduboy->pressed(UP_BUTTON) == true && (!lock || lock_free))
        {
            count += count_times;
            lock = true;
            lock_free_frame_count++;
        }

        // down
        if (arduboy->pressed(DOWN_BUTTON) == true && (!lock || lock_free))
        {
            count -= count_times;
            lock = true;
        }

        // right
        if (arduboy->pressed(RIGHT_BUTTON) == true && (!lock || lock_free))
        {
            count_times *= 10;
            lock = true;
        }

        // left
        if (arduboy->pressed(LEFT_BUTTON) == true && (!lock || lock_free))
        {
            count_times *= 0.1;
            if (count_times == 0)
            {
                count_times = 1;
            }
            lock = true;
        }

        // func
        if (arduboy->pressed(A_BUTTON) == true && (!lock || lock_free))
        {
            reset_counter();
        }
    }

    void check_lock_free_flag()
    {

        if (arduboy->pressed(A_BUTTON) == true || arduboy->pressed(B_BUTTON) == true || arduboy->pressed(UP_BUTTON) == true || arduboy->pressed(DOWN_BUTTON) == true || arduboy->pressed(RIGHT_BUTTON) == true || arduboy->pressed(LEFT_BUTTON) == true)
        {
            lock_free_frame_count++;
        }

        if (lock_free_frame_count >= lock_free_frame_limit)
        {
            lock = false;
            lock_free = true;
        }
    }

    void check_lock_flag()
    {

        if (arduboy->notPressed(A_BUTTON) == true && arduboy->notPressed(B_BUTTON) == true && arduboy->notPressed(UP_BUTTON) == true && arduboy->notPressed(DOWN_BUTTON) == true && arduboy->notPressed(RIGHT_BUTTON) == true && arduboy->notPressed(LEFT_BUTTON) == true)
        {
            lock = false;
            lock_free = false;
            lock_free_frame_count = 0;
        }
    }
};
#endif // Func_counter_hpp
