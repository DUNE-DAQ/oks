#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

trace=0
since=''
until=''
num=''
args=''

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      ;;

    -h* | --he*)
      echo 'Usage: oks-log.sh [-v] [-u user-repository-dir] [-h] [-s date/time] [-t date/time] [-n num] '
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose       trace this script execution'
      echo '   -u | --user-rep-dir  define user repository directory'
      echo '   -h | --help          print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS log utility shows the details of git repository commit logs (hash, author, date and updated files).'
      echo '   The variable TDAQ_DB_USER_REPOSITORY defines the user database repository.'
      echo ''
      exit 0
      ;;

    -u* | --user-re*)
      shift
      TDAQ_DB_USER_REPOSITORY="$1"
      export TDAQ_DB_USER_REPOSITORY
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-log.sh]: export TDAQ_DB_USER_REPOSITORY=$TDAQ_DB_USER_REPOSITORY"
      fi
      ;;

    *)
      break
      ;;

  esac
  shift
done

if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  echo 'ERROR [oks-log.sh]: user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
  exit 1
fi

########################################################################################################################

if [ $trace -eq 1 ]
then
  echo " -> [oks-log.sh]: cd ${TDAQ_DB_USER_REPOSITORY}"
fi

cd "${TDAQ_DB_USER_REPOSITORY}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-log.sh]: cannot change directory to TDAQ_DB_USER_REPOSITORY"
  exit 1
fi

########################################################################################################################

echo "git fetch --all"
git fetch --all

if [ $? -ne 0 ]
then
	echo "ERROR [oks-log.sh]: git fetch has failed"
	exit 1
fi

echo "git log -m --date=raw --pretty=format:'%H|%an|%ad|%s' --first-parent master --name-only $@"
git log -m --date=raw --pretty=format:"%H|%an|%ad|%s" --first-parent master --name-only $@

if [ $? -ne 0 ]
then
	echo "ERROR [oks-log.sh]: git log has failed"
	exit 1
fi

########################################################################################################################
