#include "vec.h"
#include "assert.h"
#include "stdio.h"

void print_int(void *ptr)
{
	printf("%d\n", *(int *)ptr);
}

void test_vec_new()
{
	t_vec t1;

	assert(vec_new(&t1, 0, 0) == -1);
	assert(vec_new(&t1, 0, 1) > 0);
	assert(t1.memory == NULL);
	assert(vec_new(&t1, 1, 0) == -1);
	assert(vec_new(&t1, 10, 1) > 0);
	assert(t1.memory != NULL);
	vec_free(&t1);
	printf("test_vec_new successful!\n");
}

void test_vec_free()
{
	t_vec t1;

	assert(vec_new(&t1, 10, 1) > 0);
	vec_free(&t1);
	assert(t1.len == 0);
	assert(t1.alloc_size == 0);
	assert(t1.elem_size == 0);
	assert(t1.memory == NULL);
	printf("test_vec_free successful!\n");
}

void test_vec_from()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	assert(memcmp(t1.memory, base, sizeof(base)) == 0);
	vec_free(&t1);
	printf("test_vec_from successful!\n");
}

void test_vec_copy()
{
	t_vec	t1;
	t_vec	t2;
	int		base[] = {1, 2, 3, 4, 5};

	assert(vec_from(&t1, base, sizeof(base) / sizeof(int), sizeof(int)) > 0);
	assert(vec_new(&t2, sizeof(base) / sizeof(int), sizeof(int)) > 0);
	assert(vec_copy(&t2, &t1) > 0);
	assert(memcmp(t2.memory, base, sizeof(base)) == 0);
	vec_free(&t1);
	vec_free(&t2);
	printf("test_vec_copy successful!\n");
}

void test_vec_resize()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	assert(vec_resize(&t1, 100) > 0);
	assert(memcmp(t1.memory, base, sizeof(base)) == 0);
	vec_free(&t1);
	printf("test_vec_resize successful!\n");
}

void test_vec_push()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		expect[] = {2, 4};

	assert(vec_new(&t1, 1, sizeof(int)) > 0);
	vec_push(&t1, &base[1]);
	vec_push(&t1, &base[3]);
	assert(memcmp(t1.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	printf("test_vec_push successful!\n");
}

void test_vec_pop()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		expect = 0;

	assert(vec_new(&t1, 1, sizeof(int)) > 0);
	vec_push(&t1, &base[1]);
	vec_push(&t1, &base[3]);
	vec_pop(&expect, &t1);
	assert(expect == 4);
	vec_pop(&expect, &t1);
	assert(expect == 2);
	assert(t1.len == 0);
	vec_free(&t1);
	printf("test_vec_pop successful!\n");
}

void test_vec_get()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		*expect;

	assert(vec_new(&t1, 1, sizeof(int)) > 0);
	vec_push(&t1, &base[1]);
	vec_push(&t1, &base[3]);
	expect = vec_get(&t1, 0);
	assert(*expect == 2);
	expect = vec_get(&t1, 1);
	assert(*expect == 4);
	assert(t1.len == 2);
	vec_free(&t1);
	printf("test_vec_get successful!\n");
}

void test_vec_insert()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		insert[] = {42, 666, 7};
	int		expect[] = {1, 42, 2, 3, 666, 4, 5, 7};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	vec_insert(&t1, &insert[0], 1);
	vec_insert(&t1, &insert[1], 4);
	vec_insert(&t1, &insert[2], 7);
	assert(memcmp(t1.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	printf("test_vec_insert successful!\n");
}

void test_vec_remove()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		insert[] = {42, 666, 7};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	vec_insert(&t1, &insert[0], 1);
	vec_insert(&t1, &insert[1], 4);
	vec_insert(&t1, &insert[2], 7);
	vec_remove(&t1, 1);
	vec_remove(&t1, 3);
	vec_remove(&t1, 5);
	assert(memcmp(t1.memory, base, sizeof(base)) == 0);
	vec_free(&t1);
	printf("test_vec_remove successful!\n");
}

