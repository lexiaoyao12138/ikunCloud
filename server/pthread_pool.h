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

// 响应数据包定义
typedef struct {
	char head[4];   /*change "ikun" or "file" */
	char buf[BUFSIZ];  /*send real data*/
} resp_data;

void * threadFunc(void*);

void * thread_func(void *);
void threadpool_init(thread_pool_t *, int);
void threadPool_destroy(thread_pool_t *);
void threadPool_start(thread_pool_t *);
void threadPool_stop(thread_pool_t *);
void clean_func(void *);
void handle_event(task_t, thread_pool_t *);
void transfer_file(int, const char *);
