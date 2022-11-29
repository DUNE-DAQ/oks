#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

trace=0
tag=''
sha=''

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      ;;

    -h* | --he*)
      echo 'Usage: oks-tag.sh [-v] [-u user-repository-dir] [-h] -c sha -t tag'
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose       trace this script execution'
      echo '   -u | --user-rep-dir  define user repository directory'
      echo '   -c | --commit-hash   the commit checksum to be tagged'
      echo '   -t | --tag           the tag name'
      echo '   -h | --help          print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS tag utility can be used to tag existing commit.'
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
        echo " -> [oks-tag.sh]: export TDAQ_DB_USER_REPOSITORY=$TDAQ_DB_USER_REPOSITORY"
      fi
      ;;

    -t | --tag)
      shift
      tag="$1"
      ;;

    -c | --commit-hash)
      shift
      sha="$1"
      ;;

    *)
      break
      ;;

  esac
  shift
done

if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  echo 'ERROR [oks-tag.sh]: user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
  exit 1
fi

########################################################################################################################

if [ $trace -eq 1 ]
then
  echo " -> [oks-tag.sh]: cd ${TDAQ_DB_USER_REPOSITORY}"
fi

cd "${TDAQ_DB_USER_REPOSITORY}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-tag.sh]: cannot change directory to TDAQ_DB_USER_REPOSITORY"
  exit 1
fi

if [ -z "${sha}" ]
then
	echo "ERROR [oks-tag.sh]: the commit hash is not set"
	exit 1
fi

if [ -z "${tag}" ]
then
	echo "ERROR [oks-tag.sh]: the commit tag is not set"
	exit 1
fi


########################################################################################################################

echo "git tag ${tag} ${sha}"
git tag "${tag}" "${sha}"

if [ $? -ne 0 ]
then
	echo "ERROR [oks-tag.sh]: git tag has failed"
	exit 1
fi

echo "git push origin ${tag}"
git push origin "${tag}"

if [ $? -ne 0 ]
then
	echo "ERROR [oks-tag.sh]: git push has failed"
	exit 1
fi


########################################################################################################################
