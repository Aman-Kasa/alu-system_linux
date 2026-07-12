#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include "multithreading.h"

/**
 * create_task - allocates and initializes a new task
 * @entry: function pointer serving as the task's entry point
 * @param: parameter to pass to the entry function
 *
 * Return: pointer to the newly created task, or NULL on failure
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task;

	task = malloc(sizeof(*task));
	if (!task)
		return (NULL);

	task->entry = entry;
	task->param = param;
	task->status = PENDING;
	task->result = NULL;

	if (pthread_mutex_init(&task->lock, NULL) != 0)
	{
		free(task);
		return (NULL);
	}
	return (task);
}

/**
 * destroy_task - destroys a task and frees its memory, including
 * any result list left behind by the task's entry function
 * @task: pointer to the task to destroy
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;

	if (task->result)
	{
		list_destroy((list_t *)task->result, free);
		free(task->result);
	}

	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * try_run_task - attempts to claim and run a single task
 * @task: task to attempt
 * @index: index of the task, used for logging
 *
 * Return: 1 if this task is still in progress/was just run, 0 if it
 * was already finished before this call
 */
static int try_run_task(task_t *task, int index)
{
	pthread_mutex_lock(&task->lock);
	if (task->status == PENDING)
	{
		task->status = STARTED;
		pthread_mutex_unlock(&task->lock);

		tprintf("[%02d] Started\n", index);
		task->result = task->entry(task->param);
		tprintf("[%02d] Success\n", index);

		pthread_mutex_lock(&task->lock);
		task->status = SUCCESS;
		pthread_mutex_unlock(&task->lock);
		return (1);
	}
	if (task->status == STARTED)
	{
		pthread_mutex_unlock(&task->lock);
		sched_yield();
		return (1);
	}
	pthread_mutex_unlock(&task->lock);
	return (0);
}

/**
 * exec_tasks - thread entry point; executes pending tasks from a list
 * @tasks: pointer to the list of tasks to execute
 *
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	int index, pending;

	if (!tasks)
		return (NULL);

	do {
		pending = 0;
		index = 0;
		for (node = tasks->head; node; node = node->next, index++)
			pending |= try_run_task((task_t *)node->content, index);
	} while (pending);

	return (NULL);
}
