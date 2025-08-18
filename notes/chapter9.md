# Chapter 9: Control Flow

- interpreter takes a big step: turing completeness
    1. "Can all true statements be proven?"
    2. "Can we compute all fucntions that we can define?"
- extra) "What do we mean when we claim a function is computable?"
  - alan turing and alonzo church
  - defined a tiny system with minimum set of machinery
    - capable of computing a large class of functions
- presumed answer to 1 and 2 was yes -> turned out to be no
- Turing's system is  turing machine
- Church's is the lambda calculus
- if your language can simulate a turing machine you are therefore turing complete
- need: arithmatic, a little control flow, use memory (theoretically abitrary amounts)
- statement -> exprStmt | ifStmt | printStmt | block;
- "if" "(" expression ")" statement ( "else statement")? ;
- we are missing two control flow operations (and + or)
- expression -> assignment;
- assignment -> IDENTIFIER "=" assignment | logic_or;
- logic_or -> logic_and ("or" logic_and)* ;
- logic_and -> equality ("and" equality)* ;
- while loops
  - TODO: continue
- statement includes whileStmt;
  - while -> "while "(" expression ")" statement;
  - we can write a program which isn't bound be the length of the source code
- for statement:
- "for" "(" (varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement;
  - first clause is the initializer -- runs exactly once
  - next is the condition, controls when to exit the loop
    - runs once at beginning and each iteration
  - last clause is the increment -- does some work each iteration (needs side effects)
  - take advantage of the fact that all while loops can be for loops
