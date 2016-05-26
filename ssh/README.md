### SSH sample programs

This directory contains test code for tunnelling traffic via the SSH
server.

There are two separate programs here:

Program | Purpose
------- | -------
ssh-sample | Forwards traffic thru SSH to a remote TCP/IP server. This is based on the `tcpip-forward-c` sample program for SSH and modified as needed. Forwarding is done directly from SSH server.
ssh-sample-exec | Executes a program (currently netcat) to redirect stdin/stdout to/from a remote server. Program runs in the user context, where SSH forwards to user forwarder, which then forwards to remote TCP/IP server.
ssh-fork | Executes the SSH program to redirect stdin/stdout to/from a remote server. This is identical to ssh-sample-exec, except that it forks SSH itself rather than using the API.

This README has two sections, one for each program.

#### Sample program ssh-sample.c

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

#### Sample program ssh-sample-exec.c

This program requires that libssh2-devel (Redhat/CentOS) or
libssh2-1-dev (Debian/Ubuntu) be installed to compile/link against the
SSH library.

The program also requires that netcat (nc) exists on the destination system.

The program can be compiled with something like:

```
cc -o ssh-sample-exec ssh-sample-exec.c -l ssh2
```

#### Sample program ssh-fork.c

There is no libssh2 API that handles connection logic, similar to how
the SSH program itself handles connections to remote systems.

It turns out that the logic for connecting to a remote host (handling
known_hosts file, parsing and reading ~/.ssh/config, determining the
certificates to use, etc) is handled by the SSH program itself. Other
programs that have similar behavior (like scp) gain this capability by
forking the SSH program to connect to the remote system.

Assuming file `req.txt` exists as a request to send to the OMI sevver,
you can achieve this type of behavior with a command like:

```
cat req.txt | ssh -l jeffcof -e none -T -x osdev64-ub16-01 "nc 127.0.0.1 5500"
```

Program ssh-fork.c does this sort of thing programmatically.
