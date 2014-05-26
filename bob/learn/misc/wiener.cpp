/**
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 * @date Fri Sep 30 16:58:42 2011 +0200
 *
 * @brief Bindings for a WienerMachine
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#include "ndarray.h"
#include <bob/machine/WienerMachine.h>

using namespace boost::python;

static void py_forward1_(const bob::machine::WienerMachine& m,
  bob::python::const_ndarray input, bob::python::ndarray output)
{
  blitz::Array<double,2> output_ = output.bz<double,2>();
  m.forward_(input.bz<double,2>(), output_);
}

static void py_forward1(const bob::machine::WienerMachine& m,
  bob::python::const_ndarray input, bob::python::ndarray output)
{
  blitz::Array<double,2> output_ = output.bz<double,2>();
  m.forward(input.bz<double,2>(), output_);
}

static object py_forward2(const bob::machine::WienerMachine& m,
  bob::python::const_ndarray input)
{
  const bob::core::array::typeinfo& info = input.type();
  bob::python::ndarray output(bob::core::array::t_float64, info.shape[0], info.shape[1]);
  blitz::Array<double,2> output_ = output.bz<double,2>();
  m.forward(input.bz<double,2>(), output_);
  return output.self();
}

static tuple get_shape(const bob::machine::WienerMachine& m)
{
  return make_tuple(m.getHeight(), m.getWidth());
}

static void set_shape(bob::machine::WienerMachine& m,
    const blitz::TinyVector<int,2>& s)
{
  m.resize(s(0), s(1));
}

static void py_set_ps(bob::machine::WienerMachine& m,
  bob::python::const_ndarray ps)
{
  m.setPs(ps.bz<double,2>());
}

void bind_machine_wiener()
{
  class_<bob::machine::WienerMachine, boost::shared_ptr<bob::machine::WienerMachine> >("WienerMachine", "A Wiener filter.\nReference:\n'Computer Vision: Algorithms and Applications', Richard Szeliski, (Part 3.4.3)", init<const size_t, const size_t, const double, optional<const double> >((arg("self"), arg("height"), arg("width"), arg("pn"), arg("variance_threshold")=1e-8), "Constructs a new Wiener filter dedicated to images of the given dimensions. The filter is initialized with zero values."))
    .def(init<const blitz::Array<double,2>&, const double> ((arg("self"), arg("ps"), arg("pn")), "Constructs a new WienerMachine from a set of variance estimates ps, a noise level pn."))
    .def(init<>((arg("self")), "Default constructor, builds a machine as with 'WienerMachine(0,0,0)'."))
    .def(init<bob::io::HDF5File&>((arg("self"), arg("config")), "Constructs a new WienerMachine from a configuration file."))
    .def(init<const bob::machine::WienerMachine&>((arg("self"), arg("machine")), "Copy constructs an WienerMachine"))
    .def(self == self)
    .def(self != self)
    .def("is_similar_to", &bob::machine::WienerMachine::is_similar_to, (arg("self"), arg("other"), arg("r_epsilon")=1e-5, arg("a_epsilon")=1e-8), "Compares this WienerMachine with the 'other' one to be approximately the same.")
    .def("load", &bob::machine::WienerMachine::load, (arg("self"), arg("config")), "Loads the filter from a configuration file.")
    .def("save", &bob::machine::WienerMachine::save, (arg("self"), arg("config")), "Saves the filter to a configuration file.")
    .add_property("pn", &bob::machine::WienerMachine::getPn, &bob::machine::WienerMachine::setPn, "Noise level Pn")
    .add_property("variance_threshold", &bob::machine::WienerMachine::getVarianceThreshold, &bob::machine::WienerMachine::setVarianceThreshold, "Variance flooring threshold (min variance value)")
    .add_property("ps",make_function(&bob::machine::WienerMachine::getPs, return_value_policy<copy_const_reference>()), &py_set_ps, "Variance Ps estimated at each frequency")
    .add_property("w", make_function(&bob::machine::WienerMachine::getW, return_value_policy<copy_const_reference>()), "The Wiener filter W (W=1/(1+Pn/Ps)) (read-only)")
    .add_property("height", &bob::machine::WienerMachine::getHeight, &bob::machine::WienerMachine::setHeight, "Height of the filter/image to process")
    .add_property("width", &bob::machine::WienerMachine::getWidth, &bob::machine::WienerMachine::setWidth, "Width of the filter/image to process")
    .add_property("shape", &get_shape, &set_shape)
    .def("__call__", &py_forward1, (arg("self"), arg("input"), arg("output")), "Filters the input and saves results on the output.")
    .def("forward", &py_forward1, (arg("self"), arg("input"), arg("output")), "Filters the input and saves results on the output.")
    .def("forward_", &py_forward1_, (arg("self"), arg("input"), arg("output")), "Filters the input and saves results on the output. Input is not checked.")
    .def("__call__", &py_forward2, (arg("self"), arg("input")), "Filters the input and returns the output. This method implies in copying out the output data and is, therefore, less efficient as its counterpart that sets the output given as parameter. If you have to do a tight loop, consider using that variant instead of this one.")
    .def("forward", &py_forward2, (arg("self"), arg("input")), "Filter the input and returns the output. This method implies in copying out the output data and is, therefore, less efficient as its counterpart that sets the output given as parameter. If you have to do a tight loop, consider using that variant instead of this one.")
    ;
}
