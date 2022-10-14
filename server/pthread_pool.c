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
    // printf("Tasks...\n");
    // peerfd = pthreadpool->queue.pFront->peerfd;
    temp_node = pthreadpool->queue.pFront;
    pthread_mutex_unlock(&pthreadpool->queue.mutex);
    // 任务出队
    task_dequeue(&pthreadpool->queue);
    // 对应push, 线程pop
    pthread_cleanup_pop(1);
		// 进行任务处理
    handle_event(temp_node, pthreadpool);
		printf("client: %s:%d 断开连接\n", temp_node->ip, temp_node->port);
    // printf("Task done\n");
  }
}

void clean_func(void *parg) {
  // printf("clean\n");
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

int epoll_add(int epoll_fd,int read_fd){
    struct epoll_event event;
    memset(&event,0,sizeof(event));
    event.data.fd = read_fd;
    event.events |= EPOLLIN;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,read_fd,&event);
    ERROR_CHECK(ret,-1,"epoll");
    return 0;
}

int epoll_del(int epoll_fd,int del_fd){
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_DEL,del_fd,NULL);
    ERROR_CHECK(ret,-1,"epoll");
    close(del_fd);
    return 0;
}

int tcp_init(char *host, int port) {
  int on = 1, res;
  // 1. 创建监听套接字
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  ERROR_CHECK(listenfd, -1, "socket");

  res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  ERROR_CHECK(res, -1, "setsockopt");

  res = setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
  ERROR_CHECK(res, -1, "setsockopt");

  // 2. 服务器要绑定网络地址
  struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(port);
  serveraddr.sin_addr.s_addr = inet_addr(host);

  res = bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  ERROR_CHECK(res, -1, "bind");

  // 3. 对客户端进行监听
  res = listen(listenfd, 10);
  ERROR_CHECK(res, -1, "listen");
  return listenfd;
}
