*On an elegant [idea](https://antonz.org/defer-in-c) from Anton Zhiyanov.*

## Usage

    #include "defer.h"
    
    defer (void (*function) (void *const), void *const variable);
    defer_mutable (void (*function) (void *const), void *const variable);

## defer

The `defer` statement runs a function when the associated pointer goes out of scope.

The `defer` statement  must take two parameters:

- an automatic storage pointer variable ;
- a function that takes one parameter, a pointer which type is compatible with the associate pointer variable.

> The `defer` statement can only be applied to auto scope variables; it may not be applied to pointers with static storage duration.

> The return value of the function (if any) is ignored.

When multiple `defer` statements are in the same scope, at exit from the scope their associated functions are run in reverse order of definition (last defined, first called).

## defer_mutable

If the automatic storage pointer variable is subject to redefinition (reallocation) after the defer statement, `defer_mutable` should be used instead of `defer`.

## Examples

See [test_defer.c](test_defer.c) for examples.

Compile with `make test_defer`.

*This text is [inspired from gcc](https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html#index-cleanup).*
