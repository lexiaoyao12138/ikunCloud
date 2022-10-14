#ifndef __mysql_H__
#define __mysql_H__

#include "../public/public.h"

MYSQL *mysql_init_();    //数据库初始化

void ERROR_CHECK_MYSQL(MYSQL *mysql, int ret)       // 验证操作是否出错
{
    if(ret != 0)
    {
        printf("Error query: %s\n", mysql_error(mysql));
        exit(1);
    }
}

void add_file(MYSQL *mysql, char *parent_id, char *filename, int owner_id, char *md5, int filesize, int type); // 向虚拟文件表插入数据

void add_user(MYSQL *mysql, char *username, char *salt, char *cryptpasswd, char *path);    // 向用户表插入数据

void drop_table(MYSQL *mysql, char *tablename);     // 删除表

void create_file_table(MYSQL *mysql);  // 创建虚拟文件表

void create_user_table(MYSQL *mysql);  // 创建用户表

int check_md5(MYSQL *mysql, char *filename, char *MD5); // 验证文件的MD5是否已在数据库存在)




#endif
