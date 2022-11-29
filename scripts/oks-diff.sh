#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

trace=0
unmerged=0
sha1=''
sha2=''

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      ;;

    -n | --unmerged)
      unmerged=1
      ;;

    -s | --sha)
      shift
      sha1="$1"
      shift
      sha2="$1"
      ;;

    -h* | --he*)
      echo 'Usage: oks-diff.sh [-v] [-u user-repository-dir] [--unmerged] [--sha sha1 sha2] [-h]'
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose          trace this script execution'
      echo '   -u | --user-rep-dir     define user repository directory'
      echo '   -n | --unmerged         show names of unmerged files'
      echo '   -s | --sha sha1 sha2    show names of files modified between revisions sha1 and sha2'
      echo '   -h | --help             print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS diff utility shows unmerged files or files modified between revisions.'
      echo '   The variable TDAQ_DB_USER_REPOSITORY can be used to define the user database repository.'
      echo ''
      exit 0
      ;;

    -u* | --user-re*)
      shift
      TDAQ_DB_USER_REPOSITORY="$1"
      export TDAQ_DB_USER_REPOSITORY
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-diff]: export TDAQ_DB_USER_REPOSITORY=$TDAQ_DB_USER_REPOSITORY"
      fi
      ;;
  esac
  shift
done

if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  echo 'ERROR [oks-diff.sh]: user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
  exit 1
fi

########################################################################################################################

if [ $trace -eq 1 ]
then
  echo " -> [oks-diff.sh]: cd ${TDAQ_DB_USER_REPOSITORY}"
fi

########################################################################################################################

cd "${TDAQ_DB_USER_REPOSITORY}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-diff.sh]: cannot change directory to TDAQ_DB_USER_REPOSITORY"
  exit 1
fi

########################################################################################################################

opts=''

if [ "$unmerged" -eq 1 ]
then
  opts="--diff-filter=U"
elif [ ! -z $sha2 ]
then
  opts="--diff-filter=M $sha1 $sha2"
  echo "git fetch --all"
  git fetch --all
  if [ $? -ne 0 ]
  then
	echo "ERROR [oks-diff.sh]: git fetch has failed"
	exit 1
  fi
else
  echo "ERROR [oks-diff.sh]: choose unmerged or sha option"
  exit 1
fi

########################################################################################################################

echo "git diff --name-only $opts"
git diff --name-only $opts
if [ $? -ne 0 ]
then
	echo "ERROR [oks-diff.sh]: git diff has failed"
	exit 1
fi

########################################################################################################################