void test_vec_append()
{
	t_vec	t1;
	t_vec	t2;
	int		base1[] = {1, 2, 3};
	int		base2[] = {4, 5, 6};
	int		expect[] = {1, 2, 3, 4, 5, 6};

	assert(vec_from(&t1, base1, 3, sizeof(int)) > 0);
	assert(vec_from(&t2, base2, 3, sizeof(int)) > 0);
	assert(vec_append(&t1, &t2) > 0);
	assert(memcmp(t1.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	vec_free(&t2);
	printf("test_vec_append successful!\n");
}

void test_vec_prepend()
{
	t_vec	t1;
	t_vec	t2;
	int		base1[] = {1, 2, 3};
	int		base2[] = {4, 5, 6};
	int		expect[] = {4, 5, 6, 1, 2, 3};

	assert(vec_from(&t1, base1, 3, sizeof(int)) > 0);
	assert(vec_from(&t2, base2, 3, sizeof(int)) > 0);
	assert(vec_prepend(&t1, &t2) > 0);
	assert(memcmp(t1.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	vec_free(&t2);
	printf("test_vec_prepend successful!\n");
}

void iter_tester(void *src)
{
	*(int *)src += 1;
}

void test_vec_iter()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		expect[] = {2, 3, 4, 5, 6};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	vec_iter(&t1, iter_tester);
	assert(memcmp(t1.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	printf("test_vec_iter successful!\n");
}

void map_tester(void *src)
{
	*(int *)src += 1;
}

void test_vec_map()
{
	t_vec	t1;
	t_vec	t2;
	int		base[] = {1, 2, 3, 4, 5};
	int		expect[] = {2, 3, 4, 5, 6};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	assert(vec_new(&t2, 5, sizeof(int)) > 0);
	vec_map(&t2, &t1, map_tester);
	assert(memcmp(t2.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	vec_free(&t2);
	printf("test_vec_map successful!\n");
}

bool filter_tester(void *src)
{
	if (*(int *)src % 2 == 0)
		return (true);
	return (false);
}

void test_vec_filter()
{
	t_vec	t1;
	t_vec	t2;
	int		base[] = {1, 2, 3, 4, 5};
	int		expect[] = {2, 4};

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	assert(vec_new(&t2, 5, sizeof(int)) > 0);
	vec_filter(&t2, &t1, filter_tester);
	assert(memcmp(t2.memory, expect, sizeof(expect)) == 0);
	vec_free(&t1);
	vec_free(&t2);
	printf("test_vec_filter successful!\n");
}

void reduce_tester(void *dst, void *src)
{
	*(int *)dst += *(int *)src;
}

void test_vec_reduce()
{
	t_vec	t1;
	int		base[] = {1, 2, 3, 4, 5};
	int		result = 0;

	assert(vec_from(&t1, base, 5, sizeof(int)) > 0);
	vec_reduce(&result, &t1, reduce_tester);
	assert(result == 15);
	vec_free(&t1);
	printf("test_vec_reduce successful!\n");
}

int compare(void *a, void *b)
{
	return (*(int *)a - *(int *)b);
}

void test_vec_sort()
{
	t_vec	t1;
	int		base[] = {3, 2, 2, 7, 4, 2, 45, 3, -8, -5};
	int		expect[] = {-8, -5, 2, 2, 2, 3, 3, 4, 7, 45};

	assert(vec_from(&t1, base, 10, sizeof(int)) > 0);
	vec_sort(&t1, compare);
	assert(memcmp(t1.memory, expect, sizeof(expect)) == 0);
	printf("test_vec_sort successful!\n");
	vec_free(&t1);
}

int main(void)
{
	test_vec_new();
	test_vec_free();
	test_vec_from();
	test_vec_copy();
	test_vec_resize();
	test_vec_push();
	test_vec_pop();
	test_vec_get();
	test_vec_insert();
	test_vec_remove();
	test_vec_append();
	test_vec_prepend();
	test_vec_iter();
	test_vec_map();
	test_vec_filter();
	test_vec_reduce();
	test_vec_sort();
}
