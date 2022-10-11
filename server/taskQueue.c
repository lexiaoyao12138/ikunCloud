#include "../public/public.h"
#include "pthread_pool.h"

void queue_init(taskQueue_t *que) {
  if (que) {
    que->pFront = NULL;
    que->pRear = NULL;
    que->queSize = 0;
    que->exitFlag = 0;

    int ret = pthread_mutex_init(&que->mutex, NULL);
    PTHREAD_CHECK(ret, "pthread_mutex_init");

    ret = pthread_cond_init(&que->cond, NULL);
    PTHREAD_CHECK(ret, "pthread_cond_init");
  }
}

void queue_destroy(taskQueue_t *que) {
  if (que) {
    int ret = pthread_mutex_destroy(&que->mutex);
    PTHREAD_CHECK(ret, "pthread_mutex_destroy");

    pthread_cond_destroy(&que->cond);
    PTHREAD_CHECK(ret, "pthread_cond_destroy");
  }
}

int queue_isempty(taskQueue_t *que) { return que->queSize == 0; }

int get_tasksize(taskQueue_t *que) { return que->queSize; }

void task_enqueue(taskQueue_t *que, int peerfd) {
  pthread_mutex_lock(&que->mutex);
  task_t *pnewTask = (task_t *)calloc(1, sizeof(task_t));
  pnewTask->peerfd = peerfd;
  pnewTask->pnext = NULL;

  if (que->queSize == 0) {
    que->pFront = que->pRear = pnewTask;
  } else {
    que->pRear->pnext = pnewTask;
    que->pRear = pnewTask;
  }
  que->queSize++;

  pthread_mutex_unlock(&que->mutex);
  pthread_cond_signal(&que->cond);
}

int task_dequeue(taskQueue_t *que) {
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
    que->queSize--;

    pthread_mutex_unlock(&que->mutex);
    free(delNote);
    return peerfd;
  }else{
    pthread_mutex_unlock(&que->mutex);
    return -1;
  }
}

