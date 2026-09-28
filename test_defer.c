#include "defer.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

static const size_t NB_ELEM = 10;

static void
testa (void) {
  int *const vla = calloc (NB_ELEM, sizeof (*vla));
  assert (vla);
  defer (free, vla); // Uses the current value of ptr at the moment when the defer is met.

  // vla = realloc (vla, 2 * NB_ELEM * sizeof (*vla)); /* Would leak */
}

static int
free_from_address (void *address) {
  free (*(void **)address);
  return 0;
}

static void
testb (void) {
  int *vla = calloc (NB_ELEM, sizeof (*vla));
  assert (vla);
  defer (free_from_address, &vla);

  vla = realloc (vla, 2 * NB_ELEM * sizeof (*vla));
}

static void
testc (void) {
  int *vla = calloc (NB_ELEM, sizeof (*vla));
  assert (vla);
  defer_mutable (free, vla); // Uses the value that ptr has at the end of the execution of the block.

  vla = realloc (vla, 2 * NB_ELEM * sizeof (*vla)); // Reallocation after defer statement.
}

static void
testd (void) {
  mtx_t mtx;
  assert (mtx_init (&mtx, mtx_plain) == thrd_success);
  defer (mtx_destroy, &mtx);
  assert (mtx_lock (&mtx) == thrd_success);
  defer (mtx_unlock, &mtx);
}

static void
teste (void) {
  char *sa = 0;
  defer_mutable (free, sa);
  assert ((sa = strdup ("Hello a"))); // // Allocation after defer statement.
}

static void
testf (void) {
  char buf[5];
  FILE *f = fmemopen (buf, 5, "r");
  assert (f);
  defer (fclose, f);
}

int
main (void) {
  testa ();
  testb ();
  testc ();
  testd ();
  teste ();
  testf ();
}
