/*
 * threadpool.c - Source code for a simple thread pool.
 *
 * Author: Philip R. Simonson
 * Date  : 06/29/2021
 *
 ***************************************************************************
 *
 * Changes:
 *     - Redesigned 06/30/2021 - Now uses a linked list.
 *
 ***************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef __linux
#include <unistd.h>
#include <pthread.h>
#endif

#include "threadpool.h"

/* Task structure for the thread pool. */
typedef struct threadpool_task {
    thread_func_t func;
    void *arg;
    struct threadpool_task *next;
} threadpool_task_t;

/* Main structure for the thread pool. */
struct threadpool {
    threadpool_task_t *task_first;
    threadpool_task_t *task_last;
    pthread_mutex_t task_mutex;
    pthread_cond_t task_cond;
    pthread_cond_t tasking_cond;
    size_t tasking_count;
    size_t thread_count;
    bool stop;
};

/* ---------------------------- Private Functions ------------------------ */

/* Create a task.
 */
static threadpool_task_t *threadpool_task_create(thread_func_t func, void *arg)
{
    threadpool_task_t *task;

    if(func == NULL) return NULL;
    task = malloc(sizeof(threadpool_task_t));
    if(task != NULL) {
        task->func = func;
        task->arg = arg;
        task->next = NULL;
    }
    return task;
}
/* Destroy a task.
 */
static void threadpool_task_destroy(threadpool_task_t *task)
{
    if(task != NULL) {
        free(task);
    }
}
/* Get a task from the thread pool.
 */
static threadpool_task_t *threadpool_task_get(threadpool_t *tp)
{
    threadpool_task_t *task;

    if(tp == NULL) return NULL;
    task = tp->task_first;
    if(task == NULL) return NULL;

    if(task->next == NULL) {
        tp->task_first = NULL;
        tp->task_last = NULL;
    }
    else {
        tp->task_first = task->next;
    }
    return task;
}
/* Processes all tasks in the thread pool.
 */
static void *threadpool_worker(void *arg)
{
    threadpool_t *tp = (threadpool_t *)arg;
    threadpool_task_t *task;

    for(;;) {
        pthread_mutex_lock(&tp->task_mutex);

        while(tp->task_first == NULL && !tp->stop)
            pthread_cond_wait(&tp->task_cond, &tp->task_mutex);

        if(tp->stop)
            break;

        task = threadpool_task_get(tp);
        tp->tasking_count++;
        pthread_mutex_unlock(&tp->task_mutex);

        if(task != NULL) {
            task->func(task->arg);
            threadpool_task_destroy(task);
        }

        pthread_mutex_lock(&tp->task_mutex);
        tp->tasking_count--;
        if(!tp->stop && tp->tasking_count == 0 && tp->task_first == NULL)
            pthread_cond_signal(&tp->tasking_cond);
        pthread_mutex_unlock(&tp->task_mutex);
    }

    tp->thread_count--;
    pthread_cond_signal(&tp->tasking_cond);
    pthread_mutex_unlock(&tp->task_mutex);
    return NULL;
}

/* ----------------------------- Public Functions ------------------------ */

/* Create the thread pool.
 */
threadpool_t *threadpool_create(size_t num)
{
    threadpool_t *tp;
    pthread_t thread;
    size_t i;

    if(num == 0)
        num = 4;

    tp = calloc(1, sizeof(threadpool_t));
    if(tp != NULL) {
        tp->thread_count = num;
        pthread_mutex_init(&tp->task_mutex, NULL);
        pthread_cond_init(&tp->task_cond, NULL);
        pthread_cond_init(&tp->tasking_cond, NULL);
        tp->task_first = NULL;
        tp->task_last = NULL;

        for(i = 0; i < num; i++) {
            pthread_create(&thread, NULL, threadpool_worker, tp);
            pthread_detach(thread);
        }
    }
    return tp;
}
/* Destroy the thread pool.
 */
void threadpool_destroy(threadpool_t *tp)
{
    threadpool_task_t *task1, *task2;

    if(tp == NULL) return;

    pthread_mutex_lock(&tp->task_mutex);
    task1 = tp->task_first;
    while(task1 != NULL) {
        task2 = task1->next;
        threadpool_task_destroy(task1);
        task1 = task2;
    }
    tp->stop = true;
    pthread_cond_broadcast(&tp->task_cond);
    pthread_mutex_unlock(&tp->task_mutex);
    threadpool_wait(tp);
    pthread_mutex_destroy(&tp->task_mutex);
    pthread_cond_destroy(&tp->task_cond);
    pthread_cond_destroy(&tp->tasking_cond);
    free(tp);
}
/* Adding tasks to the thread pool.
 */
bool threadpool_add_task(threadpool_t *tp, thread_func_t func, void *arg)
{
    threadpool_task_t *task;

    if(tp == NULL) return false;

    task = threadpool_task_create(func, arg);
    if(task == NULL)
        return false;

    pthread_mutex_lock(&tp->task_mutex);
    if(tp->task_first == NULL) {
        tp->task_first = task;
        tp->task_last = tp->task_first;
    }
    else {
        tp->task_last->next = task;
        tp->task_last = task;
    }
    pthread_cond_broadcast(&tp->task_cond);
    pthread_mutex_unlock(&tp->task_mutex);
    return true;
}
/* Wait for processing to complete.
 */
void threadpool_wait(threadpool_t *tp)
{
    if(tp == NULL) return;

    pthread_mutex_lock(&tp->task_mutex);
    for(;;) {
        if((!tp->stop && tp->tasking_count != 0) || (tp->stop && tp->thread_count != 0)) {
            pthread_cond_wait(&tp->tasking_cond, &tp->task_mutex);
        }
        else {
            break;
        }
    }
    pthread_mutex_unlock(&tp->task_mutex);
}
