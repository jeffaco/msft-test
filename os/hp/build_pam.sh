cc -o pam_auth -DSTANDALONE -DHAVE_SECURITY_PAM_APPL_H -Dhpux pam_auth.c -lpam -DPIC +Z -Ae -mt -D__parisc__

exit $?

# This was the original compile command from the makefile

 cc -DHAVE_CONFIG_H -I. -I../../.. -I/var/home/a-jeffco/dev/source/ext/common/openwsman/src/authenticators/pam -I/var/home/a-jeffco/dev/source/ext/common/openwsman/include -I/var/home/a-jeffco/dev/source/ext/include -Ae -mt -Dhpux=true +W 4075,2546 -D__INLINE__= -D__timezone=timezone -D__parisc__ -DINTERFACE_PEGASUS -DPEGASUS_PLATFORM_HPUX_PARISC_ACC -c /var/home/a-jeffco/dev/source/ext/common/openwsman/src/authenticators/pam/pam_auth.c -Wp,-M.deps/pam_auth.TPlo  +Z -DPIC -o .libs/pam_auth.o
