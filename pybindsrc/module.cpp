/**
 * @file module.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "oks/kernel.hpp"

namespace py = pybind11;

namespace dunedaq::oks::python {


PYBIND11_MODULE(_daq_oks_py, m)
{

  using namespace pybind11::literals; // to bring in the `_a` literal


  m.doc() = "C++ implementation of the application dal modules";

  py::class_<OksFile>(m, "OksFile");
  py::class_<OksClass, std::unique_ptr<OksClass, py::nodelete>>(m, "OksClass")
      .def("get_name",&OksClass::get_name, py::return_value_policy::reference)
      .def("get_description",&OksClass::get_description, py::return_value_policy::reference)
      .def("get_is_abstract",&OksClass::get_is_abstract)
      .def("all_super_classes",&OksClass::all_super_classes, py::return_value_policy::reference)
      .def("direct_super_classes",&OksClass::direct_super_classes, py::return_value_policy::reference)
      .def("all_sub_classes",&OksClass::all_sub_classes, py::return_value_policy::reference)
  ;
  py::class_<OksObject, std::unique_ptr<OksObject, py::nodelete>>(m, "OksObject");


  py::class_<OksKernel>(m, "OksKernel")
      .def(py::init<bool, bool, bool, bool, const char *, std::string>(),
        "silence_mode"_a = false, "verbose_mode"_a = false, "profiling_mode"_a = false, "allow_repository"_a = true, "version"_a = nullptr, "branch_name"_a = "")
      .def("get_host_name",&OksKernel::get_host_name)
      .def("get_domain_name",&OksKernel::get_domain_name)
      .def("get_user_name",&OksKernel::get_user_name)
      // ---
      .def("load_file",&OksKernel::load_file, "name"_a, "bind"_a = true)
      .def("load_schema",&OksKernel::load_schema, "name"_a, "parent"_a = nullptr)
      .def("close_all_schema",&OksKernel::close_all_schema)

      // ---
      .def("load_data",&OksKernel::load_file, "name"_a, "bind"_a = true)
      .def("close_all_data",&OksKernel::close_all_data)
      // -- 
      .def("classes",&OksKernel::classes, py::return_value_policy::reference)
      .def("number_of_classes",&OksKernel::number_of_classes)
      .def("objects",&OksKernel::objects, py::return_value_policy::reference)
      .def("number_of_objects",&OksKernel::number_of_objects)
      //
      // .def("find_class",&OksKernel::find_class)
      .def("find_object",&OksKernel::find_object, py::return_value_policy::reference)

      // 
      .def("get_test_duplicated_objects_via_inheritance_mode", &OksKernel::get_test_duplicated_objects_via_inheritance_mode)
      .def("set_test_duplicated_objects_via_inheritance_mode", &OksKernel::set_test_duplicated_objects_via_inheritance_mode)
      .def("get_allow_duplicated_objects_mode", &OksKernel::get_allow_duplicated_objects_mode)
      .def("set_allow_duplicated_objects_mode", &OksKernel::set_allow_duplicated_objects_mode)

    ;

}

} // namespace dunedaq::oks::python
