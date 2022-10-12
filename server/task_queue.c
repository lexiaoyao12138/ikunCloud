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

void task_enqueue(task_queue_t *que, int peerfd) {
  pthread_mutex_lock(&que->mutex);
  task_t *pnewTask = (task_t *)calloc(1, sizeof(task_t));
  pnewTask->peerfd = peerfd;
  pnewTask->pnext = NULL;

  if (que->size == 0) {
    que->pFront = que->pRear = pnewTask;
  } else {
    que->pRear->pnext = pnewTask;
    que->pRear = pnewTask;
  }
  que->size++;

  pthread_mutex_unlock(&que->mutex);
  pthread_cond_signal(&que->cond);
}

<<<<<<< HEAD:server/taskQueue.c
int task_dequeue(taskQueue_t *que) {
=======
int task_dequeue(task_queue_t *que) {
	puts("task_dequeue.......");
>>>>>>> 9223a7a71ae0d99307dc192715ebbace760e6e79:server/task_queue.c
  int pth = pthread_mutex_lock(&que->mutex);

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
    free(delNote);
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
