/**
 * @author Tiago de Freitas Pereira <tiago.pereira@idiap.ch>
 * @date Sun 01 Fev 09:40:00 2015
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

static int extract_GMMStats_1d(PyObject *list,
                             std::vector<boost::shared_ptr<bob::learn::em::GMMStats> >& training_data)
{
  for (int i=0; i<PyList_GET_SIZE(list); i++){
  
    PyBobLearnEMGMMStatsObject* stats;
    if (!PyArg_Parse(PyList_GetItem(list, i), "O!", &PyBobLearnEMGMMStats_Type, &stats)){
      PyErr_Format(PyExc_RuntimeError, "Expected GMMStats objects");
      return -1;
    }
    training_data.push_back(stats->cxx);
  }
  return 0;
}

static int extract_GMMStats_2d(PyObject *list,
                             std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > >& training_data)
{
  for (int i=0; i<PyList_GET_SIZE(list); i++)
  {
    PyObject* another_list;
    PyArg_Parse(PyList_GetItem(list, i), "O!", &PyList_Type, &another_list);

    std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > another_training_data;
    for (int j=0; j<PyList_GET_SIZE(another_list); j++){

      PyBobLearnEMGMMStatsObject* stats;
      if (!PyArg_Parse(PyList_GetItem(another_list, j), "O!", &PyBobLearnEMGMMStats_Type, &stats)){
        PyErr_Format(PyExc_RuntimeError, "Expected GMMStats objects");
        return -1;
      }
      another_training_data.push_back(stats->cxx);
    }
    training_data.push_back(another_training_data);
  }
  return 0;
}

template <int N>
static PyObject* vector_as_list(const std::vector<blitz::Array<double,N> >& vec)
{
  PyObject* list = PyList_New(vec.size());
  for(size_t i=0; i<vec.size(); i++){
    blitz::Array<double,N> numpy_array = vec[i];
    PyObject* numpy_py_object = PyBlitzArrayCxx_AsNumpy(numpy_array);
    PyList_SET_ITEM(list, i, numpy_py_object);
  }
  return list;
}

template <int N>
int list_as_vector(PyObject* list, std::vector<blitz::Array<double,N> >& vec)
{
  for (int i=0; i<PyList_GET_SIZE(list); i++)
  {
    PyBlitzArrayObject* blitz_object; 
    if (!PyArg_Parse(PyList_GetItem(list, i), "O&", &PyBlitzArray_Converter, &blitz_object)){
      PyErr_Format(PyExc_RuntimeError, "Expected numpy array object");
      return -1;
    }
    auto blitz_object_ = make_safe(blitz_object);
    vec.push_back(*PyBlitzArrayCxx_AsBlitz<double,N>(blitz_object));
  }
  return 0;
}



static auto JFATrainer_doc = bob::extension::ClassDoc(
  BOB_EXT_MODULE_PREFIX ".JFATrainer",
  "JFATrainer"
  "References: [Vogt2008,McCool2013]",
  ""
).add_constructor(
  bob::extension::FunctionDoc(
    "__init__",
    "Constructor. Builds a new JFATrainer",
    "",
    true
  )
  .add_prototype("other","")
  .add_prototype("","")
  .add_parameter("other", ":py:class:`bob.learn.em.JFATrainer`", "A JFATrainer object to be copied.")
);


static int PyBobLearnEMJFATrainer_init_copy(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {

  char** kwlist = JFATrainer_doc.kwlist(0);
  PyBobLearnEMJFATrainerObject* o;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist, &PyBobLearnEMJFATrainer_Type, &o)){
    JFATrainer_doc.print_usage();
    return -1;
  }

  self->cxx.reset(new bob::learn::em::JFATrainer(*o->cxx));
  return 0;
}


static int PyBobLearnEMJFATrainer_init(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // get the number of command line arguments
  int nargs = (args?PyTuple_Size(args):0) + (kwargs?PyDict_Size(kwargs):0);

  switch(nargs){
    case 0:{
      self->cxx.reset(new bob::learn::em::JFATrainer());
      return 0;
    }
    case 1:{
      // If the constructor input is JFATrainer object
      return PyBobLearnEMJFATrainer_init_copy(self, args, kwargs);
    }
    default:{
      PyErr_Format(PyExc_RuntimeError, "number of arguments mismatch - %s requires only 0 and 1 argument, but you provided %d (see help)", Py_TYPE(self)->tp_name, nargs);
      JFATrainer_doc.print_usage();
      return -1;
    }
  }
  BOB_CATCH_MEMBER("cannot create JFATrainer", 0)
  return 0;
}


static void PyBobLearnEMJFATrainer_delete(PyBobLearnEMJFATrainerObject* self) {
  self->cxx.reset();
  Py_TYPE(self)->tp_free((PyObject*)self);
}


int PyBobLearnEMJFATrainer_Check(PyObject* o) {
  return PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyBobLearnEMJFATrainer_Type));
}


static PyObject* PyBobLearnEMJFATrainer_RichCompare(PyBobLearnEMJFATrainerObject* self, PyObject* other, int op) {
  BOB_TRY

  if (!PyBobLearnEMJFATrainer_Check(other)) {
    PyErr_Format(PyExc_TypeError, "cannot compare `%s' with `%s'", Py_TYPE(self)->tp_name, Py_TYPE(other)->tp_name);
    return 0;
  }
  auto other_ = reinterpret_cast<PyBobLearnEMJFATrainerObject*>(other);
  switch (op) {
    case Py_EQ:
      if (*self->cxx==*other_->cxx) Py_RETURN_TRUE; else Py_RETURN_FALSE;
    case Py_NE:
      if (*self->cxx==*other_->cxx) Py_RETURN_FALSE; else Py_RETURN_TRUE;
    default:
      Py_INCREF(Py_NotImplemented);
      return Py_NotImplemented;
  }
  BOB_CATCH_MEMBER("cannot compare JFATrainer objects", 0)
}


/******************************************************************/
/************ Variables Section ***********************************/
/******************************************************************/

