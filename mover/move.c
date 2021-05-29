#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "hwioab.h"

#define BUF_SIZE (500)
#define PORT ("8035")

typedef unsigned char cmd_t;
#define CMD_DELIM (4)
#define CMD_MOVE  ((cmd_t)((1 << CMD_DELIM) - 1))
#define CMD_SPEED ((cmd_t)(~CMD_MOVE))


int get_speed(cmd_t cmd)
{
	return (cmd & CMD_SPEED) >> CMD_DELIM;
}

enum move_t get_move(cmd_t cmd)
{
	int id = (cmd & CMD_MOVE);
	enum move_t move = STOP;
	switch (id) {
		case 0:
			move = STOP;
			break;
		case 1:
			move = FORWARD;
			break;
		case 2:
			move = BACKWARD;
			break;
		case 3:
			move = LEFT;
			break;
		case 4:
			move = RIGHT;
			break;
		case 5:
			move = FAST_RIGHT;
			break;
		case 6:
			move = FAST_LEFT;
			break;
		default:
			move = STOP;
			break;
	}
	return move;
}

int main(int argc, char *argv[])
{
	(void)argv;
	(void)argc;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	char buf[BUF_SIZE];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(NULL, PORT, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		Try each address until we successfully bind(2).
		If socket(2) (or bind(2)) fails, we (close the socket
		and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;                  /* Success */
		}

		close(sfd);
	}

	freeaddrinfo(result);           /* No longer needed */

	if (rp == NULL) {               /* No address succeeded */
			fprintf(stderr, "Could not bind\n");
			exit(EXIT_FAILURE);
	}

	/* Read datagrams and echo them back to sender */
	for ( ; ; ) {
		peer_addr_len = sizeof(peer_addr);
		nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&peer_addr, &peer_addr_len);
		if (nread == -1) {
			continue;               /* Ignore failed request */
		}

		char host[NI_MAXHOST], service[NI_MAXSERV];

		s = getnameinfo((struct sockaddr *)&peer_addr,
										peer_addr_len, host, NI_MAXHOST,
										service, NI_MAXSERV, NI_NUMERICSERV);
		if (s != 0) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
		}

		if (sendto(sfd, buf, (size_t)nread, 0, (struct sockaddr *)&peer_addr, peer_addr_len) != nread) {
			fprintf(stderr, "Error sending response\n");
		}
		for (int i = 0; i < nread; i++) {
			enum move_t move = get_move((cmd_t)buf[i]);
			int speed = get_speed((cmd_t)buf[i]);
			hwioab_output(speed, move);
		}
	}
	return 0;
}
