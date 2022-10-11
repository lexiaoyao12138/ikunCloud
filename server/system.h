#include "../public/public.h"

int command_cd(char *);
int command_ls(char *);
int command_pwd(char *);
int command_rm(char *, char *);
int command_mkdir(char *);
int command_tree();

int epoll_add(int, int);
int epoll_del(int, int);
