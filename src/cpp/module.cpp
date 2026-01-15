#include <Python.h>
#include <numpy/arrayobject.h>

#include "emulator.h"

// List of nes module methods.
static PyMethodDef nes_methods[] = {
  { NULL, NULL, 0, NULL }
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "nes",     // name of module
    "Python package for NES emulation",
    -1,
    nes_methods,
};

// Initialize the nes module.
PyMODINIT_FUNC PyInit_nes(void) {
  PyObject *m;

  EmulatorType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&EmulatorType) < 0)
    return NULL;

  m = PyModule_Create(&moduledef);
  if (m == NULL) {
    return NULL;
  }

  Py_INCREF(&EmulatorType);
  PyModule_AddObject(m, "Emulator", (PyObject *)&EmulatorType);

  InvalidRomError = PyErr_NewException("nes.invalid_rom", NULL, NULL);
  Py_INCREF(InvalidRomError);
  PyModule_AddObject(m, "error", InvalidRomError);

  IllegalStateError = PyErr_NewException("nes.illegal_state", NULL, NULL);
  Py_INCREF(IllegalStateError);
  PyModule_AddObject(m, "error", IllegalStateError);

  import_array();
  return m;
}
