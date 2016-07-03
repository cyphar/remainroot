/* ohmic: a fairly reliable hashmap library
 * Copyright (c) 2013 Aleksa Sarai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <string.h>

#include "ohmic.h"

struct ohm_t *ohm_init(int size, int (*hash_func)(void *, size_t)) {
	if(size < 1)
		return NULL;

	/* set default hash function, if none given */
	if(!hash_func)
		hash_func = ohm_hash;

	/* allocate hashmap */
	struct ohm_t *new_ohm = malloc(sizeof(struct ohm_t));

	/* allocate and initialise all values */
	new_ohm->table = malloc(sizeof(struct ohm_node *) * size);
	new_ohm->size = size;
	new_ohm->count = 0;

	/* initialise all entries as empty */
	int i;
	for(i = 0; i < size; i++)
		new_ohm->table[i] = NULL;

	/* set hashing function */
	new_ohm->hash = hash_func;

	return new_ohm;
} /* ohm_init() */

void ohm_free(struct ohm_t *hashmap) {
	if(!hashmap)
		return;

	struct ohm_node *current_node, *parent_node;

	int i;
	for(i = 0; i < hashmap->size; i++) {
		/* set width-wise node */
		current_node = hashmap->table[i];
		while(current_node) {
			/* free all depth-wise nodes */
			free(current_node->key);
			free(current_node->value);

			/* update pointers to next node */
			parent_node = current_node;
			current_node = current_node->next;

			/* free old "current" node */
			free(parent_node);
		}
	}

	/* finally, free the hashmap itself */
	free(hashmap->table);
	free(hashmap);
} /* ohm_free() */

void *ohm_search(struct ohm_t *hashmap, void *key, size_t keylen) {
	if(!key || keylen < 1)
		return NULL;

	/* get hash index */
	int index = hashmap->hash(key, keylen) % hashmap->size;
	if(index < 0 || !hashmap->table[index])
		return NULL;

	/* initialise start point for key search */
	struct ohm_node *current_node = hashmap->table[index];

	/* check all depth-wise matches */
	while(current_node) {
		/* only check node if keylen is correct */
		if(current_node->keylen == keylen) {
			/* compare keys */
			if(!memcmp(current_node->key, key, keylen))
				return current_node->value;
		}
		/* get next node */
		current_node = current_node->next;
	}

	return NULL; /* nothing found */
} /* ohm_search() */

void *ohm_insert(struct ohm_t *hashmap, void *key, size_t keylen, void *value, size_t valuelen) {
	if(!key || keylen < 1 || !value || valuelen < 1)
		return NULL;

	/* get hash index */
	int index = hashmap->hash(key, keylen) % hashmap->size;
	if(index < 0)
		return NULL;

	/* initialise start point for key search */
	struct ohm_node *parent_node = NULL, *current_node;
	current_node = hashmap->table[index];

	/* try and replace any existing key */
	while(current_node) {
		if(current_node->keylen == keylen) {
			if(!memcmp(current_node->key, key, keylen)) {
				/* key found */
				if(current_node->valuelen != valuelen) {
					/* node value needs to change size */
					current_node->value = realloc(current_node->value, valuelen);
					current_node->valuelen = valuelen;

					if(!current_node->value)
						return NULL;
				}
				/* copy over the new value and update item count */
				memcpy(current_node->value, value, valuelen);
				hashmap->count++;

				return current_node->value;
			}
		}

		/* next node */
		parent_node = current_node;
		current_node = current_node->next;
	}

	/* need to make a new key */
	current_node = malloc(sizeof(struct ohm_node));
	current_node->next = NULL;

	/* allocate and set key information */
	current_node->key = malloc(keylen);
	current_node->keylen = keylen;
	memcpy(current_node->key, key, keylen);

	/* allocate and set value information */
	current_node->value = malloc(valuelen);
	current_node->valuelen = valuelen;
	memcpy(current_node->value, value, valuelen);

	/* link to parent nodes or own bucket */
	if(parent_node)
		parent_node->next = current_node;
	else
		hashmap->table[index] = current_node;

	hashmap->count++;
	return current_node->value;
} /* ohm_insert() */

int ohm_remove(struct ohm_t *hashmap, void *key, size_t keylen) {
	if(!key || keylen < 1)
		return 1;

	/* get hash index */
	int index = hashmap->hash(key, keylen) % hashmap->size;
	if(index < 0)
		return 1;

	/* initialise start point for key search */
	struct ohm_node *parent_node = NULL, *current_node;
	current_node = hashmap->table[index];

	/* try and delete any existing key */
	while(current_node) {
		if(current_node->keylen == keylen) {
			if(!memcmp(current_node->key, key, keylen)) {
				/* key found, free values */
				free(current_node->value);
				free(current_node->key);

				/* link up chains orphaned by deletion of current node */
				if(parent_node)
					parent_node->next = current_node->next;
				else
					hashmap->table[index] = current_node->next;

				/* free memory and update item count */
				free(current_node);
				hashmap->count--;

				/* item found and deleted, return success*/
				return 0;
			}
		}

		/* next node*/
		parent_node = current_node;
		current_node = current_node->next;
	}

	/* item not found, return error */
	return 1;
} /* ohm_remove() */

