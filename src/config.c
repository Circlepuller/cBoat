/**
 * config.c
 *
 * Made by Dan Saunders for cBoat
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array.h>
#include <config.h>

config_t *config_init(char *filename)
{
	config_t *self = (config_t *)malloc(sizeof(config_t));

	self->keys = array_init(0, NULL);
	self->values = array_init(0, NULL);

	if ((self->fp = fopen(filename, "r")) == NULL) {
		free(self);

		return NULL;
	}

	config_parse_file(self);

	fclose(self->fp);

	return self;
}

void config_parse_file(config_t *self)
{
	int i = 0;
	char c, s[2],
		 *buf = malloc(4097 * sizeof(char));
		 
	memset(buf, 0, 4097 * sizeof(char));

	while ((c = fgetc(self->fp)) != EOF) {
		s[0] = c;
		s[1] = 0;

		if (c == '\n' && i) {
			buf[i++] = 0;
			buf = realloc(buf, i * sizeof(char));
			config_parse_line(self, buf);
			buf = realloc(buf, 4097 * sizeof(char));
			memset(buf, 0, 4097 * sizeof(char));
			i = 0;
		} else if (c != '\n') {
			strcat(buf, s);
			i++;
		}
	}

	free(buf);
}

// This parser is crap.
void config_parse_line(config_t *self, char *line)
{
	int i;
	char *key, *value;

	if (line[0] == ';' || line[0] == '#' || !isgraph(line[0])) return;
	if (!strchr(line, '=')) return;

	// I'll add debug stuff here soon
	for (i = 0; line[i] != '='; i++) if (!isgraph(line[i])) return;
	for (i++; i < strlen(line); i++) if (!isprint(line[i])) return;

	key = strtok_r(line, "=", &value);

	if (!strlen(key) || !strlen(value)) return;

	array_append(self->keys, key);
	array_append(self->values, value);
}

bool config_has_key(config_t *self, char *key)
{
	return array_index(self->keys, key) > -1;
}

bool config_get_bool(config_t *self, char *key)
{
	char *keys[] = {"true", "yes"};
	int i = 0;

	// TODO: Number truths
	while (i < 2) if (!strcmp(keys[i++], key)) return true;

	return false;
}

char *config_get_string(config_t *self, char *key)
{
	int i;

	for (i = 0; i < self->keys->length; i++) {
		if (!strcmp(key, array_get(self->keys, i))) {
			return array_get(self->values, i);
		}
	}

	return NULL;
}

void config_free(config_t *self)
{
	array_free(self->keys);
	array_free(self->values);
	free(self);
}
