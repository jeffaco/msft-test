#!/bin/sh

###############################################################################
# Script to fetch timeout settings for PCIe
###############################################################################

device="82599ES"
for i in `lspci | grep $device | awk '{ print $1}'`; do
    out=`setpci -s $i 0xC8.w`
    echo "Current $i Set to $out"
done
