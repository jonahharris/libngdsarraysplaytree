
/* ========================================================================= */
/* -- INCLUSIONS ----------------------------------------------------------- */
/* ========================================================================= */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Public */
#include "ngds_array_splay_tree.h"

/* Private */
#include "ngds_array_splay_tree_private.h"

/* ========================================================================= */
/* -- DEFINITIONS ---------------------------------------------------------- */
/* ========================================================================= */

#ifdef NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT
# define NG_SPLAY_ROOT_INDEX    0
#else /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
# define NG_SPLAY_ROOT_INDEX    1
#endif /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */

/* ========================================================================= */
/* -- MACROS --------------------------------------------------------------- */
/* ========================================================================= */

#define max(x,y) ((x) < (y) ? (y) : (x))
#define NODE_IS_EMPTY(me, index)    (NULL == (&me->node_array[index])->key)
#define NODE_IS_VALID(me, index)    (index < me->allocated_element_count)

/* Bitset */
#define BITMASK(b)          (1 << ((b) % sizeof(uint64_t)))
#define BITSLOT(b)          ((b) / sizeof(uint64_t))
#define BITSET(a, b)        ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b)      ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b)       ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb)       ((nb + sizeof(uint64_t) - 1) / sizeof(uint64_t))

/* ========================================================================= */
/* -- FORWARD DECLARATIONS ------------------------------------------------- */
/* ========================================================================= */

static inline int left_child_of (const int);
static inline int right_child_of (const int);
static inline int parent_of (const int);
static void perform_tree_print (ngds_array_splay_tree_t *,
  ngds_node_printer, int, int);

/* ========================================================================= */
/* -- STATIC FUNCTIONS ----------------------------------------------------- */
/* ========================================================================= */

static inline int
left_child_of (
  const int             idx
) {
#ifdef NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT
  return ((idx * 2) + 1);
#else /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
  return (idx * 2);
#endif /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
} /* left_child_of() */

/* ------------------------------------------------------------------------- */

static inline int
right_child_of (
  const int             idx
) {
#ifdef NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT
  return ((idx * 2) + 2);
#else /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
  return ((idx * 2) + 1);
#endif /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
} /* right_child_of() */

/* ------------------------------------------------------------------------- */

static inline int
parent_of (
  const int             idx
) {

  /* Root of the tree has no parent */
  assert(NG_SPLAY_ROOT_INDEX != idx);

#ifdef NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT
  return ((idx - 1) / 2);
#else /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
  assert(1 != idx);
  return (idx / 2);
#endif /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
} /* parent_of() */

/* ------------------------------------------------------------------------- */

static void
perform_tree_print (
  ngds_array_splay_tree_t    *me,
  ngds_node_printer           print_cb,
  int                         idx,
  int                         d
) {
  int ii;

  for (ii = 0; ii < d; ++ii) {
    printf("  ");
  }

  printf("[%d] %c ", idx,
#ifdef NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT
    (1 == (idx % 2))
#else /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
    (0 == (idx % 2))
#endif /* NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT */
    ? 'l' : 'r');

  if (me->allocated_element_count <= idx
      || NULL == me->node_array[idx].key
  ) {
    printf("\n");
    return;
  }

  if (NULL != print_cb) {
    print_cb(&me->node_array[idx]);
  } else {
    printf("%p", me->node_array[idx].key);
  }
  printf("\n");

  perform_tree_print(me, print_cb, left_child_of(idx), (d + 1));
  perform_tree_print(me, print_cb, right_child_of(idx), (d + 1));

} /* perform_tree_print() */

/* ------------------------------------------------------------------------- */

#if 0
static void
__enlarge (
  ngds_array_splay_tree_t *me
) {
  int ii, end;
  ngds_array_splay_tree_node_t *array_n;

printf("enlarging\n");
  /* double capacity */
  me->allocated_element_count *= 2;
  array_n = calloc(me->allocated_element_count, sizeof(ngds_array_splay_tree_node_t));

  /* copy old data across to new array */
/* FIX FOR START */
  for (ii = NG_SPLAY_ROOT_INDEX, end = ngds_array_splay_tree_cardinality(me)
      ; ii < end
      ; ii++
  ) {
    if (me->node_array[ii].key) {
      memcpy(&array_n[ii], &me->node_array[ii], sizeof(ngds_array_splay_tree_node_t));
    } else {
      array_n[ii].key = NULL;
    }
  }

  /* swap arrays */
  free(me->node_array);
  me->node_array = array_n;
}
#endif

