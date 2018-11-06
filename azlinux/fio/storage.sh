# Run fio in loop, abort on error, record results

set -e

COUNTER=0
while [ $COUNTER -lt 500 ]; do
    if [ -f fio.stop ]; then
        echo "Stopping due to \"fio.stop\" marker file ..."
        rm -f fio.stop
        break
    fi

    let COUNTER=COUNTER+1
    echo "Running iteration $COUNTER ..."

    OUTFILE=results-`date +'%F-%T'`.txt
    fio --output $OUTFILE randomwriters.fio
done

exit 0
