#include <stdlib.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * create_task - Creates a new task
 * @entry: Pointer to the entry function
 * @param: Parameter for the entry function
 *
 * Return: Pointer to the created task
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task = malloc(sizeof(task_t));

	if (!task)
		return (NULL);

	task->entry = entry;
	task->param = param;
	task->status = PENDING;
	task->result = NULL;
	pthread_mutex_init(&task->lock, NULL);
	return (task);
}

/**
 * destroy_task - Destroys a task
 * @task: Pointer to the task to destroy
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;

	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * exec_tasks - Thread entry to execute a list of tasks
 * @tasks: List of tasks to be executed
 *
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	int i = 0;

	if (!tasks)
		return (NULL);

	for (node = tasks->head; node != NULL; node = node->next)
	{
		task = (task_t *)node->content;
		pthread_mutex_lock(&task->lock);
		if (task->status == PENDING)
		{
			task->status = STARTED;
			tprintf("[%02d] Started\n", i);
			task->result = task->entry(task->param);
			task->status = SUCCESS;
			tprintf("[%02d] Success\n", i);
		}
		pthread_mutex_unlock(&task->lock);
		i++;
	}
	return (NULL);
}
