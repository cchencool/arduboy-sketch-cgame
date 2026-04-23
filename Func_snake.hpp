

#if !defined(Func_snake_hpp)
#define Func_snake_hpp

#include <arduino.h>
#include "Arduboy2.h"
#include "Snaker.hpp"
#include "Base_func.hpp"
#include "Func_settings.hpp"


extern Arduboy2 *arduboy;
extern Btn_ctrl *btn_ctrl;
extern Base_func *func_settings;

PROGMEM const String pause_str = "Pause";
PROGMEM const String find_egg_str = "Found Egg!";

uint8_t time_counter = 0;

class Func_snake : public Base_func
{

  private:
    Snaker *snaker; // = new Snaker(10);
    Point *food_pt;
    boolean is_eaten = false;

  public:
    Func_snake()
    {
        // if (func_settings != NULL)
        // {
        //     this->snaker = new Snaker(func_settings->get_config());
        // }
        // else
        // {
        this->snaker = new Snaker(INITIAL_SNAKE_LENGTH);
        this->food_pt = new Point;
        this->food_pt->x = random(126) + 1;
        this->food_pt->y = random(62) + 1;
        // }
    }

    ~Func_snake()
    {
        delete snaker;
        delete food_pt;
        snaker = NULL;
        food_pt = NULL;
    }

    void play()
    {
        draw_square();

        draw_food();

        if_eat();

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

        if (btn_ctrl->a_click())
        {
            if (snaker->get_status() == ACTIVE)
            {
                snaker->stop(false);
            }
            else if (snaker->get_status() == INACTIVE)
            {
                snaker->start();
            }
        }

        if (snaker->get_status() == ACTIVE)
        {
            boolean move_result = snaker->move(1);
            if (!move_result) {
                snaker->reset_body();
            }
        }
        else
        {
            arduboy->setCursor(95, 55);
            arduboy->print(pause_str);
        }

        if (snaker->if_find_egg())
        {
            arduboy->setCursor(0, 0);
            arduboy->print(find_egg_str);
            if (++time_counter % 15 == 0)
            {
                snaker->reset_egg();
            }
        }

        // snaker->show(arduboy, &Arduboy2::drawPixel);
        snaker->show(arduboy);
    }

    void exit(Base_func **p)
    {
        this->snaker->stop(false);
        /**
         * if we delete the obj to free mem. it may occur some problem when keep enter func_snake and exit.
         * so far, I think it may cause by the 2.5Kb memory cannot afford such frequent new/delete obj.
         * 
         * 2018/05/13
         */
        // Base_func::exit(p);
    }

    boolean if_eat()
    {
        Point *head = this->snaker->getHead();
        if (head != NULL 
        && head->x - this->food_pt->x <= 1 
        && head->x - this->food_pt->x >= 0 
        && head->y - this->food_pt->y <= 1
        && head->y - this->food_pt->y >= 0)
        {
            this->snaker->grow();
            is_eaten = true;
            // delete this->food_pt;
            // this->food_pt = NULL;
        }
    }

    void draw_food()
    {
        boolean is_valid = false;
        while (is_eaten && !is_valid)//this->food_pt == NULL) 
        {
            // this->food_pt = new Point;
            this->food_pt->x = random(126) + 1;
            this->food_pt->y = random(62) + 1;

            // invalid area
            if (this->food_pt->x > 0
            && this->food_pt->x < 35//15
            && this->food_pt->y > 50
            && this->food_pt->y < 64)
            {
                is_valid = false;
            }
            else
            {
                is_valid = true;
            }
            is_eaten = false;
        }


        arduboy->drawPixel(this->food_pt->x, this->food_pt->y, WHITE);
        arduboy->drawPixel(this->food_pt->x + 1, this->food_pt->y, WHITE);
        arduboy->drawPixel(this->food_pt->x, this->food_pt->y + 1, WHITE);
        arduboy->drawPixel(this->food_pt->x + 1, this->food_pt->y + 1 , WHITE);
    }

    void draw_square()
    {
        arduboy->drawLine(0, 0, WIDTH - BODER_WIDTH, 0, WHITE); // -(up)
        arduboy->drawLine(0, 0, 0, HEIGHT - BODER_WIDTH, WHITE);    // |(left)
        arduboy->drawLine(WIDTH - BODER_WIDTH, 0, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);    // |(right)
        arduboy->drawLine(0, HEIGHT - BODER_WIDTH, WIDTH - BODER_WIDTH, HEIGHT - BODER_WIDTH, WHITE);   // -(down)
    }
};

#endif // Func_snake_hpp
