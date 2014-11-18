/**
 * array.h
 *
 * Made by Dan Saunders for cBoat
 */

#ifndef ARRAY_H_
#define ARRAY_H_

typedef struct {
	char **items;
	int length;
} array_t;

array_t *array_init(int, char *);
int array_split(array_t *, char *, const char *, int);
char *array_append(array_t *, char *);
char *array_get(array_t *, int);
int array_index(array_t *, char *);
char *array_insert(array_t *, int, char *);
void array_remove(array_t *, char *);
void array_pop(array_t *, int);
void array_free(array_t *);

#endif
