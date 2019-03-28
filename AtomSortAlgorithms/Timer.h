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

    private:
        double duration = 0;
        steady_clock::time_point End;
        steady_clock::time_point start;
};

#endif // TIMER_H
