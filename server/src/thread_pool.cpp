#include "../include/thread_pool.h"
#include "../include/pack_def.h"

//判断线程是否存活
int ThreadPool::if_thread_alive(pthread_t tid)
{
    if ((pthread_kill(tid, 0)) == -1)
    {
        if (errno == ESRCH)
            return false;
    }

    return true;
}

//创建线程,线程池里有一些线程和一个管理线程。线程回调函数分别是Custom和Manager
pool_t *ThreadPool::Pool_create(int max, int min, int que_max)
{
    pool_t *p;
    if ((p = (pool_t *)malloc(sizeof(pool_t))) == NULL)
    {
        perror("malloc pool error!");
    }
    p->thread_max = max;
    p->thread_min = min;
    p->thread_alive = 0;
    p->thread_busy = 0;
    p->thread_shutdown = true;
    p->thread_wait = 0;
    p->queue_max = que_max;
    p->queue_cur = 0;
    p->queue_front = 0;
    p->queue_rear = 0;
    if (pthread_cond_init(&p->not_full, NULL) != 0 ||
        pthread_cond_init(&p->not_empty, NULL) != 0 ||
        pthread_mutex_init(&p->lock, NULL) != 0)
    {
        perror("init cond or mutex error!");
    }
    if ((p->tids = (pthread_t *)malloc(sizeof(pthread_t) * max)) == NULL)
    {
        perror("malloc tids error!");
    }
    bzero(p->tids, sizeof(pthread_t) * max);
    if ((p->queue_task = (task_t *)malloc(sizeof(task_t) * que_max)) == NULL)
    {
        perror("malloc task queue error!");
    }
    int err;
    for (int i = 0; i < min; i++)
    {
        if ((err = pthread_create(&p->tids[i], NULL, Custom, (void *)p)) > 0)
        {
            printf("create custom error:%s\n", strerror(err));
            return NULL;
        }
        ++(p->thread_alive);
    }
    if ((err = pthread_create(&(p->manager_tid), NULL, Manager, (void *)p)) > 0)
    {
        printf("create custom error:%s\n", strerror(err));
        return NULL;
    }
    return p;
}

//生产者，锁加条件变量实现互斥
int ThreadPool::Producer_add(pool_t *p, void *(task)(void *arg), void *arg)
{
    pthread_mutex_lock(&p->lock);
    while (p->queue_cur == p->queue_max && p->thread_shutdown)
    {
        pthread_cond_wait(&p->not_full, &p->lock);
    }
    if (!p->thread_shutdown)
    {
        pthread_mutex_unlock(&p->lock);
        return -1;
    }
    p->queue_task[p->queue_front].task = task;
    p->queue_task[p->queue_front].arg = arg;
    p->queue_front = (p->queue_front + 1) % p->queue_max;
    ++(p->queue_cur);
    pthread_cond_signal(&p->not_empty);
    pthread_mutex_unlock(&p->lock);
    return 0;
}

//线程回调函数
void *ThreadPool::Custom(void *arg)
{
    pool_t *p = (pool_t *)arg;
    task_t task;
    while (p->thread_shutdown)
    {
        pthread_mutex_lock(&p->lock);
        while (p->queue_cur == 0 && p->thread_shutdown)
        {
            pthread_cond_wait(&p->not_empty, &p->lock);
        }
        if (!p->thread_shutdown)
        {
            pthread_mutex_unlock(&p->lock);
            pthread_exit(NULL);
        }
        if (p->thread_wait > 0 && p->thread_alive > p->thread_min)
        {
            --(p->thread_wait);
            --(p->thread_alive);
            pthread_mutex_unlock(&p->lock);
            pthread_exit(NULL);
        }
        //定义了函数指针，拿出一个任务,对应的任务在tcp_net中有，比如连接请求和分配任务请求。
        task.task = p->queue_task[p->queue_rear].task;
        task.arg = p->queue_task[p->queue_rear].arg;
        p->queue_rear = (p->queue_rear + 1) % p->queue_max;
        --(p->queue_cur);
        pthread_cond_signal(&p->not_full);
        ++(p->thread_busy);
        pthread_mutex_unlock(&p->lock);
        //执行核心工作
        (*task.task)(task.arg);
        pthread_mutex_lock(&p->lock);
        --(p->thread_busy);
        pthread_mutex_unlock(&p->lock);
    }
    return 0;
}

//管理者来判断是否需要创建非核心线程来工作。
void *ThreadPool::Manager(void *arg)
{
    pool_t *p = (pool_t *)arg;
    int alive;
    int cur;
    int busy;
    int add = 0;
    while (p->thread_shutdown)
    {
        pthread_mutex_lock(&p->lock);
        alive = p->thread_alive;
        busy = p->thread_busy;
        cur = p->queue_cur;
        pthread_mutex_unlock(&p->lock);
        if ((cur > alive - busy || (float)busy / alive * 100 >= (float)80) ||
            p->thread_max > alive)
        {
            for (int i = 0; i < (p->thread_max) && add < kAddCount; i++, add++)
            {
                pthread_mutex_lock(&p->lock);
                if (p->tids[i] == 0 || !if_thread_alive(p->tids[i]))
                {
                    pthread_create(&p->tids[i], NULL, Custom, (void *)p);
                    ++(p->thread_alive);
                }
                pthread_mutex_unlock(&p->lock);
            }
        }
        if (busy * 2 < alive - busy && alive > p->thread_min)
        {
            pthread_mutex_lock(&p->lock);
            p->thread_wait = kAddCount;
            pthread_mutex_unlock(&p->lock);
            for (int i = 0; i < kAddCount; i++)
            {
                pthread_cond_signal(&p->not_empty);
            }
        }
        sleep(kTimeOut);
    }
    return 0;
}
