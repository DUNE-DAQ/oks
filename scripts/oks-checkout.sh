#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

if [ -z "${TDAQ_DB_REPOSITORY}" ]
then
  echo 'ERROR [oks-checkout.sh]: variable TDAQ_DB_REPOSITORY is not set'
  exit 1
fi

git_repo=`oks_git_repository`

if [ -z "${git_repo}" ]
then
  echo 'ERROR [oks-checkout.sh]: failed to get oks get repository'
  exit 1
fi

########################################################################################################################

trace=0
hash=''
tag=''
date=''
branch='master'

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      echo " -> [oks-checkout.sh]: git repository: ${git_repo}"
      ;;

    -u* | --user-re*)
      shift
      TDAQ_DB_USER_REPOSITORY="$1"
      export TDAQ_DB_USER_REPOSITORY
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-checkout.sh]: export TDAQ_DB_USER_REPOSITORY=$TDAQ_DB_USER_REPOSITORY"
      fi
      ;;

    -t | --tag)
      shift
      tag="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-checkout.sh]: tag: ${tag}"
      fi
      ;;

    -d | --date)
      shift
      date="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-checkout.sh]: date: ${date}"
      fi
      ;;

    -c | --commit-hash | --hash)
      shift
      hash="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-checkout.sh]: commit hash: ${hash}"
      fi
      ;;

    -b | --branch)
      shift
      branch="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-checkout.sh]: branch: ${branch}"
      fi
      ;;

    -h* | --he*)
      echo 'Usage: oks-checkout.sh [-v] [-u user-repository-dir] [-b branch] [-c commit-hash] [-t tag] [-d date] [-h]'
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose       trace this script execution'
      echo '   -u | --user-rep-dir  define user repository directory'
      echo '   -b | --branch        checkout branch, create if does not exist, use master branch by default'
      echo '   -c | --commit-hash   checkout repository with given commit hash'
      echo '   -t | --tag           checkout repository with given tag'
      echo '   -d | --date          checkout repository for given timestamp'
      echo '   -h | --help          print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS checkout utility allows to checkout files and directories from OKS'
      echo '   git repository into user repository, where they can be modified and committed back.'
      echo '   The variable TDAQ_DB_REPOSITORY defines the OKS database repository directory of git repository.'
      echo '   The variable TDAQ_DB_USER_REPOSITORY defines the user database repository.'
      echo '   If the latter one is not set, the files are check out into current working directory.'
      echo ''
      exit 0
      ;;
  esac
  shift
done

if [ ! -z "${hash}" ] && [ ! -z "${tag}" ]
then
	echo "ERROR [oks-checkout.sh]: cannot use commit-hash and tag parameters simultaneously"
	exit 1
fi

if [ ! -z "${hash}" ] && [ ! -z "${date}" ]
then
  echo "ERROR [oks-checkout.sh]: cannot use commit-hash and date parameters simultaneously"
  exit 1
fi

if [ ! -z "${tag}" ] && [ ! -z "${date}" ]
then
  echo "ERROR [oks-checkout.sh]: cannot use tag and date parameters simultaneously"
  exit 1
fi

if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  echo 'ERROR [oks-checkout.sh]: user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
  exit 1
fi

########################################################################################################################

if [ $trace -eq 1 ]
then
  echo " -> [oks-checkout.sh]: cd ${TDAQ_DB_USER_REPOSITORY}"
fi

cd "${TDAQ_DB_USER_REPOSITORY}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-checkout.sh]: cannot change directory to user repository ${TDAQ_DB_USER_REPOSITORY}"
  exit 1
fi

########################################################################################################################

echo "git clone -q -n ${git_repo} ."
git clone -q -n "${git_repo}" .

if [ $? -ne 0 ]
then
  echo "ERROR [oks-checkout.sh]: git clone has failed"
  exit 1
fi

command="git checkout -q -B ${branch}"

if [ ! -z "${tag}" ]
then
  echo "$command tags/${tag}"
  $command tags/${tag}
  if [ $? -ne 0 ]
  then
    echo "ERROR [oks-checkout.sh]: git checkout has failed"
    exit 1
  fi
else
  if [ ! -z "${date}" ]
  then
    if [ $trace -eq 1 ]
    then
      echo " -> [oks-checkout.sh]: calculate commit hash for date ${date}: git rev-list -1 --before="${date}" ${branch}"
    fi

    hash=$(git rev-list -1 --before="${date}" "${branch}")

    if [ $? -ne 0 ]
    then
      echo "ERROR [oks-checkout.sh]: checkout has failed"
      exit 1
    fi
  fi

  if [ -z "${hash}" ] && git ls-remote --exit-code --heads origin "$branch"
  then
    hash="origin/${branch}"
  fi

  echo "$command ${hash}"
  $command ${hash}
  if [ $? -ne 0 ]
  then
    echo "ERROR [oks-checkout.sh]: git checkout has failed"
    exit 1
  fi
fi

if [ $trace -eq 1 ]
then
  echo " -> [oks-checkout.sh]: checkout completed"
fi

echo "checkout oks version `git rev-parse HEAD`"

########################################################################################################################
