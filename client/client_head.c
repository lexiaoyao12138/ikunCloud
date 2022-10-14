#include "client.h"

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

void read_data(int *peerfd) {
  resp_data data;
  char buf[BUFSIZ];
  int res;
	char buff[5] = {0};

  res = read(*peerfd, buff, 4);
	printf("buff: %s\n", buff);
  if (res == 0) {
    printf("客户端断开连接！");
    exit(0);
  }
  ERROR_CHECK(res, -1, "read");

  if (strcmp(buff, "file") == 0) {
		printf("-----------");
    recv_file(*peerfd);
  } else {
    bzero(buf, sizeof(*buf));
    res = read(*peerfd, buf, sizeof(buf));
    puts(buf);
  }
}
