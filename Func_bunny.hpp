/**
 * Func_bunny.hpp - 小兔子跳跃游戏
 *
 * 继承 Base_func，实现：
 *   - 小兔子角色渲染
 *   - 物理引擎（重力、跳跃）
 *   - 任意按键控制跳跃，按键越快跳得越高
 */

#if !defined(Func_bunny_hpp)
#define Func_bunny_hpp

#include <arduino.h>
#include "Arduboy2.h"
#include "Base_func.hpp"

// ==================== 外部全局变量声明 ====================

extern Btn_ctrl *btn_ctrl;  // 按键控制器
extern Arduboy2 *arduboy;   // Arduboy2 实例

// ==================== 游戏常量 ====================

#define GROUND_Y 50          // 地面 Y 坐标
#define BUNNY_X 30           // 兔子 X 坐标
#define GRAVITY 1            // 重力加速度
#define MIN_JUMP_POWER 4    // 最小跳跃力度
#define MAX_JUMP_POWER 10    // 最大跳跃力度
#define MIN_INTERVAL 5       // 最小按键间隔（帧）
#define MAX_INTERVAL 30      // 最大按键间隔（帧）

// ==================== 小兔子位图（3x5） ====================

const uint8_t PROGMEM bunny_bitmap[] = {
  0b010,  //  .*.
  0b111,  //  ***
  0b010,  //  .*.
  0b101,  //  *.*
  0b101   //  *.*
};

/**
 * Func_bunny - 小兔子跳跃游戏模块
 */
class Func_bunny : public Base_func
{
private:
  int8_t bunny_y;           // 兔子 Y 坐标
  int8_t velocity_y;        // Y 方向速度
  uint16_t last_jump_time;  // 上次跳跃时间（帧计数）
  uint16_t frame_count;     // 当前帧计数
  bool on_ground;           // 是否在地面上

public:
  /**
   * 构造函数：初始化游戏状态
   */
  Func_bunny()
  {
    this->bunny_y = GROUND_Y - 5;  // 兔子站在地面上
    this->velocity_y = 0;
    this->last_jump_time = 0;
    this->frame_count = 0;
    this->on_ground = true;
  }

  /**
   * 析构函数：无特殊资源释放
   */
  ~Func_bunny()
  {
  }

  /**
   * play - 模块主逻辑，每帧由 Navigator 调用
   */
  void play()
  {
    this->frame_count++;

    // 绘制地面
    this->draw_ground();

    // 检测按键并处理跳跃
    this->handle_input();

    // 更新物理状态
    this->update_physics();

    // 绘制兔子
    this->draw_bunny();

    // 显示提示
    this->draw_instructions();
  }

  /**
   * exit - 退出游戏模块
   *
   * @param p 指向模块指针的指针
   */
  void exit(Base_func **p)
  {
    Base_func::exit(p);
  }

private:
  /**
   * draw_ground - 绘制地面
   */
  void draw_ground()
  {
    arduboy->drawLine(0, GROUND_Y, WIDTH - 1, GROUND_Y, WHITE);
  }

  /**
   * handle_input - 处理按键输入
   *
   * 检测任意按键，计算按键间隔以决定跳跃力度
   */
  void handle_input()
  {
    // 检测任意按键
    bool any_key =
      btn_ctrl->up_click() ||
      btn_ctrl->down_click() ||
      btn_ctrl->left_click() ||
      btn_ctrl->right_click() ||
      btn_ctrl->a_click();

    if (any_key && this->on_ground)
    {
      // 计算按键间隔
      uint16_t interval = this->frame_count - this->last_jump_time;
      this->last_jump_time = this->frame_count;

      // 限制间隔范围
      if (interval < MIN_INTERVAL) interval = MIN_INTERVAL;
      if (interval > MAX_INTERVAL) interval = MAX_INTERVAL;

      // 间隔越短，跳跃力度越大（反比例关系）
      int8_t jump_power = map(interval, MIN_INTERVAL, MAX_INTERVAL,
                               MAX_JUMP_POWER, MIN_JUMP_POWER);

      // 应用跳跃速度（负数表示向上）
      this->velocity_y = -jump_power;
      this->on_ground = false;
    }
  }

  /**
   * update_physics - 更新物理状态
   *
   * 应用重力，检测地面碰撞
   */
  void update_physics()
  {
    if (!this->on_ground)
    {
      // 应用重力
      this->velocity_y += GRAVITY;

      // 更新位置
      this->bunny_y += this->velocity_y;

      // 检测地面碰撞
      if (this->bunny_y >= GROUND_Y - 5)
      {
        this->bunny_y = GROUND_Y - 5;
        this->velocity_y = 0;
        this->on_ground = true;
      }
    }
  }

  /**
   * draw_bunny - 绘制小兔子
   *
   * 使用位图数据绘制 3x5 的小兔子
   */
  void draw_bunny()
  {
    for (uint8_t y = 0; y < 5; y++)
    {
      uint8_t row = pgm_read_byte(&bunny_bitmap[y]);
      for (uint8_t x = 0; x < 3; x++)
      {
        if (row & (0b100 >> x))
        {
          arduboy->drawPixel(BUNNY_X + x, this->bunny_y + y, WHITE);
        }
      }
    }
  }

  /**
   * draw_instructions - 绘制游戏提示
   */
  void draw_instructions()
  {
    arduboy->setCursor(50, 10);
    arduboy->print(F("Bunny Jump"));

    arduboy->setCursor(5, 60);
    arduboy->print(F("Any key: jump"));
  }
};

#endif // Func_bunny_hpp
