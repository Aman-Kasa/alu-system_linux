#include <Python.h>
#include <stdio.h>

/**
 * print_python_string - Prints information about Python strings
 * @p: Pointer to a PyObject
 */
void print_python_string(PyObject *p)
{
    PyASCIIObject *ascii = (PyASCIIObject *)p;
    const char *value;

    printf("[.] string object info\n");

    /* Verify if the object is actually a string */
    if (!PyUnicode_Check(p))
    {
        printf("  [ERROR] Invalid String Object\n");
        return;
    }

    /* Print type based on whether it is compact/ready */
    if (PyUnicode_IS_COMPACT_ASCII(p))
        printf("  type: compact ascii\n");
    else
        printf("  type: compact unicode object\n");

    /* Get length */
    printf("  length: %ld\n", PyUnicode_GET_LENGTH(p));

    /* Get the UTF-8 representation for value */
    value = PyUnicode_AsUTF8(p);
    printf("  value: %s\n", value);
}
