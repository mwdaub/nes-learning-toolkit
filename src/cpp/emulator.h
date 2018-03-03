#include <Python.h>
#include <structmember.h>
#include <numpy/arrayobject.h>

#include "nes/types.h"
#include "nes/console.h"
#include "nes/ines.h"

using namespace std;
using namespace nes;

// Python error for when a file is not a valid ROM format.
static PyObject *InvalidRomError;

// Python error for when a file is not a valid ROM format.
static PyObject *IllegalStateError;

// Emulator.
typedef struct {
  PyObject_HEAD
  Console* console;
} Emulator;

// Emulator dealloc method.
static void Emulator_dealloc(Emulator* self) {
  delete self->console;
  self->console = NULL;
  Py_TYPE(self)->tp_free((PyObject*)self);        
};

// Emulator init method.
static int Emulator_init(Emulator *self, PyObject *args, PyObject *kwds) {
  char* romFileName;
  static char* kwlist[] = {"rom_file", NULL};
  if (! PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &romFileName)) {
    return -1;
  }

  string str(romFileName);
  Cartridge* cartridge = LoadiNESFile(str);
  self->console = new Console(cartridge);
  self->console->SetAudioSamplesPerFrame(735);

  return 0;
}

static PyObject * Emulator_load_state(Emulator *self, PyObject *args) {
  char* stateFileName;

  // parse arguments
  if (!PyArg_ParseTuple(args, "s", &stateFileName)) {
    return NULL;
  }

  string str(stateFileName);
  self->console->LoadState(str);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Emulator_reset(Emulator* self, PyObject* args) {
  // parse arguments
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  self->console->Reset();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject * Emulator_save_state(Emulator* self, PyObject *args) {
  char* stateFileName;

  // parse arguments
  if (!PyArg_ParseTuple(args, "s", &stateFileName)) {
    return NULL;
  }

  string str(stateFileName);
  self->console->SaveState(str);

  Py_INCREF(Py_None);
  return Py_None;
}

// Set the controller input.
static PyObject * Emulator_set_input(Emulator* self, PyObject *args) {
  uint8 input;
  // parse arguments
  if (!PyArg_ParseTuple(args, "B", &input)) {
    return NULL;
  }

  self->console->SetButtons1(input);

  Py_INCREF(Py_None);
  return Py_None;
}

// Emulate a single frame.
static PyObject * Emulator_emulate(Emulator* self, PyObject *args) {
  uint32 numFrames = 1;
  // parse arguments
  if (!PyArg_ParseTuple(args, "|I", &numFrames)) {
    return NULL;
  }

  for (int i = 0; i < numFrames; i++) {
    self->console->StepFrame();
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject * Emulator_get_pixel_indexes(Emulator* self, PyObject *args) {
  // parse arguments
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  // Dimensions of the game pixel indexes.
  static long int dims[] = {Screen::kHeight, Screen::kWidth};

  PyObject* array = PyArray_SimpleNew(3, dims, NPY_UINT8);
  uint8* data = (uint8*) PyArray_DATA(array);
  self->console->PixelIndexes(data);
  return array;
}

static PyObject * Emulator_get_pixel_values(Emulator* self, PyObject *args) {
  // parse arguments
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  // Dimensions of the game pixels.
  static long int dims[] = {Screen::kHeight, Screen::kWidth, Screen::kNumChannels};

  PyObject* array = PyArray_SimpleNew(3, dims, NPY_UINT8);
  uint8* data = (uint8*) PyArray_DATA(array);
  self->console->PixelValues(data);
  return array;
}

// Read the value at the given memory address.
//
// Key SMB values: 0x0756 size (0 = small, 1 = big, 2 = fire), 0x075A lives, 0x075C stage number, 0x075E coins, 0x075F world number
// 0x07F8 clock hundreds, 0x07F9 clock tens, 0x07FA clock ones
// 0x07D8 score hundred thousands, 0x07D9 score ten thousands, 0x07DA score thousands, 0x07DB score hundreds, 0x07DC score tens, 0x07DD score ones
// 0x07DE-0x07E3 also seems to contain the score.
//
// Arguments: uint16 representing the memory address.
// Returns: uint8 representing the memory value.
static PyObject * Emulator_read_memory(Emulator* self, PyObject *args) {
  uint16 memAddress;

  // parse arguments
  if (!PyArg_ParseTuple(args, "H", &memAddress)) {
    return NULL;
  }

  // read memory
  uint8 memValue = self->console->cpu->Read(memAddress);
  return Py_BuildValue("B", memValue);
}

// Start a new session for recording controller input and A/V output.
static PyObject * Emulator_new_session(Emulator* self, PyObject *args) {
  // parse arguments
  if (!PyArg_ParseTuple(args, "")) {
    return NULL;
  }

  self->console->NewSession();

  Py_INCREF(Py_None);
  return Py_None;
}

// Save the existing session.
static PyObject * Emulator_save_session(Emulator* self, PyObject *args) {
  char* outputFileName;

  // parse arguments
  if (!PyArg_ParseTuple(args, "s", &outputFileName)) {
    return NULL;
  }

  if (!self->console->session) {
    // TODO
    return NULL;
  }

  string str(outputFileName);
  self->console->session->Save(str);

  Py_INCREF(Py_None);
  return Py_None;
}

// Replay a previously recorded session for recording controller input.
static PyObject * Emulator_replay_session(Emulator* self, PyObject *args) {
  char* inputFileName;
  char* videoFileName;
  char* audioFileName;

  // parse arguments
  if (!PyArg_ParseTuple(args, "sss", &inputFileName, &videoFileName, &audioFileName)) {
    return NULL;
  }

  if (self->console->cpu->cycles) {
    // TODO
    return NULL;
  }

  string input(inputFileName);
  string video(videoFileName);
  string audio(audioFileName);
  self->console->Replay(input, video, audio);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef Emulator_methods[] = {
  { "save_state", (PyCFunction)Emulator_save_state, METH_VARARGS, "Save NES savestate." },
  { "load_state", (PyCFunction)Emulator_load_state, METH_VARARGS, "Load NES savestate." },
  { "reset", (PyCFunction)Emulator_reset, METH_VARARGS, "Resets the NES." },
  { "set_input", (PyCFunction)Emulator_set_input, METH_VARARGS, "Set the controller input." },
  { "emulate", (PyCFunction)Emulator_emulate, METH_KEYWORDS, "Emulate the number of specified frames." },
  { "get_pixel_indexes", (PyCFunction)Emulator_get_pixel_indexes, METH_VARARGS, "Get an array containing the current pixel indexes. Each index is in the range [0..63]" },
  { "get_pixel_values", (PyCFunction)Emulator_get_pixel_values, METH_VARARGS, "Get an array containing the current RGB pixel values." },
  { "read_memory", (PyCFunction)Emulator_read_memory, METH_VARARGS, "Read the value at the given memory address." },
  { "new_session", (PyCFunction)Emulator_new_session, METH_VARARGS, "Start a new recording session." },
  { "save_session", (PyCFunction)Emulator_save_session, METH_VARARGS, "Save the existing recording session." },
  { "replay_session", (PyCFunction)Emulator_replay_session, METH_VARARGS, "Replay a previously recorded session." },
  {NULL}  /* Sentinel */
};

static PyTypeObject EmulatorType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "nes.Emulator",                 /* tp_name */
  sizeof(Emulator),         /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)Emulator_dealloc, /* tp_dealloc */
  0,                         /* tp_print */
  0,                         /* tp_getattr */
  0,                         /* tp_setattr */
  0,                         /* tp_compare */
  0,                         /* tp_repr */
  0,                         /* tp_as_number */
  0,                         /* tp_as_sequence */
  0,                         /* tp_as_mapping */
  0,                         /* tp_hash */
  0,                         /* tp_call */
  0,                         /* tp_str */
  0,                         /* tp_getattro */
  0,                         /* tp_setattro */
  0,                         /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,        /* tp_flags */
  "NES object",              /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  Emulator_methods,         /* tp_methods */
  0,                         /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)Emulator_init,  /* tp_init */
  0,                         /* tp_alloc */
  0,                         /* tp_new */
};
