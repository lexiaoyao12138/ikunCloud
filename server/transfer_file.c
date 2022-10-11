#include "pthread_pool.h"

int send_circle(int fd, const char * buf, int length)
{
  const char* pbuf = buf;
	int ret = 0;
	int left = length;

	while(left > 0) {
		ret = send(fd, pbuf, left, 0);
		if(ret < 0) {
			perror("send");
			return -1;
		}

		pbuf += ret;
		left -= ret;
	}
	return length - left;
}

void transfer_file(int peerfd, const char * filename)
{
	int res;
  int fds[2];
	pipe(fds);

	//读取本地文件
	int rfd = open(filename, O_RDWR);
	ERROR_CHECK(rfd, -1, "open");
  
	Mudhead_t truck;
	memset(&truck, 0, sizeof(truck));

	truck.length = strlen(filename);
	strncpy(truck.data, filename, truck.length);
  
	//传送文件名
	res = send_circle(peerfd, (const char *)&truck, truck.length);
  ERROR_CHECK(res, -1, "send filename");

	//传送文件内容
  struct stat st;
	fstat(rfd, &st);
	printf("file length:%ld\n", st.st_size);
  res = send_circle(peerfd, (const char*)&st.st_size, st.st_size);
	ERROR_CHECK(res, -1, "send file_content");

	char* pbuf = (char *)mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, rfd, 0);
	int ret = send(peerfd, pbuf, st.st_size, 0);
	printf("send ret:%d\n", ret);

	munmap(pbuf, st.st_size);
  close(rfd);
}
