#include <Python.h>
#include <stdio.h>

/**
 * print_python_int - Prints ANY Python integer (handles arbitrary precision)
 * @p: Pointer to a PyObject
 */
void print_python_int(PyObject *p)
{
    PyObject *str_obj;
    const char *str_val;

    if (!PyLong_Check(p))
    {
        printf("Invalid Int Object\n");
        return;
    }

    /* Convert the long object to a string object using Python's internal logic */
    str_obj = PyObject_Str(p);
    if (str_obj == NULL)
        return;

    /* Get the UTF-8 string representation */
    str_val = PyUnicode_AsUTF8(str_obj);
    if (str_val != NULL)
        printf("%s\n", str_val);

    /* Decrement reference count to free memory */
    Py_DECREF(str_obj);
}
