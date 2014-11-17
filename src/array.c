/**
 * array.c
 *
 * Made by Dan Saunders for cBoat
 */

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

char *array_append(array_t *self, char *item)
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

char *array_get(array_t *self, int i)
{
	if (i < self->length) {
		return self->items[i < 0 ? self->length + i : i];
	} else {
		return NULL;
	}
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
