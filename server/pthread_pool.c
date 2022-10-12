#include "pthread_pool.h"
#include "system.h"

void threadpool_init(thread_pool_t * pthreadpool, int pthreadNum)
{
    if(pthreadpool)
    {
        pthreadpool->threadNumber = pthreadNum;
        pthreadpool->thread = (pthread_t *)calloc(pthreadNum, sizeof(pthread_t));
        queue_init(&pthreadpool->queue);
    }
}
 
void threadPool_destroy(thread_pool_t * pthreadpool)
{
    if(pthreadpool)
    {
        free(pthreadpool->thread);
        queue_destroy(&pthreadpool->queue);
    }

}

void threadPool_start(thread_pool_t * pthreadpool)
{
    for(int i = 0; i < pthreadpool->threadNumber; ++i)
    {
        int ret = pthread_create(&pthreadpool->thread[i], NULL, thread_func, pthreadpool);
        PTHREAD_CHECK(ret, "pthread_create");
    }

}

void threadPool_stop(thread_pool_t * pthreadpool)
{
    while(!queue_isempty(&pthreadpool->queue))
    {
        sleep(1);
    }
    queue_wakeup(&pthreadpool->queue);
    for(int i = 0; i < pthreadpool->threadNumber; ++i)
    {
        pthread_join(pthreadpool->thread[i], NULL);    
    }
}

void *thread_func(void * arg)
{
    int peerfd;
    while(1)
    {
        thread_pool_t * pthreadpool = (thread_pool_t *)arg;
        //互斥锁
        pthread_mutex_lock(&pthreadpool->queue.mutex);
        //线程清理函数
        pthread_cleanup_push(clean_func, (void *)pthreadpool);
        while(queue_isempty(&pthreadpool->queue))
        {
            //等待解锁
            pthread_cond_wait(&pthreadpool->queue.cond, &pthreadpool->queue.mutex);
            if(pthreadpool->queue.size == 0)
            {
                
            }
        }
        printf("Tasks...\n");
        peerfd = pthreadpool->queue.pFront->peerfd;
        pthread_mutex_unlock(&pthreadpool->queue.mutex);
        //任务出队
        task_dequeue(&pthreadpool->queue);
        //对应push, 线程pop
        pthread_cleanup_pop(1);
        handle_event(peerfd, pthreadpool);
        printf("Task done\n");
    }
}

void handle_command_cd();

void handle_command_pwd();

// void handle_command_rm();

// void handle_command_mkdir();

void handle_command_ls(int peerfd);

void handle_event(int peerfd, thread_pool_t * pthreadpool)
{
    char buf[BUFSIZ];
    bzero(buf, sizeof(buf));
    int res = read(peerfd, buf, sizeof(buf));
    ERROR_CHECK(res, -1, "read");
    int cmd_type = atoi(buf);

		puts("start handle client");
    switch(cmd_type)
    {
    case COMMAND_CD:
       // handle_command_cd();
        printf("cd\n");
        break;
    case 3:
			handle_command_ls(peerfd);
			break;
    // case COMMAND_PWD:
    //     handle_command_pwd();
    //     break;
    // case COMMAND_PUT:
    // case COMMAND_GET:
    // case COMMAND_RM:
    //     handle_command_rm();
    //     break;
    // case COMMAND_MKDIR:
    //     handle_command_mkdir();
    //     break;
            
    }

}

void handle_command_ls(int peerfd) {
	char path[BUFSIZ] = "./";
	command_ls(path);
	puts(path);
	int res = write(peerfd, path, strlen(path));
	ERROR_CHECK(res, -1, "write");
}

void handle_command_cd()
{
    struct user_info  user_msg;
    command_cd(CURRENT_PATH, &user_msg);
}

void handle_command_pwd()
{
    char *cwd;
    command_pwd(cwd);
}

void handle_command_rm(char *filename)
{
    command_rm(filename, CURRENT_PATH);
}

void handle_command_mkdir(char *path)
{
    command_mkdir(path);
}

void clean_func(void *parg)
{
    printf("clean\n");
    thread_pool_t * pthreadpool = (thread_pool_t *)parg;
    pthread_mutex_unlock(&pthreadpool->queue.mutex);
}
