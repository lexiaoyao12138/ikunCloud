#include "../public/public.h"

typedef struct task_s {
	int peerfd;
	struct task_s * pnext;
}task_t;

typedef struct taskQueue_s {
	task_t * pFront;
	task_t * pRear;
	int queSize;
	int exitFlag;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}taskQueue_t;


void queue_init(taskQueue_t *);
void queue_destroy(taskQueue_t *);
int queue_isempty(taskQueue_t *);
void task_enqueue(taskQueue_t *, int peerfd);
int task_dequeue(taskQueue_t *);
int get_tasksize(taskQueue_t *);
