
#include <arduino.h>
#include "Arduboy2.h"
#include "Snaker.hpp"

Snaker::Snaker(uint8_t length) : length(length)
{

    this->status = INACTIVE;
    this->head_drc = RIGHT;
    this->find_egg = false;

    this->head = NULL;
    this->tail = NULL;
    this->init_body(length);
}

Snaker::~Snaker()
{
    for (uint8_t i = 0; i < this->turning_points.size(); i++)
    {
       delete this->turning_points[i];
    }
    this->turning_points.clear();
    delete this->head;
    delete this->tail;
    this->head = NULL;
    this->tail = NULL;

    // for (uint8_t i = 0; i < this->body_points.size(); i++)
    // {
    //     delete body_points[i];
    // }
    // body_points.clear();
    // delete this->p_for;
    // this->p_for = NULL;
}

boolean Snaker::init_body(uint8_t length)
{
    if (length > 0)
    {
        if (this->head == NULL)
        {
            this->head = new Turn_Point;
        }
        this->head->pt.x = 5 + length;
        this->head->pt.y = 5;
        this->head->drc = RIGHT;

        if (this->tail == NULL)
        {
            this->tail = new Turn_Point;
        }
        this->tail->pt.x = 5;
        this->tail->pt.y = 5;
        this->tail->drc = RIGHT;

        this->length = length;

        // Point *p;
        // for (uint8_t i = 0; i < length; i++)
        // {
        //     p = new Point;
        //     p->x = 5 + i;
        //     p->y = 5 + i;
        //     this->body_points.push_back(p);
        // }
        return true;
    }
    else
    {
        return false;
    }
}

boolean Snaker::reset_body()
{
    for (uint8_t i = 0; i < this->turning_points.size(); i++)
    {
       delete this->turning_points[i];
    }
    this->turning_points.clear();
    delete this->head;
    delete this->tail;
    this->head = NULL;
    this->tail = NULL;

    return this->init_body(INITIAL_SNAKE_LENGTH);

    // for (uint8_t i = 0; i < this->body_points.size(); i++)
    // {
    //     delete this->body_points[i];
    // }
    // this->body_points.clear();
    // this->init_body(INITIAL_SNAKE_LENGTH);

    // if (this->body_points.size() > 0)
    // {
    //     this->head_drc = RIGHT;
    //     Point *p;
    //     for (uint8_t i = 0; i < this->body_points.size(); i++)
    //     {
    //         p = this->body_points[i];
    //         p->x = 5 + i;
    //         p->y = 5 + i;
    //     }
    //     return true;
    // }
    // else
    // {
    //     return false;
    // }
}

Status Snaker::get_status()
{
    return this->status;
}

boolean Snaker::start()
{
    if (this->status == INACTIVE)
    {
        this->status = ACTIVE;
        return true;
    }
    else
    {
        return false;
    }
}

boolean Snaker::stop(boolean is_dead)
{
    if (is_dead)
    {
        // save record.
    }

    if (this->status == ACTIVE)
    {
        this->status = INACTIVE;
        return true;
    }
    else
    {
        return false;
    }
}

boolean Snaker::turn_to(Direction turn_drc)
{
    if (this->status == INACTIVE)
    {
        return false;
    }

    boolean trun_result = false;
    Direction reverse_drc;

    switch (turn_drc)
    {
    case UP:
        reverse_drc = DOWN;
        break;
    case DOWN:
        reverse_drc = UP;
        break;
    case LEFT:
        reverse_drc = RIGHT;
        break;
    case RIGHT:
        reverse_drc = LEFT;
        break;

    default:
        break;
    }

    if (this->head_drc != turn_drc && this->head_drc != reverse_drc)
    {
        this->head_drc = turn_drc;
        trun_result = true;

        Turn_Point *turn_pt = new Turn_Point;
        turn_pt->pt.x = this->head->pt.x;
        turn_pt->pt.y = this->head->pt.y;
        turn_pt->drc = this->head_drc;

        this->turning_points.push_back(turn_pt);
    }
    else
    {
        trun_result = false;
    }

    return trun_result;
}

// boolean Snaker::move_one()
// {
//     if (this->status == INACTIVE)
//     {
//         return false;
//     }

//     boolean has_move = false;

//     Point *p_head = this->body_points[this->body_points.size() - 1];

//     std::vector<Point*>::iterator vec_itr = this->body_points.begin();

