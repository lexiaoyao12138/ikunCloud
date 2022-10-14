// #include "task_queue.h"
#include "../public/public.h"


typedef struct {
  int length;
  char data[BUFSIZ];
} dumptruck_t;

typedef enum {
  response_str = 2,    /*普通命令类型*/
  response_file = 3,    /*文件命令类型*/
} Response_type;

// 接受客户端数据包格式
typedef struct {
  Response_type type;    /*接受数据类型*/
  int response_len;      /*接收数据长度*/
  char response[BUFSIZ]; /*接受的内容*/
} resp_node;


struct user_info {
  int peerfd;      // 用户客户端套接字
  char path[1024]; // 用户当前工作目录
};

typedef struct task_s {
  int peerfd;            /*客户端标识*/
  int count;             /*1 or 2*/
  command_type type;     /*动作*/
  int argument_len;      /*参数的字符长度*/
  char argument[BUFSIZ]; /*命令参数*/
  struct task_s *pnext;
} task_t;

typedef struct task_queue_s {
  task_t *pFront;
  task_t *pRear;
  int size;     // 任务队列大小
  int exitFlag; // 退出标志
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} task_queue_t;

typedef struct {
  pthread_t *thread;
  int threadNumber;
  task_queue_t queue; // 任务队列
} thread_pool_t, *pthread_pool_t;

// 队列初始化
void queue_init(task_queue_t *);
// 队列销毁
void queue_destroy(task_queue_t *);
// 判断队列是否为空
int queue_isempty(task_queue_t *);
// 任务入队
void task_enqueue(task_queue_t *, int peerfd);
/// 任务出队
int task_dequeue(task_queue_t *);
// 得到队列大小
int get_tasksize(task_queue_t *);
// 
void queue_wakeup(task_queue_t *);

int tcp_init(char *host, int port);

void *threadFunc(void *);

/* 处理各种命令封装接口*/
void handle_command_cd(int peerfd, char *argument);
void handle_command_pwd(int peerfd);
void handle_command_rm(char *argument);
void handle_command_mkdir(char *argument);
void handle_command_ls(int peerfd, char *path);
void handle_command_put(int peerfd);
void handle_command_get(int peerfd, char *);

int send_circle(int fd, const char * buf, int length);
int recv_circle(int fd, char * pbuf, int length);
void send_file(int peerfd, const char * filename);
void recv_file(int peerfd);

int command_cd(char *, struct user_info *);
int command_ls(char *);
int command_pwd(char *);
int command_rm(char *);
int command_mkdir(char *);
int command_tree();   /*未实现*/


int epoll_add(int, int);
int epoll_del(int, int);

// 向客户端发送协议数据包
void send_data(int*, int, char*);

void modify_command_type(task_t *node, char *buf);
void *thread_func(void *);
void threadpool_init(thread_pool_t *, int);
void threadPool_destroy(thread_pool_t *);
void threadPool_start(thread_pool_t *);
void threadPool_stop(thread_pool_t *);
void clean_func(void *);
void handle_event(task_t *, thread_pool_t *);
void transfer_file(int, const char *);

// 判断命令中是否有空格
int ishave_space(char *);

// 提取命令中的命令部分
void get_command(char*, char*);

// 提取命令中的参数部分
void get_argument(char*, char*);

// 设置任务节点
int set_task_node(task_t *);
