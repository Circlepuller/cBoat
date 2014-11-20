/**
 * cBoat
 * Made by Dan Saunders
 *
 * An IRC bot written in C
 */

#if __STDC_VERSION__ >= 199901L
#define _POSIX_C_SOURCE 200809L
#else
#error "__STDC_VERSION__ must be greater or equal to 199901L"
#endif

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <unistd.h>

#include <socket.h>
#include <irc.h>
#include <config.h>

#ifndef IRC_BUFLEN
#error "IRC_BUFLEN must be defined"
#endif

char const *program_name;

void die(char const *message)
{
	fprintf(stderr, "%s: error: %s\n", program_name, message);
	exit(EXIT_FAILURE);
}

void event_handler(irc_t *irc, char *command, char *prefix, char *args)
{
	array_t *params;
	char *nick, *user, *host, *source, *text, *action;

	irc_parse_prefix(irc, prefix, &nick, &user, &host);

	/**
	 * 11/17/14: Implemented array_split(). Sorta wacky behavior - but it works for me. :s
	 *           Next up - implement it elsewhere.
	 */

	if (!strcmp(command, "PING")) {
		irc_raw(irc, "PONG %s", args);
	} else if (!strcmp(command, "PRIVMSG")) {
		params = array_init(0, NULL);

		irc_parse_text(irc, args, &source, &text);
		array_split(params, text, " ", 2);
		
		action = array_get(params, 0);

		if (!strcmp(action, "!dice")) {
			irc_raw(irc, "PRIVMSG %s :%s rolled a %d!", strchr(source, '#') != NULL ? source : irc->nick, nick, (rand() % 6) + 1);
		} else if (strchr(source, '#') != NULL && action != NULL) {
			if (!strcmp(nick, "Circlepuller") && !strcmp(action, "!say")) {
				irc_privmsg(irc, source, array_get(params, 1));
			}
		} else if (!strcmp(nick, "Circlepuller") && !strcmp(action, "!quit")) {
			irc_quit(irc, strlen(array_get(params, 1)) ? array_get(params, 1) : "Goodbye...");
		}

		array_free(params);
	} else if (!strcmp(command, "JOIN") && !strcmp(irc->nick, nick)) {
		irc_parse_text(irc, args, &source, &text);

		irc_raw(irc, "PRIVMSG %s :Hi, I'm %s!", source, irc->nick);
	}
}

int const main(int const argc, char *const *argv)
{
	config_t *config = NULL;
	irc_t *irc;
	int c;
	char *nick = NULL,
		 *user = NULL,
		 *real = NULL;

	srand(time(NULL));

	program_name = argv[0];

	// getopt() functionality WIP
	while ((c = getopt(argc, argv, "c:")) != -1)
		switch (c) {
			case 'c':
				if ((config = config_init(optarg)) == NULL) die(strerror(errno));
				break;

			case 'n':
				nick = optarg;
				break;
			case 'u':
				user = optarg;
				break;
			case 'r':
				real = optarg;
				break;

			default:
				die("something in the option parser broke");
		}
	
	if (config == NULL) {
		die("a config must be supplied (for now) with option '-c'");
	}

	nick = config_has_key(config, "nick") ? config_get_string(config, "nick") : nick;
	user = config_has_key(config, "user") ? config_get_string(config, "user") : user;
	real = config_has_key(config, "real") ? config_get_string(config, "real") : real;

	if ((irc = irc_init(config_has_key(config, "ssl") ? config_get_bool(config, "ssl") : false)) == NULL) {
		config_free(config);
		die(strerror(errno));
	}

	// TODO: Replace atoi() with a safer function
	if (irc_connect(irc, config_get_string(config, "host"), atoi(config_get_string(config, "port")))) {
		irc->hook = event_handler;

		if (config_has_key(config, "pass")) irc_pass(irc, config_get_string(config, "pass"));

		irc_nick(irc, nick);
		irc_user(irc, user, 0, real);
		irc_loop(irc);
		irc_close(irc);
	} else {
		// TODO: Figure out what the program isn't freeing here on failure
		irc_free(irc);
		die(strerror(errno));
	}

	irc_free(irc);
	config_free(config);

	exit(EXIT_SUCCESS);
	return 0;
}
