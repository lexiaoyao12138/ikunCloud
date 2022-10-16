#include "pthread_pool.h"

#define now_path (path+strlen(path))

int command_cd(char* arg,struct user_info* u_info){
    int ret = chdir(arg);
    ERROR_CHECK(ret,-1,"chdir");
    strcpy(u_info->path,arg);
    return 0;
}

int command_ls(char *path){
    struct dirent* pdirent;
    struct stat sta;

    char buf[100] = {0};
    char file_path[100] = {0};
    strcpy(file_path,path);
    int p_path = strlen(file_path);

    int res = 0;
    errno = 0;
    int length = 0;
    //打开目录流
    DIR* pdir = opendir(path);
    ERROR_CHECK(pdir,NULL,"opendir");
    bzero(path,strlen(path));
    //读目录流
    while((pdirent = readdir(pdir)) != NULL){
        char *name = pdirent -> d_name;
        if(name[0] == '.'){
            continue;
        }
        sprintf(file_path+p_path,"/%s",name);
        res = stat(file_path,&sta);
        ERROR_CHECK(res,-1,"stat");
        
        switch(sta.st_mode & S_IFMT){
        case S_IFBLK :sprintf(now_path,"b");break;
        case S_IFCHR :sprintf(now_path,"c");break;
        case S_IFDIR :sprintf(now_path,"d");break;
        case S_IFIFO :sprintf(now_path,"p");break;
        case S_IFLNK :sprintf(now_path,"l");break;
        case S_IFREG :sprintf(now_path,"-");break;
        case S_IFSOCK:sprintf(now_path,"s");break;
        default      :sprintf(now_path,"?");break;
        }
        
        for(int i = 0; i < 3; ++i){
            for(int j = 0;j < 3; ++j){
                if(sta.st_mode >> ((2 - i) * 3 + (2 - j))){
                    switch(j){
                    case 0:sprintf(now_path,"r");break;
                    case 1:sprintf(now_path,"w");break;
                    case 2:sprintf(now_path,"x");break;
                    }
                }else{
                    sprintf(now_path,"-");
                }
            }
        }

        struct passwd* pwd = getpwuid(sta.st_uid);
        struct group*  grp = getgrgid(sta.st_gid);


        sprintf(now_path," %.3ld %s\t%s\t%ld",
                           sta.st_nlink,pwd->pw_name,
                           grp->gr_name,sta.st_size);

        char time[1024] = {0};
        strcpy(time,ctime(&sta.st_mtime));
        int i = 0;
        time[strlen(time) - 1] = '\0';
        
        sprintf(now_path,"\t%s %s\n",time,name);
    }

    closedir(pdir);
    return 0;
}

//传入前将path清空,其需要的空间较大，BUFSIZ可能不足以容纳
int  command_tree(const char* dirname,char *path, int indent)
{
    DIR *pdir = opendir(dirname);
    struct dirent *pd;
    struct stat sbuf;
    chdir(dirname);

    while((pd = readdir(pdir)) != NULL)
    {
        lstat(pd->d_name, &sbuf);

        if(strcmp(pd->d_name, ".") == 0 || 
                strcmp(pd->d_name, "..") == 0)
        {
            continue;
        }
        int depth = indent;
        while(depth--)
            sprintf(now_path,"| _");
        sprintf(now_path,"%s\n", pd->d_name);   
        depth = indent;
        if(S_ISDIR(sbuf.st_mode))
        {
            command_tree(pd->d_name,path, depth+2);
            sprintf(now_path,"|\n");
        }

    }
    chdir("..");
    closedir(pdir);
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
	char first = path[0];
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
