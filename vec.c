#include "vec.h"

size_t vec_len(t_vec *src)
{
	return (src->len);
}

size_t vec_alloc_size(t_vec *src)
{
	return (src->alloc_size);
}

size_t vec_elem_size(t_vec *src)
{
	return (src->elem_size);
}

size_t vec_byte_size(t_vec *src)
{
	return (vec_elem_size(src) * vec_len(src));
}

static void vec_init(t_vec *src, size_t elem_size)
{
	src->len = 0;
	src->elem_size = elem_size;
	src->alloc_size = 0;
	src->memory = NULL;
}

static int vec_alloc(t_vec *src, size_t alloc_size)
{
	src->memory = malloc(alloc_size);
	src->alloc_size = alloc_size;
	if (!src->memory)
		return (-1);
	return (1);
}

static bool vec_null(t_vec *src)
{
	return (src->memory == NULL);
}

static void *vec_memory(t_vec *src, size_t byte_index)
{
	if (!src || vec_null(src) || byte_index > vec_alloc_size(src))
		return (NULL);
	return (&src->memory[byte_index]);
}

int vec_new(t_vec *dst, size_t init_len, size_t elem_size)
{
	if (!dst || elem_size == 0)
		return (-1);
	vec_init(dst, elem_size);
	if (init_len > 0)
		return (vec_alloc(dst, init_len * elem_size));
	return (1);
}

int vec_new_from(t_vec *dst, void *src, size_t len, size_t elem_size)
{
	if (!dst || !src || elem_size == 0)
		return (-1);
	else if (vec_new(dst, len, elem_size) < 0)
		return (-1);
	memcpy(
		vec_memory(dst, 0),
		src,
		vec_alloc_size(dst));
	dst->len = len;
	return (1);
}

void vec_free(t_vec *src)
{
	if (!src || vec_alloc_size(src) == 0)
		return ;
	free(src->memory);
	vec_init(src, 0);
}

static int vec_realloc(t_vec *src, size_t target_len)
{
	if (!src)
		return (-1);
	else if (vec_null(src))
		return vec_new(src, target_len, vec_elem_size(src));
	src->memory = realloc(src->memory, target_len * vec_elem_size(src));
	src->alloc_size = target_len * vec_elem_size(src);
	return (1);
}

void *vec_get(t_vec *src, size_t index)
{
	if (!src || vec_null(src) || index >= vec_len(src))
		return (NULL);
	return (vec_memory(src, vec_elem_size(src) * index));
}

int vec_rcopy(t_vec *dst, t_vec *src, size_t start, size_t end)
{
	size_t	copy_size;

	if (!dst
		|| !src
		|| vec_null(src)
		|| vec_null(dst)
		|| vec_elem_size(src) != vec_elem_size(dst)
		|| end > vec_len(src)
		|| start > end
		|| vec_alloc_size(dst) < (end - start) * vec_elem_size(dst))
		return (-1);
	if (vec_byte_size(src) < vec_alloc_size(dst))
		copy_size = vec_byte_size(src);
	else
		copy_size = vec_alloc_size(dst);
	memcpy(
		vec_memory(dst, 0),
		vec_memory(src, start * vec_elem_size(src)),
		copy_size);
	dst->len = end - start;
	return (1);
}

int vec_ncopy(t_vec *dst, t_vec *src, size_t nelems)
{
	return (vec_rcopy(dst, src, 0, nelems));
}

int vec_copy(t_vec *dst, t_vec *src)
{
	return (vec_ncopy(dst, src, vec_len(src)));
}

int vec_push(t_vec *dst, void *src)
{
	if (!dst || !src)
		return (-1);
	else if (vec_null(dst))
		vec_new(dst, 1, vec_elem_size(dst));
	if (vec_byte_size(dst) >= vec_alloc_size(dst))
		if (vec_realloc(dst, dst->len * 2) < 0)
			return (-1);
	memcpy(
		vec_memory(dst, vec_byte_size(dst)),
		src,
		vec_elem_size(dst));
	dst->len++;
	return (1);
}

int vec_pop(void *dst, t_vec *src)
{
	if (!dst || !src)
		return (-1);
	else if (vec_null(src) || vec_len(src) == 0)
		return (0);
	memcpy(
		dst,
		vec_get(src, vec_len(src) - 1),
		vec_elem_size(src));
	src->len--;
	return (1);
}

int vec_clear(t_vec *src)
{
	if (!src)
		return (-1);
	src->len = 0;
	return (1);
}

int vec_insert(t_vec *dst, void *src, size_t index)
{
	if (!dst || !src || index > dst->len)
		return (-1);
	else if (index == dst->len)
		return (vec_push(dst, src));
	if (vec_byte_size(dst) >= vec_alloc_size(dst))
		if (vec_realloc(dst, vec_byte_size(dst) * 2) < 0)
			return (-1);
	memmove(
		vec_get(dst, index + 1),
		vec_get(dst, index),
		(dst->len - index) * vec_elem_size(dst));
	memcpy(
		vec_get(dst, index),
		src, vec_elem_size(dst));
	dst->len++;
	return (1);
}

