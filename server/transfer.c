#include "../public/public.h"

typedef struct {
    int length;
    char data[BUFSIZ];
}dumpTruck_t;

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

int recv_circle(int fd, char * pbuf, int length)
{
	int ret = 0;
	int left = length;

	while(left > 0) {
		ret = recv(fd, pbuf, left, 0);
		if(ret < 0) {
			perror("recv");
			return -1;
		}
		pbuf += ret;
		left -= ret;
	}
	return length - left;
}

int mmap_send_circle(int fd, int rfd,long int length)
{
    int max_mmap = 536870912;
    int mmap_size = 0;
	int ret = 0;
    int done = 0;//已完成传输
	long int left = length;

	while(left > 0) {
        if(left < max_mmap){
            mmap_size = left;//发送的文件小于max_mmap
        }
        else{
            mmap_size = max_mmap;//发送的文件大于max_mmap
        }
	    char* pbuf = (char *)mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, rfd, done);

        do{
            ret = send_circle(fd,pbuf,mmap_size);//循环发送已映射的文件数据
            if(ret < mmap_size){
                puts("Mapped data send incomplete!\n");
                puts("resend!\n");
            }
        }
        while(ret < mmap_size);

	    munmap(pbuf, mmap_size);

		done += ret;
		left -= ret;
	}
	return length - left;
}

void send_file(int peerfd, const char * filename)
{
	int res;
    int fds[2];
	pipe(fds);

	//读取本地文件
	int rfd = open(filename, O_RDWR);
	ERROR_CHECK(rfd, -1, "open");
  
	dumpTruck_t truck;
	memset(&truck, 0, sizeof(truck));

	truck.length = strlen(filename);
	strncpy(truck.data, filename, truck.length);
  
	//传送文件名
	res = send_circle(peerfd, (const char *)&truck, sizeof(truck));
    ERROR_CHECK(res, -1, "send filename");
    puts("file name send complete!\n");

	//传送文件内容
    struct stat st;
	fstat(rfd, &st);
    res = send_circle(peerfd, (const char*)&st.st_size, sizeof(off_t));
	ERROR_CHECK(res, -1, "send file_content");
    printf("file length:%ld",st.st_size);

    res = mmap_send_circle(peerfd , rfd,  st.st_size);
    printf("%s send complete!",filename);

    close(rfd);
}

void recv_file(int peerfd)
{
	int res;
    int fds[2];
    off_t length = 0;
    char buf[BUFSIZ] = {0};
	pipe(fds);

    dumpTruck_t truck;
	memset(&truck, 0, sizeof(truck));

	res = recv_circle(peerfd, (char *)&truck, sizeof(truck));
    ERROR_CHECK(res, -1, "recv filename");
    printf("file name:%s,name length:%d",truck.data,truck.length);

	//接收本地文件
	int wfd = open(truck.data, O_RDWR | O_CREAT | O_TRUNC, 0666);
	ERROR_CHECK(wfd, -1, "open");

	//接收文件内容
    printf("recv file data ");
    res = recv_circle(peerfd, (char*)&length, sizeof(off_t));
	ERROR_CHECK(res, -1, "recv file_content");
    printf("file length:%ld\n",length);
    
    int sendnum = 0;
    while(length > 0){
        if(length > BUFSIZ){
            sendnum = BUFSIZ;
        }else{
            sendnum = length;
        }
        recv_circle(peerfd,buf,sendnum);
        length -= sendnum;
        write(wfd,buf,sendnum);
    }
    puts("recv complete!\n");

    close(wfd);
}
