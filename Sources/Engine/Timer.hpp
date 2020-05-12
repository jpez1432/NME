
#ifndef TIMER_HPP
#define TIMER_HPP

#include <GLFW/Glfw3.h>

class CTimer
{

private:

    double LastTime;
    double CurrentTime;
    double DeltaTime;
    double LastFrame;

    unsigned int Counter;
    unsigned int Fps;

public:

    CTimer(void);

    const double Update(void);
    void Reset(void);

    unsigned int &GetFps(void)
    {
        return Fps;
    }

    const double &GetDelta(void)
    {
        return DeltaTime;
    }

};

#endif
