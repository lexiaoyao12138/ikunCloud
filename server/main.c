#include "../public/public.h"
#include "server.h"
#include "pthread_pool.h"
#include "tcp.h"
#include "system.h"

int exit_pipe[2];

void sign_function(int sign) {
	char *buf = "exit";
	write(exit_pipe[1], buf, strlen(buf));
}

int main() {
	int res;          /*return values*/ 
	int epoll_ready_num;
	int clientFd;
	char buf[BUFSIZ];
	config_t *config;
	struct epoll_event *epoll_event_list;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;

	pipe(exit_pipe);
	pid_t pid = fork();
	if (pid > 0) {
		wait(NULL);
		exit(0);
	}

	// 读取配置文件 
	char *path = CONFIG_PATH;
	config = init_config_t();
	get_config(path, config);

	thread_pool_t pthread_pool;
	threadpool_init(&pthread_pool, config->pthread_num);
	threadPool_start(&pthread_pool);

	// 获取监听网络套接字
	int listenfd = tcp_init(config->host, config->port);

	// 创建epoll树节点
	int epfd = epoll_create1(0);
	ERROR_CHECK(epfd, -1, "epoll_create");

	// 加入监听套接字
	epoll_add(epfd, listenfd);
	epoll_add(epfd, exit_pipe[0]);
	epoll_event_list = (struct epoll_event*)calloc(config->epoll_num, sizeof(struct epoll_event));
	
	while (1) {
		epoll_ready_num = epoll_wait(epfd, epoll_event_list, config->epoll_num, -1);
		if (epoll_ready_num == -1 && errno == EINTR) {
			 continue;
		} else if (epoll_ready_num == -1) {
			perror("epoll_wait");
			exit(-1);
		} else if (epoll_ready_num == 0){
			printf("epoll_wait timeout");
		} else {
			for (int i = 0; i < epoll_ready_num; i++) {
				int tempFd = epoll_event_list[i].data.fd;
				// 新连接加入
				if (tempFd == listenfd) {
					client_addr_len = sizeof(client_addr);
					clientFd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
					if (clientFd == -1) {
						perror("accpet");
						continue;
					}
					bzero(buf, sizeof(buf));
					// 终端打印连接的ip和port
					const char *client_ip = inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, buf, sizeof(buf));
					printf("client ip: %s, port: %d\n", client_ip, ntohs(client_addr.sin_port));

					// 加入队列
					task_enqueue(&pthread_pool.queue, clientFd);
				} 
				if (tempFd == exit_pipe[0]) {
					bzero(buf, sizeof(buf));
					read(tempFd, buf, sizeof(buf));

					threadPool_stop(&pthread_pool);
				}

			}
		}
	}
	close(listenfd);

	return 0;
}
