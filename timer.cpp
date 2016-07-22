#include "myglheaders.h"
#include "timer.h"
#include "stdio.h"

Timer::Timer(){
    glGenQueries(1, &id);
}
Timer::~Timer(){
    glDeleteQueries(1, &id);
}
void Timer::begin(){
    glBeginQuery(GL_TIME_ELAPSED, id);
}
int Timer::end(){
    glEndQuery(GL_TIME_ELAPSED);
    glGetQueryObjectiv(id, GL_QUERY_RESULT, &nanoseconds);
    return nanoseconds / 1000000;
}
void Timer::endPrint(){
    glEndQuery(GL_TIME_ELAPSED);
    glGetQueryObjectiv(id, GL_QUERY_RESULT, &nanoseconds);
    printf("Timer ms: %i\n", nanoseconds / 1000000);
}
