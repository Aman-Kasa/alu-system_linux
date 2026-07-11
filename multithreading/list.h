#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct node_s
{
    void *content;
    struct node_s *prev;
    struct node_s *next;
} node_t;

typedef struct list_s
{
    node_t *head;
    node_t *tail;
    size_t size;
} list_t;

/* ADD THIS LINE HERE */
typedef void (*node_func_t)(void *);

/* Your list function prototypes here... */

#endif /* LIST_H */
