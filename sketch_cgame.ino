
#include <avr/pgmspace.h>
#include <ArduinoSTL.h>
#include "Arduboy2.h"
#include "arduino.h"
#include "Navigator.hpp"
#include "Btn_ctrl.hpp"
#include "Func_snake.hpp"
#include "Func_counter.hpp"
#include "Func_settings.hpp"

Arduboy2 *arduboy = new Arduboy2;
Btn_ctrl *btn_ctrl = new Btn_ctrl;
Navigator *navigator = new Navigator;

// shold use Base_func
Base_func *func_snake;    // = new Func_snake;
Base_func *func_counter;  // = new Func_counter;
Base_func *func_settings; // = new Func_settings;

PROGMEM const String error_str = "error occur!";

boolean is_error = false;

void setup()
{
  // put your setup code here, to run once:

  arduboy->begin();
  // could save code space.
  // arduboy->boot();

  // Serial.print("System, start up...");

  arduboy->setFrameRate(15);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!(arduboy->nextFrame()))
  {
    return;
  }

  arduboy->clear();

  if (is_error)
  {
    arduboy->setCursor(50, 50);
    arduboy->print(error_str);
  }
  else
  {

    if (!navigator->get_has_make_choice())
    {
      navigator->show_navigator();
    }
    else
    {
      if (navigator->get_func_choice() == SNAKE)
      {
        navigator->play_snake();
      }
      else if (navigator->get_func_choice() == COUNTER)
      {
        navigator->play_count();
      }
      else if (navigator->get_func_choice() == SETTINGS)
      {
        navigator->play_settings();
      }
    }
  }

  if ((navigator->get_has_make_choice() || is_error) && btn_ctrl->b_click())
  {
    navigator->stop_game();
    navigator->set_has_made_choice(false);
  }

  arduboy->display();
}
