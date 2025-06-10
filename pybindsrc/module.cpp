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
#include "oks/attribute.hpp"
#include "oks/class.hpp"
#include "oks/relationship.hpp"
#include "oks/method.hpp"
#include "oks/file.hpp"

namespace py = pybind11;

namespace dunedaq::oks::python {


PYBIND11_MODULE(_daq_oks_py, m)
{

  using namespace pybind11::literals; // to bring in the `_a` literal


  m.doc() = "C++ implementation of the application dal modules";

  // py::class_<OksFile::Map>(m, "OksFileMap");

  py::class_<OksKernel>(m, "OksKernel")
      .def(py::init<bool, bool, bool, bool, const char *, std::string>(),
        "silence_mode"_a = false, "verbose_mode"_a = false, "profiling_mode"_a = false, "allow_repository"_a = true, "version"_a = nullptr, "branch_name"_a = "")
      .def("get_host_name",&OksKernel::get_host_name)
      .def("get_domain_name",&OksKernel::get_domain_name)
      .def("get_user_name",&OksKernel::get_user_name)
      // ---
      // .def("load_file",&OksKernel::load_file, "name"_a, "bind"_a = true)
      // .def("load_schema",&OksKernel::load_schema, "name"_a, "parent"_a = nullptr)
      // .def("close_all_schema",&OksKernel::close_all_schema)

      // ---
      // .def("load_data",&OksKernel::load_file, "name"_a, "bind"_a = true)
      // .def("close_all_data",&OksKernel::close_all_data)
      // -- 
      // .def("classes",&OksKernel::classes, py::return_value_policy::reference_internal)
      // .def("number_of_classes",&OksKernel::number_of_classes)
      // .def("objects",&OksKernel::objects, py::return_value_policy::reference_internal)
      // .def("number_of_objects",&OksKernel::number_of_objects)
      //
      // .def("find_class",&OksKernel::find_class)
      // .def("find_object",&OksKernel::find_object, py::return_value_policy::reference_internal)

      // 
      // .def("get_test_duplicated_objects_via_inheritance_mode", &OksKernel::get_test_duplicated_objects_via_inheritance_mode)
      // .def("set_test_duplicated_objects_via_inheritance_mode", &OksKernel::set_test_duplicated_objects_via_inheritance_mode)
      // .def("get_allow_duplicated_objects_mode", &OksKernel::get_allow_duplicated_objects_mode)
      // .def("set_allow_duplicated_objects_mode", &OksKernel::set_allow_duplicated_objects_mode)

    // 	get_verbose_mode
    // 	set_verbose_mode 
    .def("get_verbose_mode",&OksKernel::get_verbose_mode)
    .def("set_verbose_mode",&OksKernel::set_verbose_mode)

    //  get_silence_mode
    // 	set_silence_mode
    .def("get_silence_mode",&OksKernel::get_silence_mode)
    .def("set_silence_mode",&OksKernel::set_silence_mode)

    // 	set_profiling_mode
    .def("set_profiling_mode",&OksKernel::set_profiling_mode)

    // 	get_allow_duplicated_classes_mode
    // 	set_allow_duplicated_classes_mode
    .def("get_allow_duplicated_classes_mode",&OksKernel::get_allow_duplicated_classes_mode)
    .def("set_allow_duplicated_classes_mode",&OksKernel::set_allow_duplicated_classes_mode)

    //  get_allow_duplicated_objects_mode 
    //  set_allow_duplicated_objects_mode 
    .def("get_allow_duplicated_objects_mode",&OksKernel::get_allow_duplicated_objects_mode)
    .def("set_allow_duplicated_objects_mode",&OksKernel::set_allow_duplicated_objects_mode)

    // 	get_test_duplicated_objects_via_inheritance_mode
    // 	set_test_duplicated_objects_via_inheritance_mode
    .def("get_test_duplicated_objects_via_inheritance_mode",&OksKernel::get_test_duplicated_objects_via_inheritance_mode)
    .def("set_test_duplicated_objects_via_inheritance_mode",&OksKernel::set_test_duplicated_objects_via_inheritance_mode)

    //! get_mutex 
    // find_schema_file
    // find_data_file
    .def("find_schema_file",&OksKernel::find_schema_file, py::return_value_policy::reference_internal)
    .def("find_data_file",&OksKernel::find_data_file, py::return_value_policy::reference_internal)

    // create_list_of_schema_classes
    // create_list_of_data_objects
    .def("create_list_of_schema_classes",&OksKernel::create_list_of_schema_classes, py::return_value_policy::reference_internal)
    .def("create_list_of_data_objects",&OksKernel::create_list_of_data_objects, py::return_value_policy::reference_internal)

    // 	create_file_info
    .def("create_file_info",&OksKernel::create_file_info, py::return_value_policy::reference_internal)

    // 	get_file_path
    .def("get_file_path",&OksKernel::get_file_path, "path"_a, "parent_file"_a = nullptr, "strict_path"_a=true)

    // get_repository_version
    .def("get_repository_version",&OksKernel::get_repository_version)

    // is_user_repository_created()
    .def("is_user_repository_created",&OksKernel::is_user_repository_created)

    // get_user_repository_root
    // set_user_repository_root
    .def("get_user_repository_root",&OksKernel::get_user_repository_root)
    .def("set_user_repository_root",&OksKernel::set_user_repository_root, "path"_a, "version"_a = true)

    // get_includes
    .def("get_includes",&OksKernel::get_includes)
    // k_close_dangling_includes
    
    // load_file
    .def("load_file",&OksKernel::load_file, "name"_a, "bind"_a = true, py::return_value_policy::reference_internal)
    // load_schema
    .def("load_schema",&OksKernel::load_schema, "name"_a, "parent"_a = nullptr, py::return_value_policy::reference_internal)
    // new_schema
    .def("new_schema",&OksKernel::new_schema, "name"_a, py::return_value_policy::reference_internal)
    // save_schema
    // .def("save_schema",py::overload_cast<OksFile*, bool, OksFile*>(&OksKernel::save_schema), "file_h"_a, "force"_a = false, "true_file_h"_a = nullptr)
    // .def("save_schema",py::overload_cast<OksFile*, bool, const OksClass::Map&>(&OksKernel::save_schema), "file_h"_a, "force"_a, "classes")
    // backup_schema
    .def("backup_schema",&OksKernel::backup_schema, "pf"_a, "suffix"_a = ".bak")
    // save_as_schema
    .def("save_as_schema",&OksKernel::save_as_schema, "name"_a, "file_h"_a)
    // save_all_schema
    .def("save_all_schema",&OksKernel::save_all_schema)
    // close_schema
    .def("close_schema",&OksKernel::close_schema,  "file_h"_a)
    // close_all_schema
    .def("close_all_schema",&OksKernel::close_all_schema)
    // set_active_schema
    .def("set_active_schema",&OksKernel::set_active_schema, "file_h"_a)
    // get_active_schema
    .def("get_active_schema",&OksKernel::get_active_schema)
    // schema_files
    .def("schema_files",&OksKernel::schema_files)

    // load_data
    .def("load_data",&OksKernel::load_data, "name"_a, "bind"_a = true, py::return_value_policy::reference_internal)
    // reload_data
    .def("reload_data",&OksKernel::reload_data, "files"_a, "allow_schema_extension"_a = true)
    
    // new_data
    .def("new_data",&OksKernel::new_data, "name"_a, "logical_name"_a = "", "type"_a = "", py::return_value_policy::reference_internal)

    // // save_data
    // // backup_data
    // // save_as_data
    // save_all_data
    .def("save_all_data", &OksKernel::save_all_data, "force_defaults"_a = false)
    // // close_data
    // close_all_data
    .def("close_all_data",&OksKernel::close_all_data)

    // set_active_data
    .def("set_active_data", &OksKernel::set_active_data)
    // get_active_daat
    .def("get_active_data", &OksKernel::get_active_data, py::return_value_policy::reference_internal)
    // data_files
    .def("data_files", &OksKernel::data_files, py::return_value_policy::reference_internal)

    // insert_repository_dir
    .def("insert_repository_dir",&OksKernel::insert_repository_dir)
    // remove_repository_dir 
    .def("remove_repository_dir",&OksKernel::remove_repository_dir)
    // classes
    .def("classes",&OksKernel::classes, py::return_value_policy::reference_internal)
    // number_of_classes
    .def("number_of_classes",&OksKernel::number_of_classes)
    // objects
    .def("objects",&OksKernel::objects, py::return_value_policy::reference_internal)
    // number_of_objects
    .def("number_of_objects",&OksKernel::number_of_objects) 
    // find_class
    .def("find_class",static_cast<OksClass*(OksKernel::*)(const std::string&) const>(&OksKernel::find_class), "class_name"_a) 
    // // get_all_classes
    // registrate_all_classes
    .def("registrate_all_classes",&OksKernel::registrate_all_classes) 
    // bind_objects
    .def("bind_objects",&OksKernel::bind_objects) 
    // get_bind_objects_status
    .def("get_bind_objects_status",&OksKernel::get_bind_objects_status) 
    // get_bind_classes_status
    .def("get_bind_classes_status",&OksKernel::get_bind_classes_status) 
    // unset_repository_created
    .def("unset_repository_created",&OksKernel::unset_repository_created) 

    ;

    py::class_<OksFile>(m, "OksFile")
      .def("add_include_file", &OksFile::add_include_file, "file_name"_a)
      .def("remove_include_file", &OksFile::remove_include_file, "file_name"_a)
      .def("remove_include_file", &OksFile::remove_include_file, "file"_a)
      .def("get_include_files", &OksFile::get_include_files, py::return_value_policy::reference_internal)
      .def("get_all_include_files", &OksFile::get_all_include_files, py::return_value_policy::reference_internal)

      .def("add_comment", &OksFile::add_comment, "text"_a, "author"_a = "")
      .def("modify_comment", &OksFile::modify_comment, "creation_time"_a, "text"_a, "author"_a = "")
      .def("remove_comment", &OksFile::remove_comment, "creation_time"_a)

      .def("get_short_file_name", &OksFile::get_short_file_name)
      .def("get_full_file_name", &OksFile::get_full_file_name)
      .def("get_well_formed_name", &OksFile::get_well_formed_name)
      .def("get_repository_name", &OksFile::get_repository_name)
      ;
 
 
    py::class_<OksClass, std::unique_ptr<OksClass, py::nodelete>>(m, "OksClass")
      .def(py::init<const std::string&, OksKernel*, bool>(), "name"_a, "kernel"_a, "transient"_a = false)
      .def(py::init<const std::string&, const std::string&, bool, OksKernel*, bool>(), "name"_a, "description"_a, "is_abstract"_a, "kernel"_a, "transient"_a = false)
      .def(py::init<const std::string&, const std::string&, bool, OksKernel*>(), "name"_a, "description"_a, "is_abstract"_a, "kernel"_a)
      .def("get_name",&OksClass::get_name, py::return_value_policy::reference_internal)
      .def("get_description",&OksClass::get_description, py::return_value_policy::reference_internal)
      .def("set_description",&OksClass::set_description, "description"_a)
      .def("get_is_abstract",&OksClass::get_is_abstract)
      .def("set_is_abstract",&OksClass::set_is_abstract, "is_abstract"_a)
      .def("get_file",&OksClass::get_file, py::return_value_policy::reference_internal)
      .def("set_file",&OksClass::set_file, "file"_a, "update_owner"_a = true)

      .def("all_super_classes",&OksClass::all_super_classes, py::return_value_policy::reference_internal)
      .def("direct_super_classes",&OksClass::direct_super_classes, py::return_value_policy::reference_internal)
      .def("all_sub_classes",&OksClass::all_sub_classes, py::return_value_policy::reference_internal)
      .def("add_super_class", &OksClass::add_super_class)
      .def("remove_super_class", &OksClass::remove_super_class)
      .def("swap_super_classes", &OksClass::swap_super_classes, "c1"_a, "c2"_a)
      .def("destroy", &OksClass::destroy, "class"_a)
      
      // ** Add/Remove **∂
      // Attributes
      .def("add", py::overload_cast<OksAttribute*>(&OksClass::add), "attribute"_a)
      .def("remove", py::overload_cast<const OksAttribute*>(&OksClass::remove), "attribute"_a)
      .def("swap", py::overload_cast<const OksAttribute*, const OksAttribute*>(&OksClass::swap), "a1"_a, "a2"_a)
      .def("number_of_direct_attributes", &OksClass::number_of_direct_attributes)
      .def("number_of_all_attributes", &OksClass::number_of_all_attributes)
      .def("all_attributes", &OksClass::all_attributes, py::return_value_policy::reference_internal)
      .def("find_attribute", &OksClass::find_attribute, "name"_a, py::return_value_policy::reference_internal)
      .def("find_direct_attribute", &OksClass::find_direct_attribute, "name"_a, py::return_value_policy::reference_internal)
      
      // Relationships
      .def("add", py::overload_cast<OksRelationship*>(&OksClass::add), "r"_a)
      .def("remove", py::overload_cast<const OksRelationship*, bool>(&OksClass::remove), "r"_a, "call_delete"_a = true)
      .def("swap", py::overload_cast<const OksRelationship*, const OksRelationship*>(&OksClass::swap), "r1"_a, "r2"_a)
      .def("all_relationships", &OksClass::all_relationships, py::return_value_policy::reference_internal)
      .def("direct_relationships", &OksClass::direct_relationships, py::return_value_policy::reference_internal)
      .def("find_relationship", &OksClass::find_relationship, "name"_a, py::return_value_policy::reference_internal)
      .def("find_direct_relationship", &OksClass::find_direct_relationship, "name"_a, py::return_value_policy::reference_internal)
      .def("number_of_direct_relationships", &OksClass::number_of_direct_relationships)
      .def("number_of_all_relationships", &OksClass::number_of_all_relationships)

      // Methods
      .def("add", py::overload_cast<OksMethod*>(&OksClass::add), "method"_a)
      .def("remove", py::overload_cast<const OksMethod*>(&OksClass::remove), "method"_a)
      .def("swap", py::overload_cast<const OksMethod*, const OksMethod*>(&OksClass::swap), "m1"_a, "m2"_a)
      .def("all_methods", &OksClass::all_methods, py::return_value_policy::reference_internal)
      .def("direct_methods", &OksClass::direct_methods, py::return_value_policy::reference_internal)
      .def("find_method", &OksClass::find_method, "name"_a, py::return_value_policy::reference_internal)
      .def("find_direct_method", &OksClass::find_direct_method, "name"_a, py::return_value_policy::reference_internal)
      .def("number_of_direct_methods", &OksClass::number_of_direct_methods)
      .def("number_of_all_methods", &OksClass::number_of_all_methods)
  ;

  

    // TODO: Split across multiple files
    py::class_<OksAttribute, std::unique_ptr<OksAttribute, py::nodelete>>(m, "OksAttribute")
      // Initialisers
      .def(py::init<const std::string&, OksClass*>(), "name"_a, "class_ptr"_a=nullptr)
      // .def(py::init<const std::string&, const std::string&, bool, const std::string&, const std::string&, const std::string&, bool, OksAttribute::Format=OksAttribute::Format::Dec, OksClass*>())
      // Setters and Getters
      .def("set_name", &OksAttribute::set_name)
      .def("get_type", &OksAttribute::get_type)
      .def("set_type", &OksAttribute::set_type)
      .def("get_range", &OksAttribute::get_range)
      .def("set_range", &OksAttribute::set_range)
      .def("get_data_type", py::overload_cast<const std::string&>(&OksAttribute::get_data_type))
      .def("get_data_type", py::overload_cast<const char*, size_t>(&OksAttribute::get_data_type))
      // .def("get_data_type", py::overload_cast<>(&OksAttribute::get_data_type))
      .def("get_format", &OksAttribute::get_format)
      .def("set_format", &OksAttribute::set_format)
      .def("is_integer", &OksAttribute::is_integer)
      .def("is_number", &OksAttribute::is_number)
      .def("get_is_multi_values", &OksAttribute::get_is_multi_values)
      .def("set_is_multi_values", &OksAttribute::set_is_multi_values)
      .def("get_init_value", &OksAttribute::get_init_value)
      .def("set_init_value", &OksAttribute::set_init_value)
      .def("get_description", &OksAttribute::get_description)
      .def("set_description", &OksAttribute::set_description)
      .def("get_is_no_null", &OksAttribute::get_is_no_null)
      .def("set_is_no_null", &OksAttribute::set_is_no_null)
      .def("find_token", &OksAttribute::find_token)
      ;

    py::enum_<OksRelationship::CardinalityConstraint>(m, "CardinalityConstraint")
      .value("Zero", OksRelationship::CardinalityConstraint::Zero)
      .value("One", OksRelationship::CardinalityConstraint::One)
      .value("Many", OksRelationship::CardinalityConstraint::Many)
      .export_values()
    ;

    py::class_<OksRelationship, std::unique_ptr<OksRelationship, py::nodelete>>(m, "OksRelationship")
      // Initialisers
      .def(py::init<const std::string&, const std::string&, OksRelationship::CardinalityConstraint, OksRelationship::CardinalityConstraint, bool,
                    bool, bool, const std::string&, OksClass*>(),
                  "name"_a, "type"_a, "low_cc"_a, "high_cc"_a, "composite"_a = false, "exclusive"_a = false, "dependent"_a = false,
                  "description"_a = "", "parent"_a = nullptr)
      .def("get_name", &OksRelationship::get_name, py::return_value_policy::reference_internal)
      .def("set_name", &OksRelationship::set_name)
      .def("get_type", &OksRelationship::get_type, py::return_value_policy::reference_internal)
      .def("get_class_type", &OksRelationship::get_class_type, py::return_value_policy::reference_internal)
      .def("set_type", &OksRelationship::set_type)
      .def("get_description", &OksRelationship::get_description)
      .def("set_description", &OksRelationship::set_description)
      .def("get_low_cardinality_constraint", &OksRelationship::get_low_cardinality_constraint)
      .def("set_low_cardinality_constraint", &OksRelationship::set_low_cardinality_constraint)
      .def("get_high_cardinality_constraint", &OksRelationship::get_high_cardinality_constraint)
      .def("set_high_cardinality_constraint", &OksRelationship::set_high_cardinality_constraint)
      .def("get_is_composite", &OksRelationship::get_is_composite)
      .def("set_is_composite", &OksRelationship::set_is_composite)
      .def("get_is_exclusive", &OksRelationship::get_is_exclusive)
      .def("set_is_exclusive", &OksRelationship::set_is_exclusive)
      .def("get_is_dependent", &OksRelationship::get_is_dependent)
      .def("set_is_dependent", &OksRelationship::set_is_dependent)
      ;

      py::class_<OksMethod, std::unique_ptr<OksMethod, py::nodelete>>(m, "OksMethod")
      .def(py::init<const std::string&, OksClass*>(), "name"_a, "class_ptr"_a = nullptr)
      .def("get_name", &OksMethod::get_name, py::return_value_policy::reference_internal)
      .def("set_name", &OksMethod::set_name)
      .def("get_description", &OksMethod::get_description)
      .def("set_description", &OksMethod::set_description)
      .def("implementations", &OksMethod::implementations, py::return_value_policy::reference_internal)
      .def("add_implementation", &OksMethod::add_implementation, "language"_a, "prototype"_a, "body"_a)
      .def("remove_implementation", &OksMethod::remove_implementation, "language"_a)
      .def("find_implementation", &OksMethod::find_implementation, "name"_a, py::return_value_policy::reference_internal)
      ;
  
    }
} // namespace dunedaq::oks::python