int vec_remove(t_vec *src, size_t index)
{
	if (!src || index > vec_len(src))
		return (-1);
	else if (index == vec_len(src))
	{
		src->len--;
		return (1);
	}
	memmove(
		vec_get(src, index),
		vec_memory(src, vec_elem_size(src) * (index + 1)),
		(vec_len(src) - index) * vec_elem_size(src));
	src->len--;
	return (1);
}

int vec_append(t_vec *dst, t_vec *src)
{
	int		ret;
	size_t	alloc_size;

	if (!dst || !src || vec_null(src))
		return (-1);
	else if (vec_null(dst))
		vec_new(dst, 1, vec_elem_size(dst));
	alloc_size = vec_byte_size(dst) + vec_byte_size(src);
	if (vec_alloc_size(dst) < alloc_size)
	{
		if (vec_alloc_size(dst) * 2 < alloc_size)
			ret = vec_realloc(dst, alloc_size);
		else
			ret = vec_realloc(dst, vec_alloc_size(dst) * 2);
		if (ret < 0)
			return (-1);
	}
	memcpy(
		vec_memory(dst, vec_byte_size(dst)),
		vec_memory(src, 0),
		vec_byte_size(src));
	dst->len += src->len;
	return (1);
}

int vec_prepend(t_vec *dst, t_vec *src)
{
	t_vec	 new;
	size_t	 alloc_size;

	if (!dst || !src)
		return (-1);
	else if (vec_null(dst))
		vec_new(dst, 1, vec_elem_size(dst));
	alloc_size = vec_byte_size(dst) + vec_byte_size(src);
	if (vec_new(&new, alloc_size / vec_elem_size(dst), vec_elem_size(dst)) < 0)
		return (-1);
	vec_copy(&new, src);
	new.len = vec_len(src) + dst->len;
	memcpy(
		vec_memory(&new, vec_byte_size(dst)),
		vec_memory(dst, 0),
		vec_byte_size(dst));
	vec_free(dst);
	*dst = new;
	return (1);
}

void vec_iter(t_vec *src, void (*f) (void *))
{
	size_t	i;

	if (!src || vec_null(src))
		return ;
	i = 0;
	while (i < vec_len(src))
	{
		f(vec_get(src, i));
		i++;
	}
}

void *vec_find(t_vec *src, bool (*f) (void *))
{
	size_t	i;

	if (!src || vec_null(src))
		return (NULL);
	i = 0;
	while (i < vec_len(src))
	{
		if (f(vec_get(src, i)) == true)
			return (vec_get(src, i));
		i++;
	}
	return (NULL);
}

int vec_map(t_vec *dst, t_vec *src, void (*f) (void *))
{
	void	*res;
	size_t	i;

	if (!dst || !src || vec_null(src))
		return (-1);
	else if (vec_null(dst))
		vec_new(dst, 1, vec_elem_size(dst));
	res = malloc(vec_elem_size(dst));
	if (!res)
		return (-1);
	i = 0;
	while (i < vec_len(src))
	{
		memcpy(res, vec_get(src, i), vec_elem_size(dst));
		f(res);
		vec_push(dst, res);
		i++;
	}
	free(res);
	return (1);
}

int vec_filter(t_vec *dst, t_vec *src, bool (*f) (void *))
{
	void	*res;
	size_t	i;

	if (!dst || !src || vec_null(src))
		return (-1);
	else if (vec_null(dst))
		vec_new(dst, 1, vec_elem_size(dst));
	res = malloc(vec_elem_size(dst));
	if (!res)
		return (-1);
	i = 0;
	while (i < vec_len(src))
	{
		memcpy(res, vec_get(src, i), vec_elem_size(dst));
		if (f(res) == true)
			vec_push(dst, res);
		i++;
	}
	free(res);
	return (1);
}

int vec_reduce(void *dst, t_vec *src, void (*f) (void *, void *))
{
	size_t	i;

	if (!dst || !src || vec_null(src))
		return (-1);
	i = 0;
	while (i < vec_len(src))
	{
		f(dst, vec_get(src, i));
		i++;
	}
	return (1);
}

static void memswap8(unsigned char *a, unsigned char *b)
{
	if (a == b)
		return ;
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

static void memswap(unsigned char *a, unsigned char *b, size_t bytes)
{
	size_t	i;

	if (!a || !b)
		return ;
	i = 0;
	while (i < bytes)
	{
		memswap8(&a[i], &b[i]);
		i++;
	}
}

static void vec_sort_recurse(t_vec *src,
	long int low,
	long int high,
	int (*f)(void *, void *))
{
	long int pivot;
	long int a;
	long int b;

	if (low >= high)
		return ;
	pivot = low;
	a = low;
	b = high;
	while (a < b)
	{
		while (a <= high && f(vec_get(src, a), vec_get(src, pivot)) <= 0)
			a++;
		while (b >= low && f(vec_get(src, b), vec_get(src, pivot)) > 0)
			b--;
		if (a < b)
			memswap(vec_get(src, a), vec_get(src, b), vec_elem_size(src));
	}
	memswap(vec_get(src, pivot), vec_get(src, b), vec_elem_size(src));
	vec_sort_recurse(src, low, b - 1, f);
	vec_sort_recurse(src, b + 1, high, f);
}

void	vec_sort(t_vec *src, int (*f)(void *, void *))
{
	if (!src || vec_null(src))
		return ;
	vec_sort_recurse(src, 0, vec_len(src) - 1, f);
}
