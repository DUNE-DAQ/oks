# DUNE DAQ modification notice:
# This file has been modified from the original ATLAS oks source for the DUNE DAQ project.
# Fork baseline commit: oks-08-03-04 (2022-04-14).
# Renamed since fork: no.

cmake_minimum_required(VERSION 3.12)
project(oks VERSION 1.4.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(Boost COMPONENTS thread date_time regex program_options REQUIRED)
find_package(ers REQUIRED)
find_package(logging REQUIRED)
find_package(okssystem REQUIRED)
find_package(CLI11 REQUIRED)
find_package(fmt)


# JCF, Oct-18-2022: stripped out functionality which requires authentication packages daq_tokens and pam
daq_add_library( *.cpp LINK_LIBRARIES ers::ers Boost::thread Boost::date_time Boost::regex stdc++fs okssystem::okssystem logging::logging)

daq_add_python_bindings(*.cpp)


daq_add_application(oks_dump oks_dump.cxx LINK_LIBRARIES oks)
daq_add_application(oks_check_schema oks_check_schema.cxx LINK_LIBRARIES CLI11::CLI11 fmt::fmt oks)
daq_add_application(oks_git_repository oks_git_repository.cxx LINK_LIBRARIES oks)
daq_add_application(oks_clone_repository oks_clone_repository.cxx LINK_LIBRARIES oks Boost::program_options)
daq_add_application(oks_validate_repository oks_validate_repository.cxx LINK_LIBRARIES oks Boost::program_options)



daq_add_application(oks_save_all oks_save_all.cxx TEST LINK_LIBRARIES oks CLI11::CLI11 fmt::fmt)
daq_add_application(oks_check_inheritance oks_check_inheritance.cxx TEST LINK_LIBRARIES oks CLI11::CLI11 fmt::fmt)


daq_install()
