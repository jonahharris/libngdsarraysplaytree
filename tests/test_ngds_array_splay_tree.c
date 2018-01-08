
#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NG_SPLAY_ARRAY_HAS_ZERO_INDEX_ROOT 0
#include "ngds_array_splay_tree.h"
#include "ngds_array_splay_tree_private.h"

#include "tests/CuTest.h"

/* ========================================================================= */
/* -- GLOBAL FUNCTIONS ----------------------------------------------------- */
/* ========================================================================= */

void *allocations[3] = { NULL };

/* ========================================================================= */
/* -- STATIC FUNCTIONS ----------------------------------------------------- */
/* ========================================================================= */

static int uint_compare (
  const void *e1,
  const void *e2
) {
  return (e2 - e1);
}

  //ngds_array_splay_tree_print(t, node_printer);
static void node_printer (
  const ngds_array_splay_tree_node_t *node
) {
  int key = *((int *) &node->key);
  int val = *((int *) &node->value);
  printf("key [%d] val [%d]", key, val);
}

void *
mymalloc (size_t num_bytes) {
  static int allocation_count = 0;
  allocations[allocation_count] = malloc(num_bytes);
  return allocations[allocation_count++];
}

/*
 * Zig Right (Splay on 3)
 *
 *        5                    3
 *       / \                  / \
 *      3   6 =============> 2   5
 *     / \                      / \
 *    2   4                    4   6
 */
void
perform_zig_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 3, 2, 5, 0, 0, 4, 6 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_rotate_right(t, 1);

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_zig_test() */

/* ------------------------------------------------------------------------- */

/*
 * Zag Left (Splay on 5)
 *
 *        3                        5
 *       / \                      / \
 *      2   5   =============>   3   6
 *         / \                  / \
 *        4   6                2   4
 */
