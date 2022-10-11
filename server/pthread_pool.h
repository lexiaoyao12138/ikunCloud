#include "taskQueue.h"

typedef struct {
	pthread_t * thread;    //线程池
    int threadNumber;    //线程数
	taskQueue_t queue;    //任务队列
}threadPool_t, *pthreadPool_t;

typedef struct {
	int length;
	char data[BUFSIZ];
}dumptruck_t;

void * thread_func(void *);
void threadpool_init(threadPool_t *, int);
void threadPool_destroy(threadPool_t *);
void threadPool_start(threadPool_t *);
void threadPool_stop(threadPool_t *);
void clean_func(void *);
void handle_event(int, threadPool_t *);
