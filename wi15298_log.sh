#
# Bash script to log fake messages to generate log activity
# (WI 15298)
#

# Log line will be something like:
# Dec 11 16:22:45 scxaixbvt-01 auth|security:notice su: from root to root at /dev/pts/0

HH=0
MM=0
SS=0

while (true); do
    SS=$(( $SS + 1 ))
    if ( [ $SS -gt 59 ] ); then
	MM=$(( $MM + 1 ))
	SS=0
    fi
    if ( [ $MM -gt 59 ] ); then
	HH=$(( $HH + 1 ))
	MM=0
    fi

    printf "Dec 12 %02d:%02d:%02d scxaixbvt-01 auth|security:notice su: from logtest to root at /dev/pts/0\n" $HH $MM $SS >> /var/log/syslog.log

    sleep 60
done
