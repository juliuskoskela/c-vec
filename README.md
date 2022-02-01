# Vec: A Dynamic Vector in C

The `subject.md` file contains exercises for this implementation if you want to
do it yourself. In that case, don't peek at the solutions in `vec.c` beforehand!

The C-standard library doesn't offer a good dynamic data structure often found
in other languages such `vector` in C++ or `Vec` in Rust. This is an
overview of the idea, design and implementation of such data structure in C.

## Features of a Dynamic Data Structure

When we talk about a dynamic data structures, we usually mean a container with a
growing memory buffer. Such a buffer has a certain strategy to allocate more
memory when the existing memory runs out. A typical strategy is to double the
size of the buffer each time a limit is reached. This way we minimize the number
of reallocations in the buffer which is our most expensive operation. While we
still have space left in the buffer we can append to the buffer with negligible cost.
When we reach the limit and reallocate, we incur the cost of the allocating the space for a new buffer and copying over the data from the old buffer.

Such data structures can be typed, but in this tutorial we will make our best to
mimic the usefulness of vectors in other languages by providing a
generic interface which will accept any type of element.

## Dynamic Vector

Now let's implement a dynamic vector data structure. Let's consider the
equivalent Rust called `Vec`. `Vec` implements tons of functions, but in essence
it's a growable and shrinkable buffer of elements of type `<T>`, and the main operations
are `push` and `pop`. Pushing a new element to the end of the buffer and
"popping" a.k.a. removing the element from the end and returning you a pointer to the element (this is an important detail). In addition, we implement the `insert` and `remove` operations that allow you to specify an index to which the element is added or which is removed. Furthermore, we usually have a `get` method, or we can access the elements by their index using the `[]` syntax.

It's important to note that there is a difference in performance between the
operations. Adding to the end of the array and deleting from the end will always
be cheapest. Everything else, such as inserting an element at the beginning or in the middle of the array, will incur the penalty of copying data around.

However, in various tests I've made, those penalties have been quite small
compared to the penalties that incur with different data structures claiming to
solve this problem. Sure you can prepend to a linked list without removing or
shuffling around elements, but in a dynamic vector you are dealing
with data that is all laid out sequantially in memory. This is important because modern processors are very fast when the required data can be found in the cache. In a linked list or other pointer-type list, the cache misses
from pointer indirection far outweigh any other gains in my experience.

### Design

First, let's talk a little about design patterns. We should adopt conventions
that make our API easy to reason with and practices that make the code safe and
easy to debug.

1. Allocation and deallocation happen at the same level as the variable
   declaration.

This first pattern helps to avoid unnecessary allocations leading to increased
performance as well as easier time with leaks. Let's see what it means.

```c

// This "constructor" returns an allocated pointer to a `t_foo`. But what if
// we didn't need a pointer, because we use t_foo in the caller's scope only?
t_foo *create_a_foo(int foosize);

// We could just return a `t_foo`, but now the problem is the error condition.
// You can't return a NULL if something goes wrong.
t_foo create_a_foo(int foosize);

// This is how we create a `t_foo` without allocating a pointer (if it wasn't
// already allocated) and we can return an integer representing some error condition.
int create_a_foo(t_foo *foo, int foosize);

// ...or if we want to be passing foo around directly as a parameter to
// another function, we can return a pointer to `t_foo`.
t_foo *create_a_foo(t_foo *foo, int foosize);

```

2. Parameter order is (dst, src, params, fpointers).

We always have the destination before the source if a destination pointer or variable
is applicable.

```c

int foo(char *dst, char *src, int len, (*bar)(char *));

```

### Struct

We create a struct called `s_vec` and typedef it to `t_vec`.

```c

typedef struct s_vec
{
    unsigned char *memory;    // Pointer to the first byte of allocated memory.
    size_t  elem_size;  // Size of a vector element in bytes.
    size_t  alloc_size; // Total size of allocated bytes.
    size_t  len;        // Length of the used-up part of the vector in
                        // `elem_size` chunks.
}   t_vec;

```

When we access elements in the vector our bounds are 0 -> len - 1. We might have
allocated more memory in total, but we will only access memory in the byte-range
0 -> len * elem_size.

### Implementation

Here is our `vec.h` header file with implementation prototypes;

```c

#ifndef VEC_H
# define VEC_H

#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "stdbool.h"

typedef struct s_vec
{
    unsigned char   *memory;
    size_t          elem_size;
    size_t          alloc_size;
    size_t          len;
}   t_vec;

int     vec_new(t_vec *src, size_t init_len, size_t elem_size);
void    vec_free(t_vec *src);
int     vec_new_from(t_vec *dst, void *src, size_t len, size_t elem_size);
int     vec_resize(t_vec *src, size_t target_size);
int     vec_clear(t_vec *src);
int     vec_push(t_vec *src, void *elem);
int     vec_pop(void *dst, t_vec *src);
int     vec_copy(t_vec *dst, t_vec *src);
void    *vec_get(t_vec *src, size_t index);
int     vec_insert(t_vec *dst, void *elem, size_t index);
int     vec_remove(t_vec *src, size_t index);
int     vec_append(t_vec *dst, t_vec *src);
int     vec_prepend(t_vec *dst, t_vec *src);
void    vec_iter(t_vec *src, void (*f) (void *));
void    *vec_find(t_vec *src, bool (*f) (void *));
int     vec_map(t_vec *dst, t_vec *src, void (*f) (void *));
int     vec_filter(t_vec *dst, t_vec *src, bool (*f) (void *));
int     vec_reduce(void *dst, t_vec *src, void (*f) (void *, void *));
void    vec_sort(t_vec *src, int (*f)(void *, void *));

#endif

```

