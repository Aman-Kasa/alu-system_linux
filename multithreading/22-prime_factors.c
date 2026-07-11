#include "multithreading.h"
#include <stdlib.h>
#include <pthread.h>

/**
 * create_task - Creates a new task structure
 * @entry: Pointer to the entry function of the task
 * @param: Parameter to be passed to the entry function
 *
 * Return: Pointer to the created task, or NULL on failure
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
	
	/* Initialize the mutex lock for this specific task */
	pthread_mutex_init(&task->lock, NULL);

	return (task);
}

/**
 * destroy_task - Destroys a task and frees its resources
 * @task: Pointer to the task to destroy
 */
void destroy_task(task_t *task)
{
	if (!task)
		return;

	/* Destroy the mutex to free OS resources */
	pthread_mutex_destroy(&task->lock);
	
	/* If a result was generated, free the list and its pointer */
	if (task->result)
	{
		list_destroy(task->result, free);
		free(task->result);
	}
	
	free(task);
}

/**
 * exec_tasks - Thread entry point to execute a list of tasks
 * @tasks: Pointer to the list of tasks to execute
 *
 * Return: Always NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	int i = 0;

	if (!tasks)
		return (NULL);

	node = tasks->head;
	while (node)
	{
		task = (task_t *)node->content;
		
		/* Lock before checking/modifying status to prevent race conditions */
		pthread_mutex_lock(&task->lock);
		if (task->status == PENDING)
		{
			task->status = STARTED;
			tprintf("[%02d] Started\n", i);
			
			/* Unlock so other threads can traverse past this node */
			pthread_mutex_unlock(&task->lock);

			/* Execute heavy computation outside the lock */
			task->result = task->entry(task->param);

			/* Re-lock to safely update the final status */
			pthread_mutex_lock(&task->lock);
			task->status = task->result ? SUCCESS : FAILURE;
			if (task->status == SUCCESS)
				tprintf("[%02d] Success\n", i);
			else
				tprintf("[%02d] Failure\n", i);
		}
		pthread_mutex_unlock(&task->lock);

		node = node->next;
		i++;
	}

	return (NULL);
}
