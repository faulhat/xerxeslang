# xerxeslang
A simple programming language I made back when I was a freshman in HS

It uses Polish notation but its similarities with normal Lisp dialects pretty much end there. It is an interpreted language.

You can compile the code using:
```
mkdir bin # If it's not already there
make build
```
Then, you can run a Xerxes program using:
```
bin/xerxes [program]
```
There are some sample programs in the test_files directory for you to try out!

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

There are a whole lot of builtin functions too. You can look through the test_files directory and see them. My personal favorite is tictactoe. You can write your own files too.

TODO
+ Start over and make a good version.
