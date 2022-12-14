#include "server.h"
#include <cjson/cJSON.h>

// 结构体初始化
config_t *init_config_t() {
	config_t *config = (config_t *)calloc(1, sizeof(config_t));
	config->host = (char *)malloc(sizeof(char));
	return config;
}

// 获取ip和端口
void get_config(const char *path, config_t *config) {
	int res;
	char buf[BUFSIZ];
  // config_t *config = (config_t *)calloc(1, sizeof(config_t));
  int config_fd = open(path, O_RDONLY);
	ERROR_CHECK(config_fd, -1, "open");

	bzero(buf, sizeof(buf));
  res = read(config_fd, buf, sizeof(buf));
  ERROR_CHECK(res, -1, "read");

  cJSON *json = cJSON_Parse(buf);
	ERROR_CHECK(json, NULL, "cJSON_Parse");

  cJSON *chost = cJSON_GetObjectItem(json, "host");
	strcpy(config->host, chost->valuestring);
  printf("host: %s\n", config->host);

  cJSON *cport = cJSON_GetObjectItem(json, "port");
	config->port = cport->valueint;
  printf("port: %d\n", cport->valueint);

	cJSON *cphthread_num = cJSON_GetObjectItem(json, "pthreadNum");
	config->pthread_num = cphthread_num->valueint;
	printf("phthread_num: %d\n", config->pthread_num);
	
	cJSON *cepoll_num = cJSON_GetObjectItem(json, "epoll_num");
	config->epoll_num = cphthread_num->valueint;
	printf("epoll_num: %d\n", config->epoll_num);

  cJSON_Delete(json);
}
