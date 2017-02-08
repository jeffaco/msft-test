#! /bin/bash

SLEEP_TIME=$(($RANDOM % 240))
# SLEEP_TIME=5

SLEEP_LOG="`date +'%F %T'`: Sleep time=$SLEEP_TIME"
echo $SLEEP_LOG >> /home/scxuser/sleep.log
echo $SLEEP_LOG

sleep $SLEEP_TIME
exit 0
