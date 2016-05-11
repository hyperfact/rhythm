/*
 * Copyright (c) 2016 Stephen Huai <hyperfact@gmail.com>
 * generic version based on Niels Provos and Nick Mathewson's
 *
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
 *
 * Copyright (c) 2006 Maxim Yegorushkin <maxim.yegorushkin@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <stdlib.h>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
#ifndef nullptr
#define nullptr 0
#endif // nullptr

typedef int min_heap_bool_t;
typedef int min_heap_error_t;
typedef int min_heap_index_t;
typedef unsigned min_heap_size_t;

const min_heap_index_t MIN_HEAP_INVALID_INDEX = -1;

typedef struct min_heap_node {
	min_heap_index_t	index;
} min_heap_node_t;

typedef struct min_heap {
	min_heap_node_t **p;
	min_heap_size_t n, a;

	min_heap_bool_t (*comp)(min_heap_node_t *, min_heap_node_t *);

	void *(*alloc)(size_t);
	void (*dealloc)(void *);
} min_heap_t;

inline void min_heap_elem_init(min_heap_node_t* e);
inline min_heap_bool_t min_heap_elem_is_top(const min_heap_node_t *e);

inline void min_heap_ctor(min_heap_t* s);
inline void min_heap_dtor(min_heap_t* s);
inline min_heap_bool_t min_heap_empty(min_heap_t* s);
inline min_heap_size_t min_heap_size(min_heap_t* s);
inline min_heap_node_t *min_heap_top(min_heap_t* s);
inline min_heap_error_t min_heap_reserve(min_heap_t* s, min_heap_size_t n);
inline min_heap_error_t min_heap_push(min_heap_t* s, min_heap_node_t* e);
inline min_heap_node_t *min_heap_pop(min_heap_t* s);
inline min_heap_error_t min_heap_adjust(min_heap_t *s, min_heap_node_t* e);
inline min_heap_error_t min_heap_erase(min_heap_t* s, min_heap_node_t* e);
inline void min_heap_shift_up(min_heap_t* s, min_heap_index_t hole_index, min_heap_node_t* e);
inline void min_heap_shift_up_unconditional(min_heap_t* s, min_heap_index_t hole_index, min_heap_node_t* e);
inline void min_heap_shift_down(min_heap_t* s, min_heap_index_t hole_index, min_heap_node_t* e);

////////////////////////////////////////////////////////////////////////////////
static min_heap_bool_t _min_heap_default_comp(min_heap_node_t *e1, min_heap_node_t *e2) { return e1 < e2; }
static void *_min_heap_default_alloc(size_t size) { return ::malloc(size); }
static void _min_heap_default_dealloc(void *p) { ::free(p); }

inline min_heap_index_t _min_heap_parent_index(min_heap_index_t index) {
	assert(index >= 0);
	return ((index - 1) / 2);
}
inline min_heap_index_t _min_heap_min_child_index(min_heap_index_t index) {
	assert(index >= 0);
	return (2 * (index + 1));
}

void min_heap_elem_init(min_heap_node_t* e) {
	e->index = MIN_HEAP_INVALID_INDEX;
}

min_heap_bool_t min_heap_elem_is_top(const min_heap_node_t *e) {
	return (e->index == 0);
}

void min_heap_ctor(min_heap_t* s) {
	s->p = nullptr;
	s->n = 0;
	s->a = 0;
	
	s->alloc = _min_heap_default_alloc;
	s->dealloc = _min_heap_default_dealloc;
	s->comp = _min_heap_default_comp;
}

void min_heap_dtor(min_heap_t* s) {
	if (s->p)
		s->dealloc(s->p);
}

min_heap_bool_t min_heap_empty(min_heap_t* s) {
	return (0 == s->n);
}

min_heap_size_t min_heap_size(min_heap_t* s) {
	return s->n;
}

min_heap_node_t *min_heap_top(min_heap_t *s) {
	return ((s->n && s->p) ? (*s->p) : (nullptr));
}

min_heap_error_t min_heap_reserve(min_heap_t *s, min_heap_size_t n) {
	if (s->a < n) {
		min_heap_node_t **p;
		min_heap_size_t a = (s->a) ? (s->a * 2) : 8;
		if (a < n)
			a = n;
		size_t size = a * sizeof(*p);
		if (nullptr == (p = (min_heap_node_t **)s->alloc(size)))
			return -1;
		if (s->p) {
			::memcpy_s(p, size, s->p, s->n*sizeof(*p));
			s->dealloc(s->p);
		}

		s->p = p;
		s->a = a;
	}
	return 0;
}

min_heap_error_t min_heap_push(min_heap_t *s, min_heap_node_t *e) {
	if (min_heap_reserve(s, s->n + 1))
		return -1;
	min_heap_shift_up(s, s->n++, e);
	return 0;
}

min_heap_node_t *min_heap_pop(min_heap_t* s) {
	if (s->n) {
		min_heap_node_t *e = *s->p;
		min_heap_shift_down(s, 0u, s->p[--s->n]);
		e->index = MIN_HEAP_INVALID_INDEX;
		return e;
	}
	return nullptr;
}

min_heap_error_t min_heap_adjust(min_heap_t *s, min_heap_node_t* e) {
	if (MIN_HEAP_INVALID_INDEX == e->index) {
		return min_heap_push(s, e);
	} else {
		min_heap_index_t parent = _min_heap_parent_index(e->index);
		/* The position of e has changed; we shift it up or down
		* as needed.  We can't need to do both. */
		if (e->index > 0 && s->comp(s->p[parent], e))
			min_heap_shift_up_unconditional(s, e->index, e);
		else
			min_heap_shift_down(s, e->index, e);
		return 0;
	}
}

