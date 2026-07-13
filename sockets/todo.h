#ifndef TODO_H
#define TODO_H

#define MAX_TODOS 1024
#define MAX_TITLE 256
#define MAX_DESC 1024

/**
 * struct todo - a single TODO item
 * @id: unique positive integer identifier, starting from 0
 * @title: the todo's title
 * @description: the todo's description
 */
typedef struct todo
{
	int id;
	char title[MAX_TITLE];
	char description[MAX_DESC];
} todo_t;

int add_todo(const char *title, const char *description);
todo_t *find_todo(int id);
int delete_todo(int id);
void todo_to_json(const todo_t *todo, char *buf, size_t buf_size);
void todos_to_json(char *buf, size_t buf_size);

#endif /* TODO_H */
