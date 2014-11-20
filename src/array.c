/**
 * array.c
 *
 * Made by Dan Saunders for cBoat
 */
 
#if __STDC_VERSION__ >= 199901L
#define _POSIX_C_SOURCE 200809L
#else
#error "__STDC_VERSION__ must be greater or equal to 199901L"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array.h>

array_t *array_init(int size, char *item)
{
	array_t *self = (array_t *)malloc(sizeof(array_t));
	int i = 0;

	self->length = 0;

	for (; i < size; i++) array_append(self, item);
	
	return self;
}

int array_split(array_t *self, char *str, char const *delimiters, int len)
{
	char *item, *end = NULL;
	int res = !len ? 0 : 1;

	if (!len || len > 1) {
		item = strtok_r(str, delimiters, &end);
		res++;

		while (item != NULL) {
			array_append(self, item);

			item = !len || res < len ? strtok_r(NULL, delimiters, &end) : NULL;
			res++;
		}
	}

	if (len) array_append(self, end == NULL ? str : end);

	return res;
}

inline char *array_append(array_t *self, char *item)
{
	return array_insert(self, -1, item);
}

int array_index(array_t *self, char *item)
{
	int i = 0;

	for (; i < self->length; i++) {
		if (!strcmp(self->items[i], item)) return i;
	}

	return -1;
}

inline char *array_get(array_t *self, int i)
{
	return i < self->length ? self->items[i < 0 ? self->length + i : i] : NULL;
}

char *array_insert(array_t *self, int i, char *item)
{
	int n;
	
	self->length++;
	i = i < 0 ? self->length + i : i;

	if (self->length == 1) {
		self->items = malloc(sizeof(char *));
	} else {
		self->items = realloc(self->items, self->length * sizeof(char *));

		for (n = i; n < self->length - 1; n++) self->items[n + 1] = self->items[n];
	}

	return self->items[i] = strdup(item);
}

void array_remove(array_t *self, char *item)
{
	int i = 0;

	for (; i < self->length; i++) {
		if (!strcmp(item, array_get(self, i))) {
			array_pop(self, i);

			return;
		}
	}
}

void array_pop(array_t *self, int i)
{
	if (i >= self->length) return;

	i = i < 0 ? self->length + i : i;

	if (self->items[i] != NULL) free(self->items[i]);

	for (; i < self->length; i++) self->items[i] = self->items[i + 1];

	self->items = realloc(self->items, --self->length * sizeof(char *));
}

void array_free(array_t *self)
{
	int i = 0;

	if (self->length) {
		while (i < self->length) free(self->items[i++]);
		
		free(self->items);
	}

	free(self);
}
