#include "system.h"

int command_cd(char *){
    
}

int command_ls(char *)
{
    
}

int command_pwd(char * cwd)
{  
    cwd = getcwd(NULL, 0);
    ERROR_CHECK(cwd, NULL, "get_current_dir_name");

    puts(cwd);

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
