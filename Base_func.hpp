/**
 * Base_func.hpp - 功能模块抽象基类（接口）
 *
 * 所有可被 Navigator 调用的功能模块（贪吃蛇、计数器、设置）
 * 都必须继承此类，以实现统一的 play / exit 生命周期
 */

#if !defined(Base_func_hpp)
#define Base_func_hpp

/**
 * Base_func - 功能模块接口
 *
 * 子类需实现：
 *   play()   - 模块主逻辑，每帧调用
 *   exit()   - 退出时释放资源
 *   get_config() - 返回配置（可选，默认返回 0）
 */
class Base_func
{

public:
    /** 模块主逻辑，每帧由 Navigator 调用 */
    virtual void play();

    /**
     * 退出函数，释放当前功能模块实例
     * @param p 指向功能模块指针的指针（用于置空外部引用）
     */
    virtual void exit(Base_func **p)
    {
        delete *p;    // 释放模块内存
        *p = NULL;    // 将外部指针置空，防止悬垂指针
    }

    /**
     * 获取配置参数（子类可重写）
     * @return 配置值（用于传递给其他模块，如蛇的初始长度）
     */
    virtual uint8_t get_config()
    {
        return 0;
    }

};

#endif // Base_func_hpp
