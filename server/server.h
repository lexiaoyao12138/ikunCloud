#include "../public/public.h"

typedef struct {
	char *host;  /*ip address*/
	int port;    /*server port*/
	int pthread_num;  /*open pthread number*/ 
} config_t;

config_t *init_config_t();
void get_config(const char *, config_t*);
