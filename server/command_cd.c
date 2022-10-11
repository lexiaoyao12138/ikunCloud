#include "system.h"
#include "../public/public.h"

struct user_info{
    int peerfd;//用户客户端套接字
    char path[1024];//用户当前工作目录

};

int command_cd(char* arg,struct user_info){
    int ret = chdir(arg);
    ERROR_CHECK(ret,-1,"chdir");
    
    return 0;
}
