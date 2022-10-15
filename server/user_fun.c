//编译时需连接动态库
//-lcrypt
#define _XOPEN_SOURCE //额外需要
#include "../public/public.h"

typedef struct {
	int length;
	char data[BUFSIZ];
}dumptruck_t;

int p_send(int fd,const char *msg);

int p_recv(int fd,char *msg);

//传入套接字，用户名及密码
int client_login(int clientfd,const char *name,const char *key){
    int res = 0;
    char salt[13] = {0};
    char control = '0';
    //char encode[512] = {0};
    
    //向客户端发送用户名

    res = p_send(clientfd,name);
    //接收客户端发来的盐值//或用户不存在

    res = recv_circle(clientfd,&control,1);
    if(res == -1){
        puts("timeout");
        return -1;
    }    
    //不存在该用户
    if(control == '0'){
        puts("Dont have this user");
        return -1;
    }
    puts(&control);
    //存在该用户，接收盐值
    res = p_recv(clientfd,salt);
    if(res == -1){
        puts("timeout");
        return -1;
    }    
    puts(salt);
   
    //使用接收的盐值将密码加密后传输给服务器
    char *p_encode = crypt(key,salt);
    puts(p_encode);
    //利用盐值将明文密码加密
    res = p_send(clientfd,p_encode);
    //接收服务器的验证结果
    res = recv_circle(clientfd,&control,1);
    if(res == -1){
        puts("timeout");
        return -1;
    }    
    //密文验证失败打印提示信息，返回-1
    if(control == '0')
        puts("Password error");
    //返回客户端登录结果
    return control == '1' ? 0 : -1;
}

//传入套接字
int server_login(int peerfd){
    int res = 0;
    char salt[512] = {0};
    char name[100] = {0};
    
    //接收用户名
    
    res = p_recv(peerfd,name);
    if(res == -1){
        puts("timeout");
        return -1;
    }    
    
    //获取用户名所对应的盐值及密文
    struct spwd *user_spwd = (struct spwd*)calloc(1,sizeof(struct spwd));
    //若无该用户则向客户端发送——“0”
    if((user_spwd = getspnam(name)) == NULL){
        res = send_circle(peerfd,"0",1);
        return -1;
    }
    res = send_circle(peerfd,"1",1);
    //有该用户，将control“1”及盐值发送至客服端
    int p_salt = 0;
    int num = 0;
    while(num < 3){
        salt[p_salt] = *(user_spwd->sp_pwdp + p_salt);
        if(salt[p_salt] == '$')
            num++;
        p_salt++;
    }
    puts(salt);
    p_send(peerfd,salt);
    //send_circle(peerfd,salt,strlen(salt));
    //接收盐值数据
    res = p_recv(peerfd,salt);
    if(res == -1){
        puts("timeout");
        return -1;
    }    
    puts(salt);
    //判断客户端密文与本地是否相同
    if(strcmp(salt,user_spwd->sp_pwdp) == 0){
        send_circle(peerfd,"1",1);
        return 0;//相同
    }else{
        send_circle(peerfd,"0",1);
        return -1;//不同
    }
}

//简易协议先发长度再发信息
int p_recv(int fd,char *msg){
    int length = 0;
    recv_circle(fd,(char*)&length,4);
    length = recv_circle(fd,msg,length);
    return length;
}

int p_send(int fd,const char *msg){
    int length = strlen(msg);
    send_circle(fd,(char*)&length,4);
    length = send_circle(fd,msg,length);
    return length;
}
