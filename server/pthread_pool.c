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
        int ret = pthread_create(&pthreadpool->thread[i], NULL, thread_func, pthreadpool);
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

void *thread_func(void * arg)
{
    int peerfd;
    while(1)
    {
        threadPool_t * pthreadpool = (threadPool_t *)arg;
        //互斥锁
        pthread_mutex_lock(&pthreadpool->queue.mutex);
        //线程清理函数
        pthread_cleanup_push(clean_func, (void *)pthreadpool);
        while(queue_isempty(&pthreadpool->queue))
        {
            //等待解锁
            pthread_cond_wait(&pthreadpool->queue.cond, &pthreadpool->queue.mutex);
            if(pthreadpool->queue.size == 0)
            {
                printf("No tasks\n");
                pthread_exit(NULL);
            }
        }
        printf("Tasks...\n");
        peerfd = pthreadpool->queue.pFront->peerfd;
        //任务出队
        task_dequeue(&pthreadpool->queue);
        //对应push, 线程pop
        pthread_cleanup_pop(1);
        handle_event(peerfd, pthreadpool);
        printf("Task done\n");
    }
}

void handle_event(int peerfd, threadPool_t * pthreadpool)
{
    char buf[BUFSIZ];
    bzero(buf, sizeof(buf));
    int res = read(peerfd, buf, sizeof(buf));
    ERROR_CHECK(res, -1, "read");
    int cmd_type = atoi(buf);

    switch(cmd_type)
    {
    case COMMAND_CD:
        command_cd()
    case COMMAND_LS:
    case COMMAND_PWD:
    case COMMAND_PUT:
    case COMMAND_GET:
    case COMMAND_RM:
    case COMMAND_MKDIR:
    case:
        
            
    }

}

void clean_func(void *parg)
{
    printf("clean\n");
    threadPool_t * pthreadpool = (threadPool_t *)parg;
    pthread_mutex_unlock(&pthreadpool->queue.mutex);
}
