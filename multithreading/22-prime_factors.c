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
 * destroy_task - destroys a task and frees its memory
 * @task: pointer to the task to destroy
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;

	pthread_mutex_destroy(&task->lock);
	free(task);
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
	task_t *task;
	int index, pending;

	if (!tasks)
		return (NULL);

	do {
		pending = 0;
		index = 0;
		for (node = tasks->head; node; node = node->next, index++)
		{
			task = (task_t *)node->content;

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
				pending = 1;
			}
			else if (task->status == STARTED)
			{
				pthread_mutex_unlock(&task->lock);
				pending = 1;
				sched_yield();
			}
			else
				pthread_mutex_unlock(&task->lock);
		}
	} while (pending);

	return (NULL);
}
