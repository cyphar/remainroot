/* ohmic: a fairly reliable hashmap library
 * Copyright (c) 2013 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __OHMIC_H__
#define __OHMIC_H__

/* hashmap structures */
struct ohm_node {
	void *key;
	size_t keylen;

	void *value;
	size_t valuelen;

	struct ohm_node *next;
};

struct ohm_t {
	struct ohm_node **table;
	int count;
	int size;
	int (*hash)(void *, size_t);
};

struct ohm_iter {
	void *key;
	size_t keylen;

	void *value;
	size_t valuelen;

	struct ohm_iter_internal {
		struct ohm_t *hashmap;
		struct ohm_node *node;
		int index;
	} internal;
};

/* basic hashmap functionality */
struct ohm_t *ohm_init(int, int (*)(void *, size_t));
void ohm_free(struct ohm_t *);

void *ohm_search(struct ohm_t *, void *, size_t);

void *ohm_insert(struct ohm_t *, void *, size_t, void *, size_t);
int ohm_remove(struct ohm_t *, void *, size_t);

struct ohm_t *ohm_resize(struct ohm_t *, int);

/* functions to iterate (correctly) through the hashmap */
struct ohm_iter ohm_iter_init(struct ohm_t *);
void ohm_iter_inc(struct ohm_iter *);

/* functions to copy, duplicate and merge hashmaps */
struct ohm_t *ohm_dup(struct ohm_t *);
void ohm_cpy(struct ohm_t *, struct ohm_t *);

void ohm_merge(struct ohm_t *, struct ohm_t *);

/* default hashing function (modulo of djb2 hash -- not reccomended) */
int ohm_hash(void *, size_t);

#endif /* __OHMIC_H__ */
