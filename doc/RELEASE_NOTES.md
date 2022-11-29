# OKS

## tdaq-09-03-00

### Show hidden oks data xml attributes for text editors

Use new oks_refresh utility with -x option to enforce all attributes to be shown.

### Deprecated OWL date/time format

The OWL date/time formats are deprecated and will be removed in next public release. ERS warning is reported by oks library, when a file containing data stored in deprecated format is loaded.
Such file can be refreshed using oks editors or new **oks_refresh** utility. For example:
```
$ git clone <url> .
$ TDAQ_DB_PATH=`pwd`:$TDAQ_DB_PATH oks_refresh -f ./<file>
$ git commit -m "refresh <file> to update date/time format" ./<file>
$ git push origin master
```

### Ordering of multi-value attributes and relationships

Use new **ordered** attribute for multi-value attribute or relationship to sort its data on save. The default value is **no**.
Implement sort of multi-value attributes and relationships in OKS data editor.


## tdaq-09-02-01

### Postponed changes

The postponed changes are stored into a git branch and should not be applied immediately in the ongoing data taking session. If needed, they can be shared with other experts and tested using DAQ control and configuration tools. At an appropriate moment the branch will be merged with master branch and deleted.

When branch is created, the gitea pull request (also known as [merge request](https://docs.gitlab.com/ee/user/project/merge_requests/) in gitlab) can be created. All gitea pull requests can be accessed from a single point and applied using gitea web interface. In future, the pull requests will be integrated with DAQ Shifter Assistant.

A git branch can be created and updated using git, oks and gitea web interfaces.

#### Create and update branch

Create or checkout a branch into temporary private area:
```
$ cd `oks_clone_repository -b <branch-name>`
```

A branch can also be created using git command line and gitea web interfaces.

#### Modify oks files

Edit changes using config tools, a text editor or gitea web interface, for example:
```
$ export TDAQ_DB_USER_REPOSITORY=`pwd`
$ oks_data_editor x/y/z
$ vim x/y/z
```

#### Commit and push modifications

Use oks commit:
```
$ oks-commit.sh -m 'put here your commit message' -u `pwd`
```
or git command line interface:
```
$ git commit -m 'put here your commit message' x/y/z
$ git push origin <branch-name>
```

#### Edit branch script

The oks edit branch utility allows to create new or checkout existing git branch, modify and commit changes into it. It performs steps similar to described in three above sections.
Run:
```
oks-edit-branch.sh -h
```
to get more information about command line parameters and required process environment settings.

#### Create gitea pull request (recommended)

Create a new pull request for your branch using gitea Web interface (you need to be authorised first) using URL:
```
<gitea-release-url>/compare/master...<branch-name>
```

#### Merge postponed changes
When the changes have to be merged with the master branch, use git command line interface:
```
$ git checkout master
$ git merge <branch-name>
$ git push origin master
$ git push origin --delete <branch-name>
$ git branch -D <branch-name>
```
or, if the gitea pull request was created, use gitea web interface. Browse all pull requests:
```
<gitea-release-url>/pulls
```
Select your pull request, merge changes and delete branch.

#### How to run partition or use config-based tools

In case, if the postponed changes need to be validated running DAQ partition, a shared file system has to be used to checkout a branch. For example create it in NFS area:
```
$ export SHARED=<some-path> # e.g. "/tbed/scratch/`whoami`/$CMTRELEASE" on TestBed or "$HOME/oks/$CMTRELEASE" on Point-1
$ mkdir -p $SHARED
$ cd $SHARED
$ oks_clone_repository -b <branch-name> -o .
$ chgrp -R zp .
$ chmod -R g+w .
$ export TDAQ_DB_USER_REPOSITORY=`pwd`
```
Then edit and commit changes to branch and run setup:
```
$ setup_daq <path-to-partition-file> <partition-name>
```

### Undo commit

If there is a need to undo already committed and pushed changes, the [git revert](https://git-scm.com/docs/git-revert) command has to be used.

To undo first commit, one has to:
1. clone git repository
2. find hash of the wrong commit
3. run the revert git command with this hash
4. push changes to git

For example:

* clone repository and run [git log](https://git-scm.com/docs/git-log) to see details of recent commits:
```
$ cd `oks_clone_repository`
$ git log -5
```

* find wrong commit in above log, run git revert command and push changes:
```
$ git revert --no-edit ab12... # use real commit hash
$ git push origin master
```

At this point the wrong commit is undone. The repository is fixed and its contains information about both, the wrong commit and its reverting.


## tdaq-09-01-00

Jira: [ADTCC-214](https://its.cern.ch/jira/browse/ADTCC-214), [ADTCC-226](https://its.cern.ch/jira/browse/ADTCC-226) and [ADTCC-227](https://its.cern.ch/jira/browse/ADTCC-227) 

[TWiki page](https://twiki.cern.ch/twiki/bin/view/Atlas/DaqHltOks#4_OKS_Git_Repository).


***Replace cvs repository by git repository.***

The oks repository is stored on a git server. To access oks configuration the git repository is cloned into a temporal area. When changes are committed, the git server validates them verifying consistency of all repository files and checking user permissions as defined by the Access Manager policy. If commit is successful, the git server updates latest version of oks files on the filesystem (git repository mapping). Contrary to cvs implementation, the repository mapping is not required to access oks data using git repository. It is only implemented for convenience of users (fast browse of files) or filesystem-based access.

This is the main difference between oks git and cvs repository implementations:
* the oks git implementation uses git database to store and access oks files (like DBMS) and ignores the files on the filesystem;
* the oks cvs implementation used filesystem to store and access oks files.

The ignorance of files on the filesystem by oks git is done on purpose to preserve used configurations, and to implement configuration archiving on top of git database.

If user needs to access configuration from files stored on filesystem, it is necessary to disable oks git as explained in next section.

### User git repository permissions

The permissions to update files in oks git repository are defined by the [Access Manager](https://twiki.cern.ch/twiki/bin/view/Atlas/DaqHltAccessManager) policy rules and enabled user roles.

### Environment Variables

The TDAQ_DB_REPOSITORY is the only setting that enables or disables use of the oks git repository. If it is set, the oks git repository is used. If not set, the filesystem repositories are used.

There are 3 shell functions available after release setup on TestBed and Point-1 to enable, disable and get status of the oks git repository:
* oks-git-on - to enable use of the oks git repository
* oks-git-off - to disable use of the oks git repository
* oks-git-status - to give status of the oks git repository use

The variable TDAQ_DB_REPOSITORY contains one or several git URLs (it does not point to a filesystem anymore). If there are several URLs, the used one depends on the OKS_GIT_PROTOCOL variable. If it is not set, the first URL is used. To get URL use oks_git_repository utility, e.g. to clone repository:
```
$ git clone `oks_git_repository`
```

The TDAQ_DB_USER_REPOSITORY can point to user git area. If it is set, oks-based tools will not clone git repository, but will use that area instead. It is responsibility of user to create that area and remove when not needed.

The TDAQ_DB_VERSION can be used to access particular revision of oks git repository. It is used internally by setup_daq to preserve concrete revision to be used by data taking session. In case of configuration reload this variable is set into a new value and distributed to all processes accessing oks configuration. The variable can be used in two formats:
* hash:$value - select revision by explicit hash
* date:$value - select latest revision before given date or timestamp
If the variable is not defined, the latest revision will be checked out.

The variable OKS_REPOSITORY_MAPPING_DIR points to the repository mapping.

As before, the TDAQ_DB_PATH can be used for a filesystem-based access and can contain several colon-separated filesystem repositories. With filesystem based access, the oks data from git repository can be included using the repository mapping variable, for example:
```
$ unset TDAQ_DB_REPOSITORY
$ export TDAQ_DB_PATH=$OKS_REPOSITORY_MAPPING_DIR:/det/a/b/c:/home/x/y/z
```

### Filepaths

To access an oks file from git repository one has to use its repository filename, for example:
```
$ oks_data_editor daq/segments/setup.data.xml
```

One should not use any filesystem relative or absolute paths referencing oks git repository files. There are two exceptions.

#### Filepaths relative to repository mapping

If the filepaths are relative to the OKS_REPOSITORY_MAPPING_DIR. That is done to help users with shell path-completion (hitting _TAB_ key), for example the following commands will correctly checkout the files from git repository and run appropriate tools:
```
$ export OKS_REPOSITORY_MAPPING_DIR=/tbed/git/oks/tdaq-99-00-00
$ oks_data_editor /tbed/git/oks/tdaq-99-00-00/daq/partitions/part_hlt.data.xml
$ setup_daq /tbed/git/oks/tdaq-99-00-00/daq/partitions/part_hlt.data.xml part_hlt
```

However, the preferable way is to use the repository filenames, e.g.:
```
$ oks_data_editor daq/partitions/part_hlt.data.xml
$ setup_daq daq/partitions/part_hlt.data.xml part_hlt
```

#### Filepaths relative to user repository

If the files are checked out by user, one should set TDAQ_DB_USER_REPOSITORY to access them.

The following example will result an error, since user provides path relative to current working directory:
```
$ cd `mktemp -d`
$ git clone `oks_git_repository` .
$ oks_dump -f oks_dump -f daq/segments/setup.data.xml
```

To make it working it is necessary to set the above variable:
```
$ cd `mktemp -d`
$ git clone `oks_git_repository` .
$ export TDAQ_DB_USER_REPOSITORY=`pwd`
$ oks_dump -f oks_dump -f daq/segments/setup.data.xml
```

### OKS tools

The tools from previous implementation can be used with oks git repository.

#### oks_dump

As in the previous implementation, the oks_dump can be used for fast check of oks configuration. Now it shows details of git operations, if the git repository is used, e.g.:
```
$ oks_dump -f daq/segments/setup-initial.data.xml
2020-Aug-11 11:01:24.524 [OKS checkout] => oks-checkout.sh -u /tmp/oks.ou9Qxy
git clone -q -n ssh://gitea@pc-tdq-git.cern.ch/oks/tdaq-09-01-00.git .
git checkout -q 
checkout oks version eb175611bc85f3061541cc20c62be8a00b4b26e1
2020-Aug-11 11:01:26.377 [OKS checkout] => done in 46.548 ms
Reading data file "/tmp/oks.ou9Qxy/daq/segments/setup-initial.data.xml" in normal format (51591 bytes)...
...
```

#### oks-import.sh

As before, this tool allows to import or to update user files from previous repositories or release installation areas. Change current working directory to a repository root and import required files and directories.

For example, to import some files from TDAQ release:
```
bash$ cd ${TDAQ_INST_PATH}/share/data
bash$ oks-import.sh -m "import release files" daq/schema daq/sw/*.data.xml daq/segments/common-environment.data.xml
```

To import some sub-detector folders from previous release:
```
bash$ cd /atlas/oks/tdaq-09-00-00
bash$ oks-import.sh -m "commit message" tile/hw tile/sw
```

#### OKS data editor

New functionality:
* commit updated files into git repository
* show notification about new commits in the git repository and merge local changes with master branch
* browse archived versions in the git repository and switch to an archived version

There is no need to create a temporal user repository, it is done automatically.

#### Direct use of GIT interface

The git interface is exposed to user. It is possible to clone git repository, copy, delete files, use any editors to update them and commit changes back to the git repository, e.g.:
```
$ cd `mktemp -d`
$ git clone `oks_git_repository` .
$ modify, copy, remove any files
$ git commit -m 'describe changes' x y z ...
$ git push origin
```

There are web interfaces to see historical changes and perform minimal modifications using web text editors on [Point-1](http://pc-atlas-www.cern.ch/gitea) and [TestBed](http://pc-tbed-git.cern.ch/gitea).
The CERN gitlab [atlas-tdaq-oks](https://gitlab.cern.ch/atlas-tdaq-oks) project is a read-only mirror of Point-1 and TestBed repositories. It can be accessed world-wide by authenticated ATLAS users.

All changes have to be stored into _master_ branch. The other branches are not yet supported (for the moment they will be rejected by git server). In future they are planned to be used for merge requests, when the committed changes have to be ignored during few next runs.

#### Undo commits

To undo changes use git revert. Avoid use commands causing losing commit history such as git reset.

Clone git repository. Then get the hash code of last commit, revert that commit and push changes:
```
$ git log -1
$ git revert <hash_code_from_git_log>
$ git push
```

### Archiving

The oks2coral archiving in Oracle is disabled. If files are stored in git repository, they are automatically archived into git database.

The run number database stores a reference to the used config version and repository filename.
```
$ rn_ls -c "oracle://atonr_adg/rn_r" -w ATLAS_RUN_NUMBER -s '2020-07-31T12:00:00' -t '2020-08-02T12:00:00' -a '%xml'
====================================================================================================================================================================================================================
|    Name |    Num | Start At (UTC)           |    Duration |   User |                  Host | Partition |                                       Version | Config Name                         | Comment           |
====================================================================================================================================================================================================================
| point-1 | 380689 | 2020-Jul-31 16:33:59.818 | 0:00:12.248 | isolov | pc-tdq-onl-05.cern.ch | all_hosts | hash:6800fe3b63a18859ef688612b44e051e4f36e345 | daq/partitions/all_hosts.data.xml   | Clean stop of run |
| point-1 | 380688 | 2020-Jul-31 14:52:26.896 |             | tdaqsw | pc-tdq-onl-01.cern.ch |   initial | hash:2c39688d965cbac84832e77342c316ee9d96adb3 | daq/segments/setup-initial.data.xml |                   |
====================================================================================================================================================================================================================
```

In turn, the git repository is tagged by run number and partition name.

To clone git repository use *oks_clone_repository* utility. The user can specify output directory and particular version by commit hash, date or tag. For example checkout by tag configuration used for run 380689 and partition *all_hosts*:
```
$ oks_clone_repository --version tag:r380689@all_hosts
```
or by hash (one can provide first few characters of the hash, as long as that partial hash is at least four characters long and unambiguous), e.g.:
```
oks_clone_repository --version hash:6800fe3b
```

## tdaq-08-03-01

### OKS data file format
Jira: [ADTCC-185](https://its.cern.ch/jira/browse/ADTCC-185) 

#### Store values inside tags

Store attribute and relationship data inside values of tags

Format for single value attributes
```
<attr name="xxx" type="yyy" val="zzz"/>
```
Format for multi value attributes
```
<attr name="xxx" type="yyy">
 <data val="zzz1"/>
 <data val="zzz2"/>
</attr>
```
Format for 0..1 and 1..1 relationships
```
<rel name="xxx" class="yyy" id="zzz"/>
```
Format for 0..N and 1..N relationships
```
<rel name="xxx">
  <ref class="yyy1" id="zzz1">
  <ref class="yyy2" id="zzz2">
</rel>
```

#### Skip empty data

Do not store attributes with values equal to empty initial and empty relationships

#### Compatibility and data conversion

The changes are backward compatible. New OKS library is able to read old "extended" and "compact" data formats.
For conversion open data file stored in old format using OKS Data Editor or DBE and save it.
Do not mixture old and new formats in the same file, when update file in a text editor.

## tdaq-06-00-00

Change a meaning of the OKS attribute range for string data type. Before the tokens of range were used for lexical comparison of the string. Now regular expression is used instead.

To enforce a value of string attribute be non empty one can use ".+" regular expression for range.

If attribute range for string type is defined, initial value of that attribute may be required to validate schema, for example non-empty string may not have default value set to empty string.

If an OKS schema had range defined for a string attribute for previous release used for configuration databases it has to be converted to enumeration type. No other changes are needed for a programming code using config and generated DAL packages.

For IS schema the range is not needed, since IS does not validate values of IS objects, so irrespectively to any range defined for IS attribute, the IS will allow to put any value. It is up to user decide to remove range in this case, or to convert attribute type from string to enumeration. However in the latter case the programming code using such schema may need be changed.

## tdaq-04-00-00

* Fix bug with wrong objects list returned by *referenced_by()* method from config package. The patch fixes calculation of RCRs by _OksObject::SetRelationshipValue(const OksDataInfo * odi, OksData * d)_ method (see bug [82615](https://savannah.cern.ch/bugs/?82615)).
* Fix problem when OKS file was created incorrectly by third-party tools: if creation date tag in the info section is missing (see bug [70563](https://savannah.cern.ch/bugs/?70563)), after saving by OKS on reload it resulted "not-a-date-time" error.  
* Fix several internal problems connected with execution of OKS code in several threads and several OKS kernels discovered during RDB writer server exploitation:  
    * bug [76158](https://savannah.cern.ch/bugs/?76158): the server may go into error state when several clients are updating OKS server repository;
    * bug [78326](https://savannah.cern.ch/bugs/?78326): the server may crash, when several clients actively update database caused by fast boost allocator with null mutex;
    * bug [82762](https://savannah.cern.ch/bugs/?82762): the server may crash under certain conditions during database reload.
  
## tdaq-02-01-00
  
To simplify future release patching procedure the oks package was split on two packages: oks and oks_utils. The oks package only contains library. All utilities including editors, relational oks and oks server were moved to oks_utils package.

## tdaq-02-00-03

* use Boost date and time format instead of OWL package classes (patched in tdaq-02-00-03)  
* performance improvements of XML files parsing (partly implemented as tdaq-02-00-03 patch)  
* substitute round brackets variable name by value before error reporting (patch [3619](https://savannah.cern.ch/patch/index.php?3619))
* checking date and time attribute initial values (patch [3656](https://savannah.cern.ch/patch/index.php?3656))
* provide a possibility for fast objects destruction required for RDB server (patch [3798](https://savannah.cern.ch/patch/?3798) and [62853](https://savannah.cern.ch/bugs/index.php?62853))
* throw exception, if a schema file is modified on data files reload (patch [3798](https://savannah.cern.ch/patch/?3798))
* add mode, when duplicated classes are not allowed

## tdaq-02-00-01

### OKS Server

* the oks-commit.sh supports directories in addition to files
* add oks-import.sh utility to simplify import of new directories and files  

* the repository locks remain from abnormally terminated oks commits can be removed on Point-1 by DAQ experts using /oks/admin/unlock-repository.sh script via sudo
* the "Replace" dialog of OKS Data editor proposes user to check-out repository file containing modified objects
* file-related relative pathnames and absolute pathnames includes are not allowed (in particular to avoid inclusion of files stored outside current repository and to simplify consistency check by oks-commit.sh)  

Read more details on the [TWiki page](https://twiki.cern.ch/twiki/bin/view/Atlas/DaqHltOks#3_OKS_Server).

### OKS Performance Improvements

* the OKS library uses pool of threads to load OKS data files, i.e. the data files can be read in parallel  
* the number of threads by default is equal to number of the computer's CPU cores
* it can be modified via OKS_KERNEL_THREADS_POOL_SIZE environment variable
* the OKS library does not stop reading of files on first error, but continues loading of files in parallel threads until their ends or errors
    * thus the final error report may contain several errors coming from different files
    * this error report may change between different runs of OKS utilities even if the files were not updated
* for optimal performance it is recommended:
    1. to reduce number of schema files (at some point after a schema file parsing OKS requires single active thread to update the database schema)
    2. to avoid huge data files (processing of single data file is not parallelized since XML files are non indexed)

## tdaq-02-00-00

### C++ API Changes

Use new integer types from <stdint.h> to support 64-bits platform as shown in the following table:  

| OKS Type | Old C++ Type | New C++ Type |
|----------|--------------:|--------------:|
| s8 (8-bits signed integer) | unsigned char | uint8_t |  
| u8 (8-bits unsigned integer) | signed char  | int8_t |
| s16 (16-bits signed integer) | unsigned short | uint16_t |
| u16 (16-bits unsigned integer) | signed short | int16_t |
| s32 (32-bits signed integer) | unsigned long | uint32_t |
| u32 (32-bits unsigned integer) | signed long | int32_t |

### OKS Server

On Point-1 access to database repository will be controlled by the OKS Server. Read more about it on the [TWiki page](https://twiki.cern.ch/twiki/bin/view/Atlas/DaqHltOks#3_OKS_Server).  

#### OKS Data Editor Changes

* Add search by file name in the main window
* Add search by class and object ID in the Data File dialog
* Add group file operations from the File menu:
    * Save all updated files (<Ctrl-S> shortcut)
    * OKS server related operations:  
    * Release User Repository (<Ctrl-D> shortcut)
    * Update User Repository (<Ctrl-U> shortcut)
    * Commit User Repository (<Ctrl-C> shortcut)
* Add "Referenced By" function from Object dialog (available on right mouse click from dialog's icon)


## tdaq-01-09-00

### OKS Library

* reload any consistent data file (also including changes in the included files)
* speed up OKS XML loading (about 25% faster comparing with release 1.8.4)  
* when read XML schema file, throw exception if base class is not loaded
* oks query supports regular expressions (add attribute comparator '~=')

### OKS Archiving Library  

* use temporal tables to create "try" incremental data version (to reduce unnecessary overhead on Oracle stream replication as requested by ATLAS Oracle DBA)

### OKS GUI Library  

* add support for mouse wheel (can be used in most dialogs of OKS schema and data editors)  

### OKS Data Editor

* improvements in the Find/Replace dialog:
* optionally find by Class and Attribute/Relationship names  
* present result as table
* select visible classes by name and objects by UID (request [34890](http://savannah.cern.ch/bugs/?34890))  
* see search panel at bottom of main window and object dialogs
* the search panel supports simple string search (auto-select when modify the selection pattern) and regular expressions (press button appearing when this option selected to apply regular expression)
* improve performance when build class dialog containing big number of objects (can be seen in 1.8.4 when number of objects is greater than 10K)


## tdaq-01-08-04

### GUI Changes

#### OKS Data Editor Force Save

The users have possibility to save partly-inconsistent data using "Force Save" command. This is required to save partial work avoiding it's possible lost because of exterior problems. For more info see request [28879](https://savannah.cern.ch/bugs/index.php?28879)

#### OKS Editors Recovery Mode

The users have possibility to periodically save changes made with OKS Schema and Data editors. Such changes automatically go to ${FILE}.saved for any unsaved modifications. If user skips changes on exit or an editor stops the work unexpectedly, those ".saved" files remain and can be used for manual recovery. This option can be switched On/Off using an editor Options menu. From the same menu it is possible to set the period of such saving varying from 5" up to 1 hour. For more info see request [28879](https://savannah.cern.ch/bugs/index.php?28879)  

#### Comments

The users have possibility to add comments to files. The comments can be browsed and edited from File dialog of the OKS Schema and Data editors. When file is saved, the editors can to ask user to provide comments, if "Ask comment on file save" option is activated in the "Options" menu (default option if user never saved GUI options). To add a comment user has to provide non-empty text. If no comment should be added on file save, press "Cancel" button in the Comment dialog.  

### XML Format Changes

The value of the "num-of-items" attribute of the oks schema info record is ignored. This was a reason of several wrong schema files after modifications made by users with text editors, when they forgot to set the correct value of this attribute.  

### API Changes

Note: the changes are completely transparent to users of config and DAL layers. Update your code only if you are using OKS directly!  

#### OksObject Class Changes

For consistent error reporting and simplification of code the following methods have been changed.

| Old Method Spec  | New Method Spec  |
|------------------|------------------|
| OksReturnStatus GetAttributeValue(const std::string&, OksData **) const | OksData * GetAttributeValue(const std::string&) const throw (oks::exception) |
| void GetAttributeValue(const OksDataInfo *, OksData **) const | OksData * GetAttributeValue(const OksDataInfo *) const throw () |
| OksReturnStatus GetRelationshipValue(const std::string&, OksData **) const | OksData * GetRelationshipValue(const std::string&) const throw (oks::exception) |
| void GetRelationshipValue(const OksDataInfo *, OksData **) | OksData * GetRelationshipValue(const OksDataInfo *) const throw () |
| OksReturnStatus SetAttributeValue(const std::string&, OksData *) | void SetAttributeValue(const std::string&, OksData *) throw (oks::exception) |
| OksReturnStatus SetAttributeValue(const OksDataInfo *, OksData *) | void SetAttributeValue(const OksDataInfo *, OksData *) throw (oks::exception) |
| OksReturnStatus SetRelationshipValue(const std::string&, OksData *) | void SetRelationshipValue(const std::string&, OksData *) throw (oks::exception) |
| OksReturnStatus SetRelationshipValue(const OksDataInfo *, OksData *) | void SetRelationshipValue(const OksDataInfo *, OksData *) throw(oks::exception)
| OksReturnStatus SetRelationshipValue(const std::string&, OksObject *) | void SetRelationshipValue(const std::string&, OksObject *) throw (oks::exception) |
| OksReturnStatus SetRelationshipValue(const OksDataInfo *, OksObject *) | void SetRelationshipValue(const OksDataInfo *, OksObject *) throw (oks::exception) |
| OksReturnStatus SetRelationshipValue(const std::string&, const std::string&, const std::string&) | void SetRelationshipValue(const std::string&, const std::string&, const std::string&) throw (oks::exception) |
| OksReturnStatus AddRelationshipValue(const std::string&, OksObject *) | void AddRelationshipValue(const std::string&, OksObject *) throw (oks::exception) |
| OksReturnStatus AddRelationshipValue(const OksDataInfo *, OksObject *) | void AddRelationshipValue(const OksDataInfo *, OksObject *) throw (oks::exception) |
| OksReturnStatus AddRelationshipValue(const std::string&, const std::string&, const std::string&) | void AddRelationshipValue(const std::string&, const std::string&, const std::string&) throw (oks::exception) |
| OksReturnStatus RemoveRelationshipValue(const char *, OksObject *) | void RemoveRelationshipValue(const std::string&, OksObject *) throw (oks::exception) |
| OksReturnStatus RemoveRelationshipValue(const OksDataInfo *, OksObject *) | void RemoveRelationshipValue(const OksDataInfo *, OksObject *) throw (oks::exception) |
| OksReturnStatus RemoveRelationshipValue(const std::string&, const std::string&, const std::string&) | void RemoveRelationshipValue(const std::string&, const std::string&, const std::string&) throw (oks::exception) |

Also by needs of config Python bindings one new method have been added:
```
std::list<OksObject *> * get_all_rels(const std::string& name = "*") const
```

The method returns list of objects which have a reference on given one. If the relationship name is set to "*", then the method takes into account  all relationships of all objects. The method performs full scan of all OKS objects and it is not recommended at large scale to build complete graph of relations between all database object; if only composite parents are needed, them the reverse_composite_rels() method has to be used.  

By needs of tidb package there are two new methods to read OksObject from and to it put into standard streams:  
```
static OksObject * get(std::istream&, OksKernel *) throw (oks::exception)
void put(std::ostream&) const throw (oks::exception)
```

#### OksClass Class Changes  

By needs of tidb package there are two new methods to read OksClass from and to it put into standard streams:  
```
static OksClass * get(std::istream&, OksKernel *) throw (oks::exception)
void put(std::ostream&) const throw (oks::exception)
```

#### OksFile Class Changes

To improve error reporting the following methods throw exception instead of returning bad error code:  

| Old Method Spec | New Method Spec |
|-----------------|-----------------|
| Oks::ReturnStatus lock(bool = false) | void lock(bool force = false) throw (oks::exception) |
| Oks::ReturnStatus unlock() | void unlock() throw (oks::exception) |
| Oks::ReturnStatus set_logical_name(const std::string &) | void set_logical_name(const std::string& name) throw (oks::exception) |
| Oks::ReturnStatus set_type(const std::string &) | void set_type(const std::string& type) throw (oks::exception) |

#### OksKernel Class Changes

| Old Method Spec | New Method Spec |
|-----------------|-----------------|
| OksReturnStatus set_active_schema(OksFile *) | void set_active_schema(OksFile *) throw (oks::exception) |
| OksReturnStatus set_active_data(OksFile *) | void set_active_data(OksFile *) throw (oks::exception) |
| bool GetAllowDuplicatedObjectsMode() const | bool get_allow_duplicated_objects_mode() const |
| void SetAllowDuplicatedObjectsMode(const bool) | void set_allow_duplicated_objects_mode(const bool) |
| bool GetVerboseMode() cons | bool get_verbose_mode() const |
| void SetVerboseMode(const bool) | void set_verbose_mode(const bool) |
| bool GetSilenceMode() const | bool get_silence_mode() const |
| void SetSilenceMode(const bool) | void set_silence_mode(const bool) |
| bool GetProfilingMode() const | ool get_profiling_mode() const |
| void SetProfilingMode(const bool) | void set_profiling_mode(const bool) |

The OKS kernel provides new methods to check status and to change various kernel modes:
* the status of mode checking maximum length of string attributes of some OKS objects (see also 1.8.3 OKS release notes)
  ```
  static bool get_skip_max_length_check_mode()
  static void set_skip_max_length_check_mode(const bool)
  ```
  it can also be set using OKS_SKIP_MAX_LENGTH_CHECK environment variable.  
* the status of the mode testing inherited duplicated objects:
  ```
  bool get_test_duplicated_objects_via_inheritance_mode() const
  void set_test_duplicated_objects_via_inheritance_mode(const bool)
  ```

There are new methods to backup schema and data files (the operation is silent and ignores any consistency rules):
```
void backup_data(OksFile * pf, const char * suffix = ".bak") throw (oks::exception)
void backup_schema(OksFile * pf, const char * suffix = ".bak") throw (oks::exception)
```

There are new methods to create OksClass and OksObject objects from standard streams:  
```
OksObject * create_object(std::istream& input) throw (oks::exception)
OksClass * create_class(std::istream& input) throw (oks::exception)
```

## tdaq-01-08-03

### General Changes

#### Max Length for OKS Names

By needs of OKS archiving, limit maximum string length for attributes of some OKS types, which are:

| OKS Type        | Attribute   | Maximum Length |
|-----------------|-------------|----------------|
| OksObject       | Object ID   | 64             |
| OksClass        | Name        | 64             |
| ^^              | Description | 2000           |
| OksAttribute    | Name        | 128            |
| ^^              | Description | 2000           |
| ^^              | Range       | 1024           |
| OksRelationship | Name        | 128            |
| ^^              | Description | 2000           |
| OksMethod       | Name        | 128            |
| ^^              | Description | 2000           |


#### New Oks Data Types

Add new OKS Data types:

* **s64_int_type** - signed 64-bits integer ("s64"); for implementation uses typed on the **int64_t** type
* **u64_int_type** - unsigned 64-bits integer ("u64"); for implementation uses typed on the **uint64_t** type
* **class_type** - reference on class; is implemented as string with range of allowed values equal to names of classes defined by the schema; it is important to put an initial value pointing to a class; if it will remain empty, then OKS will complain trying to create a new object.

Above types are fully supported by oks xml files, GUI editors and archiving.

If there are already existing and used OKS relational archives, check oks/src/rlib/create_db.[oracle|mysql|sqlite].sql bootstrap files for technology you are using and decide if schema of existing archive tables have to be changed.

#### Bug Fixes

Use maximum compiler-supported precision when store or print out values of OKS **float** and **double** numeric types. Before the precision was limited to the C++ std::ostream default value = 6 digits. E.g. it was not possible to store in OKS a double value equal to 1.23456789, which was rounded to 1.23457.

### OKS GUI Changes

#### New Features

The Data and Schema editors store options of graphical windows in the ~/.oks-data-editor-rc.xml and  ~/.oks-schema-editor-rc.xml files. To produce those files press <Set Default Values> button from a "Set Parameters" or "Properties" windows. The saved values will be used as default ones when the editor will be started next time.

#### Bug Fixes and Known Problems of OKS Data Editor

Fix bug in a graphical view, when several objects were over-drawn on the same place.

OKS Data Editor cannot display too many graphical objects in a graphical view. The maximum allowed number of objects is limited by capabilities of used Motif Drawing Area widget limiting size of area 35767x35767 pixels. When wrap to visible area or one object per row arrangement was used, the vertical limit was already reached for M4 combined partition and resource objects. Now in such case the editor will report error message and suggest the user to set different arrangement of objects.

## tdaq-01-08-00

### OKS Archiving

Add newly appeared classes (e.g. after integration with new detector) into schema already existing in archive (before it was required to create new version):
* smaller number of versions
* reduce archive tool’s downtime (human intervention is only needed when the database schema is modified, but not when it is extended)

Add newly appeared objects into base data version (before such objects were created in the incremental version, that takes more space):
* assume that any configuration object is referenced somehow (implicitly) by the partition object
* agreed with TDAQ groups and detectors; required schema changes in areas where string values were used for references instead of relations

As result, the utility to create new base or schema version in archive becomes much more robust since it does not require to put all data from all OKS repositories into base version in one go.

### OKS Data Editor: Graphical Window

* fix several bugs when work with icons of small size
* add possibility to arrange objects of a relationship by one object per single line, that makes OKS graphical window look like more "standard"; to use it, select "Arrange" -> "One child per line" item from graphical popup menu (press right mouse button in a free area of a graphical window)

### OKS Library

* most kernel functions throw exceptions instead of printing error messages; this allows better integration with oksconfig plug-in
* improve performance of OKS methods reading and saving database by caching names of tested files and directories


## tdaq-01-07-00

### API Changes

OKS library was starting to use exceptions to report problems. The methods dealing with input / output operations (i.e. _load...()_, _save...()_ and _new...()_ methods of _OksKernel_ and related methods of _OksClass_, _OksObject_, _OksData_, etc.) throw _oks::exception_ instead of returning _OksStatus_.

The old code testing return status:
```
OksKernel kernel;
OksFile * fh1 = kernel.**load_file**("_test.in.xml_");  // (1) can return zero in case of error!
if(fh1 == 0) { std::cerr << "_ERROR: Can not load file \"test.in.xml\"\n_"; exit(1); }
OksFile * fh2 = kernel.**new_data**("_test.out.xml_");  // (2) can return zero in case of error!
if(fh2 == 0) { std::cerr << "_ERROR: Can not create file \"test.out.xml\"\n_"; exit(1); }
...                                                     // (3) some code modifying oks data
if(kernel.save_schema(fh2) != OksSuccess) {             // (4) need to check OksStatus
  std::cerr << "_ERROR: Can not save file \"test.out.xml\"\n_"; exit(1);
}
```
has to be replaced with the following one:
```
OksKernel kernel;
try {
  OksFile * fh1 = kernel.**load_file**("_test.in.xml_"); // (5) always returns non-zero!
  OksFile * fh2 = kernel.**new_data**("_test.out.xml_"); // (6) always returns non-zero!
  ...                                                    // (7) some code modifying oks data
  kernel.save_schema(fh2);                               // (8) is void
}
catch (oks::exception & ex) { std::cerr << "Caught OKS exception: " << ex << std::endl; exit(1); }
```

Using exceptions there is no more need to test return values:

* a returned pointer is always non-zero (compare lines 1, 2 with 5, 6)
* _save...()_ methods become _void_ instead of returning _OksStatus_ (compare line 4 with 8)

### Improved Reporting of XML Problems

Another advantage of exception usage is consistent error reporting, that is especially important in case of multiple include files. In the past, any error has been reported to the standard error stream in the moment of it's detection and in some cases without enough diagnostics. For example the only possibility to get name of the file where a problem took place was to read _OKS info messages_:
```
lxplus055:db6$ oks_dump -f /tmp/daq/partitions/be_test.data.xml
Reading data file "/tmp/daq/partitions/be_test.data.xml" in extended format (3331 bytes)...
* reading data file "/tmp/daq/segments/segments.data.xml" in extended format (10503 bytes)...
* loading 53 classes from file "/tmp/dal/schema/core.schema.xml"...
* reading data file "/tmp/DAQRelease/sw/repository.data.xml" in extended format (88245 bytes)...
* reading data file "/tmp/DAQRelease/sw/external.data.xml" in extended format (15272 bytes)...
* reading data file "/tmp/DAQRelease/sw/tags.data.xml" in extended format (1928 bytes)...
ERROR [OksXmlInputStream::read_tag_start()]:
(line 67, char 1)
Unexpected end of file
ERROR [OksXmlInputStream::read_tag_start()]:
(line 67, char 1)
Unexpected end of file
ERROR [OksObject::read_header()]:
(line 67, char 1)
Failed read start-of-object 'obj' tag
```

When above problem took place using oksconfig plug-in, it was not possible to identify the exact place of problem at all (at least without setting _OKS_KERNEL_SILENCE_ to _no_):
```
bash$ config_dump -d oksconfig:daq/partitions/be_test.data.xml -c Partition
ERROR [OksXmlInputStream::read_tag_start()]:
(line 67, char 2)
Unexpected end of file
ERROR [OksXmlInputStream::read_tag_start()]:
(line 67, char 2)
Unexpected end of file
```

Now the resulted exception always contains exact reason of error and keeps full chain of files inclusion and oks entities dependencies, e.g.:
```
bash$ export OKS_KERNEL_SILENCE=yes
bash$ oks_dump -f /tmpdaq/partitions/be_test.data.xml
Caught oks exception:
oks[10] ***: failed to load data file "/tmp/daq/partitions/be_test.data.xml" because:
oks[9] ***: failed to load include "daq/segments/segments.data.xml" because:
oks[8] ***: failed to load data file "/tmp/daq/segments/segments.data.xml" because:
oks[7] ***: failed to load include "DAQRelease/sw/repository.data.xml" because:
oks[6] ***: failed to load data file "/tmp/DAQRelease/sw/repository.data.xml" because:
oks[5] ***: failed to load include "DAQRelease/sw/external.data.xml" because:
oks[4] ***: failed to load data file "/tmp/DAQRelease/sw/external.data.xml" because:
oks[3] ***: failed to load include "DAQRelease/sw/tags.data.xml" because:
oks[2] ***: failed to load data file "/tmp/DAQRelease/sw/tags.data.xml" because:
oks[1] ***: failed to read 'object "i686-slc3-gcc344-dbg@Tag"'
oks[0] ***: Unexpected end of file while read tag start at (line 66, char 51)
```
The same exception will be passed to the ERS exception reported by oksconfig plug-in:
```
bash$ config_dump -d oksconfig:/tmp/daq/partitions/be_test.data.xml -c Partition
ERROR 2007-Jan-17 11:20:12 [ConfigurationImpl* _oksconfig_creator_(...)
at oksconfig/src/OksConfiguration.cpp:29] oksconfig initialization error
        was caused by: ERROR 2007-Jan-17 11:20:12 [virtual void OksConfiguration::open_db(...) at
        oksconfig/src/OksConfiguration.cpp:57] cannot load file '/tmp/daq/partitions/be_test.data.xml':
oks[10] ***: failed to load data file "/tmp/daq/partitions/be_test.data.xml" because:
...
oks[2] ***: failed to load data file "/tmp/DAQRelease/sw/tags.data.xml" because:
oks[1] ***: failed to read 'object "i686-slc3-gcc344-dbg@Tag"'
oks[0] ***: Unexpected end of file while read tag start at (line 66, char 51)
```

### OKS Archiving

Add _Release_ column to _OksSchema_ table to simplify choice of right schema by oks2coral and to allow user easier choice of archived configuration data. There is new OKS Archiving Web GUI, allowing:
* queries on archived configurations by time intervals, release, user, host and partition patterns;
* sorting result by multiple columns;
* selection which columns to be shown.

The test Web GUI replaced previous one: [http://cern.ch/isolov/cgi-bin/oks-archive.pl](http://cern.ch/isolov/cgi-bin/oks-archive.pl)
Provide bootstrap files for different RDBMS:
* create_db.mysql.sql
* create_db.oracle.sql (renamed old create_db.sql)
* create_db.sqlite.sql

Fix several run-time problem for MySQL CORAL plug-in.

#### API changes

* Add optional _release_ parameter to several functions. It is used to get HEAD schema and data version per TDAQ release. By default the release parameter points to current release (i.e. to "tdaq-01-07-00").
* Add function _get_max_schema_version()_ to know maximum schema version number to be used to choose free version number. Note, the existing method _get_head_schema_version()_ returns head schema version per release.
* Add function _get_time_host_user()_ to extract time, host and user values from attribute list. It is used by _roks_ library and by _oks_ls_data_ utility.

#### List Archives Utility

Add several _new options_ to specify archives selection criteria:
```
usage: oks_ls_data
  -c | --connect-string connect_string
  -w | --working-schema schema_name
  [-l | --list-releases]
  [-s | --schema-version schema_version]
  [-b | --base-data-only]
  [-z | --show-size]
  [-u | --show-usage]
  [-d | --show-description]
  [-t | --sorted-by parameters]
  [-r | --release release_name]
  [-e | --user user_name_pattern]
  [-o | --host hostname_pattern]
  [-p | --partition partition_name_pattern]
  [-S | --archived-since timestamp]
  [-T | --archived-till timestamp]
  [-v | --verbose-level verbosity_level]
  [-h | --help]

Options/Arguments:
  -c connect_string    database connection string
  -w schema_name       name of working schema
  -l                   list releases
  -s schema_version    print out data of this particular version (0 = HEAD version)
  -b                   print out base data versions only
  -z                   print size of version (i.e. number of relational rows to store it)
  -u                   show who, when, where and how used given version
  -d                   show description
  -t parameters        sort output by several columns; the parameters may contain the following
                       items (where first symbol is for ascending and second for descending order):
                         v | V - sort by versions;
                         t | T - sort by time;
                         u | U - sort by user names;
                         h | H - sort by hostnames;
                         p | P - sort by partition names (i.e. by descriptions);
  -r release_name      show configuration for given release name
  -e user_name_pattern show configuration for user names satisfying pattern (see syntax description below)
  -o hostname_pattern  show configuration for hostnames satisfying pattern (see syntax description below)
  -p partition_pattern show configuration for partition names satisfying pattern (see syntax description below)
  -S since_timestamp   show configuration archived since given moment (see timestamp format description below)
  -T till_timestamp    show configuration archived before given moment (see timestamp format description below)
  -v verbosity_level   set verbose output level (0 - silent, 1 - normal, 2 - extended, 3 - debug, ...)
  -h print this message 

Description:
  The utility prints out details of oks data versions archived in relational database.
  The Version is shown as sv.dv[.bv], where:
    * sv = schema version;
    * dv = data version;
    * bv = base version (optional, only appears for incremental data versions).
  The Size (numbers of relational rows) is reported in form x:y:z, where the:
    * x = number of new[/deleted/updated] objects;
    * y = number of new[/deleted/updated] attribute values;
    * z = number of new[/deleted/updated] relationship values.
 The timestamps to be provided in ISO 8601 format: "YYYY-MM-DD HH:MM:SS".
  The allowed wildcard characters used to select by user, host and partition names are:
    % (i.e. percent symbol)    - any string of zero or more characters;
    _ (i.e. underscore symbol) - any single character.
```

#### Other Implementation Changes and Bug Fixes

* replace _OksAlloc_ class used for the memory usage optimisation by the Boost class _boost/pool/pool_alloc.hpp_; header file _oks/alloc.h_ has been removed; OKS is always initialized in multi-thread safe mode;
* when save a data file, keep the file flags
* fix run-time bug appeared on 64-bits architecture (wrong calculation of const string literal size); by chance it worked correctly on 32 bits;

##### OKS Schema Editor

* attach _Range_ and _Initial Value_ properties to the right side of the attribute window to allow see long strings;
* do not show _Non Null Value_ property for _boolean_ attribute.

##### OKS Data Editor

* mark file updated, when swap objects inside relationship value;
* avoid bug when copy object to existing id;
* fix bug when load query with attribute comparator.


## tdaq-01-06-00

There are no any changes in OKS, that require a user to modify or to convert a consistent database file. However the OKS becomes more strict for saving and loading of inconsistent schema and data files (no required includes, dangling object references, wrong or missing attribute and relationship values). A user can be asked to modify data as it is required by schema before it will be possible to save them using OKS tools. The other changes in OKS are connected with extension of features using relational backend (new relational tables and utilities).

### OKS library

#### Files Consistency

* to improve diagnostic report included files, where '_no files inclusion path between referenced objects_' problem takes place
* allow duplicated objects for archiving purposes (use OKS_KERNEL_ALLOW_DUPLICATED_OBJECTS variable)
* report object id and attribute name when read data with wrong range
* check inclusion of required schema files before saving
* allow empty objects (i.e. without attribute and relationship values)

#### Schema Consistency

* report warning when attributes and relationships change between single-value/multi-value in case of redefinition in derived class or there are such conflicts in super-classes
* change exclusiveness scope of composite relations from object to relationship  
  e.g. a module can be exclusively inserted to crate and to detector, but it cannot be inserted to several crates

#### XML Parser

* report correct line number and position for certain types of problems in oks xml
* fix minor memory leak in OKS xml parser
* fix several bugs with xml comments and end of comment
* skip any attributes defined in the oks-schema or oks-data tag (they can come from automatic xml generation tools)
* allow xml style-sheets and xmlns tags
* allow different encodings of xml

#### Relational Methods using RAL

* move from POOL RAL to CORAL and follow changes in CORAL API up to latest used version (CORAL 1.3.0)
* add table to keep used configurations
* normalize schema as it was recommended by CERN Oracle DBA
* store oks date and time as string
* increase length of class name (now it is limited by 64 bytes)
* use xml authentication (see file authentication.xml pointed by the CORAL_AUTH_PATH variable)

#### General Methods

* add method to get referenced objects
* path query: check goal at non-leave object in the path to allow paths with optional branches
* fix bug when TDAQ_DB_PATH is not defined
* do not print warning in silent mode (before few ones left by mistake)

### General OKS Utilities

#### New oks-generate-schema-docs.sh

The utility generates description of the schema files using xsl conversion of standard oks schema xml files. Such conversion is performed by user's Internet browser on fly. This should work with MS Internet Explorer 6.0, Mozilla 1.7.12 and their higher versions.
```
Usage: oks-generate-schema-docs.sh [--help] [--verbose] [--search-dir in-dir] [--search-pattern schema-file-pattern] --target-dir out-dir
Arguments/Options:
  -v | --verbose verbose output
  -h | --help print this message
  -d | --search-dir in-dir directory to search schema files; current value = [/afs/cern.ch/atlas/project/tdaq/cmt/nightly/installed/share/data]
  -p | --search-pattern p pattern for schema file names; current value = [*.xml]
  -t | --target-dir out-dir directory where to put out files
  -n | --page-name name provide name for generated index.html file; current value = [TDAQ Release Schema Files]
```

The example of generated schema description for DAQ/HLT-I nightly release is available on: [http://pcatd12.cern.ch/releases/nightly/installed/share/doc/DAQRelease/html](http://pcatd12.cern.ch/releases/nightly/installed/share/doc/DAQRelease/html/)

The utility can be used by users of the DAQ/HLT-I release to generate descriptions of own schema files using --search-dir to point to area with own schema files.

#### New oks-test-duplicated-objects.sh

The utility tests duplicated objects (i.e. objects with equal class names and IDs) stored in oks data files referenced by the TDAQ_DB_PATH variable.

The utility has been created to find files, which are _bad_ from archiving point of view. In ideal case it should find no duplications.

To filter out files which need to be ignored either put file name pattern(s) into _-s_ command line option, or install into in any subdirectory of ${TDAQ_INST_PATH} file(s) with name _remove-from-oks-archive.txt_ containing such patterns (one pattern per line), e.g. run "oks-test-duplicated-objects.sh -v -s '.*share/data/ExampleConfiguration.*' '.*share/data/training.*'" to skip all files installed by the _ExampleConfiguration_ and _training_ packages.

#### oks_merge

* merge data files (use _-o_ option) and schema files (use _-s_ option)

#### oks_diff_data

* add options to compare objects of one class or single object

#### oks_diff_schema

* allow data files as input (i.e. compare schemes used by data files)
* change values returned by binary to be able to report number of found differences:
    * 0 - there are no differences between two schema files
    * 253 - bad command line
    * 254 - cannot load database file(s)
    * 255 - loaded file has no any class
    * 1..252 - number of differences (is limited by the max possible value)


#### oks_dump

* return different status in case of different problems:
    * 0 - no problems found
    * 1 - ad command line parameter
    * 2 - bad oks file(s)
    * 3 - bad query passed via -q or -p options
    * 4 - cannot find class passed via -c option
    * 5 - loaded objects have dangling references
* new option _-i_ adds possibility to read files to be printed from input-file instead of command line to help with very long list of files (that may exceed maximum command line length)
* distinguish lists of schema and data files lists on user choice:
    * use option _-f_ to print list of all oks xml files (is used as before)
    * use new option _-s_ to print list oks schema files
   * use new option _-d_ to print list oks data files
* add option _-r_ to print out list of objects referenced by found objects (can only be used with query)

### Utilities for OKS Archiving

All utilities described below require two parameters:

* the database connection string
* the name of the relational database working schema

The values of above parameters are site specific. For development purposes at CERN they are:

| the connection string: | oracle://devdb10/tdaq_dev_backup |
| the working schema: | onlcool |

For other sites or different purposes different database servers and/or accounts should be used. To create new database (for new owner or different DB server) use _oks/src/rlib/create_db.sql_ file, e.g. in case of Oracle:
```
bash$ sqlplus ${user}/${password}@${host} @$TDAQ_INST_PATH/../oks/src/rlib/create_db.sql
```
where _${user}_, _${password}_ and _${host}_ have site-specific values.

#### New oks-create-new-base-version.sh

The utility has to be used to create a new base version in the archive. It is necessary when the there are changes in the configuration schemes or there are significant changes in the configuration data. In particular a request to use this utility can be send by the oks2coral binary.

By default, the utility checks differences between _head_ schema version from archive and schemes found under TDAQ_DB_PATH. If there are changes, the utility creates new _head_ schema in the archive. Then the utility reads all data files pointed by the TDAQ_DB_PATH variable and stores them into archive. The command line parameters used by the utility are shown below:
```
Usage: oks-create-new-base-version.sh -c connect_string -w schema_name [--help] [--verbose level] [--skip-files reg-exp*]
Arguments/Options:
  -c | --connect-string connect_str    database connection string
  -w | --working-schema schema_name    name of relational database working schema
  -v | --verbose level                 switch on verbose output
  -h | --help                          print this message
  -s | --skip-files r1 ...             list of regular expressions to ignore files
```

#### New oks_tag_data

The utility is created to set a unique string tag on any existing data in OKS archive. A data can be accessed by such human meaningful tag instead of schema and data version numbers.
```
usage: oks_tag_data -c | --connect-string connect_string
                    -w | --working-schema schema_name
                    -t | --tag data_tag
                    [-e | --head-data-version]
                    [-s | --schema-version schema_version]
                    [-n | --data-version data_version]
                    [-v | --verbose-level verbosity_level]
                    [-h | --help]
Options/Arguments:
  -c connect_string   database connection string
  -w schema_name      name of working schema
  -t data_tag         unique tag
  -e                  tag head data version (for head schema or defined by -s)
  -s schema_version   use data for given schema version (extra -n or -e is required)
  -n data_version     use given data version (extra -s is required)
  -v verbosity_level  set verbose output level (0 - silent, 1 - normal, 2 - extended, 3 - debug, ...)
  -h                  print this message
```

#### New oks_ls_data

The utility is created to print out information about data in OKS archive.
```
usage: oks_ls_data
  -c | --connect-string connect_string
  -w | --working-schema schema_name
  [-s | --schema-version schema_version]
  [-b | --base-data-only]
  [-z | --show-size]
  [-u | --show-usage]
  [-v | --verbose-level verbosity_level]
  [-h | --help]

Options/Arguments:
  -c connect_string database connection string
  -w schema_name name of working schema
  -s schema_version print out data of this particular version (0 = HEAD version)
  -b print out base data versions only
  -z print size of version (i.e. number of relational rows to store it)
  -u show who, when, where and how used given version
  -v verbosity_level set verbose output level (0 - silent, 1 - normal, 2 - extended, 3 - debug, ...)
  -h print this message
```

The version is shown as sv.dv[.bv], where:
* **sv** - schema version;
* **dv** - data version;
* **bv** - base version (optional, only appears for incremental data versions).

For example:
* 1.12 - base version with schema-version = 1 and data-version = 12
* 1.34.12 - incremental version with data-version = 34 built on top of base version 1.12

The size is reported when -z option is used explicitly. To get the size it is necessary to execute additional queries and this may take some time for big number of versions. The size of a base version is defined as number of rows in relational tables (_OksObject_, _OksDataVal_ and _OksDataRel_) to describe it's data. For an incremental version the size shows numbers of additional rows to show differences from it's base version, i.e. such rows can be used to mark an object as created, removed or updated, and to provide new values for attributes and relationships of objects. The size is presented in form obj-num:attr-num:rel-num, where:
* **obj-num** - number of objects rows;
* **attr-num** - number of attribute value rows;
* **rel-num** - number of relationship rows.

For example:
* 951:8271:1498 - the base version contains 951 objects; the objects have 8271 attribute values and 1498 relationships
* 1:1:0 - the incremental version has one object updated comparing with base version (an attribute of the object was modified)

The usage of archived versions is reported when -u option is used explicitly. It requires some additional queries and may take some time for big number of versions. The usage of archived data is shown in table below the information about version. The values in _Version_ and _Size_ columns remain empty. The _Description_ column contains information about partition and run number.

#### oks_put_schema,  oks_put_data,  oks_get_schema and oks_get_data

* use xml authentication instead of explicit user name and password passed via command line
* try to re-use the same options between binaries
* to know exact options per binary run it with _--help_ option

### GUI Editors

#### Schema Editor

* meaningless "Many" cardinality is not supported for relationships
*  heck classes and files consistency during save operation

#### Data Editor

* fix bug, when create new object providing ID of already existing object
* do not exit, if there is an error with file saving (i.e. allow user to change file name or it's permissions)
* check objects and files consistency during save operation
* refresh correctly list of all files when reload a file that changed includes
* warn user about bad files during loading them (e.g. with missing includes); a user should to fix reported problem before any other modifications!


## tdaq-01-04-00

There are several changes in the relational OKS back-end:
* use bulk insert for values of oks attributes and relationships
* replace OksDataInt, OksDataNum, OksDataString and OksDataDate tables by single OksDataVal table with appropriate columns to store integer, number, string and date values
* environment variable OKS_RAL_ORDER_QUERY_RESULTS can be used as switch on/off "order by" statement of queries reading values of attributes and relationships for performance studies
* to improve performance do not read description of class methods and their implementations, when get oks data only

Change interpretation of **s8** and **u8** oks data types from symbol type to 8 bits integer type:
* u8 and s8 oks data types are interpreted as integers by any output method; before printing out non-alphanumeric symbols as char resulted wrong output
* oks_data_editor allows to edit u8 and s8 types as octal, decimal and hexadecimal numbers
* oks_schema_editor allows to set format for s8 and u8 types

The OksData stream output operator uses '_format_' field to prints out s8, u8, s16, u16, s32 and u32 types. It is used by the oks_dump program.

Fix bug appeared with few window managers, when under certain conditions oks gui applications do not react on mouse button clicks.

## tdaq-01-02-00

### OKS Relational Backend

An exercise to use a relational database to store oks schema and data information instead of xml files has been done. New _roks_ library appears. It contains code to store oks classes and oks objects to a relational database and to retrieve them back. It is based on the LCG POOL RAL package (see [http://lcgapp.cern.ch/project/persist/](http://lcgapp.cern.ch/project/persist/) for more information). Four new example applications _oks_put_schema_, _oks_put_data_, _oks_get_schema_ and _oks_get_data_ demonstrate it's usage. The file _oks/src/rlib/create_db.sql_ contains definition of the relational tables to store oks information. The exercise has been tested with Oracle on devdb.cern.ch server supported by the CERN IT.

The following sequence of steps to create relational tables, put/get schema and put/get data should to work:
```
sqlplus $[user/$passwd@$server](mailto:user/password@server) $TDAQ_INST_PATH/../oks/src/rlib/create_db.sql
oks_put_schema -c "oracle://$server" -u $user -p $passwd -f oks-file.xml -t "v1" "first" -s 1
oks_get_schema -c "oracle://$server" -u $user -p $passwd -s "/tmp/v1.schema.xml" -e
oks_put_data -c "oracle://$server" -u $user -p $passwd -f oks-file.xml -v -l -a -t "v1.1" "first"
oks_get_data -c "oracle://$server" -u $user -p $passwd -t "v1.1" -f /tmp/v1.1.data.xml
```

For more information contact the oks package developer.

### Path Query

Add support for path queries. Such query returns path between two objects by navigating via relationships in accordance with user-defined query pattern. The result of such query is a list of oks objects forming the path. The use case is to get a path in several trees of references between objects using the same leave objects. Note, for composite objects and exclusive relationships, the usage of reverse composite relationships is more effective. In such case there is the only tree built on top of given leaves.

#### API

Add _oks::QueryPath_ class to describe special type of query calculating path between two given objects. The constructor uses query as a text. Syntax of query path is shown below:
```
query-path ::= '(**path-to** "destination-object" _query-path-expression_)'
query-path-expression ::= '(_query-path-type_ "rel-name" [, "rel-name"*] [_query-path-expression_])'
query-path-type ::= '**direct** | **nested'**
```

If the string cannot be parsed, the exception _oks::bad_query_syntax_ is thrown.

When an oks query path object is created, it can be used to search a path from given source object using the following method:
```
OksObject::List * OksObject::find_path(const oks::QueryPath& query) const;
```

If a path is found, non-empty list is returned.

#### Example of query string

The example of query is shown below:
```
(path-to "my-id@my-class" (direct "A" "B" (nested "N" (direct "X" "Y" "Z"))))
```

The destination object is "my-id@my-class". The search can be started from any object of any class. In our example the start object has to have two relationships named "A" and "B". An object referenced via "A" and "B" should have relationship "N". In our example it is possible to lookup for path via nested objects linked via relationship "N". Finally all objects referenced via "N" should have relationships "X", "Y" and "Z". If the destination object is referenced by them, the path is found.

#### Generic query extensions

The oks query expression was extended to use object ID as part of query expression. The used syntax is '(**object-id** "an-object-id")'. The use case is to identify an object used in a relationship expression, e.g. get all objects of some class referencing this object. Note, this is more effective than search by non-indexed attribute value and this is the only way to define an object without non-key attributes.

The object ID expression is integrated to the oks data editor query constructor (choose "_Object ID_" radio button in an attribute expression form).

#### Example of query string

The example of query to search all objects of some class referencing via relationship "my-relationship" an object with id equal to "test".
```
(all ("my-relationship" some (object-id "test" =)))
```

For example, find all applications including subclasses, which runs on host with id lxplus001.cern.ch:
```
(all ("RunsOn" some (object-id "lxplus001.cern.ch" =)))
```

### Dangling references

Add '_bool OksKernel::get_bind_objects_status() const_' method, that returns status of last _OksKernel::bind_objects()_ method call. It can be used to check lack of dangling references after loading of database files.

OKS improves reporting of the dangling references. In addition to the dangling reference itself the oks reports an object where unresolved reference was found:
```
WARNING [OksObject::bind()]:
Cannot find object "[lxplus053.cern.ch@Computer](mailto:lxplus053.cern.ch@Computer)"
WARNING [OksObject::bind_objects()]:
There are unresolved references from object "lxplus-3x3-23 ctrl@RunControlApplication"
```

### OKS dump

The _oks_dump_ binary returns non-null status, if the loaded files have non-resolved references between objects.

It also supports path queries: use '_--path "object" "path-query"_' command line parameters, e.g. to find path between an application and partition objects:
```
bash$ oks_dump --path "onlsw_test_3x3_lxlpus@Partition" '(path-to "lxplus-3x3-21-ctrl@RunControlApplication" (direct "Segments" "OnlineInfrastructure" (nested "Segments" (direct "Applications" "IsControlledBy" "Resources"))))' daq/partitions/lxplus_tests.data.xml  
Found 3 objects in the path "(path-to "lxplus-3x3-21-ctrl@RunControlApplication" (direct "Segments" "OnlineInfrastructure" (nested "Segments" (direct "Applications" "IsControlledBy" "Resources"))))" from object "onlsw_test_3x3_lxlpus@Partition":  
Object "[onlsw_test_3x3_lxlpus@Partition](mailto:onlsw_test_3x3_lxlpus@Partition)" ...  
Object "[lxplus-3x3-2@Segment](mailto:lxplus-3x3-2@Segment)" ...
Object "[lxplus-3x3-21@Segment](mailto:lxplus-3x3-21@Segment)" ...
```


## tdaq-01-01-00

### Database Consistency

* No more identical objects allowed. In the past a warning message was printed out and an anonymous object was created, when identical object was read. Now the error message is printed out, the reading of the database file containing duplicated object is stopped and the bad status of the file is returned. The error message contains the object identity and both names of the files containing such objects.
* Non-existent attributes and relationships of objects are reported as warnings. Before the data stored in extended format were converted without any message in case of schema evolution.
* To avoid possible confusion of users with variables converters provided by the **dal** package, the syntax of environment variables description used by oks in filenames is changed. Now the valid syntax is _$(FOO)_. In previous releases it was _${FOO}_. Note, it is not recommended to use environment variables in includes, since it makes database dependent on user's setup. The recommended way is to define includes either relative to a database repository, or to the parent file.

### Queries Creation and Destruction

*  To allow proper destruction of a query make all internal query-related objects allocated on heap. In the past  a query constructed from string was not properly released and memory leak took place. Now all sub-query objects are created on heap and are properly released, when the query object is destroyed. All code using queries (the oks kernel code, tutorial, examples) has been changed.

### OKS Dump Application

* Add several command line options:
    * --files-only - prints out list names of database files
    * --class - dump given class (all objects of class or matching some query)
    * --query - print objects matching query (can only be used with class)

### Bugs Fixes

* Avoid possible segmentation fault, when read an object without loaded class.
* When load a schema, do not set automatically default value for enumeration attribute, if it was empty. It caused such default value explicitly set, when the schema is saved from the editor and such behavior was not expected by users.