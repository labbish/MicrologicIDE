#include "Timer.h"

Timer::Timer()
{
    t1=(double)clock();
    t2=(double)clock();
}

void Timer::debugTime(std::string msg){
    t2=(double)clock();
    qDebug().noquote()<<msg<<(t2-t1)<<"ms";
    t1=(double)clock();
}
