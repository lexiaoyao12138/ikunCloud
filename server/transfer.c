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
        }else if(ret == 0){
            puts("recv close!");
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
        }else if(ret == 0){
            puts("send close!");
            return -1;
        }
		pbuf += ret;
		left -= ret;
	}
	return length - left;
}

int mmap_send_circle(int fd, int rfd,off_t length,off_t have_done)
{
    int max_mmap = 536870912;
    int mmap_size = 0;
	int ret = 0;
    off_t done = have_done;//已完成传输
	off_t left = length - done;

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
                if(ret > 0)
                    puts("resend!\n");
                else
                    return -1;
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

	sleep(1);
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
    puts("file name send complete!");

	//传送文件内容
    struct stat st;
	fstat(rfd, &st);
    res = send_circle(peerfd, (const char*)&st.st_size, sizeof(off_t));
	ERROR_CHECK(res, -1, "send file_content");
    printf("file length:%ld\n",st.st_size);
    //获得客户端已接收的大小
    off_t have_done = -1;

    while(have_done == -1){
        res = recv_circle(peerfd,(char*)&have_done,sizeof(off_t));
        printf("client have recv:%ld\n",have_done);
    }
    //发送客户端未接收的部分
    res = mmap_send_circle(peerfd , rfd,  st.st_size ,have_done);
    if(res == -1){
        close(rfd);
        return;
    }
    printf("%s send complete!\n",filename);

    close(rfd);
}

void recv_file(int peerfd)
{

	printf("进入recv file\n");
	int res;
    int fds[2];
    off_t length = 0;
    char buf[BUFSIZ] = {0};
	pipe(fds);

    dumpTruck_t truck;
	memset(&truck, 0, sizeof(truck));

	res = recv_circle(peerfd, (char *)&truck, sizeof(truck));
    ERROR_CHECK(res, -1, "recv filename");
    printf("file name:%s,name length:%d\n",truck.data,truck.length);

	//接收本地文件
	int wfd = open(truck.data, O_RDWR | O_CREAT | O_APPEND, -1);
	ERROR_CHECK(wfd, -1, "open");

    //发送文件已接收大小
    struct stat st;
	fstat(wfd, &st);
    res = send_circle(peerfd, (const char*)&st.st_size, sizeof(off_t));
	ERROR_CHECK(res, -1, "send file_content");
    printf("file recved length:%ld\n",st.st_size);

	//接收文件内容
    printf("recv file data \n");
    res = recv_circle(peerfd, (char*)&length, sizeof(off_t));
	ERROR_CHECK(res, -1, "recv file_content");
    printf("file length:%ld\n",length);
    
    off_t recv_length = length - st.st_size;
    off_t recvnum = 0;

    while(recv_length > 0){
        if(recv_length > BUFSIZ){
            recvnum = BUFSIZ;
        }else{
            recvnum = recv_length;
        }
        res = recv_circle(peerfd,buf,recvnum);
        if(res == -1){
            close(wfd);
            return;
        }
        recv_length -= recvnum;
        write(wfd,buf,recvnum);
    }
    puts("recv complete!");

    close(wfd);
}
