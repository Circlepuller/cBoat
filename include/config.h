/**
 * config.h
 *
 * Made by Dan Saunders for cBoat
 */

#include <stdbool.h>

#include <array.h>

#ifndef CONFIG_H_
#define CONFIG_H_

typedef struct {
	FILE *fp;
	array_t *keys;
	array_t *values;
} config_t;

config_t *config_init(char *);
void config_parse_file(config_t *);
void config_parse_line(config_t *, char *);
bool config_has_key(config_t *, char *);
bool config_get_bool(config_t *, char *);
char *config_get_string(config_t *, char *);
void config_free(config_t *);

#endif
