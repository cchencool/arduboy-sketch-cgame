
/**
 * Interface for function.
 * 
 * functions in the navigator should implement this interface.
 * 
 * Func_snaker
 * Func_counter
 * Func_Setting
 *  
 **/

#if !defined(Base_func_hpp)
#define Base_func_hpp

class Base_func
{

public:
    virtual void play();

    virtual void exit(Base_func **p)
    {
        delete *p;
        *p = NULL;
    }

    // the return should be a map object.
    virtual uint8_t get_config()
    {
        return 0;
    }

};


#endif // Base_func_hpp
