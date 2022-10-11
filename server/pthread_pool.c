#include "pthread_pool.h"


void threadpool_init(threadPool_t * pthreadpool, int pthreadNum)
{
    if(pthreadpool)
    {
        pthreadpool->threadNumber = pthreadNum;
        pthreadpool->thread = (pthread_t *)calloc(pthreadNum, sizeof(pthread_t));
        queue_init(&pthreadpool->queue);
    }
}
 
void threadPool_destroy(threadPool_t * pthreadpool)
{
    if(pthreadpool)
    {
        free(pthreadpool->thread);
        queue_destroy(&pthreadpool->queue);
    }

}

void threadPool_start(threadPool_t * pthreadpool)
{
    for(int i = 0; i < pthreadpool->threadNumber; ++i)
    {
        int ret = pthread_create(&pthreadpool->thread[i], NULL, threadFunc, pthreadpool);
        PTHREAD_CHECK(ret, "pthread_create");
    }

}

void threadPool_stop(threadPool_t * pthreadpool)
{
    while(!queue_isempty(&pthreadpool->queue))
    {
        sleep(1);
    }
    queue_wakeup(&pthreadpool->queue);
    for(int i = 0; i < pthreadpool->threadNumber; ++i)
    {
        pthread_join(pthreadpool->thread[i], NULL);    
    }
}

void *threadFunc(void * arg)
{
    threadPool_t * pthreadpool = (threadPool_t *)arg;
    
    while(1)
    {
        if(pthreadpool)
        {
            int peerfd = task_dequeue(&pthreadpool->queue);
            if(peerfd > 0)
            {
                transfer_file(peerfd);
            }
            else
            {
                break;
            }
        }
    }
    return NULL;
}