/* ------------------------------------------------------------------------- */

static void
perform_upward_shift (
  ngds_array_splay_tree_t    *me,
  int                         src_idx,
  int                         dst_idx
) {
  if (false == NODE_IS_VALID(me, src_idx)) {
    printf("%s/%d: From node index (%d) is invalid\n",
      __PRETTY_FUNCTION__, __LINE__, src_idx);
    return;
  }
  if (false == NODE_IS_VALID(me, dst_idx)) {
    printf("%s/%d: To node index (%d) is invalid\n",
      __PRETTY_FUNCTION__, __LINE__, dst_idx);
    return;
  }

  if (true == NODE_IS_EMPTY(me, src_idx)) {
    return;
  }

  memcpy(&me->node_array[dst_idx], &me->node_array[src_idx],
    sizeof(ngds_array_splay_tree_node_t));
  memset(&me->node_array[src_idx], 0, sizeof(ngds_array_splay_tree_node_t));

  perform_upward_shift(me, left_child_of(src_idx), left_child_of(dst_idx));
  perform_upward_shift(me, right_child_of(src_idx), right_child_of(dst_idx));

} /* perform_upward_shift() */

/* ------------------------------------------------------------------------- */

static int
perform_downward_shift (
  ngds_array_splay_tree_t    *me,
  int                         src_idx,
  int                         dst_idx,
  int                         depth
) {
  if (0 == depth) {
    memset(me->shifted_element_bitmap, 0,
      me->shifted_element_bitmap_size_in_bytes);
  }

  if (false == NODE_IS_VALID(me, src_idx)) {
    printf("%s/%d: From node index (%d) is invalid\n",
      __PRETTY_FUNCTION__, __LINE__, src_idx);
    return -1;
  }
  if (false == NODE_IS_VALID(me, dst_idx)) {
    printf("%s/%d: To node index (%d) is invalid\n",
      __PRETTY_FUNCTION__, __LINE__, dst_idx);
    return -1;
  }

  if (true == NODE_IS_EMPTY(me, src_idx)) {
    return src_idx;
  }

  if (BITTEST(me->shifted_element_bitmap, src_idx)) {
    return src_idx;
  }

  perform_downward_shift(me, left_child_of(src_idx),
    left_child_of(dst_idx), (depth + 1));
  perform_downward_shift(me, right_child_of(src_idx),
    right_child_of(dst_idx), (depth + 1));

  if (!BITTEST(me->shifted_element_bitmap, src_idx)) {
    memcpy(&me->node_array[dst_idx], &me->node_array[src_idx],
      sizeof(ngds_array_splay_tree_node_t));
    memset(&me->node_array[src_idx], 0, sizeof(ngds_array_splay_tree_node_t));
    BITSET(me->shifted_element_bitmap, dst_idx);
  }

} /* perform_downward_shift() */

/* ------------------------------------------------------------------------- */

static int
find_predecessor (
  ngds_array_splay_tree_t    *me,
  int                         idx
) {
  int prev,i;

  for (prev = -1, i = left_child_of(idx)
        ; i < me->allocated_element_count && me->node_array[i].key
        ; prev = i, i = right_child_of(i));

  return prev;
} /* find_predecessor() */

/* ------------------------------------------------------------------------- */

static void
perform_splay_operation (
  ngds_array_splay_tree_t            *me,
  int                   idx
) {
  while (NG_SPLAY_ROOT_INDEX != idx) {
    int p = parent_of(idx);
    int gp = 0;

    if (NG_SPLAY_ROOT_INDEX == p) {
      if (left_child_of(p) == idx) {
        ngds_array_splay_tree_rotate_right(me, p);
      } else {
        ngds_array_splay_tree_rotate_left(me, p);
      }
      break;
    } else {
      gp = parent_of(p);
    }

    if (left_child_of(p) == idx && left_child_of(gp) == p) {
      ngds_array_splay_tree_rotate_right(me, p);
      ngds_array_splay_tree_rotate_right(me, gp);
    } else if (right_child_of(p) == idx && right_child_of(gp) == p) {
      ngds_array_splay_tree_rotate_left(me, p);
      ngds_array_splay_tree_rotate_left(me, gp);
    } else if (right_child_of(p) == idx && left_child_of(gp) == p) {
      ngds_array_splay_tree_rotate_left(me, p);
      ngds_array_splay_tree_rotate_right(me, gp);
    } else if (left_child_of(p) == idx && right_child_of(gp) == p) {
      ngds_array_splay_tree_rotate_right(me, p);
      ngds_array_splay_tree_rotate_left(me, gp);
    } else {
      assert(0);
    }
    idx = gp;
  }
} /* perform_splay_operation() */

