/**
 *  \file oks_validate_repository.cpp
 *
 *  This file is part of the OKS package.
 *  Author: <Igor.Soloviev@cern.ch>
 */


#include "oks/kernel.hpp"
#include "oks/pipeline.hpp"
#include "oks/exceptions.hpp"

#include <boost/program_options.hpp>

#include "ers/ers.hpp"
#include "logging/Logging.hpp"

#include <algorithm>
#include <chrono>
#include <vector>
#include <iostream>
#include <filesystem>
#include <mutex>

using namespace dunedaq::oks;

enum __OksValidateRepositoryExitStatus__ {
  __Success__ = 0,
  __BadCommandLine__,
  __UserAuthenticationFailure__,
  __NoRepository__,
  __ConsistencyError__,
  __IncludesCircularDependencyError__,
  __AccessManagerAuthorizationFailed__,
  __AccessManagerNoPermission__,
  __NoIncludedFile__,
  __ExceptionCaught__
};


std::string s_load_error;

static void
init_file_load_error(const std::string& file)
{
  s_load_error = "repository validation failed for file \'";
  s_load_error += file;
  s_load_error += "\':\n";
}

struct OksValidateJob : public OksJob
{
public:

  OksValidateJob(OksKernel& kernel, const std::string& file_name) :
      m_kernel(kernel), m_file_name(file_name)
  {
    ;
  }

  void
  run()
  {
    static std::mutex s_mutex;

    try
      {
        auto start_usage = std::chrono::steady_clock::now();

        m_kernel.set_silence_mode(true);

        m_kernel.load_file(m_file_name);

        if (!m_kernel.get_bind_classes_status().empty() || !m_kernel.get_bind_objects_status().empty())
          {
            std::lock_guard lock(s_mutex);

            if (s_load_error.empty())
              {
                init_file_load_error(m_file_name);

                if (!m_kernel.get_bind_classes_status().empty())
                  {
                    s_load_error += "the schema contains dangling references to non-loaded classes:\n";
                    s_load_error += m_kernel.get_bind_classes_status();
                  }

                if (!m_kernel.get_bind_objects_status().empty())
                  {
                    s_load_error += "the data contain dangling references to non-loaded objects:\n";
                    s_load_error += m_kernel.get_bind_objects_status();
                  }
              }
          }

        static std::mutex s_log_mutex;
        std::lock_guard scoped_lock(s_log_mutex);

        log_timestamp() << "validated file \"" << m_file_name << "\" in " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start_usage).count() / 1000. << " ms\n";
      }
    catch (std::exception& ex)
      {
        std::lock_guard lock(s_mutex);

        if (s_load_error.empty())
          {
            init_file_load_error(m_file_name);
            s_load_error += ex.what();

            const std::string& user_repository(m_kernel.get_user_repository_root());
            const std::size_t user_repository_len(m_kernel.get_user_repository_root().length() + 1);

            std::size_t pos;
            while ((pos = s_load_error.find(user_repository)) != std::string::npos)
              s_load_error.replace(pos, user_repository_len, "");
          }
      }
  }

private:

  OksKernel m_kernel;
  const std::string& m_file_name;

  // protect usage of copy constructor and assignment operator

private:

  OksValidateJob(const OksValidateJob&);
  OksValidateJob&
  operator=(const OksValidateJob&);

};


struct FoundCircularDependency
{
  unsigned int m_count;
  std::ostringstream m_text;

  FoundCircularDependency() :
      m_count(0)
  {
    ;
  }
} s_circular_dependency_message;


struct TestCircularDependency
{
  TestCircularDependency(const std::string * file)
  {
    p_set_includes.insert(file);
    p_vector_includes.push_back(file);
  }

  bool
  push(const std::string * file)
  {
    auto it = p_set_includes.insert(file);
    if (it.second == false)
      {
        std::ostringstream s;

        bool report = false;

        for (const auto& x : p_vector_includes)
          {
            if (x == *it.first)
              {
                s_circular_dependency_message.m_text << "\nCircular dependency [" << ++s_circular_dependency_message.m_count << "]:";
                report = true;
              }

            if (report)
              s_circular_dependency_message.m_text << '\n' << " - \"" << *x << "\"";
          }

        return false;
      }

    p_vector_includes.push_back(file);

    return true;
  }

  void
  pop()
  {
    p_set_includes.erase(p_vector_includes.back());
    p_vector_includes.pop_back();
  }

