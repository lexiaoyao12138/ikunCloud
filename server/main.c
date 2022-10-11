#include "../public/public.h"
#include "server.h"

int main() {
	int res;          /*return values*/ 
	char buf[BUFSIZ];
	config_t *config;

	// 读取配置文件 
	char *path = CONFIG_PATH;
	config = init_config_t();
	get_config(path, config);


	return 0;
}
