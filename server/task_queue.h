#include "../public/public.h"

typedef enum {
  COMMAND_CD = 2,
  COMMAND_LS = 3,
  COMMAND_PWD = 4,
  COMMAND_PUT = 5,
  COMMAND_GET = 6,
  COMMAND_RM = 7,
  COMMAND_MKDIR = 8,
} command_type;

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

void queue_init(task_queue_t *);
void queue_destroy(task_queue_t *);
int queue_isempty(task_queue_t *);
void task_enqueue(task_queue_t *, int peerfd);
int task_dequeue(task_queue_t *);
int get_tasksize(task_queue_t *);
void queue_wakeup(task_queue_t *);
