#include <stdio.h>
#include <string.h>
#include "todo.h"

static todo_t todos[MAX_TODOS];
static int todo_count;
static int next_id;

/**
 * add_todo - creates a new todo and appends it to the in-memory list
 * @title: the todo's title
 * @description: the todo's description
 *
 * Return: the id of the newly created todo, or -1 if the list is full
 */
int add_todo(const char *title, const char *description)
{
	if (todo_count >= MAX_TODOS)
		return (-1);

	todos[todo_count].id = next_id;
	strncpy(todos[todo_count].title, title, MAX_TITLE - 1);
	todos[todo_count].title[MAX_TITLE - 1] = '\0';
	strncpy(todos[todo_count].description, description, MAX_DESC - 1);
	todos[todo_count].description[MAX_DESC - 1] = '\0';

	todo_count++;
	next_id++;

	return (todos[todo_count - 1].id);
}

/**
 * find_todo - looks up a todo by id
 * @id: the id to search for
 *
 * Return: a pointer to the matching todo, or NULL if not found
 */
todo_t *find_todo(int id)
{
	int i;

	for (i = 0; i < todo_count; i++)
	{
		if (todos[i].id == id)
			return (&todos[i]);
	}
	return (NULL);
}

/**
 * delete_todo - removes a todo from the list by id
 * @id: the id of the todo to delete
 *
 * Return: 1 if the todo was deleted, 0 if no matching todo was found
 */
int delete_todo(int id)
{
	int i;
	int j;

	for (i = 0; i < todo_count; i++)
	{
		if (todos[i].id == id)
		{
			for (j = i; j < todo_count - 1; j++)
				todos[j] = todos[j + 1];
			todo_count--;
			return (1);
		}
	}
	return (0);
}

/**
 * todo_to_json - serializes a single todo into a JSON object string
 * @todo: the todo to serialize
 * @buf: destination buffer
 * @buf_size: size of the destination buffer
 */
void todo_to_json(const todo_t *todo, char *buf, size_t buf_size)
{
	snprintf(buf, buf_size,
		 "{\"id\":%d,\"title\":\"%s\",\"description\":\"%s\"}",
		 todo->id, todo->title, todo->description);
}

/**
 * todos_to_json - serializes the whole in-memory todo list into a
 * JSON array string
 * @buf: destination buffer
 * @buf_size: size of the destination buffer
 */
void todos_to_json(char *buf, size_t buf_size)
{
	char item[MAX_TITLE + MAX_DESC + 64];
	size_t offset;
	int i;

	offset = 0;
	buf[0] = '\0';
	strncat(buf, "[", buf_size - offset - 1);
	offset += 1;

	for (i = 0; i < todo_count; i++)
	{
		todo_to_json(&todos[i], item, sizeof(item));
		if (i > 0)
		{
			strncat(buf, ",", buf_size - offset - 1);
			offset += 1;
		}
		strncat(buf, item, buf_size - offset - 1);
		offset += strlen(item);
	}

	strncat(buf, "]", buf_size - offset - 1);
}
