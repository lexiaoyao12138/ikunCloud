#include "../public/public.h"
#include "system.h"

int epoll_add(int epoll_fd,int read_fd){
    struct epoll_event event;
    memset(&event,0,sizeof(event));
    event.data.fd = read_fd;
    event.events |= EPOLLIN;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,read_fd,&event);
    ERROR_CHECK(ret,-1,"epoll");
    return 0;
}

int epoll_del(int epoll_fd,int del_fd){
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_DEL,del_fd,NULL);
    ERROR_CHECK(ret,-1,"epoll");
    close(del_fd);
    return 0;
}
