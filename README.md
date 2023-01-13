# Indentation-Branching

A transpiler for non line-based programming languages allowing you to write code without line independent symbols like ";{},", by parsing indentation, newlines and in outsider cases keywords

This drastically reduces code size(by 40% in length and 10% in filesize, on ib itself), makes code easier to write, and in my opinion encurages good code formating

It also gets rid of the where to put the braces dicotomy, which plague tradition programming syntax aswell as the common mistake of missing semicolons

### Example

```
// example.c.ib

#if 1
    #include <stdio.h>
#else
    #error "not possible"

/*
    multiline comments work sorta
*/

enum dir
    negative = -1
    positive = 1
    neutral  = 0

struct point
    enum dir x
    enum dir y

int main()
    struct point pt =
        .x = positive
        .y = negative
    
    if (pt.x = positive && pt.y = negative)
    	const char strings[2] =
		"unbelievable!!" \
		"multiline string!!!\n"
		"another string"
        printf("%s%s", strings[0], strings[1])
        return 0
    else
        return 1 // comment after termination
		
```

```
$ ib example.c.ib --spaces 4 (note: this flag is only required for people who use spaces to indent)
```

```c
// example.c

#if 1
    #include <stdio.h>
#else
    #error "not possible"
#endif

/*
    multiline comments work sorta
*/

enum dir {
    negative = -1,
    positive = 1,
    neutral  = 0
};

struct point {
    enum dir x;
    enum dir y;
};

int main()
{
    struct point pt = {
        .x = positive,
        .y = negative
    };
    
    if (pt.x = positive && pt.y = negative)
    {
        const char strings[2] = {
		"unbelievable!!" \
		"multiline string!!!\n",
		"another string"
	};
        printf("%s%s", strings[0], strings[1]);
        return 0;
    }
    else
    {
        return 1; //comment after termination
    }
}
```

### Initial Build
~~~
(root) $ make bootstrap (compiles provided pretranspiled file and installs it in ${PREFIX}/bin/)
~~~

### Build with IB installed

~~~
$ make

$ make test (transpiles itself to ensure that you are not installing a faulty build)

(root) $ make install
~~~

### Languages

- C:
this transpiler is originally only intended for C, it supports even advanced c syntactical features and Im currently working to intigrate ib into the c compilation process in medias res

- CPP:
While cpp also works pretty well, I do not intend to build extra features for it, its syntax is nearly the same as c, basically all usual language features should be functional, some typical syntactics like putting template declarations in a seperate line need to be avoided or manually done by using backslashes

- GO:
While im not sure as to how well ib will intigrate with go as I don't use it alot, for now it seams functional, mind you I have already had to set an exception because go is actually a line based language in sheeps clothing

- JAVA:
Im not a big fan of this one, but as I have been forced to using it at times, yes ib "supports" java, I in no way intend to maintain advanced syntax features

### Editors

looks best with a smaller tab size, using spaces as tabs is supported by a special flag but not recommended.

displaying tabs and spaces is strongly recommended as any improper indentation will completly break your code

typically the syntax highlighting of the language works if forced

- VIM:
~~~
au! BufRead,BufNewFile *.c.ib setfiletype c

set tabstop=4
set shiftwidth=4

set list
set listchars=tab:>\ ,space:.
~~~
