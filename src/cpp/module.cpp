#include <Python.h>
#include <numpy/arrayobject.h>

#include "emulator.h"

// List of nes module methods.
static PyMethodDef nes_methods[] = {
  { NULL, NULL, 0, NULL }
};

// Initialize the nes module.
PyMODINIT_FUNC initnes(void) {
  PyObject *m;

  EmulatorType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&EmulatorType) < 0)
    return;

  m = Py_InitModule("nes", nes_methods);
  if (m == NULL) {
    return;
  }

  Py_INCREF(&EmulatorType);
  PyModule_AddObject(m, "Emulator", (PyObject *)&EmulatorType);

  InvalidRomError = PyErr_NewException("nes.invalid_rom", NULL, NULL);
  Py_INCREF(InvalidRomError);
  PyModule_AddObject(m, "error", InvalidRomError);

  import_array();
}
