# This bash shell script will spawn a number of different processes that
# may be shut down easily by deleting a marker file.
#
# This script was designed to "cycle" (delete and recreate) roughly 1/2 of
# the jobs once/second to help find "per-job" related leaks in the SCX
# process PAL, if any exist.

# TOTAL_PROCESS_COUNT:	Total number of desired processes on the system
TOTAL_PROCESS_COUNT=200


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

echo "while (true); do if [ ! -f $FILENAME ]; then break; fi; sleep 1; done" > $FILENAME
chmod u+x $FILENAME

# Create the processes
for (( i=0; i < $PROCS_TO_GENERATE; i++ )); do
    $FILENAME &
done

echo "To terminate program, remove file: $FILENAME"
echo ""

while (true); do
    if [ ! -f $FILENAME ]; then
	break
    fi
    sleep 1
done

# Give a few seconds for all jobs to finish, tell bash to check, then verify jobs are gone
echo "Shutting down ..."
sleep 4
jobs > /dev/null
jobs

exit 0
