#include "pthread_pool.h"

void queue_init(task_queue_t *que) {
  if (que) {
    que->pFront = NULL;
    que->pRear = NULL;
    que->size = 0;
    que->exitFlag = 0;

    int ret = pthread_mutex_init(&que->mutex, NULL);
    PTHREAD_CHECK(ret, "pthread_mutex_init");

    ret = pthread_cond_init(&que->cond, NULL);
    PTHREAD_CHECK(ret, "pthread_cond_init");
  }
}

void queue_destroy(task_queue_t *que) {
  if (que) {
    int ret = pthread_mutex_destroy(&que->mutex);
    PTHREAD_CHECK(ret, "pthread_mutex_destroy");

    pthread_cond_destroy(&que->cond);
    PTHREAD_CHECK(ret, "pthread_cond_destroy");
  }
}

int queue_isempty(task_queue_t *que) { return que->size == 0; }

int get_tasksize(task_queue_t *que) { return que->size; }

// void modify_command_type(command_type *type, char *buf);

// 任务入队列
void task_enqueue(task_queue_t *que, int peerfd) {
  pthread_mutex_lock(&que->mutex);

	int res;
  task_t *task_newNode = (task_t *)calloc(1, sizeof(task_t));

	// 新建任务节点
  task_newNode->peerfd = peerfd;
	// task_newNode->type = 0;
	// strcpy(task_newNode->argument, "./");
  task_newNode->pnext = NULL;

  if (que->size == 0) {
    que->pFront = que->pRear = task_newNode;
  } else {
    que->pRear->pnext = task_newNode;
    que->pRear = task_newNode;
  }
  que->size++;

  pthread_mutex_unlock(&que->mutex);
  pthread_cond_signal(&que->cond);
}

int task_dequeue(task_queue_t *que) {
  pthread_mutex_lock(&que->mutex);

  while (!que->exitFlag && queue_isempty(que)) {
    pthread_cond_wait(&que->cond, &que->mutex);
  }

  if (!que->exitFlag) {
    task_t *delNote = que->pFront;
    int peerfd = delNote->peerfd;
    if (get_tasksize(que) > 1) {
      que->pFront = que->pFront->pnext;
    } else {
      que->pFront = que->pRear = NULL;
    }
    que->size--;

    pthread_mutex_unlock(&que->mutex);
    // free(delNote);
    return peerfd;
  }else{
    pthread_mutex_unlock(&que->mutex);
    return -1;
  }
}

void queue_wakeup(task_queue_t * que)
{
    que->exitFlag = 1;
    pthread_cond_broadcast(&que->cond);

}
