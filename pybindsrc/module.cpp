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
  // py::class_<OksFile::Map>(m, "OksFileMap");
  py::class_<OksClass, std::unique_ptr<OksClass, py::nodelete>>(m, "OksClass")
      .def("get_name",&OksClass::get_name, py::return_value_policy::reference_internal)
      .def("get_description",&OksClass::get_description, py::return_value_policy::reference_internal)
      .def("get_is_abstract",&OksClass::get_is_abstract)
      .def("all_super_classes",&OksClass::all_super_classes, py::return_value_policy::reference_internal)
      .def("direct_super_classes",&OksClass::direct_super_classes, py::return_value_policy::reference_internal)
      .def("all_sub_classes",&OksClass::all_sub_classes, py::return_value_policy::reference_internal)
  ;
  py::class_<OksObject, std::unique_ptr<OksObject, py::nodelete>>(m, "OksObject");


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
    
    //! create_list_of_upsated_schema_file
    //! get_updated_repository_files

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

    // // create_list_of_updated_data_files
    // // get_modified_files
    // // get_repository_dirs
    // // commit_repository
    // // tag_repository
    // // get_repository_checkout_ts
    // // update_repository
    // // get_repository_versions_diff
    // // get_repository_unmerged_files
    // // get_repository_versions
    // // get_repository_versions_by_hash
    // // get_repository_versions_by_date
    // // read_repository_version

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
    // is_dangling
    // Class
    // .def("is_dangling",&OksKernel::is_dangling) 
    // Object
    // .def("is_dangling",&OksKernel::is_dangling) 
    // // subscribe_create_class
    // // subscribe_change_class
    // // subscribe_create_object
    // // subscribe_delete_object
    // bind_objects
    .def("bind_objects",&OksKernel::bind_objects) 
    // get_bind_objects_status
    .def("get_bind_objects_status",&OksKernel::get_bind_objects_status) 
    // get_bind_classes_status
    .def("get_bind_classes_status",&OksKernel::get_bind_classes_status) 
    // unset_repository_created
    .def("unset_repository_created",&OksKernel::unset_repository_created) 


    // // GetVersion
    // // get_skip_string_range
    // // set_skip_string_range
    // // check_read_only
    // // get_repository_root
    // // get_repository_mapping_dir
    // // get_tmp_file
    // // get_cwd
    // // reset_cwd
    ;


}

} // namespace dunedaq::oks::python