struct ohm_t *ohm_resize(struct ohm_t *old_hm, int size) {
	if(!old_hm || size < 1)
		return NULL;

	/* initialise the iterables */
	struct ohm_t *new_hm = ohm_init(size, old_hm->hash);
	struct ohm_iter i = ohm_iter_init(old_hm);

	/* reinsert all of the keys (thus rehashing them) */
	for(; i.key != NULL; ohm_iter_inc(&i))
		ohm_insert(new_hm, i.key, i.keylen, i.value, i.valuelen);

	/* free the old hashmap afterwards (safer, in case copying failed) */
	ohm_free(old_hm);
	return new_hm;
} /* ohm_resize() */

struct ohm_iter ohm_iter_init(struct ohm_t *hashmap) {
	struct ohm_iter ret;

	/* initialise at "zero" */
	ret.internal.hashmap = hashmap;
	ret.internal.node = NULL;
	ret.internal.index = -1;

	/* actually move iterator to first node of first bucket */
	ohm_iter_inc(&ret);
	return ret;
} /* ohm_iter_init() */

/* depth-first node incrementor */
void ohm_iter_inc(struct ohm_iter *i) {
	if(!i)
		return;

	/* get current node and index information */
	struct ohm_t *hashmap = i->internal.hashmap;
	int index = i->internal.index;

	/* try to hop to next node down */
	if(i->internal.node && i->internal.node->next) {
		/* get next node down */
		i->internal.node = i->internal.node->next;

		/* update internal key information */
		i->key = i->internal.node->key;
		i->keylen = i->internal.node->keylen;

		/* update internal value information */
		i->value = i->internal.node->value;
		i->valuelen = i->internal.node->valuelen;
		return;
	}
	/* no depth-wise nodes left */
	else index++;

	/* find next used bucket */
	while(index < hashmap->size && !hashmap->table[index])
		index++;

	if(index >= hashmap->size) {
		/* reached end of table */
		i->internal.node = NULL;
		i->internal.index = hashmap->size;

		/* set everything to NULL */
		i->key = NULL;
		i->keylen = 0;
		i->value = NULL;
		i->valuelen = 0;
		return;
	}

	/* update pointers to new index */
	i->internal.node = hashmap->table[index];
	i->internal.index = index;

	/* update internal key information */
	i->key = i->internal.node->key;
	i->keylen = i->internal.node->keylen;

	/* update internal value information */
	i->value = i->internal.node->value;
	i->valuelen = i->internal.node->valuelen;
} /* ohm_iter_inc() */

struct ohm_t *ohm_dup(struct ohm_t *old_hm) {
	if(!old_hm)
		return NULL;

	/* initialise the iterables */
	struct ohm_t *new_hm = ohm_init(old_hm->size, old_hm->hash);
	struct ohm_iter i = ohm_iter_init(old_hm);

	/* insert all of the keys into new hashmap */
	for(; i.key != NULL; ohm_iter_inc(&i))
		ohm_insert(new_hm, i.key, i.keylen, i.value, i.valuelen);

	/* return fully copied hashmap */
	return new_hm;
} /* ohm_dup() */

void ohm_merge(struct ohm_t *to_hm, struct ohm_t *from_hm) {
	if(!to_hm || !from_hm)
		return;

	/* initialise iterable */
	struct ohm_iter i = ohm_iter_init(from_hm);

	/* insert all keys to new hashmap */
	for(; i.key != NULL; ohm_iter_inc(&i))
		ohm_insert(to_hm, i.key, i.keylen, i.value, i.valuelen);
} /* ohm_merge() */

void ohm_cpy(struct ohm_t *to_hm, struct ohm_t *from_hm) {
	if(!to_hm || !from_hm)
		return;

	/* delete everythin in target hashmap */
	struct ohm_node *current_node, *parent_node;

	int i;
	for(i = 0; i < to_hm->size; i++) {
		/* set width-wise node */
		current_node = to_hm->table[i];
		while(current_node) {
			/* free all depth-wise nodes */
			free(current_node->key);
			free(current_node->value);

			/* update pointers to next node */
			parent_node = current_node;
			current_node = current_node->next;

			/* free old "current" node */
			free(parent_node);
		}
	}

	to_hm->count = 0;

	/* set all entries to NULL */
	for(i = 0; i < to_hm->size; i++)
		to_hm->table[i] = NULL;

	/* merge empty and template hashmaps */
	ohm_merge(to_hm, from_hm);
} /* ohm_cpy() */

/* the djb2 hashing algorithm by Dan Bernstein */
int ohm_hash(void *key, size_t keylen) {
	if(!key || keylen < 1)
		return -1;

	int hash = 5381;
	char c, *k = (char *) key;

	while((c = *k++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return abs(hash); /* a more poisson distribution method for max range probably exists, oh well. */
} /* ohm_hash() */
