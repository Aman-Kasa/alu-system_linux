#include <Python.h>
#include <stdio.h>

/**
 * print_python_bytes - Prints info about bytes objects
 */
void print_python_bytes(PyObject *p)
{
    PyBytesObject *bytes = (PyBytesObject *)p;
    Py_ssize_t size, i;

    printf("[.] bytes object info\n");
    if (p->ob_type != &PyBytes_Type)
    {
        printf("  [ERROR] Invalid Bytes Object\n");
        return;
    }

    size = ((PyVarObject *)p)->ob_size;
    printf("  size: %ld\n", size);
    printf("  trying string: %s\n", bytes->ob_sval);

    printf("  first %ld bytes:", (size + 1 > 10) ? 10 : size + 1);
    for (i = 0; i < size + 1 && i < 10; i++)
        printf(" %02hhx", (unsigned char)bytes->ob_sval[i]);
    printf("\n");
}

/**
 * print_python_float - Prints info about float objects
 */
void print_python_float(PyObject *p)
{
    PyFloatObject *f = (PyFloatObject *)p;
    char *s = NULL;

    printf("[.] float object info\n");
    if (p->ob_type != &PyFloat_Type)
    {
        printf("  [ERROR] Invalid Float Object\n");
        return;
    }

    s = PyOS_double_to_string(f->ob_fval, 'r', 0, Py_DTSF_ADD_DOT_0, NULL);
    printf("  value: %s\n", s);
    PyMem_Free(s);
}

/**
 * print_python_list - Prints info about list objects
 */
void print_python_list(PyObject *p)
{
    PyListObject *list = (PyListObject *)p;
    Py_ssize_t i, size;

    printf("[*] Python list info\n");
    if (p->ob_type != &PyList_Type)
    {
        printf("  [ERROR] Invalid List Object\n");
        return;
    }

    size = ((PyVarObject *)p)->ob_size;
    printf("[*] Size of the Python List = %ld\n", size);
    printf("[*] Allocated = %ld\n", list->allocated);

    for (i = 0; i < size; i++)
    {
        printf("Element %ld: %s\n", i, list->ob_item[i]->ob_type->tp_name);
        if (list->ob_item[i]->ob_type == &PyBytes_Type)
            print_python_bytes(list->ob_item[i]);
        if (list->ob_item[i]->ob_type == &PyFloat_Type)
            print_python_float(list->ob_item[i]);
    }
}
