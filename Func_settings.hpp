/**
 * Func_settings.hpp - 设置功能模块
 *
 * 继承 Base_func，实现：
 *   - 显示设置界面（当前仅显示蛇初始长度）
 *   - 预留 EEPROM 存储/加载配置
 */

#if !defined(Func_settings_hpp)
#define Func_settings_hpp

#include <avr/pgmspace.h>
#include "Arduboy2.h"
#include "arduino.h"
#include "Base_func.hpp"

// ==================== 外部全局变量声明 ====================

extern Btn_ctrl *btn_ctrl;  // 按键控制器
extern Arduboy2 *arduboy;   // Arduboy2 实例

/**
 * Func_settings - 设置功能模块
 *
 * 当前功能：
 *   - 显示蛇的初始长度配置值
 *   - 按 A 键提示保存（实际保存功能未实现）
 *
 * 预留：通过 EEPROM 持久化存储配置
 */
class Func_settings: public Base_func
{

  private:
    uint8_t snaker_initial_length;  // 蛇的初始长度（应存储到 EEPROM）

  public:

    /**
     * 构造函数：初始化默认配置
     *
     * 默认蛇初始长度为 30（注意：实际游戏中受 2.5KB SRAM 限制，
     * 有效最大长度约为 26~27）
     */
    Func_settings()
    {
        this->snaker_initial_length = 30;
    }

    /**
     * play - 模块主逻辑，每帧由 Navigator 调用
     *
     * 显示设置界面边框、标题、当前配置值。
     * 按 A 键显示「设置已保存」提示。
     * 按右键进入小兔子跳跃游戏。
     */
    void play()
    {
        this->draw_square();  // 绘制设置界面边框

        // 显示标题
        arduboy->setCursor(40, 10);
        arduboy->print(F("settings *"));

        // 显示蛇初始长度
        arduboy->setCursor(10, 30);
        arduboy->print(F("snaker_length: "));
        arduboy->print(String(this->snaker_initial_length));

        // 按 A 键显示保存提示
        if (btn_ctrl->a_click())
        {
            arduboy->setCursor(10, 40);
            arduboy->print(F("settings saved!"));
        }

        // 显示小兔子游戏入口提示
        arduboy->setCursor(10, 50);
        arduboy->print(F("Right: Bunny Jump"));

        // 注意：按右键进入小兔子游戏的逻辑在 sketch_cgame.ino 中处理
        // 这里只显示提示
    }

    /**
     * exit - 退出设置模块
     *
     * 空实现：设置模块不持有动态内存，无需释放。
     *
     * @param p 指向模块指针的指针
     */
    void exit(Base_func **p)
    {
    }

    /**
     * draw_square - 绘制设置界面边框
     *
     * 绘制梯形装饰边框（与计数器界面风格一致）
     */
    void draw_square()
    {
        arduboy->drawLine(0, 0, 5, 5, WHITE);          // 左上角斜线
        arduboy->drawLine(5, 5, 122, 5, WHITE);        // 上边
        arduboy->drawLine(5, 5, 5, 58, WHITE);         // 左边
        arduboy->drawLine(122, 5, 127, 0, WHITE);      // 右上角斜线
        arduboy->drawLine(122, 5, 122, 58, WHITE);     // 右边
        arduboy->drawLine(122, 58, 127, 63, WHITE);    // 右下角斜线
        arduboy->drawLine(5, 58, 122, 58, WHITE);      // 下边
        arduboy->drawLine(5, 58, 0, 63, WHITE);        // 左下角斜线
    }

    /**
     * get_config - 获取配置值（实现 Base_func 接口）
     *
     * @return 蛇的初始长度
     */
    uint8_t get_config()
    {
        return this->snaker_initial_length;
    }

    /**
     * store - 保存配置到 EEPROM（未实现）
     *
     * 预留：将 snaker_initial_length 写入 EEPROM
     * @return 预留返回值
     */
    boolean store()
    {
    }

    /**
     * load - 从 EEPROM 加载配置（未实现）
     *
     * 预留：从 EEPROM 读取 snaker_initial_length
     * @return 预留返回值
     */
    boolean load()
    {
    }
};

#endif // Func_settings_hpp