/* ========================================================================= */
/* -- PRIVATE FUNCTIONS ---------------------------------------------------- */
/* ========================================================================= */

int
ngds_array_splay_tree_rotate_left (
  ngds_array_splay_tree_t* me,
  int idx
) {
  perform_downward_shift(me, left_child_of(idx),
    left_child_of(left_child_of(idx)), 0);
  memcpy(&me->node_array[left_child_of(idx)], &me->node_array[idx],
    sizeof(ngds_array_splay_tree_node_t));
  perform_downward_shift(me, left_child_of(right_child_of(idx)),
    right_child_of(left_child_of(idx)), 0);
  me->node_array[left_child_of(right_child_of(idx))].key = NULL;
  perform_upward_shift(me, right_child_of(idx), idx);

  return right_child_of(idx);

} /* ngds_array_splay_tree_rotate_left() */

/* ------------------------------------------------------------------------- */

int
ngds_array_splay_tree_rotate_right (
  ngds_array_splay_tree_t* me,
  int idx
) {
  perform_downward_shift(me, right_child_of(idx),
    right_child_of(right_child_of(idx)), 0);
  memcpy(&me->node_array[right_child_of(idx)], &me->node_array[idx],
    sizeof(ngds_array_splay_tree_node_t));
  perform_downward_shift(me, right_child_of(left_child_of(idx)),
    left_child_of(right_child_of(idx)), 0);
  me->node_array[right_child_of(left_child_of(idx))].key = NULL;
  perform_upward_shift(me, left_child_of(idx), idx);

  return left_child_of(idx);

} /* ngds_array_splay_tree_rotate_right() */

/* ------------------------------------------------------------------------- */

ngds_array_splay_tree_node_t *
ngds_array_splay_tree_get_node_at_idx (
  ngds_array_splay_tree_t    *me,
  int                         idx
) {
  return &me->node_array[idx];
} /* ngds_array_splay_tree_get_node_at_idx() */

/* ========================================================================= */
/* -- PUBLIC FUNCTIONS ----------------------------------------------------- */
/* ========================================================================= */

ngds_array_splay_tree_t *
ngds_array_splay_tree_new (
  int                   initial_element_count,
  ngds_comparator_fptr  comparefp,
  ngds_malloc_fptr      mallocfp,
  ngds_free_fptr        freefp
) {
  ngds_array_splay_tree_t  *me;

  assert((initial_element_count > 0));
  assert(comparefp);

  if (NULL == mallocfp) {
    mallocfp = malloc;
  }

  if (NULL == freefp) {
    freefp = free;
  }

  /* TODO FIX CHECK FOR ALLOCATION FAILURES */
  me = mallocfp(sizeof(ngds_array_splay_tree_t));
  if (NULL == me) {
    assert(0);
  }

  memset(me, 0, sizeof(ngds_array_splay_tree_t));
  me->allocated_element_count = initial_element_count;
  me->utilized_element_count = 0;
  me->shifted_element_bitmap_size_in_bytes =
    (BITNSLOTS(initial_element_count) * sizeof(uint64_t));
  me->shifted_element_bitmap =
    mallocfp(me->shifted_element_bitmap_size_in_bytes);
  memset(me->shifted_element_bitmap, 0,
    me->shifted_element_bitmap_size_in_bytes);
  me->node_array = mallocfp(
    (me->allocated_element_count * sizeof(ngds_array_splay_tree_node_t)));
  memset(me->node_array, 0,
    (me->allocated_element_count * sizeof(ngds_array_splay_tree_node_t)));
  me->compare = comparefp;
  me->malloc = mallocfp;
  me->free = freefp;

  return me;
} /* ngds_array_splay_tree_new() */

/* ------------------------------------------------------------------------- */

void
ngds_array_splay_tree_destroy (
  ngds_array_splay_tree_t    *me
) {
  me->free(me->node_array);
  me->free(me->shifted_element_bitmap);
  me->free(me);
} /* ngds_array_splay_tree_destroy() */

/* ------------------------------------------------------------------------- */

void
ngds_array_splay_tree_clear (
  ngds_array_splay_tree_t    *me
) {
  me->utilized_element_count = 0;
  memset(me->node_array, 0,
    (me->allocated_element_count * sizeof(ngds_array_splay_tree_node_t)));
} /* ngds_array_splay_tree_clear() */

