/**
 * irc.c
 *
 * Made by Dan Saunders for cBoat
 */

#if __STDC_VERSION__ >= 199901L
#define _POSIX_C_SOURCE 200101L
#else
#error "__STDC_VERSION__ must be greater or equal to 199901L"
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include <config.h>
#include <socket.h>
#include <irc.h>

irc_t *irc_init(config_t *conf)
{
	irc_t *self = (irc_t *)malloc(sizeof(irc_t));

	self->conf = conf;
	self->sock = socket_init(AF_INET, SOCK_STREAM, config_has_key(self->conf, "ssl") ? config_get_bool(self->conf, "ssl") : false);

	self->hook = NULL;

	if (self->sock != NULL) {
		return self;
	}

	irc_free(self);

	return NULL;
}

bool irc_connect(irc_t *self, char *host, int port)
{
#ifdef IRC_DEBUG
	printf("-> Opening connection to %s:%d\n", host, port);
#endif

	return socket_connect(self->sock, host, port);
}

bool irc_raw(irc_t *self, char const *format, ...)
{
	char *buf = calloc(IRC_BUFLEN, sizeof(char));
	bool res;
	va_list args;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	buf = realloc(buf, (strlen(buf) + 4) * sizeof(char));


#ifdef IRC_DEBUG
	printf("<- %s\n", buf);
#endif

	strcat(buf, "\r\n");

	res = socket_write(self->sock, buf);

	free(buf);
	
	return res;
}

void irc_parse_command(irc_t *self, char *message)
{
	char *prefix = NULL,
		 *command = NULL,
		 *args = NULL;

#ifdef IRC_DEBUG
	printf("-> %s\n", message);
#endif

	if (message[0] == ':') {
		prefix = strtok_r(message, ": ", &args);
	}

	command = strtok_r(args != NULL ? NULL : message, " ", &args);

	if (self->hook != NULL) {
		self->hook(self, command, prefix, args);
	}
}

bool irc_close(irc_t *self)
{
#ifdef IRC_DEBUG
	printf("<- Closed connection to %s:%d\n", self->sock->host, self->sock->port);
#endif

	return socket_close(self->sock);
}

inline bool irc_pass(irc_t *self, char *pass)
{
	return irc_raw(self, "PASS %s", pass);
}

inline bool irc_nick(irc_t *self, char *nick)
{
	self->nick = nick;

	return irc_raw(self, "NICK %s", nick);
}

inline bool irc_user(irc_t *self, char *user, short mode, char *realname)
{
	return irc_raw(self, "USER %s %d * :%s", user, mode, realname);
}

inline bool irc_oper(irc_t *self, char *name, char *password)
{
	return irc_raw(self, "OPER %s %s", name, password);
}

inline bool irc_quit(irc_t *self, char *message)
{
	return irc_raw(self, "QUIT :%s", message);
}

inline bool irc_join(irc_t *self, char*channels)
{
	return irc_raw(self, "JOIN %s", channels);
}

inline bool irc_privmsg(irc_t *self, char *channels, char *msg)
{
	return irc_raw(self, "PRIVMSG %s :%s", channels, msg);
}

void irc_loop(irc_t *self)
{
	int i = 0;
	char *c,
		 *buf = calloc(IRC_BUFLEN + 1, sizeof(char));

	while ((c = socket_read(self->sock, 1)) != NULL && errno != 512 && strlen(c)) {
		if (!strcmp(c, "\r") && i) {
			irc_parse_command(self, buf);
			memset(buf, 0, (IRC_BUFLEN + 1) * sizeof(char));
			i = 0;
		} else if (strcmp(c, "\n")) {
			strcat(buf, c);
			i++;
		}

		free(c);
	}

	free(c);
	free(buf);
}

void irc_free(irc_t *self)
{
	socket_free(self->sock);
	free(self);
}

void irc_parse_prefix(irc_t *self, char *prefix, char **nick, char **user, char **host)
{
	*nick = *user = *host = NULL;

	if (prefix != NULL && strchr(prefix, '!') == NULL) {
		*host = prefix;
	} else if (prefix != NULL) {
		*nick = strtok_r(prefix, "!", host);
		*user = strtok_r(NULL, "@", host);
	}
}

void irc_parse_text(irc_t *self, char *args, char **source, char **text)
{
	*source = *text = NULL;

	*source = strtok_r(args, " :", text);
	(*text)++;
}