void
perform_zag_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 3, 2, 5, 4, 6 };
  int tests[] = { 0, 5, 3, 6, 2, 4 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_rotate_left(t, 1);

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_zag_test() */

/* ------------------------------------------------------------------------- */

/*
 * Zig-Zig (Splay on 2)
 *
 *        5                    3                  2
 *       / \                  / \                  \
 *      3   6 =============> 2   5 =============>   3
 *     / \                  / \                      \
 *    2   4                4   6                      5
 *                                                   / \
 *                                                  4   6
 */
void
perform_zigzig_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 2, 0, 3, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 4, 6 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_rotate_right(t, 1);
  ngds_array_splay_tree_rotate_right(t, 1);

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_zigzig_test() */

/* ------------------------------------------------------------------------- */

/*
 * Zag-Zag (Splay on 6)
 *
 *        3                        5                        6
 *       / \                      / \                      /
 *      2   5   =============>   3   6 =============>     5
 *         / \                  / \                      /
 *        4   6                2   4                    3
 *                                                     / \
 *                                                    2   4
 */
void
perform_zagzag_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 3, 2, 5, 4, 6 };
  int tests[] = { 0, 6, 5, 0, 3, 0, 0, 0, 2, 4 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (int ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_rotate_left(t, 1);
  ngds_array_splay_tree_rotate_left(t, 1);

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_zagzag_test() */

/* ------------------------------------------------------------------------- */

/*
 * Zig-Zag (Splay on 4)
 *
 *        3                      3                          4
 *       / \                    / \                        / \
 *      2   5   =============> 2   4     =============>   3   5
 *         / \                      \                    /     \
 *        4   6                      5                  2       6
 *                                    \
 *                                     6
 */
void
perform_zigzag_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 3, 2, 5, 4, 6 };
  int zigtest[] = { 0, 3, 2, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 6 };
  int zagtest[] = { 0, 4, 3, 5, 2, 0, 0, 6 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_rotate_right(t, 3);

  for (ii = 0; ii < sizeof(zigtest) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, zigtest[ii] == (int) node->value);
  }

  ngds_array_splay_tree_rotate_left(t, 1);

  for (ii = 0; ii < sizeof(zagtest) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, zagtest[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_zigzag_test() */

/* ------------------------------------------------------------------------- */

/*
 * Zag-Zig (Splay on 4)
 *
 *        5                        5                     4
 *       / \                      / \                   / \
 *      3   6 =============>     4   6 =============>  3   5
 *     / \                      /                     /     \
 *    2   4                    3                     2       6
 *                            /
 *                           2
 */
void
perform_zagzig_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int zagtest[] = { 0, 5, 4, 6, 3, 0, 0, 0, 2 };
  int zigtest[] = { 0, 4, 3, 5, 2, 0, 0, 6 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (int ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_rotate_left(t, 2);

  for (ii = 0; ii < sizeof(zagtest) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, zagtest[ii] == (int) node->value);
  }

  ngds_array_splay_tree_rotate_right(t, 1);

  for (ii = 0; ii < sizeof(zigtest) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, zigtest[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_zagzig_test() */

/* ------------------------------------------------------------------------- */

/*
 * Get 3 (With Splay)
 *
 *        5                    3
 *       / \                  / \
 *      3   6 =============> 2   5
 *     / \                      / \
 *    2   4                    4   6
 */
void
perform_get_with_splay_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 3, 2, 5, 0, 0, 4, 6 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  CuAssertTrue(tc, 3 == (int) ngds_array_splay_tree_get(t, (void *) 3, true));

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_get_with_splay_test() */

/* ------------------------------------------------------------------------- */

/*
 * Get 3 (Without Splay)
 *
 *        5                      5 
 *       / \                    / \
 *      3   6 =============>   3   6
 *     / \                    / \
 *    2   4                  2   4
 */
void
perform_get_without_splay_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 5, 3, 6, 2, 4 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  CuAssertTrue(tc, 3 == (int) ngds_array_splay_tree_get(t, (void *) 3, false));

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_get_without_splay_test() */

/* ------------------------------------------------------------------------- */

/*
 * Test Custom Allocator
 */
void
perform_custom_allocator_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 5, 3, 6, 2, 4 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, mymalloc, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  /* Point directly to the memory segment allocated for the node array */
  ngds_array_splay_tree_node_t *node_memory_segment =
    (ngds_array_splay_tree_node_t *) allocations[2];
  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node = &node_memory_segment[ii];

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_get_without_splay_test() */

/* ------------------------------------------------------------------------- */

/*
 * Test Custom Allocator
 */
void
perform_removal_without_predecessor_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 5, 3, 6, 2, 0 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_remove(t, (void *) 4);

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_removal_without_predecessor_test() */

/* ------------------------------------------------------------------------- */

/*
 * Test Custom Allocator
 */
void
perform_removal_with_predecessor_test (
  CuTest               *tc
) {
  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };
  int tests[] = { 0, 5, 2, 6, 0, 4 };
  int ii;

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], false);
  }

  ngds_array_splay_tree_remove(t, (void *) 3);

  for (ii = 0; ii < sizeof(tests) / sizeof(int); ++ii) {
    ngds_array_splay_tree_node_t *node =
      ngds_array_splay_tree_get_node_at_idx(t, ii);

    CuAssertTrue(tc, tests[ii] == (int) node->value);
  }

  ngds_array_splay_tree_destroy(t);

} /* perform_removal_with_predecessor_test() */

void
test_zagzig2 (void) {

  ngds_array_splay_tree_t *t;
  int nodes[] = { 5, 6, 3, 2, 4 };

  printf("*** TEST %s\n", __PRETTY_FUNCTION__);

  t = ngds_array_splay_tree_new(128, uint_compare, NULL, NULL);

  for (int ii = 0; ii < sizeof(nodes) / sizeof(int); ++ii) {
    ngds_array_splay_tree_insert(t,
      (void *) nodes[ii], (void *) nodes[ii], true);
  }

  ngds_array_splay_tree_print(t, node_printer);
  int *val = (int *) ngds_array_splay_tree_get(t, (void *) 5, true);
  printf("val -> %d\n", *(int *) &val);
  ngds_array_splay_tree_print(t, node_printer);
  val = (int *) ngds_array_splay_tree_get(t, (void *) 3, false);
  printf("val -> %d\n", *(int *) &val);
  //ngds_array_splay_tree_print(t, node_printer);
/*
  ngds_array_splay_tree_rotate_left(t, 2);
  ngds_array_splay_tree_print(t, node_printer);
  ngds_array_splay_tree_rotate_right(t, 1);
  ngds_array_splay_tree_print(t, node_printer);
*/

} /* test_zig() */

/* vi: set et sw=2 ts=2: */

