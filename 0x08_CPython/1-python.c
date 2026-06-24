#include <Python.h>
#include <stdio.h>

/**
 * print_python_list - Prints basic information about a Python list object
 * @p: A pointer to a PyObject (expected to be a Python list)
 */
void print_python_list(PyObject *p)
{
    Py_ssize_t size, i;
    PyListObject *list;

    /* Get the size of the Python list */
    size = PyList_Size(p);
    
    /* Cast the PyObject pointer to a PyListObject pointer to access its fields */
    list = (PyListObject *)p;

    printf("[*] Python list info\n");
    /* Cast Py_ssize_t to long int to avoid pedantic compiler warnings */
    printf("[*] Size of the Python List = %ld\n", (long int)size);
    printf("[*] Allocated = %ld\n", (long int)list->allocated);

    /* Iterate through the list elements and print their types */
    for (i = 0; i < size; i++)
    {
        printf("Element %ld: %s\n", (long int)i, Py_TYPE(list->ob_item[i])->tp_name);
    }
}
