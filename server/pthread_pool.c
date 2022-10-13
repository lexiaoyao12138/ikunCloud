#include "pthread_pool.h"
#include "system.h"
#include <stdlib.h>

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

void modify_command_type(task_t *node, char *buf) {
  printf("modify handle\n");
  char command[10];      /*保存命令*/
  char argument[BUFSIZ]; /*保存参数*/
  // 解析命令
  if (ishave_space(buf)) {
    get_command(buf, command);
    get_argument(buf, argument);
    strcpy(node->argument, argument);
  } else {
    strcpy(command, buf);
  }

  if (strcmp(command, "cd\n") == 0 || strcmp(command, "cd") == 0) {
    node->type = 2;
    return;
  }
  if (strcmp(command, "ls\n") == 0 || strcmp(command, "ls") == 0) {
    node->type = 3;
    return;
  }
  if (strcmp(command, "pwd\n") == 0 || strcmp(command, "pwd") == 0) {
    node->type = 4;
    return;
  }
  if (strcmp(command, "rm\n") == 0 || strcmp(command, "rm") == 0) {
    node->type = 7;
    return;
  }
  if (strcmp(command, "mkdir\n") == 0 || strcmp(command, "mkdir") == 0) {
    node->type = 8;
    return;
  }
  node->type = 0;
}

void handle_command_cd(int peerfd, char *argument);

void handle_command_pwd(int peerfd);

void handle_command_rm(int peerfd, char *argument);

void handle_command_mkdir(int peerfd, char *argument);

void handle_command_ls(int peerfd, char *path);

void handle_event(task_t node, thread_pool_t *pthreadpool) {
  puts("进入处理函数");
  int res;
  int command_type;
  char buf[BUFSIZ];

  int epfd = epoll_create1(0);
  epoll_add(epfd, node.peerfd);
  struct epoll_event *epoll_list =
      (struct epoll_event *)calloc(10, sizeof(struct epoll_event));

  while (1) {
    int ready_num = epoll_wait(epfd, epoll_list, 10, -1);
    for (int i = 0; i < ready_num; i++) {
      int temp_fd = epoll_list[i].data.fd;
      if (temp_fd == node.peerfd) {
        puts("start handle client");

        // 读取火车头
        // 判断命令类型  NOTE: 读取火车头
        bzero(buf, sizeof(buf));
        res = read(node.peerfd, buf, 4);
        ERROR_CHECK(res, -1, "read");
        if (strcmp(buf, "ikun") == 0) {
          bzero(buf, sizeof(buf));
          res = read(node.peerfd, buf, sizeof(buf));
          // 设置command_type的值
          modify_command_type(&node, buf);
          printf("handle_envet agrument: %s", node.argument);
        } else {
          puts("线程退出");
          node.type = 0;
        }

        printf("command_type: %d\n", node.type);
        switch (node.type) {
        case COMMAND_CD:
          handle_command_cd(node.peerfd, node.argument);
          break;
        case COMMAND_LS:
          handle_command_ls(node.peerfd, node.argument);
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
          handle_command_mkdir(node.peerfd, node.argument);
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

  res = write(*peerfd, data.head, 4);
  ERROR_CHECK(res, -1, "write");
  res = write(*peerfd, data.buf, strlen(data.buf));
  ERROR_CHECK(res, -1, "write");
}

// 处理ls命令
void handle_command_ls(int peerfd, char *path) {
  command_ls(path);
  send_data("ikun", path, &peerfd);
}

void handle_command_cd(int peerfd, char *argument) {
  struct user_info user_msg;
  command_cd(CURRENT_PATH, &user_msg);
}

void handle_command_pwd(int peerfd) {
  int res;
  char cwd[BUFSIZ];
  command_pwd(cwd);

  send_data("ikun", cwd, &peerfd);
}

void handle_command_rm(int peerfd, char *argument) {
  command_rm(argument, CURRENT_PATH);
}

void handle_command_mkdir(int peerfd, char *agrument) {
  command_mkdir(agrument);
  send_data("ikun", "ok\n", &peerfd);
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
