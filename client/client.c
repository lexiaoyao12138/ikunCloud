#include "client.h"

void get_argument(char *buf, char *argument);

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

  res = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  ERROR_CHECK(res, -1, "connect");

	epoll_add(epfd, clientfd);
	epoll_add(epfd, STDIN_FILENO);

  // 事件循环
  while (1) {
  	ready_num = epoll_wait(epfd, epoll_list, 10, -1);
		for (int i = 0; i < ready_num; i++) {
			int tempFd = epoll_list[i].data.fd;
			// 键盘输入事件
			if (tempFd == STDIN_FILENO) {
				char command[3];
				char argument[BUFSIZ];
				bzero(buf, sizeof(buf));
				res = read(STDIN_FILENO, buf, sizeof(buf));
				// get put
				strncpy(command, buf, 3);
				get_argument(buf, argument);
				if (strcmp(command, "put") == 0) {
					res = write(clientfd, "ikun", 4);
					ERROR_CHECK(res, -1, "write");
					res = write(clientfd, buf, strlen(buf));
					ERROR_CHECK(res, -1, "write");
					send_file(clientfd, argument);
				} else {
					send_data("ikun", buf, &clientfd);
				}
			}

			// 客户端响应事件
			if (tempFd == clientfd) {
				read_data(&tempFd);
			}
		}	
  }
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
