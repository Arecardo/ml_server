/*
 * A very easy implementation of thread pool using POSIX threads
 * author:  Xinrun Zhang
 * date:    Mar 14, 2021 20:12:13
 */

#ifndef SRC_THREAD_POOL_H
#define SRC_THREAD_POOL_H

#include <list>
#include <cstdio>
#include <exception> 
#include <pthread.h>
#include <string.h>
#include <iostream>

#include "../sync/sync.h"                       /* need modify here */
#include "../ClassifyTask/ClassifyTask.h"       /* need modify here */
#include "../ml/MlpClassifier.h"                /* need modify here */
#include "../ml/TfidfVectorizer.h"              /* need modify here */

class ThreadPool
{
public:
    /* constructor and destructor */
    ThreadPool(int thread_number = 8, int max_request = 10000);
    ~ThreadPool();

    /* append request interface */
    bool append(ClassifyTask* task);

private:
    /* 工作线程运行的函数，它不断从工作队列中取出任务并执行之 */
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;                            //线程池中的线程数
    int m_max_requests;                             //请求队列中允许的最大请求数
    pthread_t *m_threads;                           //描述线程池的数组，其大小为m_thread_number
    std::list<ClassifyTask *> m_workqueue;          //请求队列
    Locker m_queuelocker;                           //保护请求队列的互斥锁
    Sem m_queuestat;                                //是否有任务需要处理
};

/* constructor */
ThreadPool::ThreadPool(int thread_number, int max_request):
    m_thread_number(thread_number),
    m_max_requests(max_request)

{
    /* edge case check */
    if (thread_number <= 0 || thread_number > 15 || max_request <= 0 || max_request > 30000)
    {
        throw std::exception();
    }

    /* initialize pthread array */
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads)
    {
        throw std::exception();
    }

    /* initialize pthreads one by one */
    for(int i=0; i<thread_number; i++)
    {   
        /* pthread_create(), create a thread that running worker */
        if(pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }

        /* detach the thread so that the main process */
        /* doesn't need to wait the thread complete */
        if(pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

/* destructor */
ThreadPool::~ThreadPool()
{   
    /* just delete the thread array */
    delete[] m_threads;
}

/* public member function append */
/* the processing function in the main process call this function */
/* to append classify task to the head of the list */
bool ThreadPool::append(ClassifyTask* task)
{
    /* lock the queue*/
    m_queuelocker.lock();

    /* check if the queue is full */
    if(m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }

    /* if the queue is not full */
    /* add the task at the end of the queue */
    m_workqueue.push_back(task);

    /* unlock the queue */
    m_queuelocker.unlock();

    /* wake all the sleeping threads */
    m_queuestat.post();

    return true;
}

/* private member function worker */
void* ThreadPool::worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    pool->run();
    return pool;
}

/* private member function run */
void ThreadPool::run()
{   
    /* initialize the vectorizer and mlp classifier */
    /* modify here */
    auto tfv = std::make_shared<Tfidf_vectorizer>();
    auto mlpc = std::make_shared<MlpClassifier>();
    
    while (true)
    {
        /* wait for new request */
        m_queuestat.wait();

        /* if new task comes */
        /* check if the task is done by other threads */
        m_queuelocker.lock();
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }

        /* pull out the new task */
        ClassifyTask* newTask = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        /* start handle the task */
        newTask->process();
    }
    
}

#endif