//     switch (this->head_drc)
//     {
//     case UP:
//         if (p_head->y > 0)
//         {
//             while (vec_itr != this->body_points.end()) {
//                 (*vec_itr)->y --;
//                 vec_itr++;
//             }
//         }
//         else
//         {
//             this->stop();
//             has_move = false;
//         }
//         break;
//     case DOWN:
//         if (p_head->y < HEIGHT - 1)
//         {
//             while (vec_itr != this->body_points.end()) {
//                 (*vec_itr)->y ++;
//                 vec_itr++;
//             }
//         }
//         else
//         {
//             this->stop();
//             has_move = false;
//         }
//         break;
//     case LEFT:
//         if (p_head->x > 0)
//         {
//             while (vec_itr != this->body_points.end()) {
//                 (*vec_itr)->x --;
//                 vec_itr++;
//             }
//         }
//         else
//         {
//             this->stop();
//             has_move = false;
//         }
//         break;
//     case RIGHT:
//         if (p_head->x < WIDTH - 1)
//         {
//             while (vec_itr != this->body_points.end()) {
//                 (*vec_itr)->x ++;
//                 vec_itr++;
//             }
//         }
//         else
//         {
//             this->stop();
//             has_move = false;
//         }
//         break;
//     default:
//         break;
//     }

//     return has_move;
// }

// boolean Snaker::move(uint8_t step)
// {
//     if (step <= 0 || this->status == INACTIVE)
//     {
//         return false;
//     }

//     boolean has_move = false;

//     if (step == 1)
//     {
//         boolean one_mv_result = true;
//         Point *p_head = this->body_points[this->body_points.size() - 1];
//         Point *p_for = this->body_points[0];
//         // Point *p_for = new Point;

//         switch (this->head_drc)
//         {
//         case UP:
//             if (p_head->y > 0 + BODER_WIDTH)
//             {
//                 p_for->x = p_head->x;
//                 p_for->y = p_head->y - 1;
//             }
//             else
//             {
//                 this->stop(true);
//                 one_mv_result = false;
//             }
//             break;
//         case DOWN:
//             if (p_head->y < HEIGHT - 1 - BODER_WIDTH)
//             {
//                 p_for->x = p_head->x;
//                 p_for->y = p_head->y + 1;
//             }
//             else
//             {
//                 this->stop(true);
//                 one_mv_result = false;
//             }
//             break;
//         case LEFT:
//             if (p_head->x > 0 + BODER_WIDTH)
//             {
//                 p_for->x = p_head->x - 1;
//                 p_for->y = p_head->y;
//             }
//             else
//             {
//                 this->stop(true);
//                 one_mv_result = false;
//             }
//             break;
//         case RIGHT:
//             if (p_head->x < WIDTH - 1 - BODER_WIDTH)
//             {
//                 p_for->x = p_head->x + 1;
//                 p_for->y = p_head->y;
//             }
//             // egg.
//             else if (p_head->x == WIDTH - 1 - BODER_WIDTH && p_head->y == BODER_WIDTH)
//             {
//                 this->find_egg = true;
//                 p_for->x = WIDTH / 2;
//                 p_for->y = HEIGHT / 2;
//             }
//             else
//             {
//                 this->stop(true);
//                 one_mv_result = false;
//             }
//             break;
//         default:
//             break;
//         }

//         if (one_mv_result)
//         {
//             has_move = true;
//             // delete body_points[0];
//             this->body_points.erase(this->body_points.begin());
//             this->body_points.push_back(p_for);
//         }
//     }
//     else
//     {
//         for (uint8_t i = 0; i < step; i++)
//         {
//             has_move = this->move(1);
//             // has_move = this->move_one();
//         }
//     }

//     return has_move;
// }

