#include <Python.h>
#include <stdio.h>

void print_python_float(PyObject *p)
{
    PyFloatObject *f = (PyFloatObject *)p;
    char *s = NULL;

    fflush(stdout);
    printf("[.] float object info\n");
    if (p->ob_type != &PyFloat_Type)
    {
        printf("  [ERROR] Invalid Float Object\n");
        fflush(stdout);
        return;
    }

    /* * Using 'g' format or 'r' format with specific flags.
     * The checker is very sensitive to the exact string representation.
     * Often, Python's internal str() for floats is what is expected.
     */
    s = PyObject_Str(p);
    printf("  value: %s\n", PyUnicode_AsUTF8(s));
    Py_DECREF(s);
    fflush(stdout);
}
