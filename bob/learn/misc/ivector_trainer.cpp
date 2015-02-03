/**
 * @author Tiago de Freitas Pereira <tiago.pereira@idiap.ch>
 * @date Tue 03 Fev 10:29:00 2015
 *
 * @brief Python API for bob::learn::em
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include "main.h"
#include <boost/make_shared.hpp>

/******************************************************************/
/************ Constructor Section *********************************/
/******************************************************************/

static inline bool f(PyObject* o){return o != 0 && PyObject_IsTrue(o) > 0;}  /* converts PyObject to bool and returns false if object is NULL */

static int extract_GMMStats_1d(PyObject *list,
                             std::vector<bob::learn::misc::GMMStats>& training_data)
{
  for (int i=0; i<PyList_GET_SIZE(list); i++){
  
    PyBobLearnMiscGMMStatsObject* stats;
    if (!PyArg_Parse(PyList_GetItem(list, i), "O!", &PyBobLearnMiscGMMStats_Type, &stats)){
      PyErr_Format(PyExc_RuntimeError, "Expected GMMStats objects");
      return -1;
    }
    training_data.push_back(*stats->cxx);

  }
  return 0;
}


static auto IVectorTrainer_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".IVectorTrainer",
  "IVectorTrainer"
  "An IVectorTrainer to learn a Total Variability subspace :math:`$T$`"
  " (and eventually a covariance matrix :math:`$\\Sigma$`).",
  " References: [Dehak2010]"
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Constructor. Builds a new IVectorTrainer",
    "",
    true
  )
  .add_prototype("update_sigma","")
  .add_prototype("other","")
  .add_prototype("","")
  .add_parameter("other", ":py:class:`bob.learn.misc.IVectorTrainer`", "A IVectorTrainer object to be copied.")
  .add_parameter("update_sigma", "bool", "")
);


static int PyBobLearnMiscIVectorTrainer_init_copy(PyBobLearnMiscIVectorTrainerObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = IVectorTrainer_doc.kwlist(1);
  PyBobLearnMiscIVectorTrainerObject* o;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnMiscIVectorTrainer_Type, &o)){
    IVectorTrainer_doc.print_usage();
    return -1;
  }

  self->cxx.reset(new bob::learn::misc::IVectorTrainer(*o->cxx));
  return 0;
}


static int PyBobLearnMiscIVectorTrainer_init_bool(PyBobLearnMiscIVectorTrainerObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = IVectorTrainer_doc.kwlist(0);
  PyObject* update_sigma   = 0;

  //Parsing the input argments
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBool_Type, &update_sigma))
    return -1;
  
  self->cxx.reset(new bob::learn::misc::IVectorTrainer(f(update_sigma)));
  return 0;
}


static int PyBobLearnMiscIVectorTrainer_init(PyBobLearnMiscIVectorTrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // get the number of command line arguments
  int nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  switch(nargs){
    case 0:{
      self->cxx.reset(new bob::learn::misc::IVectorTrainer());
      return 0;
    }
    case 1:{
      //Reading the input argument
      PyObject* arg = 0;
      if (PyTuple_Size(args))
        arg = PyTuple_GET_ITEM(args, 0);
      else {
        PyObject* tmp = PyDict_Values(kwargs);
        auto tmp_ = make_safe(tmp);
        arg = PyList_GET_ITEM(tmp, 0);
      }

      // If the constructor input is IVectorTrainer object
      if(PyBobLearnMiscIVectorTrainer_Check(arg))            
        return  PyBobLearnMiscIVectorTrainer_init_copy(self, args, kwargs);
      else
        return PyBobLearnMiscIVectorTrainer_init_bool(self, args, kwargs);      
      
    }
    default:{
      PyErr_Format(PyExc_RuntimeError, "number of arguments mismatch - %s requires only 0 or 1 arguments, but you provided %d (see help)", Py_TYPE(self)->tp_name, nargs);
      IVectorTrainer_doc.print_usage();
      return -1;
    }
  }
  BOB_CATCH_MEMBER("cannot create IVectorTrainer", 0)
  return 0;
}


static void PyBobLearnMiscIVectorTrainer_delete(PyBobLearnMiscIVectorTrainerObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}


int PyBobLearnMiscIVectorTrainer_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnMiscIVectorTrainer_Type));
}


