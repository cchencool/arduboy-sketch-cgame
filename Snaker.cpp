/**
 * Snaker.cpp - 贪吃蛇游戏引擎实现
 *
 * 核心算法：
 *   - 蛇体由 head + tail + turning_points 定义
 *   - 每次移动只更新 head 和 tail 的坐标
 *   - head 转弯时创建新的转折点
 *   - tail 碰到转折点时消耗该转折点（变为新的 tail）
 *   - 吃食物时 tail 回退一格，不前进
 */

#include <arduino.h>
#include "Arduboy2.h"
#include "Snaker.hpp"

/**
 * 构造函数 - 初始化蛇
 *
 * @param length 蛇的初始长度（像素）
 */
Snaker::Snaker(uint8_t length) : length(length)
{
    this->status = INACTIVE;    // 初始为暂停状态，等待用户启动
    this->head_drc = RIGHT;     // 默认向右移动
    this->find_egg = false;     // 未找到彩蛋

    this->head = NULL;
    this->tail = NULL;
    this->init_body(length);    // 初始化蛇体坐标
}

/**
 * 析构函数 - 释放所有动态分配的内存
 *
 * 释放顺序：
 *   1. 遍历并删除所有转折点
 *   2. 清空转折点列表
 *   3. 删除头尾节点
 */
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
}

/**
 * init_body - 初始化蛇体坐标
 *
 * 蛇初始为水平直线：
 *   - 头部：(5 + length, 5)，方向向右
 *   - 尾部：(5, 5)，方向向右
 *   - 中间无转折点（直线段）
 *
 * @param length 蛇的长度
 * @return 初始化成功返回 true
 */
boolean Snaker::init_body(uint8_t length)
{
    if (length > 0)
    {
        // 初始化蛇头
        if (this->head == NULL)
        {
            this->head = new Turn_Point;
        }
        this->head->pt.x = 5 + length;  // 头部在尾部右侧 length 格
        this->head->pt.y = 5;
        this->head->drc = RIGHT;

        // 初始化蛇尾
        if (this->tail == NULL)
        {
            this->tail = new Turn_Point;
        }
        this->tail->pt.x = 5;
        this->tail->pt.y = 5;
        this->tail->drc = RIGHT;

        this->length = length;

        return true;
    }
    else
    {
        return false;
    }
}

/**
 * reset_body - 重置蛇体（游戏结束或重新开始时调用）
 *
 * 先清理所有已分配的节点，再重新初始化。
 *
 * @return 初始化成功返回 true
 */
boolean Snaker::reset_body()
{
    // 释放所有转折点
    for (uint8_t i = 0; i < this->turning_points.size(); i++)
    {
       delete this->turning_points[i];
    }
    this->turning_points.clear();

    // 释放头尾节点
    delete this->head;
    delete this->tail;
    this->head = NULL;
    this->tail = NULL;

    // 使用默认长度重新初始化
    return this->init_body(INITIAL_SNAKE_LENGTH);
}

/** 获取当前运行状态（ACTIVE / INACTIVE） */
Status Snaker::get_status()
{
    return this->status;
}

/**
 * start - 开始/恢复游戏
 *
 * @return 状态由 INACTIVE 变为 ACTIVE 时返回 true，否则返回 false
 */
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

/**
 * stop - 暂停游戏
 *
 * @param is_dead 是否为撞墙死亡（预留：可在此保存最高分记录）
 * @return 状态由 ACTIVE 变为 INACTIVE 时返回 true，否则返回 false
 */
