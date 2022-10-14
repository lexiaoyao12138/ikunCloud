#include "client.h"

// 对客户端发来的数据进行响应
void read_data(int *peerfd) {
  resp_node recv_node;
  set_recv(peerfd, &recv_node);
  // 判断
  // 如果返回简单命令结果
  if (recv_node.type == 2) {
    // 命令行打印结果
    puts(recv_node.response);
    // 如果返回文件
  } else if (recv_node.type == 3) {
    // 接收文件
    recv_file(*peerfd);
  }
}

// 接收客户端发来的数据 设置填充接收数据包
void set_recv(int *peerfd, resp_node *recv_node) {
  int res;
  int type; /*保存接收数据类型*/

  // 接收数据类型
  res = read(*peerfd, (char *)&recv_node->type, sizeof(int));
  ERROR_CHECK(res, -1, "read");

  // 第一次读取时判断客户端是否断开连接
  if (res == 0) {
    puts("The server is disconnected!");
    exit(0);
  }

  // 接收数据长度
  res = read(*peerfd, (char *)&recv_node->response_len, sizeof(int));
  ERROR_CHECK(res, -1, "write");

  // 接收数据内容
  res = read(*peerfd, (char *)&recv_node->response, recv_node->response_len);
  ERROR_CHECK(res, -1, "write");
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

void parse_command(char *buf, int peerfd) {
  int num, res, count;
  char command[6] = {0};
  char argument[BUFSIZ] = {0};
  // 处理带参数的情况
  if (ishave_space(buf)) {
    int len;   /*记录参数的长度*/
    count = 2; /*发送命令的个数*/
    // 提取动作
    get_command(buf, command);
    // 提取参数
    get_argument(buf, argument);
    num = is_num(command);
    // 命令协议
    // 发送命令单词个数
    res = write(peerfd, (char *)&count, sizeof(int));
    ERROR_CHECK(res, -1, "write");
    // 发送命令数字
    res = write(peerfd, (char *)&num, sizeof(int));
    ERROR_CHECK(res, -1, "write");

    // 发送参数字符大小
    len = strlen(argument);
    res = write(peerfd, (char *)&len, sizeof(int));
    // 发送参数
    res = write(peerfd, argument, strlen(argument));
    ERROR_CHECK(res, -1, "write");

    if (num == COMMAND_PUT) {
      send_file(peerfd, argument); /*给服务器发送文件*/
      return;
    }

    // 处理没有参数的情况
  } else {
    count = 1;
    // 没有空格就是单个命令
    num = is_num(buf);
    // 命令协议
    // 发送命令单词个数
    res = write(peerfd, (char *)&count, sizeof(int));
    ERROR_CHECK(res, -1, "write");
    // 发送命令数字
    res = write(peerfd, (char *)&num, sizeof(int));
    ERROR_CHECK(res, -1, "write");
  }
}

int is_num(char *command) {
  int type = 0;
  if (strcmp(command, "cd\n") == 0 || strcmp(command, "cd") == 0) {
    type = 2;
  }
  if (strcmp(command, "ls\n") == 0 || strcmp(command, "ls") == 0) {
    type = 3;
  }
  if (strcmp(command, "pwd\n") == 0 || strcmp(command, "pwd") == 0) {
    type = 4;
  }

  if (strcmp(command, "put\n") == 0 || strcmp(command, "put") == 0) {
    type = 5;
  }
  if (strcmp(command, "get\n") == 0 || strcmp(command, "get") == 0) {
    type = 6;
  }
  if (strcmp(command, "rm\n") == 0 || strcmp(command, "rm") == 0) {
    type = 7;
  }
  if (strcmp(command, "mkdir\n") == 0 || strcmp(command, "mkdir") == 0) {
    type = 8;
  }

  return type;
}

int epoll_add(int epoll_fd, int read_fd) {
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.data.fd = read_fd;
  event.events |= EPOLLIN;
  int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, read_fd, &event);
  ERROR_CHECK(ret, -1, "epoll");
  return 0;
}

int epoll_del(int epoll_fd, int del_fd) {
  int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, del_fd, NULL);
  ERROR_CHECK(ret, -1, "epoll");
  close(del_fd);
  return 0;
}
