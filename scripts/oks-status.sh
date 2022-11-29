#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -u* | --user-re*)
      shift
      TDAQ_DB_USER_REPOSITORY="$1"
      export TDAQ_DB_USER_REPOSITORY
      ;;

    -h* | --he*)
      echo 'Usage: oks-status.sh [-u user-repository-dir] [-h]'
      echo ''
      echo 'Arguments/Options:'
      echo '   -u | --user-rep-dir  define user repository directory'
      echo '   -h | --help          print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS git status utility allows to get list of updated, removed and added repository files.'
      echo '   The variable TDAQ_DB_USER_REPOSITORY defines the user database repository.'
      echo ''
      exit 0
      ;;
  esac
  shift
done

if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  echo 'ERROR [oks-status.sh]: user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
  exit 1
fi

########################################################################################################################

cd "${TDAQ_DB_USER_REPOSITORY}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-status.sh]: cannot change directory to user repository ${TDAQ_DB_USER_REPOSITORY}"
  exit 1
fi

########################################################################################################################

echo "git status --porcelain"
ff=`git status --porcelain`

if [ $? -ne 0 ]
then
  echo "ERROR [oks-status.sh]: git status has failed with code $?"
  exit 1
fi

if [ ! -z "$ff" ]
then
  echo "$ff" | grep '\.xml$'
fi

exit 0

########################################################################################################################
