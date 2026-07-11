#include "multithreading.h"
#include "list.h"
#include <stdlib.h>

/**
 * create_task - Allocate and initialise a task
 * @entry: Task entry function
 * @param: Parameter to pass to the entry function
 *
 * Return: Pointer to new task, or NULL on failure
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task;

	task = malloc(sizeof(task_t));
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
 * destroy_task - Free a task and its mutex
 * @task: Task to destroy
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;
	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * exec_tasks - Thread entry: execute pending tasks from the list
 * @tasks: Pointer to the list of tasks
 *
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	size_t idx;
	int found;

	while (1)
	{
		found = 0;
		idx = 0;
		for (node = tasks->head; node; node = node->next, idx++)
		{
			task = (task_t *)node->content;

			pthread_mutex_lock(&task->lock);
			if (task->status == PENDING)
			{
				task->status = STARTED;
				pthread_mutex_unlock(&task->lock);

				tprintf("[%02lu] Started\n", idx);
				task->result = task->entry(task->param);
				task->status = SUCCESS;
				tprintf("[%02lu] Success\n", idx);

				found = 1;
				break;
			}
			pthread_mutex_unlock(&task->lock);
		}
		if (!found)
			break;
	}

	return (NULL);
}
