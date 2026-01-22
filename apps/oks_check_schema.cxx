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
#include <string>

using namespace dunedaq::oks;


int main(int argc, char **argv) {
  enum Exitcode : int {SUCCESS, BADCMD, NOFILE, LOADFAIL, BADRELATIONSHIP};

  CLI::App app{"Check consistency of OKS schema file\n"+
    fmt::format("Return codes: {} Success, file is OK\n", Exitcode::SUCCESS)+
    fmt::format("              {} Bad command line\n", Exitcode::BADCMD)+
    fmt::format("              {} Failed to find file\n", Exitcode::NOFILE)+
    fmt::format("              {} Failed to load file -- invalid schema\n", Exitcode::LOADFAIL)+
    fmt::format("              {} File contains relationship to non loaded class\n", Exitcode::BADRELATIONSHIP)
  };

  std::string filename;
  app.add_option("-f,--file", filename, "Schema file")
    ->required();

  CLI11_PARSE(app, argc, argv);

  OksKernel kernel;
  try {
    const auto file = kernel.load_file(filename);
    if (file == nullptr) {
      TLOG() << "Failed to load " << filename;
      return Exitcode::LOADFAIL;
    }
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

  for (auto [name, oks_class] : kernel.classes()) {
    auto relationships = oks_class->direct_relationships();
    if (relationships != nullptr) {
      for (auto rel: *relationships) {
        auto rel_class = rel->get_class_type();
        if (rel_class == nullptr) {
          TLOG() << "Error class '" << name
                    << "' has relationship '" << rel->get_name()
                    << "' to a class '" << rel->get_type()
                    << "' that is not loaded\n";
          return Exitcode::BADRELATIONSHIP;
        }
      }
    }
    // Could check super-classes but we wouldn't have got past the
    // load if they referred to a missing class.
  }

  return 0;
}
