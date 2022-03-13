# xerxeslang
Update (03/12/22): I've returned to this project three years later to fix past me's code. The original version which I wrote in 2019 is on the ["original" branch.](https://github.com/tafaulhaber590/xerxeslang/tree/original)

Is there even anyone else here?


# Xerxes: Not really Lisp
A simple programming language I made back when I was a freshman in HS

It uses Polish notation but its similarities with normal Lisp dialects pretty much end there. It is an interpreted language.

You can compile the code using:
```
./buildall
```

Then, you can run a Xerxes program using:
```
bin/xerxes [program]
```

There are some sample programs in the test_files directory for you to try out! My personal favorite is tictactoe.

Language documentation

You define a variable with the syntax:
```
(define a 1)
```
You define a function with the syntax:
```
(function (a b)
  (/ (+ a b) 2))
```

For-loops are pretty easy:
```
"test_files/numbers_pyramid"

"Prints a pyramid of numbers"

(include builtin_lib)
(macros builtin_lib)

(for (define i 1) (< i 11) (++ i) (do
  (for (define j 0) (< j i) (++ j)
    (print j))
  (print "\n")))
```

Note that the "for" construction is actually defined as a macro in builtin_lib, so the `(macros builtin_lib)` directive is necessary.

If-statements are also pretty easy. They work the same way they do in most Lisp dialects, with the syntax:
```
(if [cond]
  [then]
  [else?])
```

# TODO
+ Namespaces
+ Put the code in parse.c into different files because it is too long
+ Allow system calls to be made from xerxes code
+ Implement CLOS
+ Fix scoping
