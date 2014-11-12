/**
 * socket.h
 *
 * Made by Dan Saunders for cBoat
 */

#include <stdbool.h>

#ifdef SOCKET_SSL
#include <openssl/ssl.h>
#endif

#ifndef SOCKET_H_
#define SOCKET_H_

typedef struct {
	int fd;

#ifdef SOCKET_SSL
	bool using_ssl;
	SSL *ssl;
	SSL_CTX *ssl_ctx;
#endif

	int family;
	char *host;
	int port;
} socket_t;

socket_t *socket_init(int, int, bool);
bool socket_connect(socket_t *, char *, int);
char *socket_read(socket_t *, size_t);
bool socket_write(socket_t *, char *);
bool socket_close(socket_t *);
void socket_free(socket_t *);

#ifdef SOCKET_SSL
bool socket_ssl_init(socket_t *);
void socket_ssl_free(socket_t *);
#endif
#endif
