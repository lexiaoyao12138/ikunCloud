#include "taskQueue.h"


typedef struct {
<<<<<<< HEAD
	pthread_t * thread;
    int threadNumber;
	taskQueue_t queue;
=======
	pthread_t * thread;    //线程池
    int threadNumber;    //线程数
	taskQueue_t queue;    //任务队列
>>>>>>> 3600c66f964f67d74e1f3986031a316a0313e34f
}threadPool_t, *pthreadPool_t;

typedef struct {
	int length;
	char data[BUFSIZ];
<<<<<<< HEAD
}dumptruck_t;
=======
}Mudhead_t;

void * threadFunc(void*);
>>>>>>> 5f819d350a4b8ff33095133ca02fb82718c29a65

void * thread_func(void *);
void threadpool_init(threadPool_t *, int);
void threadPool_destroy(threadPool_t *);
void threadPool_start(threadPool_t *);
void threadPool_stop(threadPool_t *);
<<<<<<< HEAD
void clean_func(void *);
void handle_event(int, threadPool_t *);
=======
void transfer_file(int, const char *);
>>>>>>> 5f819d350a4b8ff33095133ca02fb82718c29a65
