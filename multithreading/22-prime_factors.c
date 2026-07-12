#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

/**
 * create_task - creates a task structure
 * @entry: pointer to the task entry function
 * @param: parameter to pass to the entry function
 * Return: pointer to the created task, or NULL on failure
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task;

	task = malloc(sizeof(*task));
	if (!task)
		return (NULL);
	memset(task, 0, sizeof(*task));
	task->entry = entry;
	task->param = param;
	task->status = PENDING;
	pthread_mutex_init(&task->lock, NULL);
	return (task);
}

/**
 * destroy_task - destroys a task and its result
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
 * exec_tasks - thread entry function that executes a list of tasks
 * @tasks: pointer to the list of tasks (list_t *)
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	size_t i = 0;

	if (!tasks)
		return (NULL);

	node = tasks->head;
	while (node && i < tasks->size)
	{
		task_t *task = (task_t *)node->content;

		/* lock before checking status to avoid race */
		pthread_mutex_lock(&task->lock);
		if (task->status == PENDING)
		{
			task->status = STARTED;
			pthread_mutex_unlock(&task->lock);

			tprintf("[%lu] [%02lu] Started\n", pthread_self(), i);

			task->result = task->entry(task->param);

			pthread_mutex_lock(&task->lock);
			if (task->result)
			{
				task->status = SUCCESS;
				tprintf("[%lu] [%02lu] Success\n", pthread_self(), i);
			}
			else
			{
				task->status = FAILURE;
				tprintf("[%lu] [%02lu] Failure\n", pthread_self(), i);
			}
			pthread_mutex_unlock(&task->lock);
		}
		else
		{
			pthread_mutex_unlock(&task->lock);
		}
		i++;
		node = node->next;
	}
	return (NULL);
}
