/**
 * @file oks_check_schema.cxx
 *
 * Program to do some simple integrity checks on a schema file
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2025.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "CLI/CLI.hpp"
#include "logging/Logging.hpp"
#include "oks/class.hpp"
#include "oks/file.hpp"
#include "oks/kernel.hpp"
#include "oks/relationship.hpp"

#include <fmt/core.h>
#include <set>
#include <string>

using namespace dunedaq::oks;



int main(int argc, char **argv) {
  enum Exitcode : int {SUCCESS, BADCMD, NOFILE, LOADFAIL,
    BADRELATIONSHIP, UNRESOLVED,
    MISSING_INCLUDE};

  CLI::App app{"Check consistency of OKS schema file\n"+
    fmt::format("Return codes: {} Success, file is OK\n", Exitcode::SUCCESS)+
    fmt::format("              {} Bad command line\n", Exitcode::BADCMD)+
    fmt::format("              {} Failed to find file\n", Exitcode::NOFILE)+
    fmt::format("              {} Failed to load file -- invalid schema\n", Exitcode::LOADFAIL)+
    fmt::format("              {} File contains relationship to non loaded class\n", Exitcode::BADRELATIONSHIP)+
    fmt::format("              {} File contains object with relationship to non loaded class/object\n", Exitcode::UNRESOLVED)+
    fmt::format("              {} File refers to class in file not directly included\n", Exitcode::MISSING_INCLUDE)
  };

  std::string filename;
  app.add_option("-f,--file", filename, "Schema file")
    ->required();

  CLI11_PARSE(app, argc, argv);

  OksKernel kernel;
  OksFile* file{nullptr};
  try {
    file = kernel.load_file(filename);
  }
  catch (FailedLoadFile& fail) {
    TLOG() << fail.what() << "\n";
    return Exitcode::LOADFAIL;
  }
  catch (CanNotOpenFile& fail) {
    TLOG() << fail.what() << "\n";
    return Exitcode::NOFILE;
  }
  catch (std::exception& exc) {
    TLOG() << exc.what() << "\n";
    return Exitcode::LOADFAIL;
  }

  if (file == nullptr) {
    TLOG() << "Failed to load " << filename;
    return Exitcode::LOADFAIL;
  }

  std::set<std::string> includes;
  for (const auto& ifile : file->get_include_files()) {
    includes.insert(ifile);
    TLOG_DEBUG(2) << "direct inserting " << ifile << "\n";
  }

  Exitcode result=Exitcode::SUCCESS;

  if (filename.ends_with(".schema.xml")) {
    // Check classes defined in this schema file
    for (auto [name, oks_class] : kernel.classes()) {

      if (file->get_full_file_name() != oks_class->get_file()->get_full_file_name()) {
        // Class not defined in this file
        continue;
      }

      auto cfile = oks_class->get_file()->get_short_file_name();
      if (cfile != file->get_short_file_name() && !includes.contains(cfile)) {
        TLOG() << "Error " << filename << " misses include of " << cfile << " required by " << name;
        if (result==Exitcode::SUCCESS) {
          result =  Exitcode::MISSING_INCLUDE;
        }
      }
      auto relationships = oks_class->direct_relationships();
      if (relationships != nullptr) {
        for (auto rel: *relationships) {
          auto rel_class = rel->get_class_type();
          if (rel_class == nullptr) {
            TLOG() << "Error class '" << name
                   << "' has relationship '" << rel->get_name()
                   << "' to a class '" << rel->get_type()
                   << "' that is not loaded\n";
            if (result==Exitcode::SUCCESS) {
              result =  Exitcode::BADRELATIONSHIP;
            }
          }

        }
      }
      // Could check super-classes but we wouldn't have got past the
      // load if they referred to a missing class.
    }
  } else if (filename.ends_with(".data.xml")) {

    auto check_relationship = [&](OksData* data) {
      auto relobj = data->data.OBJECT;
      if (relobj == nullptr) {
        return;
      }
      auto ofile_name = relobj->get_file()->get_short_file_name();
      if (file->get_short_file_name() != ofile_name && !includes.contains(ofile_name)) {
        TLOG() << "Error " << filename << " misses include of " << ofile_name
               << " required by " << relobj->GetId();
        if (result==Exitcode::SUCCESS) {
          result =  Exitcode::MISSING_INCLUDE;
        }
      }
    };

    auto data_objs = kernel.create_list_of_data_objects(file);
    for (auto obj : *data_objs) {
      // Check that any objects defined in this data file have their
      // corresponding schema file included directly in this file
      auto oks_class = obj->GetClass();
      auto cfile = oks_class->get_file()->get_short_file_name();
      if (!includes.contains(cfile)) {
        TLOG() << "Error " << filename << " misses include of " << cfile
               << " required by " << obj->GetId()
               << " of class " << oks_class->get_name();
        if (result==Exitcode::SUCCESS) {
          result =  Exitcode::MISSING_INCLUDE;
        }
      }

      // Check that any objects referenced by objects in this file are
      // either defined in this file or one that is directly included
      auto relationships = oks_class->all_relationships();
      if (relationships != nullptr) {
        for (auto rel: *relationships) {
          auto relval = obj->GetRelationshipValue(rel->get_name());
          if (relval == nullptr) {
            continue;
          }
          if (rel->get_high_cardinality_constraint() == OksRelationship::Many) {
            for(OksData::List::iterator iter = relval->data.LIST->begin();
                iter != relval->data.LIST->end(); ++iter) {
              check_relationship(*iter);
            }
          } else {
            check_relationship(relval);
          }
        }
      }
    }
    delete data_objs;
  }

  auto status = kernel.get_bind_objects_status();
  if (!status.empty()) {
    if (result==Exitcode::SUCCESS) {
      result = Exitcode::UNRESOLVED;
    }
  }

  return result;
}
