/*
 * DUNE DAQ modification notice:
 * This file has been modified from the original ATLAS oks source for the DUNE DAQ project.
 * Fork baseline commit: oks-08-03-04 (2022-04-14).
 * Renamed since fork: yes (from bin/oks_git_repository.cpp to apps/oks_git_repository.cxx).
 */

/**
 *  \file oks_git_repository.cpp
 *
 *  This file is part of the OKS package.
 *  Author: <Igor.Soloviev@cern.ch>
 */

#include "oks/kernel.hpp"

#include <string.h>
#include <stdlib.h>

using namespace dunedaq::oks;

int
main(int argc, char **argv)
{
  if(argc > 1)
    {
      if(!strcmp("-h", argv[1]) || !strcmp("--help", argv[1]))
        {
          std::cout << "This program extracts OKS git repository from TDAQ_DB_REPOSITORY process environment variable." << std::endl;
          return EXIT_SUCCESS;
        }
    }

  std::cout << OksKernel::get_repository_root() << std::endl;

  return EXIT_SUCCESS;
}
