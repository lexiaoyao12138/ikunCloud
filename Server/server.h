#include "../public/public.h"

typedef struct {
  char host[20];      /*ip   address*/
  int port;        /*server   port*/
  int pthread_num; /*open   pthread   number*/
  int epoll_num;   /*epoll    event   number*/
  char db_host[20];   /*database host*/
  int db_port;     /*databse port*/
  char db_user[20];   /*databaes username*/
  char db_passwd[20];    /*database password*/
  char db_name[20];   /*datebase name*/

} config_t;

config_t *init_config_t();
void get_config(const char *, config_t *);

void mysql_con(config_t *config);
