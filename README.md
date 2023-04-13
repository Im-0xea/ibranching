# Indentation Branching

## Index

- [Introduction](#introduction)
- [Background](#background)
- [Base Concepts](#base-concepts)
- [C isms](#c-isms)
- [Editors](#editors)
    - [Vim](#vim)
- [Installation](#installation)
    - [Packages](#packages)
    - [Source](#source)

## Introduction

IB (Indentation Branching) is a transpiler which 'turns everything into python', it is my frankenstein, so seams I might aswell write some documentation to explain some of my madness.

## Background

I used to have a very old german mechanical keyboard, which was missing 'alt-gr', making it unable to type '{}' on the iso layout.

While mapping keys was possible in some cases, I wanted to be able to write C on any machine, so I decided I would try to make a program which would generate the braces for me, the indentation implied where I wanted them anyhow, so on a lazy afternoon I wrote IB for C, and have since been expanding it onto other syntactics.

## Base Concepts

The following rules are in use in some form in all languages, although as indicated by the quotes some concepts have ambiguious meanings

### Scoping

Indentation is 'scoping'.

~~~ C
 -> 
~~~
&#13;

~~~ C
{
 -> 
}
~~~ 

### Termination

Each line not followed by scoping is 'terminated'

~~~ C
print("one")
if (1)
    print("two")
~~~
&#13;

~~~ C
print("one");
if (1)
{
    print("two");
}
~~~

### End of Line

A line doesn't end if its last charcter is '\\'.

~~~ C
print("three" \
      "four" )
~~~
&#13;

~~~ C
print("three" \
      "four" );
~~~

A line ends when a comment is opened.

~~~ C
print("five") // prints number
~~~
&#13;

~~~ C
print("five"); // prints number
~~~

## C isms

IB was originally only intended for and is written in C, while the base concepts allow you to already write pretty clean C, IB brings some quality of life improvements 

### Lists

Scopes which are opened by a line ending with '=' are lists and each line inside is terminated with a ',', the list itself is also terminated.

~~~ C
int nums[] =
    1
    2
    3
~~~
&#13;

~~~ C
int nums[] =
{
    1,
    2,
    3
};
~~~
#### Enums

The same rule applies to enums.. which turns out to be a nightmare, as enums only stick out because of the keyword enum, which IB parses for.

## Editors

Ib syntax tends to look better with a smaller tabsize, it is also strongly recommended to use actual tabs instead of spaces, although it does support them, any improper indentation will cause failure.

### Vim

~~~ vim
au! BufRead,BufNewFile *.c.ib setfiletype c
au! BufRead,BufNewFile *.cpp.ib setfiletype cpp

set tabstop=4
set shiftwidth=4

set list
set listchars=tab:>\ ,space:.
~~~

## Installation

### Packages

- gentoo [my ebuild repo](https://github.com/Nik-Nothing/niki-gentoo)
- arch [AUR](https://aur.archlinux.org/packages/ib-git) [my pkgbuild repo](https://github.com/Nik-Nothing/niki-pacman)

### Source

~~~
$ make bootstrap

(root) $ make install
~~~