#### Construction and Destruction

Function `vec_new` will take an allocated pointer `dst` and
allocate len * elem_size amount of bytes in the buffer.

```c

int main(void)
{
    t_vec   v;
    int ret;

    ret = vec_new(&v, 10, sizeof(int));
    if (ret < 0)
        printf("Error!");
}

```

Deallocation:

```c

int main(void)
{
    t_vec   v;
    int ret;

    ret = vec_new(&v, 10, sizeof(int));
    if (ret < 0)
        printf("Error!");
    vec_free(&v);
}

```

A handy `from` method that creates a vec out data passed in as a pointer.

```c

int main(void)
{
    int     vals[] = {1, 2, 3};
    t_vec   v;
    int ret;

    ret = vec_new_from(&v, vals, 3, sizeof(int));
    if (ret < 0)
        printf("Error!");
    vec_free(&v);
}

```

#### Copying and Resizing

Resizing is used by many functions that we will implement later, so better do it
now. In the resizing function it's handy to call the `vec_copy` method so we'll
implement that as well.

The copy function is very simple and will only copy at most as many bytes as are
available in the `dst` vector.

```c

int main(void)
{
    int     vals[] = {1, 2, 3};
    t_vec   v;
    t_vec   d;

    vec_new_from(&v, vals, 3, sizeof(int));
    vec_new(&v, 3, sizeof(int));
    vec_copy(&d, &v);
    vec_free(&v);
    vec_free(&d);
}

```

The resizing function `vec_resize` will take in a `target_size` parameter and either shrink
(destructively) or grow the vector to the target size copying the old contents
over to the new alloaction.

```c

static int vec_resize(t_vec *src, size_t target_size);


```

#### Manipulating the Elements

We have several functions that we use to manipulate the vector. We can `push`
elements to the end of the array or `pop` them from the end or `get` a pointer
to an element at any index in the vector. These are the least expensive
operations to perform.

```c

int main(void)
{
    int     vals[] = {1, 2, 3};
    int     ret;
    int     *ptr;
    t_vec   v;

    vec_new_from(&v, vals, 3, sizeof(int));
    vec_push(&v, &vals[0]);
    vec_push(&v, &vals[1]);
    vec_pop(&ret, &v);
    ptr = vec_get(&v, 0);
    vec_free(&v);
}

```

A little more involved operation is inserting or removing an element from any
index in the vector.

```c

int main(void)
{
    int     vals[] = {1, 2, 3};
    t_vec   v;

    vec_new_from(&v, vals, 3, sizeof(int));
    vec_insert(&v, &vals[0], 2);
    vec_remove(&v, 2);
    vec_free(&t1);
}

```

#### Iterator Methods

When we iterate our vector we usually use iterator functions. It's quite
surprising how many use cases these few simple functions cover, reducing errors
and boilerplate code.

A simple iterator takes in a function pointer that is called for each element of the vector, with a pointer to the element passed as an argument. Any modifications to the element will modify the original values.

```c

void print_int(void *src)
{
    printf("%d\n", *(int *)src);
}

int main(void)
{
    t_vec   t1;
    int     base[] = {1, 2, 3, 4, 5};

    vec_new_from(&t1, base, 5, sizeof(int);
    vec_iter(&t1, print_int);
    vec_free(&t1);
}

```

A `map` function which copies over the current elements (this operation is
non-destructive), sends each element to `f` and then appends the element to a new vector `dst`.

```c

void *add_one(void *ptr)
{
    *(int *)src += 1;
}

void main()
{
    t_vec values;

    vec_new(&values, 10, sizeof(int));
    for (int i = 0; i < 10; i++)
        vec_push(&values, &i);
    vec_map(&even, &values, add_one);
}

```

Next a filtering function. Now the function pointer will return `true` or `false`
indicating if the element should be added to the resulting vector.

```c

bool filter_even(void *src)
{
    if (*(int *)src % 2 == 0)
        return (true);
    return (false);
}

void main()
{
    t_vec values;

    vec_new(&values, 10, sizeof(int));
    for (int i = 0; i < 10; i++)
        vec_push(&values, &i);
    vec_filter(&even, &values, filter_even);
}

```

Finally, a function that iterates the elements of the vector, and at each iteration,
applies the function `f` that gets the `acc` element passed to `vec_reduce` as the first
parameter (the "accumulator"), and the current element as the second parameter. With the accumulator, the results of iterating over the elements are reduced to one element such as in summing a list of integers.

```c

void sum(void *acc, void *elem)
{
    *(int *)acc += *(int *)elem;
}

void main()
{
    t_vec   t1;
    int     base[] = {1, 2, 3, 4, 5};
    int     result = 0;

    vec_new_from(&t1, base, 5, sizeof(int));
    vec_reduce(&result, &t1, reduce_tester);
    assert(result == 15);
    vec_free(&t1);
}

```

## Conclusions

That's a quick overview on how to implement a performant and easy-to-use
dynamic data structure in C. This structure can be used as a fundamental
building block in many programs. The implementation has been kept simple
because frankly, it doesn't have to be more complicated. New functions
could definitely be added to grow the functionality of `t_vec` even
further.
