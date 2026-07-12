#include <pthread.h>
#include <stdlib.h>
#include "multithreading.h"

/**
 * exec_tasks - Executes all tasks in a list until all are completed
 * @tasks: Pointer to the list of tasks
 *
 * Return: NULL
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	int i, all_done;

	if (!tasks || !tasks->head)
		return (NULL);

	/* Loop until all tasks are marked as SUCCESS */
	while (1)
	{
		all_done = 1;
		i = 0;
		node = tasks->head;

		do {
			task = (task_t *)node->content;
			pthread_mutex_lock(&task->lock);

			if (task->status == PENDING)
			{
				task->status = STARTED;
				tprintf("[%02d] Started\n", i);
				task->result = task->entry(task->param);
				task->status = SUCCESS;
				tprintf("[%02d] Success\n", i);
				all_done = 0; /* Work was done, keep checking */
			}
			else if (task->status == STARTED)
			{
				all_done = 0; /* Someone else is working, keep checking */
			}

			pthread_mutex_unlock(&task->lock);
			node = node->next;
			i++;
		} while (node != tasks->head);

		/* If we went through all tasks and none were PENDING or STARTED, we are done */
		if (all_done)
			break;
	}

	return (NULL);
}
