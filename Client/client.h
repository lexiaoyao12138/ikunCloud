#include "../public/public.h"


typedef enum {
  response_str = 2,
  response_file = 3,
} Response_type;

// 接受客户端数据包格式
typedef struct {
  Response_type type;    /*接受数据类型*/
  int response_len;      /*接收数据长度*/
  char response[BUFSIZ]; /*接受的内容*/
} resp_node;


// 设置接收数据包
void set_recv(int*, resp_node*);

// 发送数据包
void send_data(char *head, char *buf, int *peerfd);
// 接受数据包
void read_data(int *peerfd);

int epoll_add(int, int);
int epoll_del(int, int);

// 判断命令中是否有空格
int ishave_space(char *);

// 提取命令中的命令部分
void get_command(char *, char *);

// 提取命令中的参数部分
void get_argument(char *, char *);

// 命令解析
void parse_command(char *, int);

// 判断命令是几
int is_num(char *);


