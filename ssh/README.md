### SSH sample program

This program is an SSH sample program to tunnel TCP/IP traffic over a
secure connection for test purposes.

This program assumes that the destination system (hard-coded, but can
be specified by parameter) is running a server on port 8090. The
easiest way to do this is to run the following command on the
destionation server:

```
while ( true ); do echo "This is a sample server. Goodbye." | nc -l 127.0.0.1 8090; done
```

This program requires that libssh2-devel (Redhat/CentOS) or
libssh2-1-dev (Debian/Ubuntu) be installed to compile/link against the
SSH library.

The program can be compiled with something like:

```
cc -o ssh-sample ssh-sample.c -l ssh2
```