min_heap_error_t min_heap_erase(min_heap_t* s, min_heap_node_t* e) {
	if (MIN_HEAP_INVALID_INDEX != e->index) {
		min_heap_node_t *last = s->p[--s->n];
		min_heap_index_t parent = _min_heap_parent_index(e->index);
		/* we replace e with the last element in the heap.  We might need to
		shift it upward if it is less than its parent, or downward if it is
		greater than one or both its children. Since the children are known
		to be less than the parent, it can't need to shift both up and
		down. */
		if (e->index > 0 && s->comp(s->p[parent], last))
			min_heap_shift_up_unconditional(s, e->index, last);
		else
			min_heap_shift_down(s, e->index, last);
		e->index = MIN_HEAP_INVALID_INDEX;
		return 0;
	}
	return -1;
}

void min_heap_shift_up(min_heap_t* s, min_heap_index_t hole_index, min_heap_node_t* e) {
	min_heap_index_t parent = _min_heap_parent_index(hole_index);
	while (hole_index && s->comp(s->p[parent], e)) {
		(s->p[hole_index] = s->p[parent])->index = hole_index;
		hole_index = parent;
		parent = _min_heap_parent_index(hole_index);
	}
	(s->p[hole_index] = e)->index = hole_index;
}

void min_heap_shift_up_unconditional(min_heap_t* s, min_heap_index_t hole_index, min_heap_node_t* e) {
	min_heap_index_t parent = _min_heap_parent_index(hole_index);
	do {
		(s->p[hole_index] = s->p[parent])->index = hole_index;
		hole_index = parent;
		parent = _min_heap_parent_index(hole_index);
	} while (hole_index && s->comp(s->p[parent], e));
	(s->p[hole_index] = e)->index = hole_index;
}

void min_heap_shift_down(min_heap_t* s, min_heap_index_t hole_index, min_heap_node_t* e) {
	min_heap_index_t min_child = _min_heap_min_child_index(hole_index);
	while (min_child <= s->n) {
		min_child -= min_child == s->n || s->comp(s->p[min_child], s->p[min_child - 1]);
		if (!(s->comp(e, s->p[min_child])))
			break;
		(s->p[hole_index] = s->p[min_child])->index = hole_index;
		hole_index = min_child;
		min_child = _min_heap_min_child_index(hole_index);
	}
	(s->p[hole_index] = e)->index = hole_index;
}

#endif // _MIN_HEAP_H_
