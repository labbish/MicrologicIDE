#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <string>
#include <QDebug>

class Timer
{
public:
    double t1,t2;
    Timer();
    void debugTime(std::string msg);
};

#endif // TIMER_H
