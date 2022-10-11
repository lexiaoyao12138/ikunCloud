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

int command_rm(char *, char *)
{

}

int command_mkdir(char *)
{

}
