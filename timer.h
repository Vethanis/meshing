#ifndef TIMER_H
#define TIMER_H

class Timer{
    unsigned id;
    int nanoseconds;
public:
    Timer();
    ~Timer();
    void begin();
    int end();
    void endPrint();
};

#endif
