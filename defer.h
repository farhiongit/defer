#ifndef __DEFER_RELEASE_H__
#define __DEFER_RELEASE_H__

// Compile with option -Wno-cast-function-type
// From Anton Zhiyanov (https://antonz.org/defer-in-c/)
//=================================================================
#define _DEFER_CONCAT(a, b) a##b
#define _DEFER_NAME(a, b) _DEFER_CONCAT (a, b)
//=================================================================
// Deferred function and its argument.
struct _defer_ctx {
  void (*fn) (void *);
  void *arg;
};
//=================================================================
// Calls the deferred function with its argument.
static void
_defer_cleanup (struct _defer_ctx *ctx) {
  if (ctx->fn)
    ctx->fn (ctx->arg);
}

// Create a deferred function call for the current scope.
// Uses the current value of ptr at the moment when the defer is met.
#define defer_release(ptr, f)                              \
  struct _defer_ctx _DEFER_NAME (_defer_var_, __COUNTER__) \
      __attribute__ ((cleanup (_defer_cleanup)))           \
      = (struct _defer_ctx) { .fn = (void (*) (void *)) (void (*) (void)) /* unsafe cast function type */ (f), .arg = (ptr) }
/*
  char *sa = strdup ("Hello a");
  defer_release (sa, free);
*/
//=================================================================
// Calls the deferred function with its argument as the content of an address.
static void
_defer_cleanup_by_ref (struct _defer_ctx *ctx) {
  if (ctx->fn)
    ctx->fn (*(void **)(ctx->arg));
}

// Create a deferred function call for the current scope with the argument passed by reference (as an address).
// Uses the value that ptr has at the end of the execution of the block.
#define defer_mutable_release(ptr, f)                      \
  struct _defer_ctx _DEFER_NAME (_defer_var_, __COUNTER__) \
      __attribute__ ((cleanup (_defer_cleanup_by_ref)))    \
      = (struct _defer_ctx) { .fn = (void (*) (void *)) (void (*) (void)) /* unsafe cast function type */ (f), .arg = &(ptr) }
/*
  int *vla = calloc (10, sizeof (*vla));
  defer_mutable_release (vla, free);  // Uses the value that vla has at the end of the execution of the block.
  vla = realloc (vla, 20 * sizeof (*vla)); // Reallocation.
*/
//=================================================================

#endif
