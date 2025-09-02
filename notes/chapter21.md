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
- lox splits statements into two types
  - declerations (bind value with name)
  - other type is control flow, print, etc called statements
  - we disalow declarations directly inside control flow
- in this chapter, we will cover statement
  - statement -> exprStmt | printStmt;
  - decleration -> varDecl | statement;
- pop and print is all the PRINT OP does
- the key difference between expressions and statements in the VM
- every bytecode instruction has a stack effect that describes how the stack is modified
- the byte code for an entire statement has total stack effect of zero
  - even though it uses the stack while doing its thing
  - this is important bc when we get to control flow / loops, we could overflow or under
- an expression statement is an expression followed by a semicolon
- brunch = "a"; eat(brunch);
- we need the following three things to support variables
    1. declaring a new variable using "var" statement
    2. Accessing the value using an identifier
    3. store a new value in an existing variable using assignment expression
- we need to be careful for assignment to not support `a * b = c * c`
- this would be permitted unless we are careful because = has lowest precedence
- we should do this: variable() should only look for and consume = if
    1. it's in the context of a low precedence expression