boolean Snaker::move(uint8_t step)
{
    if (step <= 0 || this->status == INACTIVE)
    {
        return false;
    }

    boolean has_move = false;

    if (step == 1)
    {
        boolean one_mv_result = true;
        Point *p_head = &this->head->pt;

        switch (this->head_drc)
        {
        case UP:
            if (p_head->y > 0 + BODER_WIDTH)
            {
                p_head->y -= 1;
            }
            else
            {
                this->stop(true);
                one_mv_result = false;
            }
            break;
        case DOWN:
            if (p_head->y < HEIGHT - 1 - BODER_WIDTH)
            {
                p_head->y += 1;
            }
            else
            {
                this->stop(true);
                one_mv_result = false;
            }
            break;
        case LEFT:
            if (p_head->x > 0 + BODER_WIDTH)
            {
                p_head->x -= 1;
            }
            else
            {
                this->stop(true);
                one_mv_result = false;
            }
            break;
        case RIGHT:
            if (p_head->x < WIDTH - 1 - BODER_WIDTH)
            {
                p_head->x += 1;
            }
            // egg.
            // else if (p_head->x == WIDTH - 1 - BODER_WIDTH && p_head->y == BODER_WIDTH)
            // {
            //     this->find_egg = true;
            //     p_head->x = WIDTH / 2;
            //     p_head->y = HEIGHT / 2;
            // }
            else
            {
                this->stop(true);
                one_mv_result = false;
            }
            break;
        default:
            break;
        }

        if (one_mv_result)
        {
            has_move = true;

            // move tail
            if (this->turning_points.size() > 0 
            && this->tail->pt.x == this->turning_points[0]->pt.x 
            && this->tail->pt.y == this->turning_points[0]->pt.y)
            {
                // set the first turning point as tail.
                delete this->tail;
                this->tail = this->turning_points[0];
                this->turning_points.erase(this->turning_points.begin());
            }
            // move tail
            switch (this->tail->drc)
            {
            case UP:
                this->tail->pt.y -= 1;
                break;
            case DOWN:
                this->tail->pt.y += 1;
                break;
            case LEFT:
                this->tail->pt.x -= 1;
                break;
            case RIGHT:
                this->tail->pt.x += 1;
                break;
            default:
                break;
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < step; i++)
        {
            has_move = this->move(1);
            // has_move = this->move_one();
        }
    }

    return has_move;
}

boolean Snaker::grow()
{
    this->length++;
    // Point *tail = this->getTail();
    // Point *new_tail = new Point;

    switch (this->tail->drc)
    {
    case UP:
        this->tail->pt.y += 1;
        break;
    case DOWN:
        this->tail->pt.y -= 1;
        break;
    case LEFT:
        this->tail->pt.x += 1;
        break;
    case RIGHT:
        this->tail->pt.x -= 1;
        break;
    default:
        break;
    }
    return false;
}

Point *Snaker::getHead()
{
    return &(this->head->pt);
    // if (this->body_points.size() > 0)
    // {
    //     return this->body_points[this->body_points.size() - 1];
    // }
    // return NULL;
}

Point *Snaker::getTail()
{
    return &(this->tail->pt);
    // if (this->body_points.size() > 0)
    // {
    //     return this->body_points[0];
    // }
    // return NULL;
}

// void Snaker::show(Arduboy2 *Obj, void (Arduboy2::*p_call)(int16_t, int16_t, uint8_t))
// {
//     for (uint8_t i = 0; i < this->body_points.size(); i++)
//     {
//         (Obj->*p_call)(this->body_points[i]->x, this->body_points[i]->y, WHITE);
//     }

//     // print score. (mem cost approximately).
//     Obj->setCursor(2, 55);
//     Obj->print(String(this->body_points.size())); // * sizeof(Point) + sizeof(std::vector<Point*>)));
// }

void Snaker::show(Arduboy2 *ard)
{
    if (this->turning_points.size() < 1)
    {
        ard->drawLine(this->head->pt.x, this->head->pt.y, this->tail->pt.x, this->tail->pt.y, WHITE);
    }
    else
    {
        // uint8_t size = this->turning_points.size();
        ard->drawLine(this->tail->pt.x, this->tail->pt.y, this->turning_points[0]->pt.x, this->turning_points[0]->pt.y, WHITE);
        
        for(uint8_t i = 0; i < this->turning_points.size(); i++)
        {
            if (i != this->turning_points.size() - 1)
            {
                ard->drawLine(this->turning_points[i]->pt.x, this->turning_points[i]->pt.y, this->turning_points[i+1]->pt.x, this->turning_points[i+1]->pt.y, WHITE);
            }
            else
            {
                ard->drawLine(this->turning_points[i]->pt.x, this->turning_points[i]->pt.y, this->head->pt.x, this->head->pt.y, WHITE);
            }
        }
    }

    // debug
    // head position
    ard->setCursor(2, 35);
    ard->print(String(this->head->pt.x));
    ard->print(",");
    ard->print(String(this->head->pt.y));
    // tail position
    ard->setCursor(2, 45);
    ard->print(String(this->tail->pt.x));
    ard->print(",");
    ard->print(String(this->tail->pt.y));
    // turning point count
    ard->setCursor(2, 55);
    ard->print(String(this->turning_points.size())); // * sizeof(Point) + sizeof(std::vector<Point*>)));
    ard->print(";");
    // length
    ard->print(String(this->length)); // * sizeof(Point) + sizeof(std::vector<Point*>)));
}

boolean Snaker::if_find_egg()
{
    return this->find_egg;
}

void Snaker::reset_egg()
{
    this->find_egg = false;
}