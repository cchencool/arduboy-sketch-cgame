
#if !defined(Func_settings_hpp)
#define Func_settings_hpp

#include <avr/pgmspace.h>
#include "Arduboy2.h"
#include "arduino.h"
#include "Base_func.hpp"

extern Btn_ctrl *btn_ctrl;
extern Arduboy2 *arduboy;

// for func counter.

class Func_settings: public Base_func
{

  private:
    uint8_t snaker_initial_length;   // should store in EEPROM
  
  public:

    Func_settings()
    {
        this->snaker_initial_length = 30;
    }

    void play()
    {
        this->draw_square();

        arduboy->setCursor(40, 10);
        arduboy->print(F("settings *"));

        arduboy->setCursor(10, 30);
        arduboy->print(F("snaker_length: "));
        arduboy->print(String(this->snaker_initial_length));

        if (btn_ctrl->a_click())
        {
            arduboy->setCursor(10, 40);
            arduboy->print(F("settings saved!"));
        }

    }

    void exit(Base_func **p)
    {
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

    uint8_t get_config()
    {
        return this->snaker_initial_length;
    }

    boolean store()
    {

    }

    boolean load()
    {

    }
};
#endif // Func_settings_hpp
