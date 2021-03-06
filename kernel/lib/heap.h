#pragma once
#include "types.h"

struct heap_t;

heap_t *heap_create(void);
void heap_destroy(heap_t *heap);

__assume_aligned(16)
void *heap_calloc(heap_t *heap, size_t num, size_t size);

__assume_aligned(16)
void *heap_alloc(heap_t *heap, size_t size);
void heap_free(heap_t *heap, void *block);

__assume_aligned(16)
void *heap_realloc(heap_t *heap, void *block, size_t size);
