#include "vec.h"

int vec_new(t_vec *dst, size_t init_alloc, size_t elem_size)
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
    return (1);
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

int vec_from(t_vec *dst, void *src, size_t len, size_t elem_size)
{
    if (!src || !src || elem_size == 0)
        return (-1);
    dst->alloc_size = len * elem_size;
    dst->len = len;
    dst->elem_size = elem_size;
    dst->memory = malloc(dst->alloc_size);
    if (!dst->memory)
    {
        dst->alloc_size = 0;
        dst->len = 0;
        dst->elem_size = 0;
        return (-1);
    }
    dst->memory = memcpy(dst->memory, src, dst->alloc_size);
    return (1);
}

int vec_copy(t_vec *dst, t_vec *src)
{
    size_t  copy_size;

    if (!dst || !src)
        return (-1);
    if (src->len * src->elem_size < dst->alloc_size)
        copy_size = src->len * src->elem_size;
    else
        copy_size = src->alloc_size;
    memcpy(dst->memory, src->memory, copy_size);
    return (1);
}

int vec_resize(t_vec *src, size_t target_size)
{
    t_vec   dst;
    int     ret;

    if (!src)
        return (-1);
    ret = vec_new(&dst, target_size / src->elem_size, src->elem_size);
    if (ret < 0)
        return (-1);
    memcpy(dst.memory, src->memory, src->alloc_size);
    dst.len = src->len;
    vec_copy(&dst, src);
    vec_free(src);
    *src = dst;
    return (1);
}

int vec_push(t_vec *dst, void *src)
{
    unsigned char   *target;
    int             ret;

    if (!dst || !src)
        return (-1);
    if (dst->elem_size * dst->len >= dst->alloc_size)
    {
        ret = vec_resize(dst, dst->alloc_size * 2);
        if (ret < 0)
            return (-1);
    }
    target = &dst->memory[dst->elem_size * dst->len];
    memcpy(target, src, dst->elem_size);
    dst->len++;
    return (1);
}

int vec_pop(void *dst, t_vec *src)
{
    unsigned char   *target;

    if (!dst || !src)
        return (-1);
    if (src->memory == NULL || src->len == 0)
        return (-1);
    target = &src->memory[src->elem_size * (src->len - 1)];
    memcpy(dst, target, src->elem_size);
    src->len--;
    return (1);
}

void *vec_get(t_vec *src, size_t index)
{
    unsigned char   *ptr;

    if (index > src->len || !src)
        return (NULL);
    ptr = &src->memory[src->elem_size * index];
    return (ptr);
}

int vec_insert(t_vec *dst, void *src, size_t index)
{
    unsigned char   *pos;
    unsigned char   *mov_pos;
    int             ret;

    if (!dst || !src || index > dst->len)
        return (-1);
    if (index == dst->len)
        return (vec_push(dst, src));
    if (dst->elem_size * dst->len >= dst->alloc_size)
    {
        ret = vec_resize(dst, dst->alloc_size * 2);
        if (ret < 0)
            return (-1);
    }
    pos = vec_get(dst, index);
    mov_pos = vec_get(dst, index + 1);
    memmove(mov_pos, pos, (dst->len - index) * dst->elem_size);
    memcpy(pos, src, dst->elem_size);
    dst->len++;
    return (1);
}

int vec_remove(t_vec *src, size_t index)
{
    unsigned char   *pos;
    unsigned char   *mov_pos;

    if (!src || index > src->len)
        return (-1);
    if (index == src->len)
    {
        src->len--;
        return (1);
    }
    pos = vec_get(src, index + 1);
    mov_pos = vec_get(src, index);
    memmove(mov_pos, pos, (src->len - index) * src->elem_size);
    src->len--;
    return (1);
}

int vec_append(t_vec *dst, t_vec *src)
{
    unsigned char   *pos;
    int             ret;
    size_t          alloc_size;

    if (!dst || !src)
        return (-1);
    alloc_size = dst->len * dst->elem_size + src->len * src->elem_size;
    if (dst->alloc_size < alloc_size)
    {
        if (dst->alloc_size * 2 < dst->len * alloc_size)
            ret = vec_resize(dst, alloc_size);
        else
            ret = vec_resize(dst, dst->alloc_size * 2);
        if (ret < 0)
            return (-1);
    }
    pos = vec_get(dst, dst->len);
    memcpy(pos, src->memory, src->len * src->elem_size);
    dst->len += src->len;
    return (1);
}

int vec_prepend(t_vec *dst, t_vec *src)
{
    unsigned char   *pos;
    int             ret;
    t_vec           new;
    size_t          alloc_size;

    if (!dst || !src)
        return (-1);
    alloc_size = dst->len * dst->elem_size + src->len * src->elem_size;
    ret = vec_new(&new, alloc_size / dst->elem_size, dst->elem_size);
    if (ret < 0)
        return (-1);
    vec_copy(&new, src);
    new.len = src->len + dst->len;
    pos = vec_get(&new, src->len);
    memcpy(pos, dst->memory, dst->len * dst->elem_size);
    vec_free(dst);
    *dst = new;
    return (1);
}

void vec_iter(t_vec *src, void (*f) (void *))
{
    void    *ptr;
    size_t  i;

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

int vec_map(t_vec *dst, t_vec *src, void (*f) (void *))
{
    void    *ptr;
    void    *res;
    size_t  i;

    if (!dst || !src)
        return (-1);
    res = malloc(dst->elem_size);
    if (!res)
        return (-1);
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
    return (1);
}

int vec_filter(t_vec *dst, t_vec *src, bool (*f) (void *))
{
    void    *ptr;
    void    *res;
    size_t  i;

    if (!dst || !src)
        return (-1);
    res = malloc(dst->elem_size);
    if (!res)
        return (-1);
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
    return (1);
}

int vec_reduce(void *dst, t_vec *src, void (*f) (void *, void *))
{
    void    *ptr;
    size_t  i;

    if (!dst || !src)
        return (-1);
    i = 0;
    while (i < src->len)
    {
        ptr = vec_get(src, i);
        f(dst, ptr);
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
    size_t  i;

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
            memswap(vec_get(src, a), vec_get(src, b), src->elem_size);
    }
    memswap(vec_get(src, pivot), vec_get(src, b), src->elem_size);
    vec_sort_recurse(src, low, b - 1, f);
    vec_sort_recurse(src, b + 1, high, f);
}

void    vec_sort(t_vec *src, int (*f)(void *, void *))
{
    if (!src)
        return ;
    vec_sort_recurse(src, 0, src->len - 1, f);
}
