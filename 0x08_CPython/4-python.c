#include <Python.h>
#include <stdio.h>

/**
 * print_python_string - Prints information about Python strings
 * @p: Pointer to a PyObject (expected to be a string/unicode)
 */
void print_python_string(PyObject *p)
{
    PyASCIIObject *ascii;
    PyCompactUnicodeObject *compact;
    Py_ssize_t length;

    printf("[.] string object info\n");
    
    if (strcmp(p->ob_type->tp_name, "str") != 0)
    {
        printf("  [ERROR] Invalid String Object\n");
        return;
    }

    ascii = (PyASCIIObject *)p;
    length = ascii->length;

    if (ascii->state.compact && ascii->state.ascii)
    {
        printf("  type: compact ascii\n");
        printf("  length: %ld\n", length);
        printf("  value: %s\n", (char *)(ascii + 1));
    }
    else if (ascii->state.compact && !ascii->state.ascii)
    {
        compact = (PyCompactUnicodeObject *)p;
        printf("  type: compact unicode object\n");
        printf("  length: %ld\n", length);
        /* Cast to wide string equivalent for output */
        printf("  value: %ls\n", (wchar_t *)(compact + 1));
    }
}
