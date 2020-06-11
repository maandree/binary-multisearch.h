/*
 * ISC License
 * 
 * © 2020 Mattias Andrée <maandree@kth.se>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef BINARY_MUTLISEARCH_H
#define BINARY_MUTLISEARCH_H

#define __STDC_WANT_LIB_EXT1__
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#ifndef RSIZE_MAX
# define rsize_t size_t
#endif


struct binary_multisearch_stack_entry {
	rsize_t min;
	rsize_t max;
	ssize_t lmin;
	ssize_t lmax;
};


static ssize_t
binary_search(const void *sought, const void *list, size_t list_width, ssize_t min,
              ssize_t max, int (*cmp)(const void *sought, const void *list_elem, void *data), void *data)
{
	const char *clist = list;
	ssize_t mid = min;
	int r;

	while (min <= max) {
		mid = (ssize_t)((size_t)min + (size_t)max) / 2;
		r = cmp(sought, &clist[(size_t)mid * list_width], data);
		if (r < 0)
			max = mid - 1;
		else if (r > 0)
			min = mid += 1;
		else {
			return mid;
		}
	}

	return -mid - 1;
}


static void
binary_multisearch(const void *sought, rsize_t sought_count, size_t sought_width, const void *list, rsize_t list_count,
                   size_t list_width, int (*cmp)(const void *sought_elem, const void *list_elem, void *data), void *data,
                   ssize_t (*search_function)(const void *sought, const void *list, size_t list_width, ssize_t min,
                                              ssize_t max, int (*cmp)(const void *, const void *, void *), void *data),
                   struct binary_multisearch_stack_entry stack[restrict], ssize_t r[restrict sought_count])
{
	struct binary_multisearch_stack_entry e;
	size_t stack_i;
	ssize_t pos;
	const char *csought = sought;

	if (!sought_count)
		return;

	stack[0].min = 0;
	stack[0].max = sought_count - 1;
	stack[0].lmin = 0;
	stack[0].lmax = (ssize_t)list_count - 1;
	stack_i = 1;

	while (stack_i) {
		e = stack[--stack_i];
		while (e.max != e.min) {
			stack[stack_i].max = e.max;
			stack[stack_i].lmax = e.lmax;

			e.max = (e.min + e.max) / 2;
			pos = search_function(&csought[e.max * sought_width], list, list_width, e.lmin, e.lmax, cmp, data);
			r[e.max] = pos;

			stack[stack_i].min = e.max + 1;
			stack[stack_i].lmin = pos < 0 ? -pos - 1 : pos + 1;
			stack_i++;
		}
	}

	e.max = sought_count - 1;
	r[e.max] = search_function(&csought[e.max * sought_width], list, list_width, e.lmin, e.lmax, cmp, data);
}


static void
binary_multisearch_online(const void *sought, rsize_t sought_count, size_t sought_width, const void *list, rsize_t list_count,
                          size_t list_width, int (*cmp)(const void *sought_elem, const void *list_elem, void *data), void *data,
                          ssize_t (*search_function)(const void *sought, const void *list, size_t list_width, ssize_t min,
                                                     ssize_t max, int (*cmp)(const void *, const void *, void *), void *data),
                          struct binary_multisearch_stack_entry stack[restrict],
                          void (*result_function)(size_t index, ssize_t position, void *data))
{
	struct binary_multisearch_stack_entry e;
	size_t stack_i;
	ssize_t pos;
	const char *csought = sought;

	if (!sought_count)
		return;

	stack[0].min = 0;
	stack[0].max = sought_count - 1;
	stack[0].lmin = 0;
	stack[0].lmax = (ssize_t)list_count - 1;
	stack_i = 1;

	while (stack_i) {
		e = stack[--stack_i];
		while (e.max != e.min) {
			stack[stack_i].max = e.max;
			stack[stack_i].lmax = e.lmax;

			e.max = (e.min + e.max) / 2;
			pos = search_function(&csought[e.max * sought_width], list, list_width, e.lmin, e.lmax, cmp, data);
			result_function(e.max, pos, data);

			stack[stack_i].min = e.max + 1;
			stack[stack_i].lmin = pos < 0 ? -pos - 1 : pos + 1;
			stack_i++;
		}
	}

	e.max = sought_count - 1;
	pos = search_function(&csought[e.max * sought_width], list, list_width, e.lmin, e.lmax, cmp, data);
	result_function(e.max, pos, data);
}


#ifndef RSIZE_MAX
# undef rsize_t
#endif

#endif
