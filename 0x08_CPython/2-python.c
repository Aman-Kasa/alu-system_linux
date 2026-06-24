#include <Python.h>
#include <stdio.h>

/**
 * print_python_bytes - Prints basic information about Python bytes objects
 * @p: A pointer to a PyObject
 */
void print_python_bytes(PyObject *p)
{
    Py_ssize_t size, i;
    PyBytesObject *bytes;
    char *string;

    printf("[.] bytes object info\n");
    if (strcmp(p->ob_type->tp_name, "bytes") != 0)
    {
        printf("  [ERROR] Invalid Bytes Object\n");
        return;
    }

    /* Cast to PyVarObject to get size, and PyBytesObject to get string */
    size = ((PyVarObject *)p)->ob_size;
    bytes = (PyBytesObject *)p;
    string = bytes->ob_sval;

    printf("  size: %ld\n", size);
    printf("  trying string: %s\n", string);

    if (size < 10)
        printf("  first %ld bytes:", size + 1);
    else
        printf("  first 10 bytes:");

    for (i = 0; i <= size && i < 10; i++)
        printf(" %02hhx", string[i]);
    printf("\n");
}

/**
 * print_python_list - Prints basic information about Python lists
 * @p: A pointer to a PyObject
 */
void print_python_list(PyObject *p)
{
    Py_ssize_t size, i;
    PyListObject *list;

    printf("[*] Python list info\n");
    if (strcmp(p->ob_type->tp_name, "list") != 0)
    {
        printf("  [ERROR] Invalid List Object\n");
        return;
    }

    size = ((PyVarObject *)p)->ob_size;
    list = (PyListObject *)p;

    printf("[*] Size of the Python List = %ld\n", size);
    printf("[*] Allocated = %ld\n", list->allocated);

    for (i = 0; i < size; i++)
    {
        printf("Element %ld: %s\n", i, list->ob_item[i]->ob_type->tp_name);
        if (strcmp(list->ob_item[i]->ob_type->tp_name, "bytes") == 0)
            print_python_bytes(list->ob_item[i]);
    }
}
