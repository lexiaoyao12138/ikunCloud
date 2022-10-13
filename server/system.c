#include "system.h"


int command_cd(char* arg,struct user_info* u_info){
    int ret = chdir(arg);
    ERROR_CHECK(ret,-1,"chdir");
    strcpy(u_info->path,arg);
    return 0;
}

int command_ls(char *path){
    //打开目录流
		printf("comand_ls path: %s\n", path);
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

int command_rm(char *_dir_name, char *path)
{
    int ret = rmdir(path);
    ERROR_CHECK(ret,-1,"rmdir");
    return 0;
}

int command_mkdir(char *path)
{
    int ret = mkdir(path,0666);
    ERROR_CHECK(ret,-1,"mkdir");
    return 0 ;
}
