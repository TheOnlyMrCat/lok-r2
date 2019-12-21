# Clok – Compiler for the Lok programming language

[![Build Status](https://travis-ci.org/TheOnlyMrCat/lok.svg?branch=master)](https://travis-ci.org/TheOnlyMrCat/lok)

This is a compiler for a programming language I am making called lok. Its goal is to have its syntax be as logical
as possible. For now, snippets of the planned syntax can be seen in the
[test](https://github.com/TheOnlyMrCat/lok/tree/master/test) folder.

## Design

### Philosophies

The design 'philosophies' (?) I'm using making Lok are ones I think are logical.

First, the compiler is not allowed to search for identifiers (class names, function names, variable names, etc.)
when trying to implement functionality. This means that, among other things, exceptions are not able to derive from one
central class (like `Throwable` in Java) and foreach loops can't search for a `begin()` and `end()` function on classes.

Second, whitespace anywhere in the code must be entirely optional. In more technical terms, for every valid source input,
you must be able to remove all bytes `0x09`, `0x0A` and `0x20` and have it function exactly the same. This does not
necessarily mean you are able to insert whitespace anywhere in the code and have it function, but you must be able to remove
whitespace that is there.

Both of these design choices introduce challenges into creating the syntax and systems, notably the whitespace one because
I have to define separators in place of whitespace where whitespace would be necessary.
