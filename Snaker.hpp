

#if !defined(Snaker_hpp)
#define Snaker_hpp

#include <arduino.h>
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

// Minimal dynamic array replacing std::vector for memory-constrained AVR
template<typename T>
class Vec {
    T* data;
    uint8_t _size;
    uint8_t capacity;

public:
    Vec() : data(NULL), _size(0), capacity(0) {}
    ~Vec() { clear(); }

    uint8_t size() const { return _size; }

    void push_back(T item) {
        if (_size >= capacity) {
            uint8_t new_cap = capacity == 0 ? 4 : capacity * 2;
            T* new_data = (T*)realloc(data, new_cap * sizeof(T));
            if (new_data) {
                data = new_data;
                capacity = new_cap;
            } else {
                return;
            }
        }
        data[_size++] = item;
    }

    T& operator[](uint8_t i) { return data[i]; }
    const T& operator[](uint8_t i) const { return data[i]; }

    void erase_first() {
        if (_size > 0) {
            for (uint8_t i = 0; i < _size - 1; i++) {
                data[i] = data[i + 1];
            }
            _size--;
        }
    }

    void clear() {
        free(data);
        data = NULL;
        _size = 0;
        capacity = 0;
    }
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
    Vec<Turn_Point*> turning_points; // only store
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
