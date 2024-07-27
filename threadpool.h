/*
 * threadpool.h - Header for a simple thread pool implementation.
 *
 * Author: Philip R. Simonson
 * Date  : 06/29/2021
 *
 ****************************************************************************
 *
 * Changes:
 *    - Redesigned 06/30/2021 - Now uses a linked list.
 *
 ****************************************************************************
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdbool.h>
#include <stddef.h>

struct threadpool;
typedef struct threadpool threadpool_t;

/* Thread pool task function typedef. */
typedef void (*thread_func_t)(void *arg);

/* Create a thread pool. */
threadpool_t *threadpool_create(size_t num);
/* Destroy the thread pool. */
void threadpool_destroy(threadpool_t *tp);

/* Add a task to the thread pool. */
bool threadpool_add_task(threadpool_t *tp, thread_func_t func, void *arg);
/* Wait for all tasks to finish. */
void threadpool_wait(threadpool_t *tp);

#endif
