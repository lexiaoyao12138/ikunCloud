#include "mysql.h"

MYSQL *init_Msql()     // 数据库初始化
{
    MYSQL * Msql = NULL;    //初始化句柄
    char * host = "127.0.0.1";    //主机IP
    char * user = "root";
    char * passwd = "123lll";
    char * db = "Msql";

    Msql = mysql_init(NULL);
    if(mysql_real_connect(Msql, host, user, passwd, db, 0, NULL, 0) == NULL)
        printf("Error: %s\n", mysql_error(Msql));
    return Msql;
}


//向虚拟文件表中插入数据
//type 1 为目录文件，2 为普通文件
void add_files(MYSQL *Msql, int parent_id, char *filename, int owner_id, char *md5, int filesize, int type)
{
    char data[256];
    sprintf(data, "insert into file values(id, %d, '%s', %d, '%s', %d, %d);", parent_id, filename, owner_id, md5, filesize, type);
    ERROR_CHECK_MYSQL(Msql, mysql_query(Msql, data));
}

//加入用户
void add_user(MYSQL *Msql, char *username, char *salt, char *cryptpasswd, char *path)
{
    char data[256];
    sprintf(data, "insert into user values(id, '%s', '%s', '%s', '%s');", username, salt, cryptpasswd, path);
    puts(data);
    ERROR_CHECK_MYSQL(Msql, mysql_query(Msql, data));
}

void drop_table(MYSQL *Msql, char *tablename)     //删除表
{
    char dropfile[256];
    sprintf(dropfile, "drop table %s;", tablename);
    ERROR_CHECK_MYSQL(Msql, mysql_query(Msql, dropfile));
}

void create_file_table(MYSQL *Msql)  //创建虚拟文件表
{ 
    //id作为主键
    char *query = 
        "create table file(id int auto_increment primary key, parent_id varchar(256), filename varchar(30), owner_id int, md5 char(16), filesize int, type int);";
    ERROR_CHECK_MYSQL(Msql, mysql_query(Msql, query));
}

void create_user_table(MYSQL *Msql)  //创建用户表
{
    //id作为主键
    char *query = "create table user(id int auto_increment primary key, username varchar(20), salt char(8), cryptpasswd varchar(20), path varchar(20));";
    ERROR_CHECK_MYSQL(Msql, mysql_query(Msql, query));
}


int check_md5(MYSQL *Msql, char *filename, char *md5) //验证文件的MD5是否已在数据库存在
{
    char * query_md5 = "select md5 from file where filename = '";
    char * node = "\'";
    query_md5 = strcat(query_md5, filename);
    query_md5 = strcat(query_md5, node);
    mysql_query(Msql, query_md5);
    MYSQL_RES * md5Res = mysql_store_result(Msql);
    MYSQL_ROW md5Row = mysql_fetch_row(md5Res);
    if(strcmp(md5Row[0], md5) == 0)
    {
        printf("MD5 check success\n");
        mysql_free_result(md5Res);
        return 1;
    }
    printf("MD5 check failed\n");
    mysql_free_result(md5Res);
    return 0;
}


int main()
{
    MYSQL *Msql = init_Msql();
    //create_file_table(Msql);
    add_files(Msql, 0, "dir1", 1, "0", 0, 1);
    puts("111");
    add_user(Msql, "dogge", "qweasdfc", "hrefgsagfjjd", "../include");
    puts("222");

    return 0;
}
