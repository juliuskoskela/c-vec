#ifndef VEC_H
# define VEC_H

#include "stdlib.h"
#include "stdint.h"
#include "unistd.h"
#include "string.h"
#include "stdbool.h"

typedef struct s_vec
{
	uint8_t	*memory;
	size_t	elem_size;
	size_t	alloc_size;
	size_t	len;
}	t_vec;

ssize_t	vec_new(t_vec *src, size_t len, size_t elem_size);
void	vec_free(t_vec *src);
ssize_t	vec_from(t_vec *dst, void *src, size_t len, size_t elem_size);
ssize_t vec_push(t_vec *src, void *elem);
ssize_t vec_pop(void *dst, t_vec *src);
ssize_t vec_copy(t_vec *dst, t_vec *src);
void	*vec_get(t_vec *src, size_t index);
ssize_t	vec_insert(t_vec *dst, void *elem, size_t index);
ssize_t	vec_remove(t_vec *src, size_t index);
ssize_t vec_append(t_vec *dst, t_vec *src);
ssize_t vec_prepend(t_vec *dst, t_vec *src);
void	vec_iter(t_vec *src, void (*f) (void *));
void	vec_map(t_vec *dst, t_vec *src, void (*f) (void *));
void	vec_filter(t_vec *dst, t_vec *src, bool (*f) (void *));
void 	vec_reduce(void *dst, t_vec *src, void (*f) (void *, void *));

#endif
