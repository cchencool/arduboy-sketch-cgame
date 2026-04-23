

#if !defined(Snaker_hpp)
#define Snaker_hpp

#include <arduino.h>
#include <ArduinoSTL.h>
#include "Arduboy2.h"

#define INITIAL_SNAKE_LENGTH 15 // when the length is large than 26/27, out of memory error.
#define BODER_WIDTH 1

// struct Point
// {
//     int x;
//     int y;
// };

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// 
struct Turn_Point
{
    Point pt;
    Direction drc;
};

enum Status
{
    ACTIVE,
    INACTIVE
};

class Snaker
{

  private:
    uint8_t length;
    // std::vector<Point*> body_points;    // begin() is the tail. the last element is the head.
    Direction head_drc;
    Status status;
    // Point *p_for;   // temp var to transfer move.
    // boolean rezise(int8_t length);
    boolean find_egg;
    boolean init_body(uint8_t length);

    // for memory optimization. replace 'body_points'
    std::vector<Turn_Point*> turning_points; // only store
    Turn_Point* head;
    Turn_Point* tail;

  public:
    Snaker(uint8_t length);
    ~Snaker();

    boolean if_find_egg();
    void reset_egg();

    Status get_status();
    boolean start();
    boolean stop(boolean is_dead);
    boolean turn_to(Direction turn_drc);

    // get the head or tail point.
    Point* getHead();
    Point* getTail();

    // boolean move_one();
    boolean move(uint8_t step);
    boolean grow();
    boolean reset_body();
    // boolean shrink(uint8_t length);
    // void show(Arduboy2 *Obj, void (Arduboy2::*p_call)(int16_t, int16_t, uint8_t));
    void show(Arduboy2 *ard);
};

#endif // Snaker_hpp