boolean Snaker::stop(boolean is_dead)
{
    if (is_dead)
    {
        // 预留：保存最高分记录到 EEPROM
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

/**
 * turn_to - 改变蛇头移动方向
 *
 * 规则：
 *   - 暂停状态下不允许转弯
 *   - 不能 180 度掉头（反方向转弯无效）
 *   - 同方向转弯无效
 *
 * 成功转弯时：在当前蛇头位置创建新的转折点，加入 turning_points 列表。
 * 后续蛇身经过该点时将沿新方向前进。
 *
 * @param turn_drc 目标方向
 * @return 转弯成功返回 true
 */
boolean Snaker::turn_to(Direction turn_drc)
{
    if (this->status == INACTIVE)
    {
        return false;  // 暂停中不允许转弯
    }

    boolean trun_result = false;
    Direction reverse_drc;

    // 计算目标方向的反方向
    switch (turn_drc)
    {
    case UP:    reverse_drc = DOWN;  break;
    case DOWN:  reverse_drc = UP;    break;
    case LEFT:  reverse_drc = RIGHT; break;
    case RIGHT: reverse_drc = LEFT;  break;
    default:    break;
    }

    // 只有新方向与当前方向不同且不是反方向时才允许转弯
    if (this->head_drc != turn_drc && this->head_drc != reverse_drc)
    {
        this->head_drc = turn_drc;
        trun_result = true;

        // 创建转折点：记录当前位置和新方向
        Turn_Point *turn_pt = new Turn_Point;
        turn_pt->pt.x = this->head->pt.x;
        turn_pt->pt.y = this->head->pt.y;
        turn_pt->drc = this->head_drc;

        this->turning_points.push_back(turn_pt);
    }
    else
    {
        trun_result = false;  // 反方向或同方向转弯无效
    }

    return trun_result;
}

/**
 * move - 移动蛇体指定步数
 *
 * 移动逻辑（单步）：
 *   1. 蛇头沿当前方向前进一格，越界则撞墙停止
 *   2. 蛇尾处理：
 *      a. 如果蛇尾恰好位于第一个转折点：
 *         释放旧 tail，将第一个转折点作为新 tail，从列表中移除
 *      b. 蛇尾沿其方向前进一格
 *   3. 吃食物时不调用 move，而是在外部处理 grow()
 *
 * @param step 移动步数（目前只支持 step == 1）
 * @return 移动成功返回 true，撞墙返回 false
 */
boolean Snaker::move(uint8_t step)
{
    if (step <= 0 || this->status == INACTIVE)
    {
        return false;  // 无效步数或暂停中不移动
    }

    boolean has_move = false;

    if (step == 1)
    {
        boolean one_mv_result = true;
        Point *p_head = &this->head->pt;  // 蛇头坐标指针

        // 沿当前方向移动蛇头一格
        switch (this->head_drc)
        {
        case UP:
            if (p_head->y > 0 + BODER_WIDTH)   // 未撞上边界
            {
                p_head->y -= 1;
            }
            else
            {
                this->stop(true);              // 撞墙，标记死亡
                one_mv_result = false;
            }
            break;
        case DOWN:
            if (p_head->y < HEIGHT - 1 - BODER_WIDTH)  // 未撞下边界
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
            if (p_head->x > 0 + BODER_WIDTH)   // 未撞左边界
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
            if (p_head->x < WIDTH - 1 - BODER_WIDTH)   // 未撞右边界
            {
                p_head->x += 1;
            }
            // 彩蛋触发条件（已禁用）：
            // 蛇头到达右上角时触发彩蛋，传送到屏幕中心
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

            // 处理蛇尾移动
            // 如果蛇尾位于第一个转折点位置，则消耗该转折点
            if (this->turning_points.size() > 0
            && this->tail->pt.x == this->turning_points[0]->pt.x
            && this->tail->pt.y == this->turning_points[0]->pt.y)
            {
                // 将第一个转折点设为新 tail
                delete this->tail;
                this->tail = this->turning_points[0];
                this->turning_points.erase_first();
            }

            // 蛇尾沿其方向前进一格
            switch (this->tail->drc)
            {
            case UP:    this->tail->pt.y -= 1; break;
            case DOWN:  this->tail->pt.y += 1; break;
            case LEFT:  this->tail->pt.x -= 1; break;
            case RIGHT: this->tail->pt.x += 1; break;
            default:    break;
            }
        }
    }
    else
    {
        // 多步移动：递归调用单步移动
        for (uint8_t i = 0; i < step; i++)
        {
            has_move = this->move(1);
        }
    }

    return has_move;
}

/**
 * grow - 蛇体增长（吃到食物时调用）
 *
 * 实现方式：蛇尾沿其方向**反方向**回退一格，
 * 这样蛇体就「多了一节」。长度 +1。
 *
 * @return 始终返回 false（预留返回值）
 */
boolean Snaker::grow()
{
    this->length++;

    // 蛇尾回退一格（反方向）
    switch (this->tail->drc)
    {
    case UP:    this->tail->pt.y += 1; break;  // 反方向：向下
    case DOWN:  this->tail->pt.y -= 1; break;  // 反方向：向上
    case LEFT:  this->tail->pt.x += 1; break;  // 反方向：向右
    case RIGHT: this->tail->pt.x -= 1; break;  // 反方向：向左
    default:    break;
    }
    return false;
}

/** 获取蛇头坐标指针 */
Point *Snaker::getHead()
{
    return &(this->head->pt);
}

/** 获取蛇尾坐标指针 */
Point *Snaker::getTail()
{
    return &(this->tail->pt);
}

/**
 * show - 渲染蛇体到屏幕
 *
 * 渲染策略：
 *   - 无转折点时：在 head 和 tail 之间绘制一条直线
 *   - 有转折点时：依次连接 tail → turning_points[0] → ... → head
 *     形成折线蛇身
 *
 * 调试信息：在屏幕左下角显示 head/tail 坐标、转折点数量、蛇体长度
 *
 * @param ard Arduboy2 实例指针
 */
void Snaker::show(Arduboy2 *ard)
{
    if (this->turning_points.size() < 1)
    {
        // 无转折点：直接绘制头尾连线（直线蛇身）
        ard->drawLine(this->head->pt.x, this->head->pt.y,
                      this->tail->pt.x, this->tail->pt.y, WHITE);
    }
    else
    {
        // 有转折点：绘制折线蛇身
        // 先画 tail 到第一个转折点
        ard->drawLine(this->tail->pt.x, this->tail->pt.y,
                      this->turning_points[0]->pt.x, this->turning_points[0]->pt.y, WHITE);

        // 再画相邻转折点之间的线段
        for(uint8_t i = 0; i < this->turning_points.size(); i++)
        {
            if (i != this->turning_points.size() - 1)
            {
                // 第 i 个转折点到第 i+1 个转折点
                ard->drawLine(this->turning_points[i]->pt.x, this->turning_points[i]->pt.y,
                              this->turning_points[i+1]->pt.x, this->turning_points[i+1]->pt.y, WHITE);
            }
            else
            {
                // 最后一个转折点到蛇头
                ard->drawLine(this->turning_points[i]->pt.x, this->turning_points[i]->pt.y,
                              this->head->pt.x, this->head->pt.y, WHITE);
            }
        }
    }

    // 调试信息输出（屏幕左下角）
    // 第一行：蛇头坐标
    ard->setCursor(2, 35);
    ard->print(String(this->head->pt.x));
    ard->print(",");
    ard->print(String(this->head->pt.y));

    // 第二行：蛇尾坐标
    ard->setCursor(2, 45);
    ard->print(String(this->tail->pt.x));
    ard->print(",");
    ard->print(String(this->tail->pt.y));

    // 第三行：转折点数量 ; 蛇体长度
    ard->setCursor(2, 55);
    ard->print(String(this->turning_points.size()));
    ard->print(";");
    ard->print(String(this->length));
}

/** 查询是否发现了彩蛋 */
boolean Snaker::if_find_egg()
{
    return this->find_egg;
}

/** 重置彩蛋标志（彩蛋提示消失后调用） */
void Snaker::reset_egg()
{
    this->find_egg = false;
}
