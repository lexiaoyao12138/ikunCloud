#include "pthread_pool.h"

void threadpool_init(thread_pool_t *pthreadpool, int pthreadNum) {
  if (pthreadpool) {
    pthreadpool->threadNumber = pthreadNum;
    pthreadpool->thread = (pthread_t *)calloc(pthreadNum, sizeof(pthread_t));
    queue_init(&pthreadpool->queue);
  }
}

void threadPool_destroy(thread_pool_t *pthreadpool) {
  if (pthreadpool) {
    free(pthreadpool->thread);
    queue_destroy(&pthreadpool->queue);
  }
}

void threadPool_start(thread_pool_t *pthreadpool) {
  for (int i = 0; i < pthreadpool->threadNumber; ++i) {
    int ret =
        pthread_create(&pthreadpool->thread[i], NULL, thread_func, pthreadpool);
    PTHREAD_CHECK(ret, "pthread_create");
  }
}

void threadPool_stop(thread_pool_t *pthreadpool) {
  while (!queue_isempty(&pthreadpool->queue)) {
    sleep(1);
  }
  queue_wakeup(&pthreadpool->queue);
  for (int i = 0; i < pthreadpool->threadNumber; ++i) {
    pthread_join(pthreadpool->thread[i], NULL);
  }
}

void *thread_func(void *arg) {
  task_t *temp_node;
  while (1) {
    thread_pool_t *pthreadpool = (thread_pool_t *)arg;
    // 互斥锁
    pthread_mutex_lock(&pthreadpool->queue.mutex);
    // 线程清理函数
    // NOTE: cleanup
    pthread_cleanup_push(clean_func, (void *)pthreadpool);
    while (queue_isempty(&pthreadpool->queue)) {
      // 等待解锁
      pthread_cond_wait(&pthreadpool->queue.cond, &pthreadpool->queue.mutex);
    }
    printf("Tasks...\n");
    // peerfd = pthreadpool->queue.pFront->peerfd;
    temp_node = pthreadpool->queue.pFront;
    pthread_mutex_unlock(&pthreadpool->queue.mutex);
    // 任务出队
    task_dequeue(&pthreadpool->queue);
    // 对应push, 线程pop
    pthread_cleanup_pop(1);
    // 任务处理
    handle_event(*temp_node, pthreadpool);
    printf("Task done\n");
  }
}

void clean_func(void *parg) {
  printf("clean\n");
  thread_pool_t *pthreadpool = (thread_pool_t *)parg;
  pthread_mutex_unlock(&pthreadpool->queue.mutex);
}

int ishave_space(char *buf) {
  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] == ' ') {
      return 1;
    }
  }
  return 0;
}

void get_command(char *buf, char *command) {
  int k = 0;
  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] != ' ') {
      command[k++] = buf[i];
      continue;
    }
    break;
  }
  command[k] = '\0';
}

void get_argument(char *buf, char *argument) {
  char temp[BUFSIZ] = {0};
  int n = 0;
  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] != ' ') {
      n++;
      continue;
    }
    break;
  }
  int k = 0;
  for (int i = n + 1; i < strlen(buf) - 1; i++) {
    argument[k++] = buf[i];
  }
}