static PyObject* PyBobLearnMiscIVectorTrainer_RichCompare(PyBobLearnMiscIVectorTrainerObject* self, PyObject* other, int op) {
  BOB_TRY

  if (!PyBobLearnMiscIVectorTrainer_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobLearnMiscIVectorTrainerObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
  BOB_CATCH_MEMBER("cannot compare IVectorTrainer objects", 0)
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto acc_nij_wij2 = bob::extension::VariableDoc(
  "acc_nij_wij2",
  "array_like <float, 3D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnMiscIVectorTrainer_get_acc_nij_wij2(PyBobLearnMiscIVectorTrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccNijWij2());
  BOB_CATCH_MEMBER("acc_nij_wij2 could not be read", 0)
}
int PyBobLearnMiscIVectorTrainer_set_acc_nij_wij2(PyBobLearnMiscIVectorTrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 3D array of floats", Py_TYPE(self)->tp_name, acc_nij_wij2.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,3>(o, "acc_nij_wij2");
  if (!b) return -1;
  self->cxx->setAccNijWij2(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_nij_wij2 could not be set", -1)
}


static auto acc_fnormij_wij = bob::extension::VariableDoc(
  "acc_fnormij_wij",
  "array_like <float, 3D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnMiscIVectorTrainer_get_acc_fnormij_wij(PyBobLearnMiscIVectorTrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccFnormijWij());
  BOB_CATCH_MEMBER("acc_fnormij_wij could not be read", 0)
}
int PyBobLearnMiscIVectorTrainer_set_acc_fnormij_wij(PyBobLearnMiscIVectorTrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 3D array of floats", Py_TYPE(self)->tp_name, acc_fnormij_wij.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,3>(o, "acc_fnormij_wij");
  if (!b) return -1;
  self->cxx->setAccFnormijWij(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_fnormij_wij could not be set", -1)
}


static auto acc_nij = bob::extension::VariableDoc(
  "acc_nij",
  "array_like <float, 1D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnMiscIVectorTrainer_get_acc_nij(PyBobLearnMiscIVectorTrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccNij());
  BOB_CATCH_MEMBER("acc_nij could not be read", 0)
}
int PyBobLearnMiscIVectorTrainer_set_acc_nij(PyBobLearnMiscIVectorTrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 1D array of floats", Py_TYPE(self)->tp_name, acc_nij.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,1>(o, "acc_nij");
  if (!b) return -1;
  self->cxx->setAccNij(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_nij could not be set", -1)
}


static auto acc_snormij = bob::extension::VariableDoc(
  "acc_snormij",
  "array_like <float, 2D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnMiscIVectorTrainer_get_acc_snormij(PyBobLearnMiscIVectorTrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccSnormij());
  BOB_CATCH_MEMBER("acc_snormij could not be read", 0)
}
int PyBobLearnMiscIVectorTrainer_set_acc_snormij(PyBobLearnMiscIVectorTrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 2D array of floats", Py_TYPE(self)->tp_name, acc_snormij.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,2>(o, "acc_snormij");
  if (!b) return -1;
  self->cxx->setAccSnormij(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_snormij could not be set", -1)
}




static PyGetSetDef PyBobLearnMiscIVectorTrainer_getseters[] = { 
  {
   acc_nij_wij2.name(),
   (getter)PyBobLearnMiscIVectorTrainer_get_acc_nij_wij2,
   (setter)PyBobLearnMiscIVectorTrainer_set_acc_nij_wij2,
   acc_nij_wij2.doc(),
   0
  },  
  {
   acc_fnormij_wij.name(),
   (getter)PyBobLearnMiscIVectorTrainer_get_acc_fnormij_wij,
   (setter)PyBobLearnMiscIVectorTrainer_set_acc_fnormij_wij,
   acc_fnormij_wij.doc(),
   0
  },
  {
   acc_nij.name(),
   (getter)PyBobLearnMiscIVectorTrainer_get_acc_nij,
   (setter)PyBobLearnMiscIVectorTrainer_set_acc_nij,
   acc_nij.doc(),
   0
  },
  {
   acc_snormij.name(),
   (getter)PyBobLearnMiscIVectorTrainer_get_acc_snormij,
   (setter)PyBobLearnMiscIVectorTrainer_set_acc_snormij,
   acc_snormij.doc(),
   0
  },

  {0}  // Sentinel
};


/******************************************************************/
/************ Functions Section ***********************************/
/******************************************************************/

/*** initialize ***/
static auto initialize = bob::extension::FunctionDoc(
  "initialize",
  "Initialization before the EM steps",
  "",
  true
)
.add_prototype("ivector_machine")
.add_parameter("ivector_machine", ":py:class:`bob.learn.misc.ISVBase`", "IVectorMachine Object");
static PyObject* PyBobLearnMiscIVectorTrainer_initialize(PyBobLearnMiscIVectorTrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  /* Parses input arguments in a single shot */
  char** kwlist = initialize.kwlist(0);

  PyBobLearnMiscIVectorMachineObject* ivector_machine = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnMiscIVectorMachine_Type, &ivector_machine)) Py_RETURN_NONE;

  self->cxx->initialize(*ivector_machine->cxx);

  BOB_CATCH_MEMBER("cannot perform the initialize method", 0)

  Py_RETURN_NONE;
}


/*** e_step ***/
static auto e_step = bob::extension::FunctionDoc(
  "e_step",
  "Call the e-step procedure (for the U subspace).",
  "",
  true
)
.add_prototype("ivector_machine,stats")
.add_parameter("ivector_machine", ":py:class:`bob.learn.misc.ISVBase`", "IVectorMachine Object")
.add_parameter("stats", ":py:class:`bob.learn.misc.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnMiscIVectorTrainer_e_step(PyBobLearnMiscIVectorTrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = e_step.kwlist(0);

  PyBobLearnMiscIVectorMachineObject* ivector_machine = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnMiscIVectorMachine_Type, &ivector_machine,
                                                                 &PyList_Type, &stats)) Py_RETURN_NONE;

  std::vector<bob::learn::misc::GMMStats> training_data;
  if(extract_GMMStats_1d(stats ,training_data)==0)
    self->cxx->eStep(*ivector_machine->cxx, training_data);

  Py_RETURN_NONE;
  BOB_CATCH_MEMBER("cannot perform the e_step method", 0)
}


/*** m_step ***/
static auto m_step = bob::extension::FunctionDoc(
  "m_step",
  "Call the m-step procedure (for the U subspace).",
  "",
  true
)
.add_prototype("ivector_machine")
.add_parameter("ivector_machine", ":py:class:`bob.learn.misc.ISVBase`", "IVectorMachine Object");
static PyObject* PyBobLearnMiscIVectorTrainer_m_step(PyBobLearnMiscIVectorTrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot 
  char** kwlist = m_step.kwlist(0);

  PyBobLearnMiscIVectorMachineObject* ivector_machine = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnMiscIVectorMachine_Type, &ivector_machine)) Py_RETURN_NONE;

  self->cxx->mStep(*ivector_machine->cxx);

  BOB_CATCH_MEMBER("cannot perform the m_step method", 0)

  Py_RETURN_NONE;
}



