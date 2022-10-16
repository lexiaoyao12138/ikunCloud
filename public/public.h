#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include "cjson/cJSON.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/sendfile.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <shadow.h>
#include <syslog.h>
#include <mysql/mysql.h>
#include <crypt.h>
#include <ctype.h>


#define CURRENT_PATH "../userDir"
#define CONFIG_PATH "../config/server.json"
#define IPLEN	20   /*ip的长度*/

#define ERROR_CHECK(ret, res, name)                                            \
  {                                                                            \
    if (ret == res) {                                                          \
      printf("%s:on line %d\n", __FILE__, __LINE__);                            \
      perror(name);                                                            \
      exit(0);                                                                 \
    }                                                                          \
  }

#define PTHREAD_CHECK(res, name)                                               \
  do {                                                                         \
    if (0 != res) {                                                            \
      printf("%s: %s\n", name, strerror(res));                                 \
    }                                                                          \
  } while (0);

typedef enum {
  COMMAND_CD = 2,
  COMMAND_LS = 3,
  COMMAND_PWD = 4,
  COMMAND_PUT = 5,
  COMMAND_GET = 6,
  COMMAND_RM = 7,
  COMMAND_MKDIR = 8,
} command_type;


int send_circle(int, const char*, int);
int recv_circle(int, char*, int);
void send_file(int, const char*);
void recv_file(int);
int server_login(int );
int client_login(int ,const char*,const char*);

#endif
