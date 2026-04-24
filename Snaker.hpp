/**
 * Snaker.hpp - 贪吃蛇游戏引擎头文件
 *
 * 核心设计：使用 head/tail + 转折点 的方式替代逐像素存储蛇体，
 * 大幅降低内存占用（ATmega32u4 仅有 2.5KB SRAM）
 *
 * 渲染方式：在转折点之间绘制线段，而非逐个像素点
 */

#if !defined(Snaker_hpp)
#define Snaker_hpp

#include <arduino.h>
#include "Arduboy2.h"

// ==================== 常量定义 ====================

/** 蛇的初始长度。超过 26/27 会导致 OOM（2.5KB SRAM 限制） */
#define INITIAL_SNAKE_LENGTH 15

/** 游戏区域边框宽度（像素） */
#define BODER_WIDTH 1

// ==================== 枚举与结构体 ====================

/** 方向枚举：蛇的移动方向 */
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

/**
 * Turn_Point - 转折点结构体
 *
 * 记录蛇体在该点的坐标和转弯后的方向。
 * 蛇体由头、尾和一系列转折点定义，线段连接相邻点形成蛇身。
 */
struct Turn_Point
{
    Point pt;       // 转折点的屏幕坐标
    Direction drc;  // 经过该点后蛇的前进方向
};

/** 蛇的运行状态 */
enum Status
{
    ACTIVE,   // 游戏中：蛇持续移动
    INACTIVE  // 暂停中：蛇停止移动，等待恢复
};

// ==================== 自定义动态数组 ====================

/**
 * Vec - 极简动态数组模板类（替代 std::vector）
 *
 * 设计动机：ArduinoSTL 的 std::vector 与 Arduino 核心的 new.cpp 存在
 * std::nothrow 符号冲突，且 STL 运行时内存开销过大。
 *
 * 提供方法：size(), push_back(), operator[], erase_first(), clear()
 * 足以满足 Snaker 类对转折点列表的所有操作。
 *
 * 内存策略：使用 realloc 动态扩容，初始容量 4，每次翻倍
 */
template<typename T>
class Vec {
    T* data;          // 底层数据指针
    uint8_t _size;    // 当前元素个数
    uint8_t capacity; // 当前容量

public:
    /** 构造函数：初始化为空数组 */
    Vec() : data(NULL), _size(0), capacity(0) {}

    /** 析构函数：释放底层内存 */
    ~Vec() { clear(); }

    /** 返回当前元素个数 */
    uint8_t size() const { return _size; }

    /**
     * push_back - 在末尾添加元素
     *
     * 如果容量不足则调用 realloc 扩容：
     *   - 初始容量为 4
     *   - 之后每次翻倍（4 → 8 → 16 → ...）
     * 扩容失败时静默返回（在嵌入式设备上内存受限，无法恢复）
     *
     * @param item 要添加的元素
     */
    void push_back(T item) {
        if (_size >= capacity) {
            // 扩容：初始容量 4，之后每次翻倍
            uint8_t new_cap = capacity == 0 ? 4 : capacity * 2;
            T* new_data = (T*)realloc(data, new_cap * sizeof(T));
            if (new_data) {
                data = new_data;
                capacity = new_cap;
            } else {
                return;  // 内存不足，放弃添加
            }
        }
        data[_size++] = item;
    }

    /** 下标访问（读写） */
    T& operator[](uint8_t i) { return data[i]; }

    /** 下标访问（只读） */
    const T& operator[](uint8_t i) const { return data[i]; }

    /**
     * erase_first - 删除第一个元素
     *
     * 将后续元素向前移动一位，缩小 _size。
     * 用于蛇尾经过转折点时消耗该转折点。
     */
    void erase_first() {
        if (_size > 0) {
            // 将所有元素向前移动一位
            for (uint8_t i = 0; i < _size - 1; i++) {
                data[i] = data[i + 1];
            }
            _size--;
        }
    }

    /**
     * clear - 清空数组并释放内存
     *
     * 释放底层数据指针，重置大小和容量为零。
     */
    void clear() {
        free(data);
        data = NULL;
        _size = 0;
        capacity = 0;
    }
};

// ==================== Snaker 类 ====================

/**
 * Snaker - 贪吃蛇游戏引擎
 *
 * 核心数据结构：
 *   - head:      蛇头位置与方向（Turn_Point）
 *   - tail:      蛇尾位置与方向（Turn_Point）
 *   - turning_points: 蛇身转折点的有序列表
 *
 * 移动逻辑：
 *   1. 每帧蛇头沿当前方向前进一格
 *   2. 蛇头到达新方向时创建转折点并加入 turning_points
 *   3. 蛇尾沿其方向前进一格，遇到转折点时消耗该转折点（更新为转折点）
 *   4. 吃食物时蛇尾回退一格（不移动），长度 +1
 *
 * 渲染逻辑：
 *   通过 drawLine 连接 tail → turning_points[0] → ... → head
 *   形成连续的蛇身线段，而非逐个像素渲染
 */
class Snaker
{

  private:
    uint8_t length;              // 蛇的当前长度
    Direction head_drc;          // 蛇头当前移动方向
    Status status;               // 运行状态（ACTIVE / INACTIVE）
    boolean find_egg;            // 是否发现了彩蛋
    boolean init_body(uint8_t length);  // 初始化蛇体

    // 内存优化后的蛇体表示（替代原来的 std::vector<Point*>）
    Vec<Turn_Point*> turning_points;  // 转折点列表（按时间顺序排列）
    Turn_Point* head;                 // 蛇头转折点
    Turn_Point* tail;                 // 蛇尾转折点

  public:
    Snaker(uint8_t length);   // 构造函数：指定初始长度
    ~Snaker();                // 析构函数：释放所有动态分配的内存

    boolean if_find_egg();    // 是否发现彩蛋
    void reset_egg();         // 重置彩蛋标志

    Status get_status();      // 获取当前运行状态
    boolean start();          // 开始/恢复游戏
    boolean stop(boolean is_dead);  // 暂停游戏（is_dead 表示撞墙死亡）
    boolean turn_to(Direction turn_drc);  // 改变蛇头方向

    Point* getHead();         // 获取蛇头坐标
    Point* getTail();         // 获取蛇尾坐标

    boolean move(uint8_t step);    // 移动指定步数
    boolean grow();                // 蛇体增长（吃到食物时调用）
    boolean reset_body();          // 重置蛇体（游戏结束时调用）

    void show(Arduboy2 *ard);      // 渲染蛇体到屏幕
};

#endif // Snaker_hpp
