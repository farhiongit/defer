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
  defer_release (vla, free); // Uses the current value of vla at the moment when the defer is met.

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
  defer_release (&vla, free_from_address);

  vla = realloc (vla, 2 * NB_ELEM * sizeof (*vla));
}

static void
testc (void) {
  int *vla = calloc (NB_ELEM, sizeof (*vla));
  assert (vla);
  defer_mutable_release (vla, free); // Uses the value that vla has at the end of the execution of the block.

  vla = realloc (vla, 2 * NB_ELEM * sizeof (*vla)); // Reallocation after defer statement.
}

static void
testd (void) {
  mtx_t mtx;
  assert (mtx_init (&mtx, mtx_plain) == thrd_success);
  defer_release (&mtx, mtx_destroy);
  assert (mtx_lock (&mtx) == thrd_success);
  defer_release (&mtx, mtx_unlock);
}

static void
teste (void) {
  char *sa = 0;
  defer_mutable_release (sa, free);
  assert ((sa = strdup ("Hello a"))); // Allocation after defer statement.
}

static void
testf (void) {
  char buf[5];
  FILE *f = fmemopen (buf, 5, "r");
  assert (f);
  defer_release (f, fclose);
}

static int
disp (char *str) {
  return fprintf (stdout, "%c\n", *str); // The returned value is ignored by the deferred statement.
}

static char
testg (void) {
  static char a = 'A';
  static char b = 'B';
  static char c = 'C';
  defer_release (&a, disp);
  defer_release (&b, disp);
  defer_release (&c, disp);
  defer_release (&a, disp);
  fprintf (stdout, "-\n");
  return (a++, b++, c++);
  // Deferred functions are called here, in reversed order, after returning from the function.
}

int
main (void) {
  testa ();
  testb ();
  testc ();
  testd ();
  teste ();
  testf ();
  testg ();
  testg ();
}
