#include "mysql.h"

MYSQL * mysql_init()     // 数据库初始化
{
    MYSQL * mysql = NULL;    //初始化句柄
    char * host = "127.0.0.1";    //主机IP
    char * user = "root";
    char * passwd = "";
    char * db = "ikundb";

    mysql = mysql_init(NULL);
    if(mysql_real_connect(mysql, host, user, passwd, db, 0, NULL, 0) == NULL)
        printf("Error: %s\n", mysql_error(mysql));
    return mysql;
}

void add_file(MYSQL *mysql, char *parent_id, char *filename, int owner_id, char *md5, int filesize, int type) // 向虚拟文件表插入数据
{
    char data[256];
    sprintf(data, "insert into mysql values(id, '%s', '%s', %d, '%s', %d, %d);", parent_id, filename, owner_id, md5, filesize, type);
    ERROR_CHECK_MYSQL(mysql, mysql_query(mysql, data));
}

void add_user(MYSQL *mysql, char *username, char *salt, char *cryptpasswd, char *path)    // 向用户表插入数据
{

  MYSQL sql;
      mysql_init(&sql);
      MYSQL *conn = mysql_real_connect(&sql, "127.0.0.1", "root", "", "ikun", 3306, NULL, 0);

      if (mysql == NULL)
      {
          printf("Connect mysql fail\n");                              
          exit(1);
      }

    char data[256];
    sprintf(data, "insert into user values(id, '%s', '%s', '%s', '%s');", username, salt, cryptpasswd, path);
    ERROR_CHECK_MYSQL(conn, mysql_query(conn, data));
}

void drop_table(MYSQL *mysql, char *tablename)     // 删除表
{
    char dropfile[256];
    sprintf(dropfile, "drop table %s;", tablename);
    ERROR_CHECK_MYSQL(mysql, mysql_query(mysql, dropfile));
}

void create_file_table(MYSQL *mysql)  // 创建虚拟文件表
{ 
    //id作为主键
    char *query = "create table file(id int auto_increment primary key, parent_id varchar(256), filename varchar(30), owner_id int, md5 char(16), filesize int, type int);";
    ERROR_CHECK_MYSQL(mysql, mysql_query(mysql, query));
}

void create_user_table(MYSQL *mysql)  // 创建用户表
{
    //id作为主键
    char *query = "create table user(id int auto_increment primary key, username varchar(20), salt char(8), cryptpasswd varchar(20), path varchar(20));";
    ERROR_CHECK_MYSQL(mysql, mysql_query(mysql, query));
}

int check_md5(MYSQL *mysql, char *filename, char *MD5) // 验证文件的MD5是否已在数据库存在
{
    char * query_md5 = "select md5 from file where filename = '";
    char * node = "\'";
    query_md5 = strcat(query_md5, filename);
    query_md5 = strcat(query_md5, node);
    mysql_query(mysql, query_md5);
    MYSQL_RES * md5Res = mysql_store_result(mysql);
    MYSQL_ROW md5Row = mysql_fetch_row(md5Res);
    if(strcmp(md5Row[0], MD5) == 0)
    {
        printf("MD5 check success\n");
        mysql_free_result(md5Res);
        return 1;
    }
    printf("MD5 check failed\n");
    mysql_free_result(md5Res);
    return 0;
}
