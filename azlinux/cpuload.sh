#!/bin/bash

# Run cpuload process on a variety of processors (on SLES 12)

coreCount=`grep "processor" /proc/cpuinfo | wc -l`
coreCount=$(($coreCount - 1))

echo "Running on processors 0-$coreCount ..."

for i in $(seq 0 $coreCount)
do
    echo "running $i"
    taskset -c $i ./cpuload /root/cpuload.stop 0 &
done
