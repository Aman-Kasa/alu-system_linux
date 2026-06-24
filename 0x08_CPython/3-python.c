#include <Python.h>
#include <stdio.h>

void print_python_bytes(PyObject *p)
{
    Py_ssize_t size, i;
    PyBytesObject *bytes = (PyBytesObject *)p;

    printf("[.] bytes object info\n");
    if (!PyBytes_Check(p))
    {
        printf("  [ERROR] Invalid Bytes Object\n");
        return;
    }

    size = Py_SIZE(p);
    printf("  size: %ld\n", size);
    printf("  trying string: %s\n", bytes->ob_sval);

    printf("  first %ld bytes:", (size + 1 > 10) ? 10 : size + 1);
    for (i = 0; i < size + 1 && i < 10; i++)
        printf(" %02hhx", (unsigned char)bytes->ob_sval[i]);
    printf("\n");
}

void print_python_float(PyObject *p)
{
    PyFloatObject *f = (PyFloatObject *)p;
    char *s = NULL;

    printf("[.] float object info\n");
    if (!PyFloat_Check(p))
    {
        printf("  [ERROR] Invalid Float Object\n");
        return;
    }

    s = PyOS_double_to_string(f->ob_fval, 'r', 0, Py_DTSF_ADD_DOT_0, NULL);
    printf("  value: %s\n", s);
    PyMem_Free(s);
}

void print_python_list(PyObject *p)
{
    Py_ssize_t i, size;
    PyListObject *list = (PyListObject *)p;

    printf("[*] Python list info\n");
    if (!PyList_Check(p))
    {
        printf("  [ERROR] Invalid List Object\n");
        return;
    }

    size = Py_SIZE(p);
    printf("[*] Size of the Python List = %ld\n", size);
    printf("[*] Allocated = %ld\n", list->allocated);

    for (i = 0; i < size; i++)
    {
        printf("Element %ld: %s\n", i, list->ob_item[i]->ob_type->tp_name);
        if (PyBytes_Check(list->ob_item[i]))
            print_python_bytes(list->ob_item[i]);
        else if (PyFloat_Check(list->ob_item[i]))
            print_python_float(list->ob_item[i]);
    }
}
