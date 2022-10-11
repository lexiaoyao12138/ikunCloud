#include "../public/public.h"
#include "taskQueue.h"


typedef struct {
	pthread_t * thread; 
  int threadNumber;
	taskQueue_t queue;
}threadPool_t, *pthreadPool_t;

typedef struct {
	int length;
	char data[BUFSIZ];
}Mudhead_t;

void * threadFunc(void*);

void threadpool_init(threadPool_t *);
void threadPool_destroy(threadPool_t *);
void threadPool_start(threadPool_t *);
void threadPool_stop(threadPool_t *);


