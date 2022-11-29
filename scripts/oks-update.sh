#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

trace=0
hash=''
tag=''
date=''
option=''

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      ;;

    -f | --discard* | --force*)
      option=' --force'
      ;;

    -m | --merge*)
      option=' --merge'
      ;;

    -h* | --he*)
      echo 'Usage: oks-update.sh [-v] [-u user-repository-dir] [-c commit-hash] [-t tag] [-d date] [-f | -m] [-h] '
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose            trace this script execution'
      echo '   -u | --user-rep-dir       define user repository directory'
      echo '   -c | --commit-hash        checkout repository with given commit hash'
      echo '   -t | --tag                checkout repository with given tag'
      echo '   -d | --date               checkout repository for given timestamp'
      echo '   -f | --force | --discard  discard local changes (force update)'
      echo '   -m | --merge              merge changes'
      echo '   -h | --help               print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS update utility allows to update files and directories in user git repository.'
      echo '   Without options it updates to HEAD of the master branch.'
      echo '   Otherwise options allow to choose concrete commit hash, tag or timestamp.'
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
        echo " -> [oks-update.sh]: export TDAQ_DB_USER_REPOSITORY=$TDAQ_DB_USER_REPOSITORY"
      fi
      ;;

    -t | --tag)
      shift
      tag="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-update.sh]: tag: ${tag}"
      fi
      ;;

    -d | --date)
      shift
      date="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-update.sh]: date: ${date}"
      fi
      ;;

    -c | --commit-hash | --hash)
      shift
      hash="$1"
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-update.sh]: commit hash: ${hash}"
      fi
      ;;

  esac
  shift
done

if [ ! -z "${hash}" ] && [ ! -z "${tag}" ]
then
	echo "ERROR [oks-update.sh]: cannot use commit-hash and tag parameters simultaneously"
	exit 1
fi

if [ ! -z "${hash}" ] && [ ! -z "${date}" ]
then
	echo "ERROR [oks-update.sh]: cannot use commit-hash and date parameters simultaneously"
	exit 1
fi

if [ ! -z "${tag}" ] && [ ! -z "${date}" ]
then
	echo "ERROR [oks-update.sh]: cannot use tag and date parameters simultaneously"
	exit 1
fi


if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  echo 'ERROR [oks-update.sh]: user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
  exit 1
fi

########################################################################################################################

if [ $trace -eq 1 ]
then
  echo " -> [oks-update.sh]: cd ${TDAQ_DB_USER_REPOSITORY}"
fi

cd "${TDAQ_DB_USER_REPOSITORY}"

if [ $? -ne 0 ]
then
  echo "ERROR [oks-update.sh]: cannot change directory to TDAQ_DB_USER_REPOSITORY"
  exit 1
fi

########################################################################################################################

git_cmd="git checkout${option}"

if [ ! -z "${tag}" ]
then
	echo "${git_cmd} -q tags/${tag}"
	${git_cmd} -q tags/${tag}
	if [ $? -ne 0 ]
	then
		echo "ERROR [oks-update.sh]: git checkout has failed"
		exit 1
	fi
else
	if [ ! -z "${date}" ]
	then
		if [ $trace -eq 1 ]
		then
			echo " -> [oks-update.sh]: calculate commit hash for date ${date}: git rev-list -1 --before="${date}" master"
		fi
		
		hash=$(git rev-list -1 --before="${date}" master)
		
		if [ $? -ne 0 ]
		then
			echo "ERROR [oks-update.sh]: git rev-list has failed"
			exit 1
		fi
	fi


	if [ ! -z "${hash}" ] && [ ! "${hash}" == "origin/master" ]
	then
	    echo "${git_cmd} -q -B master ${hash}"
	    ${git_cmd} -q -B master ${hash}
	    if [ $? -ne 0 ]
	    then
		    echo "ERROR [oks-update.sh]: git checkout has failed"
		    exit 1
	    fi
    else
    	echo "${git_cmd} -q -B master origin/master"
    	${git_cmd} -q -B master origin/master
	    if [ $? -ne 0 ]
	    then
		    echo "ERROR [oks-update.sh]: git checkout has failed"
		    exit 1
	    fi
    fi
fi


if [ $trace -eq 1 ]
then
  echo " -> [oks-update.sh]: update completed"
fi

echo "update oks version `git rev-parse HEAD`"

########################################################################################################################
