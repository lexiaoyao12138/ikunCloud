#include "../public/public.h"

typedef struct task_s {
	int peerfd;
	struct task_s * pnext;
}task_t;

typedef struct task_queue_s {
	task_t * pFront;
	task_t * pRear;
	int size;    //任务队列大小
	int exitFlag;    //退出标志
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}task_queue_t;


void queue_init(task_queue_t *);
void queue_destroy(task_queue_t *);
int queue_isempty(task_queue_t *);
void task_enqueue(task_queue_t *, int peerfd);
int task_dequeue(task_queue_t *);
int get_tasksize(task_queue_t *);
void queue_wakeup(task_queue_t *);
