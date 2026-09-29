# Deferring resource release in C11

*On an elegant [idea](https://antonz.org/defer-in-c) from Anton Zhiyanov.*

## Usage

    #include "defer.h"
    
    defer_release_with (void *variable, void (*function) (void *));
    defer_release_mutable_with (void *variable, void (*function) (void *));

## Description

A defer statement (`defer_release_with` or `defer_release_mutable_with`) defers the execution of a function until leaving the scope containing the statement.

It takes two parameters:

- an automatic storage pointer `variable` ;
- a function `function` that takes one parameter, a pointer which type is compatible with the associate pointer `variable`.

The deferred `function` is automatically called when leaving the body of the function or the scope enclosing the defer statement
(`defer_release_with` or `defer_release_mutable_with`), whatever the reason.

The function (with prototype `void (*) (void *)`) is called with, as single argument, the associated pointer `variable` set by the defer statement.

When multiple defer statements are in the same scope, at exit from the scope, their associated functions are run in reverse order of definition (last defined, first called).

> The defer statement can only be applied to auto scope pointers; it may not be applied to pointers with static storage duration.

> The return value of the `function` (if any) is ignored.

### defer_release_with

The statement uses the current value of pointer **at the moment when the `defer_release_with` statement is met**.

### defer_release_mutable_with

If the automatic storage pointer `variable` is subject to redefinition (reallocation) after the defer statement,
`defer_release_mutable_with` should be used instead of `defer_release_with`.

The statement uses the value that pointer has **at the moment the `function` is called**.

## Examples

Example:

```c
    <type> <function_name> (<arguments...>) {
        ...
        int *aa = malloc (10 * sizeof (*aa));
        // Defer automatic deallocation when leaving the function (whatever the reason):
        defer_release_mutable_with (aa, free);
        ...
        if (<condition>)
          return <type>;
        // The deferred action is automatically executed: the array aa is released.
        ...
        aa = realloc (aa, 20 * sizeof (*aa)); // Reallocation supported by defer_mutable.
        ...
        FILE *f = fopen ("toto", "r");
        // Defer file closure when leaving the function (whatever the reason):
        defer_release_with (f, fclose);
        ...
        mtx_t m;
        mtx_init (&m, mtx_plain);
        // Defer the destruction of the mutex when leaving the function (whatever the reason):
        defer_release_with (&m, mtx_destroy);
        ...
        return <type>;
        // The deferred actions are automatically executed: the mutex is destroyed, the file f is closed, the reallocated array aa is released (in this order).
    }
```

See [test_defer.c](test_defer.c) for other examples.

Compile with `make test_defer`.

*This text is [inspired from gcc](https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html#index-cleanup).*
