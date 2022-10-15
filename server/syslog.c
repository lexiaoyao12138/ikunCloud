#include "../public/public.h"
#include <syslog.h>

// log client login and logout
// loginflag = 1 means client already login
void log_clnt(int client_fd, const char *clnt, int loginflag)
{
    // input to the client_fd
    // current time
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    char *ntime = asctime(timenow);
    ntime[strlen(ntime) - 1] = '\0';
    int ret = dprintf(client_fd, "%s", ntime);
    ERROR_CHECK(ret, -1, "dprintf");
    if(loginflag == 1)
    {
        // 成功登入
        dprintf(client_fd, "  client: %s login success\n", clnt);
    }
    else
    {
        // 成功登出
        dprintf(client_fd, "  client: %s logout success\n", clnt);
    }

    // sent messages to system logger
    openlog(clnt, LOG_CONS | LOG_PID, LOG_LOCAL0);
    if(loginflag == 1){
        syslog(LOG_INFO, "client: %s login success", clnt);
    }
    else
    {
        syslog(LOG_INFO, "client: %s exit success", clnt);
    }
    closelog();
}


// log executed command
// clnt is the name of client executing command
// cmd is what the client did
// path is where the client did
void log_cmd(int cmd_fd, const char *ip, int cmd, char *path)
{
    
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    char *ntime =asctime(timenow);
    ntime[strlen(ntime) - 1] = '\0';
    int ret = dprintf(cmd_fd, "%s", ntime);
    ERROR_CHECK(ret, -1, "dprintf");
    char *type;
    printf("%d\n", cmd);
    switch(cmd)
    {
    case 2:
        type = "cd";
        break;
    case 3:
        type = "ls";
        break;
    case 4:
        type = "pwd";
        break;
    case 5:
        type = "put file";
        break;
    case 6:
        type = "get file";
        break;
    case 7:
        type = "rm";
        break;
    case 8:
        type = "mkdir";
        break;
    default:
        return;
    }
    dprintf(cmd_fd, "  %s %s %s\n", ip, type, path);

    // sent messages to syslog
    openlog(ip, LOG_CONS | LOG_PID, LOG_LOCAL0);
    syslog(LOG_INFO, "%s(%s)", type, path);
    closelog();
}

