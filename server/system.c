#include "pthread_pool.h"


int command_cd(char* arg,struct user_info* u_info){
    int ret = chdir(arg);
    ERROR_CHECK(ret,-1,"chdir");
    strcpy(u_info->path,arg);
    return 0;
}

int command_ls(char *path){
    //打开目录流
    DIR* pdir = opendir(path);
    ERROR_CHECK(pdir,NULL,"opendir");
    //读目录流
    struct dirent* pdirent;
    errno = 0;
    int length = 0;
    while((pdirent = readdir(pdir)) != NULL){
        char *name = pdirent -> d_name;
        if(name[0] == '.'){
            continue;
        }
        sprintf(path+length,"%s|",name);
        length += strlen(name)+1;
    }
    return 0;
}

int command_pwd(char * cwd)
{  
    // cwd = getcwd(NULL, 0);
		strcpy(cwd, getcwd(NULL, 0));
    ERROR_CHECK(cwd, NULL, "get_current_dir_name");
    return 0;
}

int command_rm(char *path)
{
    int res = 0;
    struct stat sta;
    char file_path[100] = {0};
    char *p_path;
    //判断当前输入的是绝对路径还是当前目录下的路径
    if(*path == '.'){
        //当前目录下的路径与当前工作目录拼接
        p_path = getcwd(file_path,sizeof(file_path));
        if(!p_path){
            perror("getcwd");
            return -1;
        }
        sprintf(file_path+strlen(file_path),"%s",path+1);
    }else if(*path == '/'){
        //绝对路径直接复制
        strcpy(file_path,path);
    }else{
        //其他路径打印错误信息
        puts("error path");
        return -1;
    }
    puts(file_path);
    //使用绝对路径获得文件类型
    res = stat(file_path,&sta);
    if(res == -1){
        perror("stat");
        return -1;
    }
    //目录使用rmdir删除
    if((sta.st_mode & S_IFMT) == S_IFDIR){
        res = rmdir(file_path);
        if(res == -1){
            perror("rmdir");
            return -1;
        }
    //文件使用unlink删除
    }else if((sta.st_mode & S_IFMT) == S_IFREG){
        res = unlink(file_path);
        if(res == -1){
            perror("unlink");
            return -1;
        }
    }
    return 0;
}

int command_mkdir(char *path)
{
    int ret = mkdir(path,0666);
    ERROR_CHECK(ret,-1,"mkdir");
    return 0 ;
}
