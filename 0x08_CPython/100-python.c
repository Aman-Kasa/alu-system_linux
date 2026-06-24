#include <Python.h>
#include <stdio.h>

/**
 * print_python_int - Prints a Python integer (handles arbitrary precision)
 * @p: Pointer to a PyObject
 */
void print_python_int(PyObject *p)
{
    PyObject *str_obj;
    const char *str_val;

    /* 1. Safety Check: Verify the object is an instance of int */
    if (!p || !PyLong_Check(p))
    {
        printf("Invalid Int Object\n");
        return;
    }

    /* 2. Convert to string object: 
       PyObject_Str calls the __str__ method of the int object,
       which is internally handled by Python's arbitrary precision logic. */
    str_obj = PyObject_Str(p);
    if (!str_obj)
    {
        return;
    }

    /* 3. Convert to a C string for printing */
    str_val = PyUnicode_AsUTF8(str_obj);
    if (str_val)
    {
        printf("%s\n", str_val);
    }

    /* 4. Cleanup: PyObject_Str returns a new reference, we must release it */
    Py_DECREF(str_obj);
}
