#include "../public/public.h"
#include "task_queue.h"
#include "../server/transfer.c"

//int commd_analyse(char* cmd_in,struct tast_s*  ){
//    char cmd_mode[5] = {0};
//    char full_cmd_mode[13] ="COMMAND_";
//    char *pcmd = cmd_in;
//    char *pmode = cmd_mode;
//    while(*(pcmd++) != ' '){
//        *(pmode++) = *pcmd;
//    }
//    strcpy(full_cmd_mode+8,cmd_mode);
//
//    command_type cmd =  COMMAND_CD;
//    switch()
//    if()
//
//}

int main( ) {
  // 1. 创建监听套接字
  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  ERROR_CHECK(clientfd, -1, "socket");

  struct sockaddr_in serveraddr;
  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(8888);
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


 //2.1 直接连接服务器
  int ret = connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  ERROR_CHECK(ret, -1, "connect");
  //当connet正常返回时，链接已经建立成功了
 
  fd_set rdset;

  //事件循环
  while(1){
     FD_SET(STDIN_FILENO, &rdset);
     FD_SET(clientfd, &rdset);
     //clientfd 3
     
     //监听标准输入 0
     
     //当select函数返回时，rdset的集合已经被重新写入了
     //也是一个传出参数，写入的是已经发生了就绪事件的
     //文件描述符
     select(clientfd + 1, &rdset, NULL, NULL, NULL);

     char buff[BUFSIZ] = {0};
     
     //接下来要做的就是判断rdset中，有没有相应的fd
     if(FD_ISSET(STDIN_FILENO,&rdset)){
         //1.第一种情况
         //获取从键盘输入的数据，换行符'\n'也会发过去
         read(STDIN_FILENO, buff, BUFSIZ);
         //发送给服务器
         send_circle(clientfd, buff, strlen(buff));
         //当ret为0时，表示该连接已经断开
         // if(ret == 0){
         //     break;
         // }
     } else if (FD_ISSET(clientfd, &rdset)) {
         //接收本次服务器的回应类型，文件/命令
         dumpTruck_t truck;
         recv_circle(clientfd,(char*)&truck,sizeof(truck)); 
         char *client_mode[2] = {"file","cmd"};
         //接收文件
         if(strcmp(truck.data,client_mode[0]) == 0){
            recv_file(clientfd);
         }
         //接收命令的返回数据
         else if(strcmp(truck.data,client_mode[1]) == 0){
            do{
                recv_circle(clientfd,(char*)&truck,BUFSIZ);
            }while(strlen(truck.data) < truck.length);
            puts(truck.data);
         }

     }
  }


  //5.关闭套接字
  close(clientfd);
}
