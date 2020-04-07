# Clok - Compiler for the Lok programming language

[![Build Status](https://img.shields.io/travis/TheOnlyMrCat/lok?logo=travis)](https://travis-ci.org/TheOnlyMrCat/lok)
![Build Status](https://img.shields.io/github/workflow/status/TheOnlyMrCat/lok/C++%20CI?logo=github)
[![Discord server](https://img.shields.io/discord/674174065761583104?color=blueviolet&logo=discord&logoColor=white)](https://discord.gg/nNu3GSs)

**This project is no longer being updated. As with all projects, it has turned into a mess of spaghetti, and I daren't touch it**

This is a compiler for a programming language I am making called lok. Its goal is to have its syntax be as logical
as possible. For now, snippets of the planned syntax can be seen in the
[test](https://github.com/TheOnlyMrCat/lok/tree/master/test) folder.

## Design

### Laws

The design laws I'm using making Lok are ones I think are logical.

1: **The compiler is not allowed to search for identifiers (class names, function names, variable names, etc.) when trying to implement functionality.**
This means that, among other things, exceptions are not able to derive from one central class (like `Throwable` in Java)
and foreach loops can't search for a `begin()` and `end()` function on classes. The main effect this has is to prohibit
a `main()` function being used as the entry point, so I've replaced it with `run` declarations.

There is only one exception to this rule: `bit`, which is reserved so I can actually have a value type.

2: **Whitespace anywhere in the code must be entirely optional.**
In more technical terms, for every valid source input, you must be able to remove all bytes `0x09`, `0x0A` and `0x20` and
have it function exactly the same. This does not necessarily mean you are able to insert whitespace anywhere in the code
and have it function, but you must be able to remove whitespace that is there.

Single-line comments are an exception to this rule. They are allowed to exist without closing syntax, and the test function
will remove single-line comments.

Both of these design choices introduce challenges into creating the syntax and systems, notably the whitespace one because
I have to define separators in place of whitespace where whitespace would be necessary.
