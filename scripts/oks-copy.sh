#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

if [ -z "${TDAQ_DB_REPOSITORY}" ]
then
  echo 'ERROR [oks-copy.sh]: variable TDAQ_DB_REPOSITORY is not set'
  exit 1
fi

git_repo=`oks_git_repository`

if [ -z "${git_repo}" ]
then
  echo 'ERROR [oks-copy.sh]: failed to get oks get repository'
  exit 1
fi

########################################################################################################################

trace=0
source=''
destination=''
hash=''

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      ;;

    -s | --source)
      shift
      source="$1"
      ;;

    -d | --destination)
      shift
      destination="$1"
      ;;
      
    -c | --commit-hash | --hash)
      shift
      hash="$1"
      ;;

    -h* | --he*)
      echo 'Usage: oks-copy.sh -s source-dir -d destination-dir -c commit-hash [-h]'
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose       trace this script execution'
      echo '   -s | --source        existing repository directory'
      echo '   -d | --destination   destination repository directory'
      echo '   -c | --commit-hash   checkout repository with given commit hash'
      echo '   -h | --help          print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS copy utility allows to copy git repository files and directories into a new location for update.'
      echo ''
      exit 0
      ;;
  esac
  shift
done


if [ -z "${source}" ] 
then
  echo "ERROR [oks-copy.sh]: the source directory is not defined"
  exit 1
fi

if [ -z "${destination}" ] 
then
  echo "ERROR [oks-copy.sh]: the destination directory is not defined"
  exit 1
fi

########################################################################################################################

echo "cd ${destination}"
cd "${destination}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-copy.sh]: cannot change directory to ${destination}"
  exit 1
fi

echo "git clone -q -n --reference ${source} ${git_repo} ."
git clone -q -n --reference ${source} ${git_repo} .

if [ $? -ne 0 ]
then
  echo "ERROR [oks-copy.sh]: git clone failed"
  exit 1
fi

echo "git checkout -q -B master ${hash}"
git checkout -q -B master ${hash}

if [ $? -ne 0 ]
then
  echo "ERROR [oks-copy.sh]: git checkout failed"
  exit 1
fi

echo "checkout oks version `git rev-parse HEAD`"

########################################################################################################################
