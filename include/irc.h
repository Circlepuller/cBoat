/**
 * irc.h
 *
 * Made by Dan Saunders for cBoat
 */

#include <stdbool.h>

#include <socket.h>
#include <config.h>

#ifndef IRC_H_
#define IRC_H_

typedef struct irc_ {
	socket_t *sock;
	char *nick;

	void (*hook)(struct irc_ *, char *, char *, char *);
} irc_t;

irc_t *irc_init(bool);
bool irc_connect(irc_t *, char *, int);
bool irc_raw(irc_t *, const char *, ...);
bool irc_close(irc_t *);
bool irc_pass(irc_t *, char *);
bool irc_nick(irc_t *, char *);
bool irc_user(irc_t *, char *, short, char *);
bool irc_oper(irc_t *, char *, char *);
bool irc_quit(irc_t *, char *);
bool irc_join(irc_t *, char *);
bool irc_privmsg(irc_t *, char *, char *);
void irc_loop(irc_t *);
void irc_free(irc_t *);

void irc_parse_command(irc_t *, char *);
void irc_parse_prefix(irc_t *, char *, char **, char **, char **);
void irc_parse_text(irc_t *, char *, char **, char **);

#endif
