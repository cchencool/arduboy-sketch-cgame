

#if !defined(Navigator_hpp)
#define Navigator_hpp

#include <ArduinoSTL.h>
#include <avr/pgmspace.h>
#include "Arduboy2.h"
#include "Btn_ctrl.hpp"
#include "Base_func.hpp"
#include "Func_snake.hpp"
#include "Func_counter.hpp"
#include "Snaker.hpp"

#define STR_START_X 6

extern Btn_ctrl *btn_ctrl;
extern Arduboy2 *arduboy;
extern Base_func *func_snake;
extern Base_func *func_counter;
extern Base_func *func_settings;
extern boolean is_error;

// for navigator.
PROGMEM const String navigator_str = "Please choose: ";
PROGMEM const String navigator_game_choice_a = "Press Up: snake.";
// PROGMEM const String navigator_game_choice_b = "Press Dn: counter.";
PROGMEM const String navigator_game_choice_c = "Press  A: settings.";

enum Support_func
{
    SNAKE,
    COUNTER,
    SETTINGS
};

class Navigator
{
  private:
    boolean has_made_choice = false;
    Support_func func_choice;

  public:
    Navigator()
    {
        this->func_choice = COUNTER;
    }

    boolean get_has_make_choice()
    {
        return this->has_made_choice;
    }

    void set_has_made_choice(boolean has_or_not)
    {
        this->has_made_choice = has_or_not;
    }

    Support_func get_func_choice()
    {
        return this->func_choice;
    }

    // display navigator.
    void show_navigator()
    {
        draw_square();

        arduboy->setCursor(STR_START_X + 15, 10);
        arduboy->print(navigator_str);

        arduboy->setCursor(STR_START_X, 30);
        arduboy->print(navigator_game_choice_a + String(sizeof(Snaker)));

        // arduboy->setCursor(STR_START_X, 40);
        // arduboy->print(navigator_game_choice_b);

        arduboy->setCursor(STR_START_X, 40);//50);
        arduboy->print(navigator_game_choice_c);

        // game A - Func_snake.
        // if (btn_ctrl->down_click() && !has_made_choice)
        // {
        //     func_choice = COUNTER;
        //     has_made_choice = true;
        // }

        // game B - Func_counter
        if (btn_ctrl->up_click() && !has_made_choice)
        {
            func_choice = SNAKE;
            has_made_choice = true;
        }

        // func C - Func_settings
        if (btn_ctrl->a_click() && !has_made_choice)
        {
            func_choice = SETTINGS;
            has_made_choice = true;
        }
    }

    // play game a. func_snake.
    void play_snake()
    {
        if (func_snake == NULL)
        {
            func_snake = new Func_snake();
        }
        if (func_snake == NULL)
        {
            is_error = true;
        }
        else
        {
            func_snake->play();
        }
    }

    // play game b. counter.
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
        }
    }

    void draw_square()
    {
        arduboy->drawLine(0, 0, WIDTH - BODER_WIDTH, 0, WHITE);
        arduboy->drawLine(0, 1, WIDTH - BODER_WIDTH, 1, WHITE);
        arduboy->drawLine(0, 0, 0, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(1, 0, 1, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(WIDTH - BODER_WIDTH, 0, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(WIDTH - BODER_WIDTH - 1, 0, WIDTH - BODER_WIDTH - 1, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(0, HEIGHT - BODER_WIDTH, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);
        arduboy->drawLine(0, HEIGHT - BODER_WIDTH - 1, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH - 1, WHITE);
    }
};
#endif // Navigator_hpp
