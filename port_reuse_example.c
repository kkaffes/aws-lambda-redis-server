#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <linux/filter.h>

#define BUFSIZE 1024

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))
/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
        int parentfd; /* parent socket */
        int childfd; /* child socket */
        int portno; /* port to listen on */
        int clientlen; /* byte size of client's address */
        struct sockaddr_in serveraddr; /* server's addr */
        struct sockaddr_in clientaddr; /* client addr */
        struct hostent *hostp; /* client host info */
        char buf[BUFSIZE]; /* message buffer */
        char *hostaddrp; /* dotted decimal host addr string */
        int n; /* message byte size */

        /*
         * check command line arguments
         */
        if (argc != 2) {
          fprintf(stderr, "usage: %s <port>\n", argv[0]);
          exit(1);
        }
        portno = atoi(argv[1]);

	/*
         * socket: create the parent socket
         */
        parentfd = socket(AF_INET, SOCK_STREAM, 0);
        if (parentfd < 0)
          error("ERROR opening socket");

	// Set TCP_FASTOPEN so that TCP handshake packets contain data that
	// allow the filter to determine whether this a Redis or LCP connection.
        int opt = 4;
        if (setsockopt(parentfd, SOL_TCP, TCP_FASTOPEN, &opt, sizeof(opt)))
                error("ERROR setting TCP_FASTOPEN");

	// Set SO_REUSEPORT so that the same port can be reused by Redis.
        int optval=1;
        setsockopt(parentfd, SOL_SOCKET, SO_REUSEPORT, (const void *)&optval,
                   sizeof(optval));

        // Magic LCP byte here
	uint32_t magic_byte = 0x00000000;
	struct sock_filter code[] = {
                { 0x30, 0, 0, 0x00000000 },
                { 0x15, 1, 0, magic_byte },
                { 0x06, 0, 0, 0x00000001 },
                { 0x06, 0, 0, 0x00000000 },
        };

	struct sock_fprog bpf = {
                .len = ARRAY_SIZE(code),
                .filter = code,
        };

	// Attach the filter that distributes the packets between LCP and Redis
        if (setsockopt(parentfd, SOL_SOCKET, SO_ATTACH_REUSEPORT_CBPF,
                       &bpf , sizeof(bpf)))
                error("ERROR setting SO_ATTACH_REUSEPORT_CBPF");

	/*
         * build the server's Internet address
         */
        bzero((char *) &serveraddr, sizeof(serveraddr));

        /* this is an Internet address */
        serveraddr.sin_family = AF_INET;

       /* let the system figure out our IP address */
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        /* this is the port we will listen on */
        serveraddr.sin_port = htons((unsigned short)portno);

        /*
         * bind: associate the parent socket with a port
         */
        if (bind(parentfd, (struct sockaddr *) &serveraddr,
                 sizeof(serveraddr)) < 0)
          error("ERROR on binding");

        /*
         * listen: make this socket ready to accept connection requests
         */
        if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
                 error("ERROR on listen");
        /*
         * main loop: wait for a connection request, echo input line,
         * then close connection.
         */
        clientlen = sizeof(clientaddr);
	while (1) {
                /*
                 * accept: wait for a connection request
                 */
                childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
                if (childfd < 0)
                  error("ERROR on accept");

                /*
                 * gethostbyaddr: determine who sent the message
                 */
                hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                                  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
                hostaddrp = inet_ntoa(clientaddr.sin_addr);
                if (hostaddrp == NULL)
                  error("ERROR on inet_ntoa\n");
                printf("Client IP: %s\n", hostaddrp);
                /*
                 * read: read input string from the client
                 */
                bzero(buf, BUFSIZE);
                n = read(childfd, buf, BUFSIZE);
                if (n < 0)
                  error("ERROR reading from socket");
                printf("server received %d bytes: %s", n, buf);

                /*
                 * write: echo the input string back to the client
                 */
                n = write(childfd, buf, strlen(buf));
                if (n < 0)
                  error("ERROR writing to socket");

                close(childfd);
        }
}

