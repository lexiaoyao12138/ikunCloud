#include "pthread_pool.h"
#include "log.h"

int set_task_node(task_t *node) {
  int res;
  int count; /*命令单词个数*/
  char buf[100];
  // 读取协议内容
  res = read(node->peerfd, (char *)&count, sizeof(int));
  ERROR_CHECK(res, -1, "read");
	if (res == 0) {
		puts("客户端断开连接！");
		return -1;
	}
  // printf("命令的类型num:%d\n", count);
  // 单命令处理
  if (count == 1) {
    node->argument_len = 0;
    strcpy(node->argument, "./");

    // 读取命令
    res = read(node->peerfd, (char *)&node->type, sizeof(int)); /*设置命令*/
    ERROR_CHECK(res, -1, "write");

    // 带参数命令处理
  } else if (count == 2) {
    // 读取命令数字
    res = read(node->peerfd, (char *)&node->type, sizeof(int)); /*设置命令*/
    ERROR_CHECK(res, -1, "write");
    printf("type: %d\n", node->type);
    // 读取参数的长度
    res = read(node->peerfd, (char *)&node->argument_len, sizeof(int));
    ERROR_CHECK(res, -1, "read");
    // 读取参数
    bzero(node->argument, sizeof(node->argument));
    res = read(node->peerfd, node->argument, node->argument_len);
    // bzero(node->argument, sizeof(node->argument));
    ERROR_CHECK(res, -1, "read");
  }
	return 0;
}

void handle_event(task_t *node, thread_pool_t *pthreadpool) {
  // puts("进入处理函数");
  int res;
  int epfd = epoll_create1(0);
  // 加入监听
  epoll_add(epfd, node->peerfd);
  struct epoll_event *epoll_list =
      (struct epoll_event *)calloc(10, sizeof(struct epoll_event));

  while (1) {
    int ready_num = epoll_wait(epfd, epoll_list, 10, -1);
    // 任务处理
    // 填充任务节点 进行协议处理
    if (set_task_node(node) == -1) {
			return;
		}

    for (int i = 0; i < ready_num; i++) {
      int temp_fd = epoll_list[i].data.fd;

      if (temp_fd == node->peerfd) {
        // puts("start handle client");

        // printf("command_type: %d\n", node->type);
        // printf("argurment: %s\n", node->argument);
    int fd_cmd = open("log1", O_RDWR|O_CREAT|O_TRUNC, 0666);                  
 ERROR_CHECK(fd_cmd, -1, "open");

          log_cmd(fd_cmd, node->ip, node->type, node->argument);
        switch (node->type) {
        case COMMAND_CD:
          handle_command_cd(node->peerfd, node->argument);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        case COMMAND_LS:
          handle_command_ls(node->peerfd, node->argument);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        case COMMAND_PWD:
          handle_command_pwd(node->peerfd);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        case COMMAND_PUT:
          handle_command_put(node->peerfd);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        case COMMAND_GET:
          handle_command_get(node->peerfd, node->argument);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        case COMMAND_RM:
          handle_command_rm(node->argument);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        case COMMAND_MKDIR:
          handle_command_mkdir(node->argument);
          log_cmd(fd_cmd, node->ip, node->type, node->argument);
          break;
        default:
          return;
        
        }
      }
    }
  }
}

void send_data(int *peerfd, int type, char *buf) {
  int res;
  resp_node send_node;
  send_node.type = type;                /*设置数据类型*/
  send_node.response_len = strlen(buf); /*设置发送数据长度*/
  strncpy(send_node.response, buf, send_node.response_len); /*设置发送数据*/

  // 开始发送
  // 发送数据类型
  res = write(*peerfd, (char *)&send_node.type, sizeof(int));
  ERROR_CHECK(res, -1, "write");
  // 发送数据长度
  res = write(*peerfd, (char *)&send_node.response_len, sizeof(int));
  ERROR_CHECK(res, -1, "write");
  // 发送数据
  res = write(*peerfd, send_node.response, send_node.response_len);
  ERROR_CHECK(res, -1, "write");
}

// 处理ls命令
void handle_command_ls(int peerfd, char *path) {
  command_ls(path);
  send_data(&peerfd, 2, path);
}

void handle_command_cd(int peerfd, char *argument) {
  struct user_info user_msg;
  command_cd(argument, &user_msg);
}

// 处理pwd命令
void handle_command_pwd(int peerfd) {
  int res;
  char cwd[BUFSIZ];
  command_pwd(cwd);
  send_data(&peerfd, 2, cwd);
}

void handle_command_rm(char *argument) { command_rm(argument); }

void handle_command_mkdir(char *agrument) { 
	int res = mkdir(agrument, 0666);
	ERROR_CHECK(res, -1, "mkdir");
}

void handle_command_put(int peerfd) {
  recv_file(peerfd);                 /*先接收文件*/
  send_data(&peerfd, 2, "接收成功"); /*再发送协议包*/
}

void handle_command_get(int peerfd, char *filename) {
  send_data(&peerfd, 3, NULL);
  send_file(peerfd, filename);
}
