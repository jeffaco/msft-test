#! /bin/bash

case $1 in
    root)
	AGENT=SCX_Agent
	;;

    user)
	AGENT=SCX_OperatingSystem
	;;

    *)
	echo "Parameter 1 must be 'root' or 'user'"
	exit 1
esac

while (true); do
    echo "`date +'%F %T'`: Enumerating $AGENT ..."
    time /opt/omi/bin/omicli ei root/scx $AGENT
    echo
    echo
    sleep 5
done
