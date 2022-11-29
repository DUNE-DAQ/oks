#!/bin/sh

########################################################################################################################

PATH=$PATH:/bin:/usr/bin:/usr/local/bin
export PATH

########################################################################################################################

trace=0
message=''
file=''
branch=''

########################################################################################################################

# temporary branch name
temp='temp_oks_commit_branch'

########################################################################################################################

# An error exit function

error_exit()
{
  echo "ERROR [oks-commit.sh]: $1" 1>&2
  exit 1
}

cleanup_exit()
{
  echo "ERROR [oks-commit.sh]: $1" 1>&2
  echo "cleanup:"

  echo "git checkout $branch"
  git checkout "$branch"

  echo "git branch -D $temp"
  git branch -D $temp >/dev/null 2>&1
  
  exit 2
}

undo_merge()
{
  echo "ERROR [oks-commit.sh]: $1" 1>&2
  echo "cleanup and undo merge:"

  echo 'git rebase --skip'
  git rebase --skip

  exit 3
}

undo_exit()
{
  echo "ERROR [oks-commit.sh]: $1" 1>&2
  echo "cleanup and undo commit:"

  echo "git checkout $branch"
  git checkout "$branch"

  echo "git branch -D $temp"
  git branch -D $temp >/dev/null 2>&1

  echo "git reset HEAD~"
  git reset HEAD~

  exit 4
}

########################################################################################################################

pull()
{
  if [ "$branch" = 'master' ] || git ls-remote --exit-code --heads origin "$branch"
  then
    echo "git pull --no-edit -r origin $branch"
    git pull --no-edit -r origin "$branch" || undo_merge 'cannot merge changes with oks git repository'
  fi
}

########################################################################################################################

commit_cmd='git'

while (test $# -gt 0)
do
  case "$1" in
    -v | --verbose)
      trace=1
      ;;

    -u* | --user-re*)
      shift
      TDAQ_DB_USER_REPOSITORY="$1"
      export TDAQ_DB_USER_REPOSITORY
      if [ $trace -eq 1 ]
      then
        echo " -> [oks-commit.sh]: export TDAQ_DB_USER_REPOSITORY=$TDAQ_DB_USER_REPOSITORY"
      fi
      ;;

    -m | --message)
      shift
      message="$1"
      ;;

    -f | --file)
      shift
      file="$1"
      ;;

    -e )
      shift
      commit_cmd="$commit_cmd -c user.email='$1'"
      ;;

    -n )
      shift
      commit_cmd="$commit_cmd -c user.name='$1'"
      ;;

    -h* | --he*)
      echo 'Usage: oks-commit.sh [-v] [-u user-repository-dir] [-h] -m message | -f commit-message-file'
      echo ''
      echo 'Arguments/Options:'
      echo '   -v | --verbose       trace this script execution'
      echo '   -u | --user-rep-dir  define user repository directory'
      echo '   -h | --help          print this message'
      echo '   -m | --message       user commit message (avoid quotes inside message)'
      echo '   -f | --file          take the commit message from the given file'
      echo ''
      echo 'Description:'
      echo '   The OKS commit utility allows to commit changes in OKS database stored on git server.'
      echo '   The variable TDAQ_DB_USER_REPOSITORY defines the user database repository.'
      echo ''
      exit 0
      ;;
  esac
  shift
done

if [ -z "${message}" ] && [ -z "${file}" ]
then
  error_exit 'there is no commit message or file with commit message'
fi

if [ -z ${TDAQ_DB_USER_REPOSITORY} ]
then
  error_exit 'user repository is not set; use TDAQ_DB_USER_REPOSITORY or -u option'
fi

########################################################################################################################

if [ $trace -eq 1 ]
then
  echo " -> [oks-commit.sh]: cd ${TDAQ_DB_USER_REPOSITORY}"
fi

cd "${TDAQ_DB_USER_REPOSITORY}" || error_exit "cannot change directory to user repository ${TDAQ_DB_USER_REPOSITORY}"

echo "git rev-parse --abbrev-ref HEAD"
branch=`git rev-parse --abbrev-ref HEAD` || error_exit "cannot detect branch name"
echo $branch

########################################################################################################################

echo "git checkout -b $temp"
git checkout -b $temp || error_exit 'cannot create temporary branch'

echo 'for f in `git ls-files -o | grep "\.xml$"`; do echo "git add $f"; git add "$f"; done'
for f in `git ls-files -o | grep '\.xml$'`; do echo "git add $f"; git add "$f"; done
if [ $? -ne 0 ]; then error_exit 'git add has failed'; fi

echo 'git update-index --refresh'
git update-index --refresh

echo 'git diff-index --quiet HEAD'
git diff-index --quiet HEAD

if [ $? -ne 0 ]
then	
  commit_cmd="$commit_cmd commit -a"
  if [ ! -z "${message}" ]
  then
    commit_cmd="${commit_cmd} -m \"${message}\""
  elif [ ! "${file}" = "/dev/null" ]
  then
    commit_cmd="${commit_cmd} -F \"${file}\""
  fi

  echo "$commit_cmd"
  eval "$commit_cmd"

  if [ $? -ne 0 ]; then cleanup_exit 'git commit has failed'; fi

  echo "git checkout $branch"
  git checkout "$branch" || undo_exit "cannot checkout $branch branch"

  echo "git merge --no-edit $temp"
  git merge --no-edit $temp || undo_exit 'failed to merge changes'

  echo "git branch -d $temp"
  git branch -d $temp || undo_exit 'failed to remove temporary branch'

  pull

  git_push_out=`mktemp`
  echo "git push origin $branch"
  git push origin "$branch" 2>&1 | tee $git_push_out
  
  if [ ${PIPESTATUS[0]} -ne 0 ]
  then
    grep -q -i 'error.*is at .* but expected .*' $git_push_out
    if [ $? -ne 0 ] ; then undo_exit 'cannot store changes in oks git repository'; fi

    echo 'WARNING [oks-commit.sh]: detected git lock conflict, try to recover ...'

    pull
    
    echo "git push origin $branch"
    git push origin "$branch" || undo_exit 'git push has failed second time'
  fi

  rm $git_push_out
else
  echo 'nothing to commit'

  echo "git checkout $branch"
  git checkout "$branch" || undo_exit "cannot checkout $branch branch"

  echo "git branch -d $temp"
  git branch -d $temp || undo_exit 'failed to remove temporary branch'
fi

if [ $trace -eq 1 ]
then
  echo " -> [oks-commit.sh]: commit completed"
fi

echo "commit oks version `git rev-parse HEAD`"

########################################################################################################################
