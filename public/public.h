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

#define COMMAND_CD 2
#define COMMAND_LS 3
#define COMMAND_PWD 4
#define COMMAND_PUT 5
#define COMMAND_GET 6
#define COMMAND_RM 7
#define COMMAND_MKDIR 8

#define ERROR_CHECK(ret, res, name)                                            \
  {                                                                            \
    if (ret == res) {                                                          \
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
