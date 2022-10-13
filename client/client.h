#include "../public/public.h"
#include "../server/system.h"

typedef struct {
	char head[4];
	char buf[BUFSIZ];
} resp_data;

// 发送数据包
void send_data(char *head, char *buf, int *peerfd);
// 接受数据包
void read_data(int *peerfd);


int epoll_add(int, int);
int epoll_del(int, int);
