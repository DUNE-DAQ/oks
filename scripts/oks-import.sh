#!/bin/sh

########################################################################################################################

if [ -z "${TDAQ_DB_REPOSITORY}" ]
then
  echo 'ERROR [oks-import.sh]: variable TDAQ_DB_REPOSITORY is not set'
  exit 1
fi

git_repo=`oks_git_repository`

if [ -z "${git_repo}" ]
then
  echo 'ERROR [oks-import.sh]: failed to get oks get repository'
  exit 1
fi

########################################################################################################################

trace=0
update=1
message=''
file=''

user_files="/tmp/oks.import.`whoami`.$$.txt"

########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      echo " -> [oks-import.sh]: git repository: ${git_repo}"
      ;;

    -n | --dry-run)
      update=0
      ;;

    -m | --message)
      shift
      message="$1"
      ;;

    -f | '--file')
      shift
      file="$1"
      ;;

    -h* | --he*)
      echo 'Usage: oks-import.sh [-v] [-t] [-n] -m message | -f commit-message-file what ...'
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose       trace this script execution'
      echo '   -n | --dry-run       print commands to update git repository, but do not commit'
      echo '   -m | --message       commit message'
      echo '   -f | --file          take the commit message from the given file'
      echo '   -h | --help          print this message'
      echo '   what+                list of directories and files to be imported'
      echo ''
      echo 'Description:'
      echo '   Import files into OKS git repository.'
      echo '   Only new and different files will be imported.'
      echo ''
      echo '   The current working directory has to correspond to repository root, e.g.:'
      echo '     bash$ cd ${TDAQ_INST_PATH}/share/data'
      echo '     bash$ oks-import.sh -m "commit message" daq/schema daq/sw/*.data.xml daq/segments/common-environment.data.xml'
      echo '   or:'
      echo '     bash$ cd /atlas/oks/tdaq-09-00-00'
      echo '     bash$ oks-import.sh -m "commit message" det-x'
      echo ''
      exit 0
      ;;

    *)
      if [ -d "$1" ]
      then
        echo find "$1" -name '*.xml'
        find "$1" -name '*.xml' >> $user_files
      elif [ -f "$1" ]
      then
        echo "$1" >> $user_files
      else
        echo "ERROR: parameter $1 is not directory or file"
        exit 1
      fi

  esac
  shift
done

src_path=`pwd`

if [ -z "${message}" ] && [ -z "${file}" ]
then
    echo "ERROR [oks-import.sh]: there is no commit message or file with commit message"
	exit 1
fi

if [ ! -f "$user_files" ]
then
  echo "ERROR [oks-import.sh]: there are no files for import"
  exit 1
fi	

########################################################################################################################
#Sort user files

user_files2="$user_files.tmp"
cat "$user_files" | sort -u > $user_files2
mv "$user_files2" "$user_files"

if [ $trace -eq 1 ]
then
  echo '-------------------------------------------------------------------'
  echo 'User files:'
  cat "$user_files"
  echo '-------------------------------------------------------------------'
fi


########################################################################################################################

work_dir=`mktemp -d --tmpdir oks.import.XXXXXX`

echo "create working area directory ${work_dir}"

echo "git clone ${git_repo} ${work_dir}"
git clone "${git_repo}" ${work_dir}

if [ $? -ne 0 ]
then
  echo "ERROR [oks-import.sh]: git clone has failed"
  exit 1
fi

cleanup_git_repo()
{
	echo "rm -rf ${work_dir}"
	rm -rf ${work_dir}
	echo "rm -f ${user_files}"
	rm -f ${user_files}
}

trap cleanup 1 2 3 6

cleanup()
{
	cleanup_git_repo
    echo "Done cleanup ... quitting."
    exit 1
}

echo '-------------------------------------------------------------------'
echo "Database git repository: ${git_repo}"
echo "User repository area: ${work_dir}"

########################################################################################################################

echo 'Process user files:'

old_tdaq_db_repo=${TDAQ_DB_REPOSITORY}
unset TDAQ_DB_REPOSITORY

TDAQ_DB_PATH=${src_path}:${work_dir}
export TDAQ_DB_PATH

for f in `cat $user_files`
do
  rf="${work_dir}/$f"
	    
  if [ -f ${rf} ]
  then
    diff -q -B -I '^<info name=' -I '^ <comment ' -I '^<comments>' -I '^</comments>' $f $rf > /dev/null
    if [ $? -eq 0 ]
    then
      echo " [=] file $f exists in repository and there is no difference"
    else
      echo " [~] file $f exists in repository and is different"
      echo "   cp -f ${src_path}/$f $rf"
      cp -f ${src_path}/$f $rf
    fi
  else
    echo " [+] file $f does not exist in repository"
    echo "   mkdir -p $(dirname $rf) && cp -f ${src_path}/$f $rf"
    mkdir -p $(dirname $rf) && cp -f ${src_path}/$f $rf
  fi
done

########################################################################################################################

if [ $update -eq 1 ]
then
  TDAQ_DB_REPOSITORY=${old_tdaq_db_repo}
  export TDAQ_DB_REPOSITORY
    
if [ -z "${file}" ]
  then
  	echo "oks-commit.sh -u ${work_dir} -m \"${message}\""
    oks-commit.sh -u ${work_dir} -m "${message}"
  else
    echo "oks-commit.sh -u ${work_dir} -f \"${file}\""
    oks-commit.sh -u ${work_dir} -f "${file}"
  fi
fi

########################################################################################################################

cleanup_git_repo
exit 0

########################################################################################################################
