# Chapter 21 - Global Variables

- this chapter, we will implement variables
- we will only implement global in this a local in next due to have differing mechanism
- jlox, uses the same concept of environments one for each scope all the way to the top
- jlox was clean but slow. a new hash table for each block or fn call != fast vm
- we will implment a better approach for local variables
  - while global variables remain slow
- Global variables in lox are "late bound" or resolved dynamically
- this means you can compile a chunk of code before it is defined
  - as long as code doesn't execute before it is defined
```{lox}
fn showVariable() {
  print global;
}

var global = "after";
showVariable();
```

- this won't work for local variables