/* ------------------------------------------------------------------------- */

void
ngds_array_splay_tree_print (
  ngds_array_splay_tree_t    *me,
  ngds_node_printer           print_cb
) {
  printf("SPLAY Tree:\n");
  perform_tree_print(me, print_cb, NG_SPLAY_ROOT_INDEX, 0);
} /* ngds_array_splay_tree_print() */

/* ------------------------------------------------------------------------- */

void
ngds_array_splay_tree_insert (
  ngds_array_splay_tree_t    *me,
  void                       *key,
  void                       *value,
  bool                        should_perform_splay
) {
  bool                          key_was_found = false;
  int                           current = NG_SPLAY_ROOT_INDEX;
  ngds_array_splay_tree_node_t *node;

  /* If this is a brand new tree */
  if (0 < me->utilized_element_count) {
    int cmp = 0;
    while (true == NODE_IS_VALID(me, current)
            && false == NODE_IS_EMPTY(me, current)) {
      node = &me->node_array[current];
      cmp = me->compare(node->key, key);
      if (0 == cmp) {
        key_was_found = true;
        break;
      } else if (0 > cmp) {
        current = left_child_of(current);
      } else {
        current = right_child_of(current);
      }
    }

    if (false == NODE_IS_VALID(me, current)) {
      printf("NEED TO EXPAND!\n");
      assert(0);
    }
  }

  if (false == key_was_found) {
    ++me->utilized_element_count;
  }

  node = &me->node_array[current];
  node->key = key;
  node->value = value;

  if (true == should_perform_splay) {
    perform_splay_operation(me, current);
  }

} /* ngds_array_splay_tree_insert() */

/* ------------------------------------------------------------------------- */

void *
ngds_array_splay_tree_get (
  ngds_array_splay_tree_t    *me,
  const void                 *key,
  bool                        should_perform_splay
) {
  ngds_array_splay_tree_node_t *node;
  int                           current = NG_SPLAY_ROOT_INDEX;
  int                           cmp = 0;
  while (true == NODE_IS_VALID(me, current)
          && false == NODE_IS_EMPTY(me, current)) {
    node = &me->node_array[current];
    cmp = me->compare(node->key, key);
    if (0 == cmp) {
      break;
    } else if (0 > cmp) {
      current = left_child_of(current);
    } else {
      current = right_child_of(current);
    }
  }

  if (false == NODE_IS_VALID(me, current)
      || true == NODE_IS_EMPTY(me, current)) {
    return NULL;
  }

  if (true == should_perform_splay) {
    perform_splay_operation(me, current);
    return (&me->node_array[NG_SPLAY_ROOT_INDEX])->value;
  } else {
    return (&me->node_array[current])->value;
  }

} /* ngds_array_splay_tree_get() */

/* ------------------------------------------------------------------------- */

int
ngds_array_splay_tree_cardinality (
  ngds_array_splay_tree_t    *me
) {
  return me->utilized_element_count;
}

/* ------------------------------------------------------------------------- */

int
ngds_array_splay_tree_size (
  ngds_array_splay_tree_t* me
) {
  return me->allocated_element_count;
}

/* ------------------------------------------------------------------------- */

void *
ngds_array_splay_tree_remove (
  ngds_array_splay_tree_t    *me,
  void                       *key
) {
  ngds_array_splay_tree_node_t *node;
  int                           current = NG_SPLAY_ROOT_INDEX;
  int                           cmp = 0;
  while (true == NODE_IS_VALID(me, current)
          && false == NODE_IS_EMPTY(me, current)) {
    node = &me->node_array[current];
    cmp = me->compare(node->key, key);
    if (0 == cmp) {
      break;
    } else if (0 > cmp) {
      current = left_child_of(current);
    } else {
      current = right_child_of(current);
    }
  }

  if (false == NODE_IS_VALID(me, current)
      || true == NODE_IS_EMPTY(me, current)) {
    return NULL;
  }

  --me->utilized_element_count;

  void *k = node->key;
  int predecessor = find_predecessor(me, current);
  if (-1 == predecessor) {
    memset(&me->node_array[current], 0, sizeof(ngds_array_splay_tree_node_t));
  } else {
    perform_upward_shift(me, left_child_of(predecessor),
      right_child_of(parent_of(predecessor)));
    perform_upward_shift(me, predecessor, current);
  }

  return k;

} /* ngds_array_splay_tree_remove() */

/* vi: set et sw=2 ts=2: */

