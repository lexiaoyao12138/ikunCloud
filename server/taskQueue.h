#include "../public/public.h"

typedef struct task_s {
	int peerfd;
	struct task_s * pnext;
}task_t;

typedef struct taskQueue_s {
	task_t * pFront;
	task_t * pRear;
	int size;    //任务队列大小
	int exitFlag;    //退出标志
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}taskQueue_t;


void queue_init(taskQueue_t *);
void queue_destroy(taskQueue_t *);
int queue_isempty(taskQueue_t *);
void task_enqueue(taskQueue_t *, int peerfd);
int task_dequeue(taskQueue_t *);
int get_tasksize(taskQueue_t *);
void queue_wakeup(taskQueue_t *);
