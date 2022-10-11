#include "tcp.h"

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