  std::vector<const std::string *> p_vector_includes;
  std::set<const std::string *, OksFile::SortByName> p_set_includes;
};


std::set<std::string>&
define_includes(const std::string& f, const std::set<std::string>& s, std::map<std::string, std::set<std::string>>& file_all_includes, std::map<std::string, std::set<std::string>>& file_explicit_includes, TestCircularDependency& cd_fuse)
{
  std::set<std::string>& all_includes = file_all_includes[f];

  if(all_includes.empty())
    {
      for(auto& x : s)
        {
          if(cd_fuse.push(&x))
            {
              all_includes.insert(x);

              std::set<std::string>& includes = define_includes(x, file_explicit_includes[x], file_all_includes, file_explicit_includes, cd_fuse);
              for(const auto& y : includes)
                all_includes.insert(y);

              cd_fuse.pop();
            }
        }
    }

  return all_includes;
}


int
main(int argc, char **argv)
{
  boost::program_options::options_description desc("This program validates OKS git repository for commit by pre-receive hook");

  std::vector<std::string> created, updated, deleted;
  bool circular_dependency_between_includes_is_error = true;
  bool verbose = false;
  std::string user;
  std::size_t pipeline_size = 4;

  try
    {
      std::vector<std::string> app_types_list;
      std::vector<std::string> segments_list;

      desc.add_options()
        ("add,a", boost::program_options::value<std::vector<std::string> >(&created)->multitoken(), "list of new OKS files and directories to be added to the repository")
        ("update,u", boost::program_options::value<std::vector<std::string> >(&updated)->multitoken(), "list of new OKS files and directories to be updated in the repository")
        ("remove,r", boost::program_options::value<std::vector<std::string> >(&deleted)->multitoken(), "list of new OKS files and directories to be removed from the repository")
        ("permissive-circular-dependencies-between-includes,C", "downgrade severity of detected circular dependencies between includes from errors to warnings")
        ("user,U", boost::program_options::value<std::string>(&user), "user id")
        ("threads-number,t", boost::program_options::value<std::size_t>(&pipeline_size)->default_value(pipeline_size), "number of threads used by validation pipeline")
        ("verbose,v", "Print debug information")
        ("help,h", "Print help message");

      boost::program_options::variables_map vm;
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

      if (vm.count("help"))
        {
          std::cout << desc << std::endl;
          return __Success__;
        }

      if (vm.count("permissive-circular-dependencies-between-includes"))
        circular_dependency_between_includes_is_error = false;

      if (vm.count("verbose"))
        verbose = true;

      boost::program_options::notify(vm);

    }
  catch (std::exception& ex)
    {
      std::cerr << "Command line parsing errors occurred:\n" << ex.what() << std::endl;
      return __BadCommandLine__;
    }


  try
    {
      OksKernel kernel;

      kernel.set_allow_duplicated_objects_mode(false);
      kernel.set_test_duplicated_objects_via_inheritance_mode(true);

      if(kernel.get_user_repository_root().empty())
        {
          std::cerr << "There is no OKS repository set (check TDAQ_DB_REPOSITORY)" << std::endl;
          return __NoRepository__;
        }

      std::filesystem::current_path(kernel.get_user_repository_root());

      auto start_usage = std::chrono::steady_clock::now();

      // directories

      std::set<std::string> directories;


      // file: explicit includes
      std::map<std::string, std::set<std::string>> file_explicit_includes;

      for (auto& p : std::filesystem::recursive_directory_iterator("."))
        if (std::filesystem::is_directory(p))
          directories.insert(p.path().native().substr(2));
        else if (std::filesystem::is_regular_file(p) && p.path().native().find("./.git") != 0 && p.path().native().find("./admin") != 0 && p.path().native().find("./README.md") != 0)
          kernel.get_includes(p.path().native(), file_explicit_includes[p.path().native().substr(2)], true);

      if (verbose)
        log_timestamp(Debug) << "scan " << file_explicit_includes.size() << " repository files in " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start_usage).count() / 1000. << " ms\n";


      auto start_usage2 = std::chrono::steady_clock::now();

      std::set<std::string> all_includes;

      // check every include exists
      for (const auto& f : file_explicit_includes)
        {
          for (const auto& i : f.second)
            {
              if(file_explicit_includes.find(i) != file_explicit_includes.end())
                {
                  all_includes.insert(i);
                }
              else
                {
                  std::cerr << "Cannot find file \"" << i << "\" included by \"" << f.first << "\"" << std::endl;
                  return __NoIncludedFile__;
                }
            }
        }

      if (verbose)
        log_timestamp(Debug) << "check existence of includes in " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start_usage2).count() / 1000. << " ms\n";


      auto start_usage3 = std::chrono::steady_clock::now();

      // file: all includes
      std::map<std::string, std::set<std::string>> file_all_includes;

      for(auto& x : file_explicit_includes)
        {
          TestCircularDependency cd_fuse(&x.first);
          define_includes(x.first, x.second, file_all_includes, file_explicit_includes, cd_fuse);
        }

      auto stop_usage3 = std::chrono::steady_clock::now();

      if (verbose)
        log_timestamp(Debug) << "calculated inclusion graph in " << std::chrono::duration_cast<std::chrono::microseconds>(stop_usage3-start_usage3).count() / 1000. << " ms\n";

      log_timestamp() << "process " << file_explicit_includes.size() << " repository files and their includes in " << std::chrono::duration_cast<std::chrono::microseconds>(stop_usage3-start_usage).count() / 1000. << " ms" << std::endl;


      if (s_circular_dependency_message.m_count)
        {
          log_timestamp((circular_dependency_between_includes_is_error == true ? Error : Warning)) << "Detected " << s_circular_dependency_message.m_count << " circular dependencies between includes of the repository files:" << s_circular_dependency_message.m_text.str() << std::endl;

          if (circular_dependency_between_includes_is_error == true)
            return __IncludesCircularDependencyError__;
        }

      if (ers::debug_level() >= 2)
        {
          std::ostringstream text;
          text << "ALL INCLUDES:\n";

          for (const auto& x : file_all_includes)
            {
              text << "FILE \"" << x.first << "\" has " << x.second.size() << " includes:\n";

              for (const auto& y : x.second)
                text << " - \"" << y << "\"\n";
            }

          TLOG_DEBUG(2) << text.str();
        }

      OksPipeline pipeline(pipeline_size);

      // do not run check for README file
      auto ignore_files = [](std::string& x)
      {
        static const std::string readme_file("README.md");
        return x != readme_file;
      };

      // all modified file paths
      std::set<std::string> modified;
      std::copy_if(created.begin(), created.end(), std::inserter(modified, modified.end()), ignore_files);
      std::copy_if(updated.begin(), updated.end(), std::inserter(modified, modified.end()), ignore_files);

      // validate independently every created or updated file
      for (const auto& x : modified)
        pipeline.addJob(new OksValidateJob(kernel, x));

      std::copy_if(deleted.begin(), deleted.end(), std::inserter(modified, modified.end()), ignore_files);

      for (const auto& f : file_explicit_includes)
        if (all_includes.find(f.first) == all_includes.end())
          {
            if (modified.empty() == false)
              {
                if (modified.find(f.first) == modified.end())
                  {
                    const auto& file_includes = file_all_includes[f.first];

                    bool found = false;

                    for (const auto& x : modified)
                      if (file_includes.find(x) != file_includes.end())
                        {
                          found = true;
                          TLOG_DEBUG(1) <<  "file \"" << f.first << "\" contains modified include \"" << x << '\"';
                          break;
                        }

                    if(found == false)
                      {
                        TLOG_DEBUG(1) << "skip file \"" << f.first << '\"';
                        continue;
                      }
                  }
                else
                  {
                    TLOG_DEBUG(1) <<  "list of modified files contains file \"" << f.first << '\"';
                  }
              }

            if (modified.find(f.first) == modified.end())
              pipeline.addJob(new OksValidateJob(kernel, f.first));
          }

      pipeline.waitForCompletion();

      if (!s_load_error.empty())
        {
          log_timestamp(Error) << s_load_error << std::endl;
          return __ConsistencyError__;
        }
    }
  catch (exception & ex)
    {
      log_timestamp(Error) << "Caught oks exception:\n" << ex << std::endl;
      return __ExceptionCaught__;
    }
  catch (std::exception & e)
    {
      log_timestamp(Error) << "Caught standard C++ exception:\n" << e.what() << std::endl;
      return __ExceptionCaught__;
    }
  catch (...)
    {
      log_timestamp(Error) << "Caught unknown exception" << std::endl;
      return __ExceptionCaught__;
    }

  return __Success__;
}
