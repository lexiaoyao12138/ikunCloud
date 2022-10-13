#include "pthread_pool.h"
#include "system.h"

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

void modify_command_type(command_type *type, char *buf) {
  if (strcmp(buf, "cd\n") == 0) {
    *type = 2;
    return;
  }
  if (strcmp(buf, "ls\n") == 0) {
    *type = 3;
    return;
  }
  if (strcmp(buf, "pwd\n") == 0) {
    *type = 4;
    return;
  }
  if (strcmp(buf, "rm\n") == 0) {
    *type = 7;
    return;
  }
  if (strcmp(buf, "mkdir\n") == 0) {
    *type = 8;
    return;
  }
  *type = 0;
}

void handle_command_cd();

void handle_command_pwd(int peerfd);

// void handle_command_rm();

// void handle_command_mkdir();

void handle_command_ls(int peerfd);

void handle_event(task_t node, thread_pool_t *pthreadpool) {
  int res;
  int command_type;
  char buf[BUFSIZ];

  int epfd = epoll_create1(0);
  epoll_add(epfd, node.peerfd);
  struct epoll_event *epoll_list =
      (struct epoll_event *)calloc(10, sizeof(struct epoll_event));

  while (1) {
    int ready_num = epoll_wait(epfd, epoll_list, 100, -1);
    for (int i = 0; i < ready_num; i++) {
      int temp_fd = epoll_list[i].data.fd;
      if (temp_fd == node.peerfd) {
        puts("start handle client");

        // 读取火车头
        // 判断命令类型  NOTE: 读取火车头
        bzero(buf, sizeof(buf));
        res = read(node.peerfd, buf, 4);
        printf("hanle_event 火车头: %s\n", buf);
        ERROR_CHECK(res, -1, "read");
        if (strcmp(buf, "ikun") == 0) {
          bzero(buf, sizeof(buf));
          res = read(node.peerfd, buf, sizeof(buf));
          puts(buf);
          // 设置command_type的值
          modify_command_type(&node.type, buf);
        } else {
          puts("线程退出");
          node.type = 0;
        }

        printf("node.type :%d\n", node.type);
        switch (node.type) {
        case COMMAND_CD:
          handle_command_cd();
          break;
        case COMMAND_LS:
          handle_command_ls(node.peerfd);
          break;
        case COMMAND_PWD:
          handle_command_pwd(node.peerfd);
          break;
        case COMMAND_PUT:
          break;
        case COMMAND_GET:
          break;
        case COMMAND_RM:
          break;
        case COMMAND_MKDIR:
          break;
        default:
          return;
        }
      }
    }
  }
}

void send_data(char *head, char *buf, int *peerfd) {
  resp_data data;
  int res;
  // 前置数据
  strcpy(data.head, "ikun");
  // 发送的数据
  strcpy(data.buf, buf);

  res = write(*peerfd, data.head, strlen(data.head));
  ERROR_CHECK(res, -1, "write");
  res = write(*peerfd, data.buf, strlen(data.buf));
  ERROR_CHECK(res, -1, "write");
}

// 处理ls命令
void handle_command_ls(int peerfd) {
  char path[BUFSIZ] = "./";
  command_ls(path);
  send_data("ikun", path, &peerfd);
  printf("ls 命令处理完毕\n");
}

void handle_command_cd() {
  struct user_info user_msg;
  command_cd(CURRENT_PATH, &user_msg);
}

void handle_command_pwd(int peerfd) {
  int res;
  char cwd[BUFSIZ];
  command_pwd(cwd);

  send_data("ikun", cwd, &peerfd);
}

void handle_command_rm(char *filename) { command_rm(filename, CURRENT_PATH); }

void handle_command_mkdir(char *path) { command_mkdir(path); }

void clean_func(void *parg) {
  printf("clean\n");
  thread_pool_t *pthreadpool = (thread_pool_t *)parg;
  pthread_mutex_unlock(&pthreadpool->queue.mutex);
}
