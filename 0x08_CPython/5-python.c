#include <Python.h>
#include <stdio.h>

/**
 * print_python_int - Prints a Python integer mapped to a C unsigned long
 * @p: Pointer to a PyObject
 */
void print_python_int(PyObject *p)
{
    PyLongObject *long_obj;
    Py_ssize_t size, i;
    unsigned long int value = 0;
    int is_negative = 0;

    if (!PyLong_Check(p))
    {
        printf("Invalid Int Object\n");
        return;
    }

    long_obj = (PyLongObject *)p;
    size = ((PyVarObject *)p)->ob_size;

    if (size < 0)
    {
        is_negative = 1;
        size = -size;
    }

    /* A 64-bit unsigned long can hold up to 3 digits in base 2^30.
       If size > 3, or if it's 3 and too large, it overflows. */
    if (size > 3 || (size == 3 && long_obj->ob_digit[2] > 15))
    {
        printf("C unsigned long int overflow\n");
        return;
    }

    for (i = 0; i < size; i++)
    {
        value += (unsigned long int)long_obj->ob_digit[i] * (1UL << (PyLong_SHIFT * i));
    }

    if (is_negative)
        printf("-%lu\n", value);
    else
        printf("%lu\n", value);
}
