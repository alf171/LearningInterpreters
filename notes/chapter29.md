# Chapter 29 - Superclasses

- last chapter of functionality added to LOX
- all that remains is inheritance & super methods
- approach is similar to jlox for this chapter
  - although there is a more complicated way to store state on the stack
- we have a new `OP_INHERIT` instruction
  - this instruction wires up the superclass to the new subclass
- we are going to use a more performance rich approach
  - step 1/ when a sub class is declared, we copy all the inheritted methods in
    - this technique has sometimes been called copy-down inheritance
    - it only works in lox bc classes are closed
    - in python, ruby, js, you can jam or remove methods afterwards
  - we dont have to worry about this approach breaking overrides
    - bc `OP_CLASS` / `OP_METHOD` occur after and tables do the rest
    - but super keyword won't work without any references to the superclass
    - super can be very helpful when you want to disambiguate btwn local and super
    - we could do something like this

```lox
class A {
    method() {
      print "A method";
    }
}
  var Bs_super = A;
  class B < A {
    method() {
      print "B method";
}
    test() {
      runtimeSuperCall(Bs_super, "method");
} }
  var Cs_super = B;
  class C < B {}
C().test();
```

- we will put `super` under its own slot
- we are start a new scope, each has its own reference (this vs super)
- unlike this, a super token is not a standalone statement
  - but super also supports access expressions `var x = super.method; x()`
- process for handling a super
    1. the instruction loads the instance onto the stack
    2. the second instruction loads the superclass where the method is resolved
    3. `OP_GET_SUER` encodes the name of the nmethod to access as an operand
- how do we run `OP_GET_SUPER`
    1. we read the method name from the const table
    2. pass it to `bindMethod()` which looks up the method in given class's table
      - creates an ObjBoundMethod to bundle the resulting closure to current instance
- the current approach is semantically correct but slow again like last chapter
- heap allocating a `ObjBoundMethod` for each super call, usually next call is OP_CALL
  - we are going to combine into one call
- inheritence complicates the tenet of all objects being in valid state
  - consider super and base class claiming the same field
  - should we fix this...
- if our language allowed adding things after creation, inheritance gets more complex
