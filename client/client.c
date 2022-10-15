#include "client.h"

void get_argument(char *buf, char *argument);

int main() {
  int res, ready_num;
  char buf[100];
  int epfd = epoll_create1(0);
  struct epoll_event *epoll_list =
      (struct epoll_event *)calloc(10, sizeof(struct epoll_event));

  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  ERROR_CHECK(clientfd, -1, "socket");

  struct sockaddr_in serveraddr;
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(8888);
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  res = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  ERROR_CHECK(res, -1, "connect");

  time_t start_time = time(NULL);
  time_t currt_time = time(NULL);
  char name[100] = {0};
  char key[100] = {0};
  int login = 0;
  do{
    if(login){
        puts("login fall please try again!");      
    }
    bzero(name,strlen(name));                                                
    bzero(key,strlen(key));
    currt_time = time(NULL);
    puts("enter name:");
    scanf("%s",name);
    puts("enter key:");
    scanf("%s",key);
  } while((login = client_login(clientfd,name,key)) && (difftime(currt_time,start_time) < 20 ));

  puts("login success!");
 
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
        ERROR_CHECK(res, -1, "read");
        // 解析命令
        parse_command(buf, clientfd);
      }

      // 客户端响应事件
      if (tempFd == clientfd) {
        read_data(&tempFd);
      }
    }
  }
}
