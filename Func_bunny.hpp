/**
 * Func_bunny.hpp - 小兔子跳跃游戏
 *
 * 继承 Base_func，实现：
 *   - 小兔子角色渲染（有耳朵的像素图案）
 *   - 物理引擎（重力、跳跃）
 *   - 任意按键控制跳跃，按键越快，跳跃越高
 *   - 障碍物系统（向左移动）
 *   - 碰撞检测
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
#define OBSTACLE_SPEED 2     // 障碍物移动速度
#define MAX_OBSTACLES 3      // 最大障碍物数量
#define OBSTACLE_GAP_MIN 40  // 障碍物最小间距
#define OBSTACLE_GAP_MAX 80  // 障碍物最大间距

// ==================== 小兔子位图（5x7，有耳朵） ====================
//  X...
//  XX..
//   X..
// XXX..
//   X..
//  XX..
//  XX..
const uint8_t PROGMEM bunny_bitmap[] = {
  0b10000,
  0b11000,
  0b01000,
  0b11100,
  0b01000,
  0b11000,
  0b11000
};

// ==================== 障碍物结构体 ====================
struct Obstacle {
  int8_t x;       // X 坐标
  int8_t height;  // 高度
  int8_t width;   // 宽度
};

/**
 * Func_bunny - 小兔子跳跃游戏模块
 */
class Func_bunny : public Base_func
{
private:
  int8_t bunny_y;              // 兔子 Y 坐标
  int8_t velocity_y;           // Y 方向速度
  uint16_t last_jump_time;    // 上次跳跃时间（帧计数）
  uint16_t frame_count;       // 当前帧计数
  bool on_ground;             // 是否在地面上
  Obstacle obstacles[MAX_OBSTACLES];  // 障碍物数组
  uint8_t obstacle_count;     // 当前障碍物数量
  int8_t next_obstacle_x;    // 下一个障碍物生成位置
  uint16_t score;            // 得分
  bool game_over;            // 游戏结束标志

public:
  /**
   * 构造函数：初始化游戏状态
   */
  Func_bunny()
  {
    this->bunny_y = GROUND_Y - 7;  // 兔子站在地面上
    this->velocity_y = 0;
    this->last_jump_time = 0;
    this->frame_count = 0;
    this->on_ground = true;
    this->obstacle_count = 0;
    this->next_obstacle_x = WIDTH + 20;
    this->score = 0;
    this->game_over = false;
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

    if (game_over)
    {
      draw_game_over();
      // 按任意键重新开始
      if (btn_ctrl->up_click() || btn_ctrl->down_click() ||
          btn_ctrl->left_click() || btn_ctrl->right_click() ||
          btn_ctrl->a_click() || btn_ctrl->b_click())
      {
        reset_game();
      }
      return;
    }

    // 绘制地面
    this->draw_ground();

    // 检测按键并处理跳跃
    this->handle_input();

    // 更新物理状态
    this->update_physics();

    // 更新障碍物
    this->update_obstacles();

    // 碰撞检测
    this->check_collisions();

    // 绘制障碍物
    this->draw_obstacles();

    // 绘制兔子
    this->draw_bunny();

    // 绘制得分
    this->draw_score();
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
      if (this->bunny_y >= GROUND_Y - 7)
      {
        this->bunny_y = GROUND_Y - 7;
        this->velocity_y = 0;
        this->on_ground = true;
      }
    }
  }

  /**
   * update_obstacles - 更新障碍物
   */
  void update_obstacles()
  {
    // 移动现有障碍物
    for (uint8_t i = 0; i < obstacle_count; i++)
    {
      obstacles[i].x -= OBSTACLE_SPEED;
    }

    // 移除离开屏幕的障碍物
    while (obstacle_count > 0 && obstacles[0].x < -10)
    {
      // 向左移动所有障碍物
      for (uint8_t i = 0; i < obstacle_count - 1; i++)
      {
        obstacles[i] = obstacles[i + 1];
      }
      obstacle_count--;
      score++;  // 每越过一个障碍物得 1 分
    }

    // 生成新障碍物
    next_obstacle_x -= OBSTACLE_SPEED;
    if (next_obstacle_x <= WIDTH && obstacle_count < MAX_OBSTACLES)
    {
      obstacles[obstacle_count].x = WIDTH + 5;
      obstacles[obstacle_count].width = random(3, 7);
      obstacles[obstacle_count].height = random(8, 16);
      obstacle_count++;

      // 计算下一个障碍物位置
      next_obstacle_x = WIDTH + random(OBSTACLE_GAP_MIN, OBSTACLE_GAP_MAX);
    }
  }

  /**
   * check_collisions - 碰撞检测
   */
  void check_collisions()
  {
    // 兔子碰撞区域（简化为矩形）
    int8_t bunny_left = BUNNY_X;
    int8_t bunny_right = BUNNY_X + 4;
    int8_t bunny_top = bunny_y;
    int8_t bunny_bottom = bunny_y + 6;

    // 检测与每个障碍物的碰撞
    for (uint8_t i = 0; i < obstacle_count; i++)
    {
      int8_t obs_left = obstacles[i].x;
      int8_t obs_right = obstacles[i].x + obstacles[i].width - 1;
      int8_t obs_top = GROUND_Y - obstacles[i].height;
      int8_t obs_bottom = GROUND_Y - 1;

      // 矩形碰撞检测
      if (bunny_right >= obs_left && bunny_left <= obs_right &&
          bunny_bottom >= obs_top && bunny_top <= obs_bottom)
      {
        game_over = true;
        return;
      }
    }
  }

  /**
   * draw_obstacles - 绘制障碍物
   */
  void draw_obstacles()
  {
    for (uint8_t i = 0; i < obstacle_count; i++)
    {
      int8_t x = obstacles[i].x;
      int8_t w = obstacles[i].width;
      int8_t h = obstacles[i].height;

      // 绘制障碍物（从地面向上）
      arduboy->drawRect(x, GROUND_Y - h, w, h, WHITE);
    }
  }

  /**
   * draw_bunny - 绘制小兔子
   *
   * 使用位图数据绘制有耳朵的小兔子
   */
  void draw_bunny()
  {
    for (uint8_t y = 0; y < 7; y++)
    {
      uint8_t row = pgm_read_byte(&bunny_bitmap[y]);
      for (uint8_t x = 0; x < 5; x++)
      {
        if (row & (0b10000 >> x))
        {
          arduboy->drawPixel(BUNNY_X + x, bunny_y + y, WHITE);
        }
      }
    }
  }

  /**
   * draw_score - 绘制得分
   */
  void draw_score()
  {
    arduboy->setCursor(2, 2);
    arduboy->print(F("Score:"));
    arduboy->print(score);
  }

  /**
   * draw_game_over - 绘制游戏结束画面
   */
  void draw_game_over()
  {
    arduboy->setCursor(40, 25);
    arduboy->print(F("GAME OVER"));

    arduboy->setCursor(30, 35);
    arduboy->print(F("Score:"));
    arduboy->print(score);

    arduboy->setCursor(10, 50);
    arduboy->print(F("Press any key"));
  }

  /**
   * reset_game - 重置游戏
   */
  void reset_game()
  {
    bunny_y = GROUND_Y - 7;
    velocity_y = 0;
    last_jump_time = frame_count;
    on_ground = true;
    obstacle_count = 0;
    next_obstacle_x = WIDTH + 20;
    score = 0;
    game_over = false;
  }
};

#endif // Func_bunny_hpp
