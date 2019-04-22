#ifndef TIMER_H
#define TIMER_H

#include <chrono>
using namespace std::chrono;
class Timer
{
    public:
        Timer();
        void pause();
        void restart();
        double gettime();
        void zero();
        double speedup;

    private:
        double duration = 0;
        steady_clock::time_point End;
        steady_clock::time_point start;
        double calltime = 0;
};

#endif // TIMER_H