static PyMethodDef PyBobLearnMiscIVectorTrainer_methods[] = {
  {
    initialize.name(),
    (PyCFunction)PyBobLearnMiscIVectorTrainer_initialize,
    METH_VARARGS|METH_KEYWORDS,
    initialize.doc()
  },
  {
    e_step.name(),
    (PyCFunction)PyBobLearnMiscIVectorTrainer_e_step,
    METH_VARARGS|METH_KEYWORDS,
    e_step.doc()
  },
  {
    m_step.name(),
    (PyCFunction)PyBobLearnMiscIVectorTrainer_m_step,
    METH_VARARGS|METH_KEYWORDS,
    m_step.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gaussian type struct; will be initialized later
PyTypeObject PyBobLearnMiscIVectorTrainer_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobLearnMiscIVectorTrainer(PyObject* module)
{
  // initialize the type struct
  PyBobLearnMiscIVectorTrainer_Type.tp_name      = IVectorTrainer_doc.name();
  PyBobLearnMiscIVectorTrainer_Type.tp_basicsize = sizeof(PyBobLearnMiscIVectorTrainerObject);
  PyBobLearnMiscIVectorTrainer_Type.tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;//Enable the class inheritance;
  PyBobLearnMiscIVectorTrainer_Type.tp_doc       = IVectorTrainer_doc.doc();

  // set the functions
  PyBobLearnMiscIVectorTrainer_Type.tp_new          = PyType_GenericNew;
  PyBobLearnMiscIVectorTrainer_Type.tp_init         = reinterpret_cast<initproc>(PyBobLearnMiscIVectorTrainer_init);
  PyBobLearnMiscIVectorTrainer_Type.tp_dealloc      = reinterpret_cast<destructor>(PyBobLearnMiscIVectorTrainer_delete);
  PyBobLearnMiscIVectorTrainer_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobLearnMiscIVectorTrainer_RichCompare);
  PyBobLearnMiscIVectorTrainer_Type.tp_methods      = PyBobLearnMiscIVectorTrainer_methods;
  PyBobLearnMiscIVectorTrainer_Type.tp_getset       = PyBobLearnMiscIVectorTrainer_getseters;
  //PyBobLearnMiscIVectorTrainer_Type.tp_call         = reinterpret_cast<ternaryfunc>(PyBobLearnMiscIVectorTrainer_compute_likelihood);


  // check that everything is fine
  if (PyType_Ready(&PyBobLearnMiscIVectorTrainer_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobLearnMiscIVectorTrainer_Type);
  return PyModule_AddObject(module, "_IVectorTrainer", (PyObject*)&PyBobLearnMiscIVectorTrainer_Type) >= 0;
}

