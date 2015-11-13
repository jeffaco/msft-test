# This bash shell script will spawn a number of different processes that
# may be shut down easily by deleting a marker file.
#
# This script was designed to "cycle" (delete and recreate) roughly 1/2 of
# the jobs once/second to help find "per-job" related leaks in the SCX
# process PAL, if any exist.

# TOTAL_PROCESS_COUNT:	Total number of desired processes on the system
TOTAL_PROCESS_COUNT=850


CURRENT_PROC_COUNT=`ps -ef | wc -l`
echo "Current process count:  $CURRENT_PROC_COUNT"
echo "Desired process count:  $TOTAL_PROCESS_COUNT"

if [ $CURRENT_PROC_COUNT -ge $TOTAL_PROCESS_COUNT ]; then
    echo "Already at target process count" >&2
    exit 1
fi

# Compute processes to create (note that our job also creates a subprocess)
PROCS_TO_GENERATE=$(( ($TOTAL_PROCESS_COUNT - $CURRENT_PROC_COUNT) / 2 ))
echo ""

FILENAME="/tmp/$USER.$$"

echo "#! /usr/local/bin/bash" > $FILENAME
echo "" >> $FILENAME
echo "while (true); do if [ ! -f $FILENAME ]; then break; fi; sleep 150; done" >> $FILENAME
chmod u+x $FILENAME

# Create the processes
for (( i=0; i < $PROCS_TO_GENERATE; i++ )); do
    if [ $RANDOM -le 16384 ]; then
	$FILENAME oracleindmd1 "(DESCRIPTION=(LOCAL=YES)(ADDRESS=(PROTOCOL=beq)))" &
    else
	$FILENAME -session 1 -terminate -bs -glx -notransfills -co /usr/lib/X11/rgb -fp /usr/lib/X11/fonts/hp_roman8/75dpi/,/usr/lib/X11/fonts/iso_8859.1/100dpi/,/usr/lib/X11/fonts/iso_8859.1/75dpi/,/usr/lib/X11/fonts/hp_kana8/,/usr/lib/X11/fonts/iso_8859.2/75dpi/,/usr/lib/X11/fonts/iso_8859.5/75dpi/,/usr/lib/X11/fonts/iso_8859.6/75dpi/,/usr/lib/X11/fonts/iso_8859.7/75dpi/,/usr/lib/X11/fonts/iso_8859.8/75dpi/,/usr/lib/X11/fonts/iso_8859.9/75dpi/,/usr/lib/X11/fonts/misc/,/usr/dt/config/xfonts/C/ -sp /dev/null &
    fi
done

echo "To terminate program, remove file: $FILENAME"
echo ""

while (true); do
    if [ ! -f $FILENAME ]; then
	break
    fi
    sleep 5
done

# Give a few seconds for all jobs to finish, tell bash to check, then verify jobs are gone
echo "Shutting down ..."
sleep 160
jobs > /dev/null
jobs

exit 0
