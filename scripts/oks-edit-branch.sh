#!/bin/sh

########################################################################################################################

path=''
create_path=''
editor='vim'
branch=''
log_message=''
commit=0

########################################################################################################################

# An error exit function

cleanup()
{
  if [ ! -z ${create_path} ]
  then
    echo "clean temporary repository area ${create_path}"
    rm -rf ${create_path}
  fi

}

error_exit()
{
  echo "ERROR [oks-edit-branch.sh]: $1" 1>&2
  cleanup
  exit 1
}

########################################################################################################################

if [ -z ${TDAQ_DB_REPOSITORY} ]
then
  error_exit 'oks git repository is not set; set TDAQ_DB_REPOSITORY or use oks-git-on'
fi

if [ ! -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  error_exit 'unset $TDAQ_DB_USER_REPOSITORY'
fi

if [ ! -z ${TDAQ_DB_VERSION} ]
then
  error_exit ' unset TDAQ_DB_VERSION'
fi


########################################################################################################################

while (test $# -gt 0)
do
  case "$1" in
    -c | --commit-anyway)
      commit=1
      ;;

    -p | --path)
      shift
      path="$1"
      ;;

    -e | --editor)
      shift
      editor="$1"
      ;;

    -m | --message)
      shift
      log_message="$1"
      ;;

    -b | --branch)
      shift
      branch="$1"
      ;;

    -h | --help)
      echo 'Usage: oks-edit-branch.sh [-c] [-p directory] [-e editor] [-m \"log message\"] -b branch file+'
      echo ''
      echo 'Arguments/Options:'
      echo '   -c | --commit-anyway  commit changes immediately after successful termination of the editor process; ask user otherwise'
      echo '   -p | --path           define existing empty repository directory; create temporary one otherwise'
      echo "   -e | --editor         specify name of text or config editor [default: \"$editor\"]"
      echo '   -b | --branch         branch name (new or already existing)'
      echo '   -m | --message        commit log message; ask user if not provided'
      echo '   -h | --help           print this message'
      echo ''
      echo 'Description:'
      echo '   The OKS edit branch utility allows to create new or checkout existing git branch, modify and commit changes into it.'
      echo '   The TDAQ_DB_REPOSITORY process environment variable has to be defined. The TDAQ_DB_USER_REPOSITORY and TDAQ_DB_VERSION'
      echo '   have to be unset. A branch name needs to be provided. If branch does not exist, it will be created.' 
      echo '   At least one existing repository file name needs to be provided. Such file or files will be open in an editor.'
      echo '   An editor can be a text editor or an oks configuration editor. If an editor requires command line options, they can be'
      echo '   passed as well. For example use -e "dbe -f" to start dbe with -f option, or -e "oks_data_editor --no-message-window" to'
      echo '   redirect output of oks data editor to standard out.'
      echo ''
      exit 0
      ;;

    *)
      file="$@"
      break
      ;;
  esac
  shift
done

if [ ! -v file ]
then
  error_exit 'there is no any file provided'
fi

for f in $file
do
  echo $f
done

if [ -z ${branch} ]
then
  error_exit 'there is no branch name parameter'
fi


########################################################################################################################

if [ -z "${path}" ]
then
  pushd `mktemp -d`
  create_path=`pwd`
else
  pushd "${path}" || error_exit "cannot cd ${path}"
fi

echo "cd `pwd`"

echo "oks_clone_repository -o . -b ${branch}"
oks_clone_repository -o . -b "${branch}" || error_exit 'cannot clone oks git repository'

echo TDAQ_DB_USER_REPOSITORY=`pwd` $editor $file
TDAQ_DB_USER_REPOSITORY=`pwd` $editor $file || error_exit 'editor failed'

########################################################################################################################

if [ $commit -eq 0 ]
then
  message="Your changes have not been committed to GIT.\nDo you want to commit now?\nIf you do, then, please, follow instructions on terminal."

  if hash kdialog 2> /dev/null; then
    kdialog --title="oks-edit-branch" --warningyesno "$message" 1>/dev/null 2>&1
    result=$?
    if [ $result -eq 0 ]; then
       commit=1
    fi
  elif hash zenity 2> /dev/null; then
    zenity --ellipsize --title="oks-edit-branch" --question --text="$message" 1>/dev/null 2>&1
    result=$?
    if [ $result -eq 0 ]; then
       commit=1
    fi    
  else    
    while true; do
       read -p "Do you wish commit changes? " answer
       case $answer in
       [Yy]* ) commit=1; break;;
       [Nn]* ) break;;
       * ) echo "Please answer yes or no.";;
       esac
    done
  fi
fi

if [ $commit -eq 0 ]
then
  echo 'exit without committing to git'
  cleanup
  exit 0
fi

########################################################################################################################

commit_cmd='oks-commit.sh -u `pwd`'

if [ ! -z "${log_message}" ]
then
  commit_cmd="${commit_cmd} -m \"${log_message}\""
else
  commit_cmd="${commit_cmd} -f /dev/null"
fi

########################################################################################################################

echo "$commit_cmd"
eval "$commit_cmd" || error_exit 'commit failed'

########################################################################################################################
