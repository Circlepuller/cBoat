/**
 * cBoat
 * Made by Dan Saunders
 *
 * An IRC bot written in C
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>

#include <socket.h>
#include <irc.h>
#include <config.h>

#ifndef IRC_BUFLEN
#error "IRC_BUFLEN must be defined"
#endif

void event_handler(irc_t *irc, char *command, char *prefix, char *args)
{
	char *nick, *user, *host, *source, *text, *action;
	irc_parse_prefix(irc, prefix, &nick, &user, &host);

	if (!strcmp(command, "PING")) {
		irc_raw(irc, "PONG %s", args);
	} else if (!strcmp(command, "PRIVMSG")) {
		irc_parse_text(irc, args, &source, &text);

		action = strtok_r(NULL, " ", &text);

		if (!strcmp(action, "!dice")) {
			irc_raw(irc, "PRIVMSG %s :%s rolled a %d!", strchr(source, '#') != NULL ? source : irc->nick, nick, (rand() % 6) + 1);
		} else if (strchr(source, '#') != NULL && action != NULL) {
			if (!strcmp(nick, "Circlepuller") && !strcmp(action, "!say")) {
				irc_privmsg(irc, source, text);
			} else if (!strcmp(nick, "Circlepuller") && !strcmp(action, "!quit")) {
				irc_quit(irc, strlen(text) ? text : "Goodbye...");
			}
		}
	} else if (!strcmp(command, "JOIN") && !strcmp(irc->nick, nick)) {
		irc_parse_text(irc, args, &source, &text);

		irc_raw(irc, "PRIVMSG %s :Hi, I'm %s!", source, irc->nick);
	}
}

int main(int argc, char *argv[])
{
	config_t *config;
	irc_t *irc;
	char *nick, *user, *real;
	
	if (argc != 2) {
		printf("usage: %s [config file]\n", argv[0]);
		exit(EXIT_FAILURE);
	} 
	
	srand(time(NULL));

	config = config_init(argv[1]);
	nick = config_get_string(config, "nick");
	user = config_get_string(config, "user");
	real = config_get_string(config, "real");
	irc = irc_init(!strcmp(config_get_string(config, "ssl"), "yes") ? true : false);

	if (irc_connect(irc, config_get_string(config, "host"), atoi(config_get_string(config, "port")))) {
		irc->hook = event_handler;

		if (config_has_key(config, "pass")) irc_pass(irc, config_get_string(config, "pass"));

		irc_nick(irc, nick);
		irc_user(irc, user, 0, real);
		irc_loop(irc);
		irc_close(irc);
	} else {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
	}

	irc_free(irc);
	config_free(config);

	exit(EXIT_SUCCESS);
	return 0;
}
