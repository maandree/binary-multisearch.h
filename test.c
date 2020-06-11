#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "binary-multisearch.h"


static struct binary_multisearch_stack_entry stack[16];
static ssize_t result[1 << 16];
static short int sought[1 << 16];
static short int list[1 << 16];
static size_t sought_count;
static size_t list_count;


static int
cmp(const void *a, const void *b, void *data)
{
	const short int *x = a, *y = b;
	(void) data;
	return *x < *y ? -1 : *x > *y;
}

static short int
random16(short int mask)
{
	return (short int)((((rand() & 255) << 8) | (rand() & 255)) & mask);
}

static void
fill_random16(short int arr[], size_t n, short int mask)
{
	while (n--)
		arr[n] = random16(mask);
}

static size_t
uniq(short int arr[], size_t n)
{
	size_t r, w = (n > 0);
	for (r = 1; r < n; r++)
		if (arr[r] != arr[r - 1])
			arr[w++] = arr[r];
	return w;
}

static void
test(size_t sn, size_t ln, short int mask)
{
	size_t i, p;

	sought_count = sn;
	list_count = ln;

	fill_random16(sought, sought_count, mask);
	fill_random16(list, list_count, mask);
	qsort_r(sought, sought_count, sizeof(*sought), cmp, NULL);
	qsort_r(list, list_count, sizeof(*list), cmp, NULL);
	sought_count = uniq(sought, sought_count);

	for (i = 0; i < sought_count; i++)
		result[i] = SIZE_MAX / 2;

	binary_multisearch(sought, sought_count, sizeof(*sought), list, list_count,
	                   sizeof(*list), cmp, NULL, binary_search, stack, result);

	for (i = 0; i < sought_count; i++) {
		if (list_count == 0) {
			if (result[i] != -1)
				exit(1);
			continue;
		}
		if (result[i] >= (ssize_t)list_count) {
			exit(2);
		}
		if (result[i] >= 0) {
			if (list[result[i]] != sought[i])
				exit(3);
			continue;
		}
		p = (size_t)(-result[i] - 1);
		if (p > list_count) {
			exit(4);
		}
		if (p == list_count) {
			if (list[p - 1] >= sought[i])
				exit(5);
			continue;
		}
		if (list[p] <= sought[i])
			exit(6);
		if (p && list[p - 1] >= sought[i])
			exit(7);
	}
}

int
main(void)
{
	size_t i, j;
	short int mask;

	srand((unsigned)time(NULL));

	for (i = 1; i <= 16; i++) {
		mask = (short int)(((size_t)1 << i) - 1);
		test(0, 0, mask);
		test(0, 10, mask);
		test(10, 0, mask);
		for (j = 0; j < 100; j++) {
			test((size_t)(unsigned short int)random16(mask) + 1,
			     (size_t)(unsigned short int)random16(mask) + 1, mask);
		}
	}

	return 0;
}
