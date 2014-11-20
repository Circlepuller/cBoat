/**
 * socket.c
 *
 * Made by Dan Saunders for cBoat
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <memory.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#ifdef SOCKET_SSL
#include <openssl/ssl.h>
#endif

#include <socket.h>

socket_t *socket_init(int domain, int type, bool use_ssl)
{
	socket_t *self = (socket_t *)malloc(sizeof(socket_t));

	self->family = domain;
	self->fd = socket(domain, type, 0);

#ifdef SOCKET_SSL
	self->using_ssl = use_ssl;
#endif

	// Return a NULL pointer if there was an error creating the socket
	if (self->fd != -1) {
		return self;
	}
	
	free(self);

	return NULL;
}

bool socket_connect(socket_t *self, char *host, int port)
{
	struct hostent *ht;
	struct sockaddr_in caddr;

	if ((ht = gethostbyname(host != NULL ? host : self->host)) == NULL) {
		errno = h_errno;

		return false;
	}

	caddr.sin_port = htons(port != 0 ? port : self->port);
	caddr.sin_family = self->family;
	memcpy(&caddr.sin_addr, ht->h_addr_list[0], ht->h_length);

	self->host = host;
	self->port = port;

#ifdef SOCKET_SSL
	if (self->using_ssl && !socket_ssl_init(self)) {
		return false;
	}
#endif

	if (!connect(self->fd, (struct sockaddr *)&caddr, sizeof(struct sockaddr_in))) {
#ifdef SOCKET_SSL
		if (self->using_ssl) {
			return SSL_connect(self->ssl) == 1;
		}
#endif

		return true;
	}
	
	return false;
}

char *socket_read(socket_t *self, size_t len)
{
	int res, end;
	char *buf = calloc(len + 1, sizeof(char));

#if SOCKET_SSL
	if (self->using_ssl) {
		end = SSL_read(self->ssl, buf, len);
		res = end > 0;
	} else {
#endif
		end = read(self->fd, buf, len);
		res = end != -1;
#if SOCKET_SSL
	}
#endif

	if (res) {
		return buf;
	}
	
	free(buf);
	
	return NULL;
}

bool socket_write(socket_t *self, char *data)
{
#if SOCKET_SSL
	if (self->using_ssl) {
		return SSL_write(self->ssl, data, strlen(data)) > 0 ? true : false;
	}
#endif

	return write(self->fd, data, strlen(data)) != -1 ? true : false;
}

bool socket_close(socket_t *self)
{
#ifdef SOCKET_SSL
	if (self->using_ssl) {
		socket_ssl_free(self);
	}
#endif

	return close(self->fd) != -1 ? true : false;
}

void socket_free(socket_t *self)
{
#ifdef SOCKET_SSL
	if (self->using_ssl) {
		socket_ssl_free(self);
	}
#endif

	free(self);
}

#ifdef SOCKET_SSL
bool socket_ssl_init(socket_t *self)
{
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	self->ssl_ctx = SSL_CTX_new(SSLv3_client_method());

	if (self->ssl_ctx == NULL) {
		return false;
	}
	
	SSL_CTX_set_options(self->ssl_ctx, SSL_OP_NO_SSLv2);

	self->ssl = SSL_new(self->ssl_ctx);
	
	SSL_set_fd(self->ssl, self->fd);

	return true;
}

void socket_ssl_free(socket_t *self)
{
	if (self->ssl != NULL) SSL_free(self->ssl);
	if (self->ssl_ctx != NULL) SSL_CTX_free(self->ssl_ctx);
}
#endif