static auto acc_v_a1 = bob::extension::VariableDoc(
  "acc_v_a1",
  "array_like <float, 3D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_acc_v_a1(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccVA1());
  BOB_CATCH_MEMBER("acc_v_a1 could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_acc_v_a1(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 3D array of floats", Py_TYPE(self)->tp_name, acc_v_a1.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,3>(o, "acc_v_a1");
  if (!b) return -1;
  self->cxx->setAccVA1(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_v_a1 could not be set", -1)
}


static auto acc_v_a2 = bob::extension::VariableDoc(
  "acc_v_a2",
  "array_like <float, 2D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_acc_v_a2(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccVA2());
  BOB_CATCH_MEMBER("acc_v_a2 could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_acc_v_a2(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 2D array of floats", Py_TYPE(self)->tp_name, acc_v_a2.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,2>(o, "acc_v_a2");
  if (!b) return -1;
  self->cxx->setAccVA2(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_v_a2 could not be set", -1)
}


static auto acc_u_a1 = bob::extension::VariableDoc(
  "acc_u_a1",
  "array_like <float, 3D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_acc_u_a1(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccUA1());
  BOB_CATCH_MEMBER("acc_u_a1 could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_acc_u_a1(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 3D array of floats", Py_TYPE(self)->tp_name, acc_u_a1.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,3>(o, "acc_u_a1");
  if (!b) return -1;
  self->cxx->setAccUA1(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_u_a1 could not be set", -1)
}


static auto acc_u_a2 = bob::extension::VariableDoc(
  "acc_u_a2",
  "array_like <float, 2D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_acc_u_a2(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccUA2());
  BOB_CATCH_MEMBER("acc_u_a2 could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_acc_u_a2(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 2D array of floats", Py_TYPE(self)->tp_name, acc_u_a2.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,2>(o, "acc_u_a2");
  if (!b) return -1;
  self->cxx->setAccUA2(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_u_a2 could not be set", -1)
}


static auto acc_d_a1 = bob::extension::VariableDoc(
  "acc_d_a1",
  "array_like <float, 1D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_acc_d_a1(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccDA1());
  BOB_CATCH_MEMBER("acc_d_a1 could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_acc_d_a1(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 1D array of floats", Py_TYPE(self)->tp_name, acc_d_a1.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,1>(o, "acc_d_a1");
  if (!b) return -1;
  self->cxx->setAccDA1(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_d_a1 could not be set", -1)
}


static auto acc_d_a2 = bob::extension::VariableDoc(
  "acc_d_a2",
  "array_like <float, 1D>",
  "Accumulator updated during the E-step",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_acc_d_a2(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return PyBlitzArrayCxx_AsConstNumpy(self->cxx->getAccDA2());
  BOB_CATCH_MEMBER("acc_d_a2 could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_acc_d_a2(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY
  PyBlitzArrayObject* o;
  if (!PyBlitzArray_Converter(value, &o)){
    PyErr_Format(PyExc_RuntimeError, "%s %s expects a 1D array of floats", Py_TYPE(self)->tp_name, acc_d_a2.name());
    return -1;
  }
  auto o_ = make_safe(o);
  auto b = PyBlitzArrayCxx_AsBlitz<double,1>(o, "acc_d_a2");
  if (!b) return -1;
  self->cxx->setAccDA2(*b);
  return 0;
  BOB_CATCH_MEMBER("acc_d_a2 could not be set", -1)
}


static auto __X__ = bob::extension::VariableDoc(
  "__X__",
  "list",
  "",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_X(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return vector_as_list(self->cxx->getX());
  BOB_CATCH_MEMBER("__X__ could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_X(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY

  // Parses input arguments in a single shot
  if (!PyList_Check(value)){
    PyErr_Format(PyExc_TypeError, "Expected a list in `%s'", __X__.name());
    return -1;
  }
    
  std::vector<blitz::Array<double,2> > data;
  if(list_as_vector(value ,data)==0){
    self->cxx->setX(data);
  }
    
  return 0;
  BOB_CATCH_MEMBER("__X__ could not be written", 0)
}



static auto __Y__ = bob::extension::VariableDoc(
  "__Y__",
  "list",
  "",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_Y(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return vector_as_list(self->cxx->getY());
  BOB_CATCH_MEMBER("__Y__ could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_Y(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY

  // Parses input arguments in a single shot
  if (!PyList_Check(value)){
    PyErr_Format(PyExc_TypeError, "Expected a list in `%s'", __Y__.name());
    return -1;
  }
    
  std::vector<blitz::Array<double,1> > data;
  if(list_as_vector(value ,data)==0){
    self->cxx->setY(data);
  }
    
  return 0;
  BOB_CATCH_MEMBER("__Y__ could not be written", 0)
}



static auto __Z__ = bob::extension::VariableDoc(
  "__Z__",
  "list",
  "",
  ""
);
PyObject* PyBobLearnEMJFATrainer_get_Z(PyBobLearnEMJFATrainerObject* self, void*){
  BOB_TRY
  return vector_as_list(self->cxx->getZ());
  BOB_CATCH_MEMBER("__Z__ could not be read", 0)
}
int PyBobLearnEMJFATrainer_set_Z(PyBobLearnEMJFATrainerObject* self, PyObject* value, void*){
  BOB_TRY

  // Parses input arguments in a single shot
  if (!PyList_Check(value)){
    PyErr_Format(PyExc_TypeError, "Expected a list in `%s'", __Z__.name());
    return -1;
  }
    
  std::vector<blitz::Array<double,1> > data;
  if(list_as_vector(value ,data)==0){
    self->cxx->setZ(data);
  }
    
  return 0;
  BOB_CATCH_MEMBER("__Z__ could not be written", 0)
}



static PyGetSetDef PyBobLearnEMJFATrainer_getseters[] = { 
  {
   acc_v_a1.name(),
   (getter)PyBobLearnEMJFATrainer_get_acc_v_a1,
   (setter)PyBobLearnEMJFATrainer_get_acc_v_a1,
   acc_v_a1.doc(),
   0
  },
  {
   acc_v_a2.name(),
   (getter)PyBobLearnEMJFATrainer_get_acc_v_a2,
   (setter)PyBobLearnEMJFATrainer_get_acc_v_a2,
   acc_v_a2.doc(),
   0
  },
  {
   acc_u_a1.name(),
   (getter)PyBobLearnEMJFATrainer_get_acc_u_a1,
   (setter)PyBobLearnEMJFATrainer_get_acc_u_a1,
   acc_u_a1.doc(),
   0
  },
  {
   acc_u_a2.name(),
   (getter)PyBobLearnEMJFATrainer_get_acc_u_a2,
   (setter)PyBobLearnEMJFATrainer_get_acc_u_a2,
   acc_u_a2.doc(),
   0
  },
  {
   acc_d_a1.name(),
   (getter)PyBobLearnEMJFATrainer_get_acc_d_a1,
   (setter)PyBobLearnEMJFATrainer_get_acc_d_a1,
   acc_d_a1.doc(),
   0
  },
  {
   acc_d_a2.name(),
   (getter)PyBobLearnEMJFATrainer_get_acc_d_a2,
   (setter)PyBobLearnEMJFATrainer_get_acc_d_a2,
   acc_d_a2.doc(),
   0
  },
  {
   __X__.name(),
   (getter)PyBobLearnEMJFATrainer_get_X,
   (setter)PyBobLearnEMJFATrainer_set_X,
   __X__.doc(),
   0
  },
  {
   __Y__.name(),
   (getter)PyBobLearnEMJFATrainer_get_Y,
   (setter)PyBobLearnEMJFATrainer_set_Y,
   __Y__.doc(),
   0
  },
  {
   __Z__.name(),
   (getter)PyBobLearnEMJFATrainer_get_Z,
   (setter)PyBobLearnEMJFATrainer_set_Z,
   __Z__.doc(),
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
.add_prototype("jfa_base,stats,rng")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object")
.add_parameter("rng", ":py:class:`bob.core.random.mt19937`", "The Mersenne Twister mt19937 random generator used for the initialization of subspaces/arrays before the EM loop.");
static PyObject* PyBobLearnEMJFATrainer_initialize(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  /* Parses input arguments in a single shot */
  char** kwlist = initialize.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;
  PyBoostMt19937Object* rng = 0;  

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!|O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats,
                                                                 &PyBoostMt19937_Type, &rng)) return 0;

  if(rng){
    boost::shared_ptr<boost::mt19937> rng_cpy = (boost::shared_ptr<boost::mt19937>)new boost::mt19937(*rng->rng);
    self->cxx->setRng(rng_cpy);
  }

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->initialize(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the initialize method", 0)

  Py_RETURN_NONE;
}


/*** e_step1 ***/
static auto e_step1 = bob::extension::FunctionDoc(
  "e_step1",
  "Call the 1st e-step procedure (for the V subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_e_step1(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  //Parses input arguments in a single shot
  char** kwlist = e_step1.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->eStep1(*jfa_base->cxx, training_data);


  BOB_CATCH_MEMBER("cannot perform the e_step1 method", 0)

  Py_RETURN_NONE;
}


/*** m_step1 ***/
static auto m_step1 = bob::extension::FunctionDoc(
  "m_step1",
  "Call the 1st m-step procedure (for the V subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_m_step1(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = m_step1.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->mStep1(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the m_step1 method", 0)

  Py_RETURN_NONE;
}


/*** finalize1 ***/
static auto finalize1 = bob::extension::FunctionDoc(
  "finalize1",
  "Call the 1st finalize procedure (for the V subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_finalize1(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  //Parses input arguments in a single shot
  char** kwlist = finalize1.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->finalize1(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the finalize1 method", 0)

  Py_RETURN_NONE;
}


/*** e_step2 ***/
static auto e_step2 = bob::extension::FunctionDoc(
  "e_step2",
  "Call the 2nd e-step procedure (for the U subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_e_step2(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = e_step2.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->eStep2(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the e_step2 method", 0)

  Py_RETURN_NONE;
}


/*** m_step2 ***/
static auto m_step2 = bob::extension::FunctionDoc(
  "m_step2",
  "Call the 2nd m-step procedure (for the U subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_m_step2(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot 
  char** kwlist = m_step2.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->mStep2(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the m_step2 method", 0)

  Py_RETURN_NONE;
}


/*** finalize2 ***/
static auto finalize2 = bob::extension::FunctionDoc(
  "finalize2",
  "Call the 2nd finalize procedure (for the U subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_finalize2(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = finalize2.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->finalize2(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the finalize2 method", 0)

  Py_RETURN_NONE;
}


/*** e_step3 ***/
static auto e_step3 = bob::extension::FunctionDoc(
  "e_step3",
  "Call the 3rd e-step procedure (for the d subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_e_step3(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = e_step3.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->eStep3(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the e_step3 method", 0)

  Py_RETURN_NONE;
}


/*** m_step3 ***/
static auto m_step3 = bob::extension::FunctionDoc(
  "m_step3",
  "Call the 3rd m-step procedure (for the d subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_m_step3(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = m_step3.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->mStep3(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the m_step3 method", 0)

  Py_RETURN_NONE;
}


/*** finalize3 ***/
static auto finalize3 = bob::extension::FunctionDoc(
  "finalize3",
  "Call the 3rd finalize procedure (for the d subspace).",
  "",
  true
)
.add_prototype("jfa_base,stats")
.add_parameter("jfa_base", ":py:class:`bob.learn.em.JFABase`", "JFABase Object")
.add_parameter("stats", ":py:class:`bob.learn.em.GMMStats`", "GMMStats Object");
static PyObject* PyBobLearnEMJFATrainer_finalize3(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = finalize3.kwlist(0);

  PyBobLearnEMJFABaseObject* jfa_base = 0;
  PyObject* stats = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kwlist, &PyBobLearnEMJFABase_Type, &jfa_base,
                                                                 &PyList_Type, &stats)) return 0;

  std::vector<std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > > training_data;
  if(extract_GMMStats_2d(stats ,training_data)==0)
    self->cxx->finalize3(*jfa_base->cxx, training_data);

  BOB_CATCH_MEMBER("cannot perform the finalize3 method", 0)

  Py_RETURN_NONE;
}


/*** enrol ***/
static auto enrol = bob::extension::FunctionDoc(
  "enrol",
  "",
  "",
  true
)
.add_prototype("jfa_machine,features,n_iter","")
.add_parameter("jfa_machine", ":py:class:`bob.learn.em.JFAMachine`", "JFAMachine Object")
.add_parameter("features", "list(:py:class:`bob.learn.em.GMMStats`)`", "")
.add_parameter("n_iter", "int", "Number of iterations");
static PyObject* PyBobLearnEMJFATrainer_enrol(PyBobLearnEMJFATrainerObject* self, PyObject* args, PyObject* kwargs) {
  BOB_TRY

  // Parses input arguments in a single shot
  char** kwlist = enrol.kwlist(0);

  PyBobLearnEMJFAMachineObject* jfa_machine = 0;
  PyObject* stats = 0;
  int n_iter = 1;


  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!i", kwlist, &PyBobLearnEMJFAMachine_Type, &jfa_machine,
                                                                  &PyList_Type, &stats, &n_iter)) return 0;

  std::vector<boost::shared_ptr<bob::learn::em::GMMStats> > training_data;
  if(extract_GMMStats_1d(stats ,training_data)==0)
    self->cxx->enrol(*jfa_machine->cxx, training_data, n_iter);

  BOB_CATCH_MEMBER("cannot perform the enrol method", 0)

  Py_RETURN_NONE;
}



static PyMethodDef PyBobLearnEMJFATrainer_methods[] = {
  {
    initialize.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_initialize,
    METH_VARARGS|METH_KEYWORDS,
    initialize.doc()
  },
  {
    e_step1.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_e_step1,
    METH_VARARGS|METH_KEYWORDS,
    e_step1.doc()
  },
  {
    e_step2.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_e_step2,
    METH_VARARGS|METH_KEYWORDS,
    e_step2.doc()
  },
  {
    e_step3.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_e_step3,
    METH_VARARGS|METH_KEYWORDS,
    e_step3.doc()
  },
  {
    m_step1.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_m_step1,
    METH_VARARGS|METH_KEYWORDS,
    m_step1.doc()
  },
  {
    m_step2.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_m_step2,
    METH_VARARGS|METH_KEYWORDS,
    m_step2.doc()
  },
  {
    m_step3.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_m_step3,
    METH_VARARGS|METH_KEYWORDS,
    m_step3.doc()
  },
  {
    finalize1.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_finalize1,
    METH_VARARGS|METH_KEYWORDS,
    finalize1.doc()
  },
  {
    finalize2.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_finalize2,
    METH_VARARGS|METH_KEYWORDS,
    finalize2.doc()
  },
  {
    finalize3.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_finalize3,
    METH_VARARGS|METH_KEYWORDS,
    finalize3.doc()
  },
  {
    enrol.name(),
    (PyCFunction)PyBobLearnEMJFATrainer_enrol,
    METH_VARARGS|METH_KEYWORDS,
    enrol.doc()
  },
  {0} /* Sentinel */
};


/******************************************************************/
/************ Module Section **************************************/
/******************************************************************/

// Define the Gaussian type struct; will be initialized later
PyTypeObject PyBobLearnEMJFATrainer_Type = {
  PyVarObject_HEAD_INIT(0,0)
  0
};

bool init_BobLearnEMJFATrainer(PyObject* module)
{
  // initialize the type JFATrainer
  PyBobLearnEMJFATrainer_Type.tp_name      = JFATrainer_doc.name();
  PyBobLearnEMJFATrainer_Type.tp_basicsize = sizeof(PyBobLearnEMJFATrainerObject);
  PyBobLearnEMJFATrainer_Type.tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;//Enable the class inheritance;
  PyBobLearnEMJFATrainer_Type.tp_doc       = JFATrainer_doc.doc();

  // set the functions
  PyBobLearnEMJFATrainer_Type.tp_new          = PyType_GenericNew;
  PyBobLearnEMJFATrainer_Type.tp_init         = reinterpret_cast<initproc>(PyBobLearnEMJFATrainer_init);
  PyBobLearnEMJFATrainer_Type.tp_dealloc      = reinterpret_cast<destructor>(PyBobLearnEMJFATrainer_delete);
  PyBobLearnEMJFATrainer_Type.tp_richcompare = reinterpret_cast<richcmpfunc>(PyBobLearnEMJFATrainer_RichCompare);
  PyBobLearnEMJFATrainer_Type.tp_methods      = PyBobLearnEMJFATrainer_methods;
  PyBobLearnEMJFATrainer_Type.tp_getset       = PyBobLearnEMJFATrainer_getseters;
  //PyBobLearnEMJFATrainer_Type.tp_call         = reinterpret_cast<ternaryfunc>(PyBobLearnEMJFATrainer_compute_likelihood);


  // check that everything is fine
  if (PyType_Ready(&PyBobLearnEMJFATrainer_Type) < 0) return false;

  // add the type to the module
  Py_INCREF(&PyBobLearnEMJFATrainer_Type);
  return PyModule_AddObject(module, "JFATrainer", (PyObject*)&PyBobLearnEMJFATrainer_Type) >= 0;
}

