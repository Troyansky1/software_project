# ifndef SYMNMF_H_
# define SYMNMF_H_
# include <Python.h>

/* Functions in symnmfmodule.c */

static void sym(PyObject *self, PyObject *args);

static void ddg(PyObject *self, PyObject *args);

static void norm(PyObject *self, PyObject *args);

static PyObject* symnmf(PyObject *self, PyObject *args);


# endif