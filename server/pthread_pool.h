#include "task_queue.h"
#include "system.h"

typedef struct {
  pthread_t *thread;
  int threadNumber;
  task_queue_t queue; // 任务队列
} thread_pool_t, *pthread_pool_t;

typedef struct {
  int length;
  char data[BUFSIZ];
} dumptruck_t;

// 响应数据包定义
typedef struct {
  char head[4];     /*change "ikun" or "file" */
  char buf[BUFSIZ]; /*send real data*/
} resp_data;

void *threadFunc(void *);

void handle_command_cd(int peerfd, char *argument);

void handle_command_pwd(int peerfd);

void handle_command_rm(int peerfd, char *argument);

void handle_command_mkdir(int peerfd, char *argument);

void handle_command_ls(int peerfd, char *path);

void handle_command_put(int peerfd);
void handle_command_get(int peerfd, char *);



void send_data(char *head, char *buf, int *peerfd);
void modify_command_type(task_t *node, char *buf);
void *thread_func(void *);
void threadpool_init(thread_pool_t *, int);
void threadPool_destroy(thread_pool_t *);
void threadPool_start(thread_pool_t *);
void threadPool_stop(thread_pool_t *);
void clean_func(void *);
void handle_event(task_t, thread_pool_t *);
void transfer_file(int, const char *);

// 判断命令中是否有空格
int ishave_space(char *);

// 提取命令中的命令部分
void get_command(char*, char*);
// 提取命令中的参数部分
void get_argument(char*, char*);
