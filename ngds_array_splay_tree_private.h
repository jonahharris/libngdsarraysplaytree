#ifndef NGDS_ARRAY_SPLAY_TREE_PRIVATE_H
#define NGDS_ARRAY_SPLAY_TREE_PRIVATE_H

#include <stdint.h>

/* ========================================================================= */
/* -- OPAQUE TYPES --------------------------------------------------------- */
/* ========================================================================= */

typedef struct ngds_array_splay_tree_node_s ngds_array_splay_tree_node_t;

/* ========================================================================= */
/* -- TYPES ---------------------------------------------------------------- */
/* ========================================================================= */

typedef void (*ngds_node_printer) (const ngds_array_splay_tree_node_t *);

struct ngds_array_splay_tree_node_s {
  void                 *key;
  void                 *value;
};

struct ngds_array_splay_tree_s {
  int                             allocated_element_count;
  int                             utilized_element_count;
  size_t                          shifted_element_bitmap_size_in_bytes;
  uint64_t                       *shifted_element_bitmap;
  ngds_array_splay_tree_node_t   *node_array;
  ngds_comparator_fptr            compare;
  ngds_malloc_fptr                malloc;
  ngds_free_fptr                  free;
};

/* ========================================================================= */
/* -- FUNCTION PROTOTYPES -------------------------------------------------- */
/* ========================================================================= */

void
ngds_array_splay_tree_print (
  ngds_array_splay_tree_t    *me,
  ngds_node_printer           print_cb);

ngds_array_splay_tree_node_t *ngds_array_splay_tree_get_node_at_idx (
  ngds_array_splay_tree_t* me, int idx);
int ngds_array_splay_tree_rotate_left(ngds_array_splay_tree_t* me, int idx);
int ngds_array_splay_tree_rotate_right(ngds_array_splay_tree_t* me, int idx);


#endif /* NGDS_ARRAY_SPLAY_TREE_PRIVATE_H */
/* vi: set et sw=2 ts=2: */

