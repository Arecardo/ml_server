#ifndef SRC_SYNC_H
#define SRC_SYNC_H

/*
 * This head defines several thread synchronization
 * tools which are used in the project. Basically based on
 * POSIX synchronizatio interfaces.
 * 
 * author:  Xinrun Zhang
 * date:    Mar 14, 2021 22:30:03
 */

#include<pthread.h>

class Locker
{
public:
    void lock();
    void unlock();
};

class Sem
{
public:
    void post();
    void wait();
};

#endif