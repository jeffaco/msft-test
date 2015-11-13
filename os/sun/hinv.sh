source ~/.bash_profile
export CMHOME=/tmp/jeffcof/configmgr
export NWHOME=/tmp/jeffcof/nanowbem

sudo $CMHOME/bin/ccmexec -rs HINV
