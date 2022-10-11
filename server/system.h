#include "../public/public.h"

struct user_info {
  int peerfd;      // 用户客户端套接字
  char path[1024]; // 用户当前工作目录
};

int command_cd(char *, struct user_info *);
// int command_ls(char *);
int command_pwd(char *);
int command_rm(char *, char *);
int command_mkdir(char *);
int command_tree();

int epoll_add(int, int);
int epoll_del(int, int);
