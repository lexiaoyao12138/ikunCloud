#include "task_queue.h"


typedef struct {
	pthread_t * thread;
    int threadNumber;
	task_queue_t queue;    //任务队列
}thread_pool_t, *pthread_pool_t;

typedef struct {
	int length;
	char data[BUFSIZ];
}dumptruck_t;

void * threadFunc(void*);

void * thread_func(void *);
void threadpool_init(thread_pool_t *, int);
void threadPool_destroy(thread_pool_t *);
void threadPool_start(thread_pool_t *);
void threadPool_stop(thread_pool_t *);
void clean_func(void *);
void handle_event(int, thread_pool_t *);
void transfer_file(int, const char *);
