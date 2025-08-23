# Chapter 13: Inheritance

- Simula was the first language with inheritance
  - this was to remove duplicate code of simulation programs
- Superclasses and Subclasses
  - we'll use these terms to describe inheritance
  - parent, child, subclass, etc are all other terms
- we'll follow ruby and use a less than sign for inheritance
- classDecl -> "class" IDENTIFIER ( "<" IDENTIFIER )? "{" function "}" ;
- syntax
  - this sort of works like a magic variable is that one lone token
  super, the subsequent . and property are inseperable parts of the super expression
  - you cant do print super
  primary -> "true" | "false" | "nil" | "this" | NUMBER | STRING \
  | IDENTIFIER | "(" expression ")" | "super" "." IDENTIFER ;
- super expression starts the method lookup from the super class but which?
- imagine C < B < A -- we have weird scenarios here
- so we all need access to the super class of what we inherit from
