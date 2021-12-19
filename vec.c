#include "vec.h"

ssize_t vec_new(t_vec *dst, size_t init_alloc, size_t elem_size)
{
	if (!dst || elem_size == 0 || init_alloc == 0)
		return (-1);
	dst->alloc_size = init_alloc * elem_size;
	dst->len = 0;
	dst->elem_size = elem_size;
	dst->memory = malloc(dst->alloc_size);
	if (!dst->memory)
	{
		dst->alloc_size = 0;
		dst->len = 0;
		dst->elem_size = 0;
		return (-1);
	}
	return ((ssize_t)dst->alloc_size);
}

void vec_free(t_vec *src)
{
	if (!src)
		return ;
	free(src->memory);
	src->memory = NULL;
	src->alloc_size = 0;
	src->elem_size = 0;
	src->len = 0;
}

ssize_t vec_from(t_vec *dst, void *src, size_t len, size_t elem_size)
{
	if (!src || !src || elem_size == 0)
		return (-1);
	dst->alloc_size = len * elem_size;
	dst->len = len;
	dst->elem_size = elem_size;
	dst->memory = malloc(dst->alloc_size);
	dst->memory = memcpy(dst->memory, src, dst->alloc_size);
	return ((ssize_t)dst->alloc_size);
}

ssize_t vec_copy(t_vec *dst, t_vec *src)
{
	size_t	copy_size;

	if (!dst || !src)
		return (-1);
	if (src->len * src->elem_size < dst->alloc_size)
		copy_size = src->len * src->elem_size;
	else
		copy_size = src->alloc_size;
	memcpy(dst->memory, src->memory, copy_size);
	return ((ssize_t)dst->alloc_size);
}

static ssize_t vec_resize(t_vec *src, size_t target_size)
{
	t_vec	dst;
	ssize_t	ret;

	if (!src)
		return (-1);
	ret = vec_new(&dst, target_size / src->elem_size, src->elem_size);
	memcpy(dst.memory, src->memory, src->alloc_size);
	dst.len = src->len;
	if (ret < 0)
		return (-1);
	vec_copy(&dst, src);
	vec_free(src);
	*src = dst;
	return ((ssize_t)src->alloc_size);
}

ssize_t vec_push(t_vec *dst, void *src)
{
	uint8_t	*target;
	ssize_t	ret;

	if (!dst || !src)
		return (-1);
	if ((dst->elem_size * dst-> len) + 1 > dst-> alloc_size)
	{
		ret = vec_resize(dst, dst->alloc_size * 2);
		if (ret < 0)
			return (-1);
	}
	target = &dst->memory[dst->elem_size * dst->len];
	memcpy(target, src, dst->elem_size);
	dst->len++;
	return ((ssize_t)dst->alloc_size);
}

ssize_t vec_pop(void *dst, t_vec *src)
{
	uint8_t	*target;

	if (!dst || !src)
		return (-1);
	if (src == NULL || src->memory == NULL || src->len == 0)
		return (-1);
	target = &src->memory[src->elem_size * (src->len - 1)];
	memcpy(dst, target, src->elem_size);
	src->len--;
	return ((ssize_t)src->len);
}

void *vec_get(t_vec *src, size_t index)
{
	uint8_t	*ptr;

	if (index > src->len || !src)
		return (NULL);
	ptr = &src->memory[src->elem_size * index];
	return (ptr);
}

ssize_t vec_insert(t_vec *dst, void *src, size_t index)
{
	uint8_t	*pos;
	uint8_t	*mov_pos;
	ssize_t	ret;

	if (!dst || !src || index > dst->len)
		return (-1);
	if (index == dst->len)
		return (vec_push(dst, src));
	if ((dst->elem_size * dst-> len) + 1 > dst-> alloc_size)
	{
		ret = vec_resize(dst, dst->alloc_size * 2);
		if (ret < 0)
			return (-1);
	}
	pos = &dst->memory[dst->elem_size * index];
	mov_pos = &dst->memory[dst->elem_size * (index + 1)];
	memmove(mov_pos, pos, (dst->len - index) * dst->elem_size);
	memcpy(pos, src, dst->elem_size);
	dst->len++;
	return ((ssize_t)dst->alloc_size);
}

ssize_t vec_remove(t_vec *src, size_t index)
{
	uint8_t	*pos;
	uint8_t	*mov_pos;

	if (!src || index > src->len)
		return (-1);
	if (index == src->len)
	{
		src->len--;
		return ((ssize_t)src->alloc_size);
	}
	pos = &src->memory[src->elem_size * (index + 1)];
	mov_pos = &src->memory[src->elem_size * index];
	memmove(mov_pos, pos, (src->len - index) * src->elem_size);
	src->len--;
	return ((ssize_t)src->alloc_size);
}

void vec_iter(t_vec *src, void (*f) (void *))
{
	void	*ptr;
	size_t	i;

	if (!src)
		return ;
	i = 0;
	while (i < src->len)
	{
		ptr = vec_get(src, i);
		f(ptr);
		i++;
	}
}

void vec_map(t_vec *dst, t_vec *src, void (*f) (void *))
{
	void	*ptr;
	void	*res;
	size_t	i;

	if (!dst || !src)
		return ;
	res = malloc(dst->elem_size);
	i = 0;
	while (i < src->len)
	{
		ptr = vec_get(src, i);
		memcpy(res, ptr, dst->elem_size);
		f(res);
		vec_push(dst, res);
		i++;
	}
	free(res);
}

void vec_filter(t_vec *dst, t_vec *src, bool (*f) (void *))
{
	void	*ptr;
	void	*res;
	size_t	i;

	if (!dst || !src)
		return ;
	res = malloc(dst->elem_size);
	i = 0;
	while (i < src->len)
	{
		ptr = vec_get(src, i);
		memcpy(res, ptr, dst->elem_size);
		if (f(res) == true)
			vec_push(dst, res);
		i++;
	}
	free(res);
}

void vec_reduce(void *dst, t_vec *src, void (*f) (void *, void *))
{
	void	*ptr;
	size_t	i;

	if (!dst || !src)
		return ;
	i = 0;
	while (i < src->len)
	{
		ptr = vec_get(src, i);
		f(dst, ptr);
		i++;
	}
}