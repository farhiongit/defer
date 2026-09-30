# Deferring resource release in C11

*On an elegant [idea](https://antonz.org/defer-in-c) from Anton Zhiyanov.*

## Usage

    #include "defer.h"
    
    defer_release (void *variable, void (*function) (void *));
    defer_release_mutable (void *variable, void (*function) (void *));

## Description

A defer statement (`defer_release` or `defer_release_mutable`) defers the execution of a function until leaving the scope containing the statement.

It takes two parameters:

- a pointer `variable` (usually with automatic storage) ;
- a function `function` (with prototype `<return_type> (*) (<type> *)`) that takes one parameter: a pointer which type is compatible with the associated pointer `variable`.

The deferred function `function` is automatically called (whatever the reason) :

- with, as single argument, the associated pointer `variable` set by the defer statement ;
- after returning from the enclosing function or after leaving the body of the scope enclosing the defer statement.

When multiple defer statements are set in the same scope, their associated functions are run in reverse order of definition (last defined, first called).

> The defer statement can be applied to any type of pointer (static, automatic ou allocated) but it is mostly useful with automatic pointers.

> The return value of the `function` (if any) is ignored.

### defer_release

The statement uses the current value of pointer **at the moment when the `defer_release` statement is met**.

### defer_release_mutable

If the automatic storage pointer `variable` is subject to redefinition (reallocation) after the defer statement,
`defer_release_mutable` should be used instead of `defer_release`.

The statement uses the value that pointer has **at the moment the `function` is called**.

## Examples

Example:

```c
    <type> <function_name> (<arguments...>) {
        ...
        int *aa = malloc (10 * sizeof (*aa));
        // Defer automatic deallocation when leaving the function (whatever the reason):
        defer_release_mutable (aa, free);
        ...
        if (<condition>)
          return <type>;
        // The deferred action is automatically executed: the array aa is released.
        ...
        aa = realloc (aa, 20 * sizeof (*aa)); // Reallocation supported by defer_mutable.
        ...
        FILE *f = fopen ("toto", "r");
        // Defer file closure when leaving the function (whatever the reason):
        defer_release (f, fclose);
        ...
        mtx_t m;
        mtx_init (&m, mtx_plain);
        // Defer the destruction of the mutex when leaving the function (whatever the reason):
        defer_release (&m, mtx_destroy);
        ...
        return <type>;
        // The deferred actions are automatically executed: the mutex is destroyed, the file f is closed, the reallocated array aa is released (in this order).
    }
```

See [test_defer.c](test_defer.c) for other examples.

Compile with `make test_defer`.

## Internals

This implementation uses the attribute `cleanup` supported by `gcc` and `clang`, together with a [structure](https://antonz.org/defer-in-c) proposed by Anton Zhiyanov.

Hence,

- it is not necessary to dereference the pointer to the allocated resource: library functions `free`, `fclose`, `mtx_unlock` or `mtx_destroy`, to name a few, can be used directly in the defer statement ;
- the defer statement can be applied to static pointers as well (even not very useful) as well as automatic pointers.

*This text is [inspired from gcc](https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html#index-cleanup).*
