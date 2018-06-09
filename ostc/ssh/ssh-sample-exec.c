#include <libssh2.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

    return rc;
}

int main(int argc, char *argv[])
{
    const char *hostname = "10.185.82.159";  // osdev64-ub16-01.scx.com
    const char *commandline = "nc 127.0.0.1 5500";
    const char *username    = "jeffcof";
    const char *password    = "";
    const char *privateKey = "/home/jeffcof/.ssh/id_rsa";
    const char *publicKey = "/home/jeffcof/.ssh/id_rsa.pub";

    const int USEC_IN_SECOND = 1000000;

    unsigned long hostaddr;
    int sock;
    struct sockaddr_in sin;
    const char *fingerprint;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channel;
    int rc;
    int exitcode;
    char *exitsignal=(char *)"none";
    int bytecount = 0;
    size_t len;
    LIBSSH2_KNOWNHOSTS *nh;
    int type;
    char buf[16384];

#ifdef WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2,0), &wsadata);
    if (err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    if (argc > 1)
        /* must be ip address only */
        hostname = argv[1];

    if (argc > 2) {
        username = argv[2];
    }
    if (argc > 3) {
        password = argv[3];
    }
    if (argc > 4) {
        commandline = argv[4];
    }

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }

    hostaddr = inet_addr(hostname);

    /* Connect to the remote SSH server */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance */
    session = libssh2_session_init();
    if (!session)
        return -1;

    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(session, 0);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(session, sock)) ==
           LIBSSH2_ERROR_EAGAIN);
    if (rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }

    nh = libssh2_knownhost_init(session);
    if(!nh) {
        /* eeek, do cleanup here */
        return 2;
    }

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts",
                               LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    fingerprint = libssh2_session_hostkey(session, &len, &type);
    if(fingerprint) {
        struct libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
        /* introduced in 1.2.6 */
        int check = libssh2_knownhost_checkp(nh, hostname, 22,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);
#else
        /* 1.2.5 or older */
        int check = libssh2_knownhost_check(nh, hostname,
                                            fingerprint, len,
                                            LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                            LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                            &host);
#endif
        fprintf(stderr, "Host check: %d, key: %s\n", check,
                (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    }
    else {
        /* eeek, do cleanup here */
        return 3;
    }
    libssh2_knownhost_free(nh);

    if ( strlen(password) != 0 ) {
        /* We could authenticate via password */
        while ((rc = libssh2_userauth_password(session, username, password)) ==
               LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
            goto shutdown;
        }
    }
    else {
        /* Or by public key */
        while ((rc = libssh2_userauth_publickey_fromfile(session, username,
                                publicKey, privateKey, password)) ==
               LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            goto shutdown;
        }
    }

#if 0
    libssh2_trace(session, ~0 );
#endif

    /* Exec non-blocking on the remove host */
    while( (channel = libssh2_channel_open_session(session)) == NULL &&
           libssh2_session_last_error(session,NULL,NULL,0) ==
           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, session);
    }
    if( channel == NULL )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
    while( (rc = libssh2_channel_exec(channel, commandline)) ==
           LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, session);
    }
    if( rc != 0 )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }


    // Initial string to transmit to server
    {
        const char *xmlData = "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:wsmid=\"http://schemas.dmtf.org/wbem/wsman/identity/1/wsmanidentity.xsd\"><s:Header/><s:Body><wsmid:Identify/></s:Body></s:Envelope>";

        sprintf(buf,
                "POST /wsman HTTP/1.1\r\n"
                "Content-Type: application/soap+xml;charset=UTF-8\r\n"
                "User-Agent: Microsoft WinRM Client\r\n"
                "Host: localhost:7778\r\n"
                "Content-Length: %d\r\n"
                "Authorization: Basic c2N4dXNlcjpzY3h1c2Vy\r\n" /* "scxuser:scxuser" */
                "\r\n"
                "%s", strlen(xmlData), xmlData);
        len = strlen(buf);
        printf("Transmitting query request to server ...\n");
    }

//  while (1)
    {
        ssize_t wr;
        int i;

        int counter = 0;
        wr = 0;
        while(wr < len) {
            printf("  Sending %d bytes to server ...\n", len-wr);

            i = libssh2_channel_write(channel, buf + wr, len - wr);
            if (LIBSSH2_ERROR_EAGAIN == i) {
                continue;
            }
            if (i < 0) {
                fprintf(stderr, "libssh2_channel_write: %d\n", i);
                goto shutdown;
            }
            printf("    (Bytes written: %d)\n", i);
            wr += i;
        }
        len = 0;

        printf("  Reading response from server ...\n");

        while (counter < 5) {
            char wbuffer[1024];

            counter++;

            /* Sleep for 1/10th of a second; give time for OMI to respond */
            /* More intelligence in protocol will allow this to be optimized */
            usleep(USEC_IN_SECOND / 10);

            len = libssh2_channel_read(channel, buf, sizeof(buf));
            if (LIBSSH2_ERROR_EAGAIN == len) {
                printf("libssh2_channel_read says LIBSSH2_ERROR_EAGAIN ...\n");
                continue;
            }
            else if (len < 0) {
                fprintf(stderr, "libssh2_channel_read: %d", (int)len);
                goto shutdown;
            }
            memcpy(wbuffer, buf, len);
            wbuffer[len] = '\0';
            printf("%s", wbuffer);

            if (libssh2_channel_eof(channel)) {
                fprintf(stderr, "The server at %s disconnected!\n", hostname);
                goto shutdown;
            }

            // In general, once we get a response, we're done
            // (Server doesn't apepar to split responses in the channel)
            // Again, more intelligence in protocol would help here ...
            break;
        }
    }

    /* for( ;; ) */
    /* { */
    /*     /\* loop until we block *\/ */
    /*     int rc; */
    /*     do */
    /*     { */
    /*         char buffer[0x4000]; */
    /*         rc = libssh2_channel_read( channel, buffer, sizeof(buffer) ); */
    /*         if( rc > 0 ) */
    /*         { */
    /*             int i; */
    /*             bytecount += rc; */
    /*             fprintf(stderr, "We read:\n"); */
    /*             for( i=0; i < rc; ++i ) */
    /*                 fputc( buffer[i], stderr); */
    /*             fprintf(stderr, "\n"); */
    /*         } */
    /*         else { */
    /*             if( rc != LIBSSH2_ERROR_EAGAIN ) */
    /*                 /\* no need to output this for the EAGAIN case *\/ */
    /*                 fprintf(stderr, "libssh2_channel_read returned %d\n", rc); */
    /*         } */
    /*     } */
    /*     while( rc > 0 ); */

    /*     /\* this is due to blocking that would occur otherwise so we loop on */
    /*        this condition *\/ */
    /*     if( rc == LIBSSH2_ERROR_EAGAIN ) */
    /*     { */
    /*         waitsocket(sock, session); */
    /*     } */
    /*     else */
    /*         break; */
    /* } */

    exitcode = 127;
    while( (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
        waitsocket(sock, session);

    if( rc == 0 )
    {
        exitcode = libssh2_channel_get_exit_status( channel );
        libssh2_channel_get_exit_signal(channel, &exitsignal,
                                        NULL, NULL, NULL, NULL, NULL);
    }

    if (exitsignal)
        fprintf(stderr, "\nGot signal: %s\n", exitsignal);
    else 
        fprintf(stderr, "\nEXIT: %d bytecount: %d\n", exitcode, bytecount);

    libssh2_channel_free(channel);
    channel = NULL;

shutdown:

    libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}
