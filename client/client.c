#include "client.h"

int main() {
  int res, ready_num;
	char buf[BUFSIZ];
	int epfd = epoll_create1(0);
	struct epoll_event *epoll_list = (struct epoll_event*)calloc(10, sizeof(struct epoll_event));

  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  ERROR_CHECK(clientfd, -1, "socket");

  struct sockaddr_in serveraddr;
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(8888);
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // 2.1 直接连接服务器
  res = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  ERROR_CHECK(res, -1, "connect");
  // 当connet正常返回时，链接已经建立成功了

	epoll_add(epfd, clientfd);
	epoll_add(epfd, STDIN_FILENO);

  // 事件循环
  while (1) {
  	ready_num = epoll_wait(epfd, epoll_list, 10, -1);
		for (int i = 0; i < ready_num; i++) {
			int tempFd = epoll_list[i].data.fd;
			// 键盘输入事件
			if (tempFd == STDIN_FILENO) {
				bzero(buf, sizeof(buf));
				res = read(STDIN_FILENO, buf, sizeof(buf));
				send_data("ikun", buf, &clientfd);
			}

			// 客户端响应事件
			if (tempFd == clientfd) {
				read_data(&tempFd);
			}
		}	
  }
}
