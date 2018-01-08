#ifndef NGDS_ARRAY_SPLAY_TREE_H
#define NGDS_ARRAY_SPLAY_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/* ========================================================================= */
/* -- OPAQUE TYPES --------------------------------------------------------- */
/* ========================================================================= */

typedef struct ngds_array_splay_tree_s ngds_array_splay_tree_t;

/* ========================================================================= */
/* -- TYPES ---------------------------------------------------------------- */
/* ========================================================================= */

/**
 * Function used to compare two elements, returns positive in
 * case the first element is greater than the second, negative
 * in case the opposite happens and zero in case they are equal.
 */
typedef int (*ngds_comparator_fptr) (const void *, const void *);
typedef void *(*ngds_malloc_fptr) (size_t);
typedef void (*ngds_free_fptr) (size_t);

/* ========================================================================= */
/* -- FUNCTION PROTOTYPES -------------------------------------------------- */
/* ========================================================================= */

ngds_array_splay_tree_t *
ngds_array_splay_tree_new (
  int                   initial_element_count,
  ngds_comparator_fptr  comparefp,
  ngds_malloc_fptr      mallocfp,
  ngds_free_fptr        freefp
);

void ngds_array_splay_tree_clear (ngds_array_splay_tree_t *me);
void ngds_array_splay_tree_insert (ngds_array_splay_tree_t *me,
  void *key, void *value, bool should_perform_splay);
void *ngds_array_splay_tree_get (ngds_array_splay_tree_t *me, const void *key,
  bool should_perform_splay);
void *ngds_array_splay_tree_remove(ngds_array_splay_tree_t *, void *);
int ngds_array_splay_tree_cardinality (ngds_array_splay_tree_t *me);
void ngds_array_splay_tree_destroy (ngds_array_splay_tree_t *);
void ngds_array_splay_tree_empty (ngds_array_splay_tree_t *);

#if 0

int ngds_array_splay_tree_cardinality(ngds_array_splay_tree_t* me);

int ngds_array_splay_tree_allocated_element_count(ngds_array_splay_tree_t* me);

int ngds_array_splay_tree_height(ngds_array_splay_tree_t* me);



void* ngds_array_splay_tree_get_from_idx(ngds_array_splay_tree_t* me, int idx);

/**
 * Rotate on X:
 * Y = X's parent
 * Step A: Y becomes left child of X
 * Step B: X's left child's becomes Y's right child */
void ngds_array_splay_tree_rotate_left(ngds_array_splay_tree_t* me, int idx);

/**
 * Rotate on X:
 * Y = X's left child
 * Step A: X becomes right child of X's left child
 * Step B: X's left child's right child becomes X's left child */
void ngds_array_splay_tree_rotate_right(ngds_array_splay_tree_t* me, int idx);
#endif

# ifdef __cplusplus
}
# endif

#endif /* NGDS_ARRAY_SPLAY_TREE_H */
/* vi: set et sw=2 ts=2: */

