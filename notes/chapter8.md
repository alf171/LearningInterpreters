# Chapter 8 Statements and State

- our interpreter needs internal state to support variables
- can't compose software without ways to refer to pieces
- they can do thing called side effects also

- Statements
  1. expression Statements
      - lets you place an expression where a statement is expected
      - exist to evaluate expressions which have side effects
  2. print statment
      - evaultes an expression and displays results to the user
      - baking print right into language instead of std::library thing
      - we'll need this now to observe the behavior of our interpreter

- gain the ability to parse entire lox scripts
    1. program -> statement* EOF;
    2. statement -> exprStmt | printStmt;
    3. exprStmt -> expression ";";
    4. printStmt -> "print" expression ";";

- next, we need to turn our grammar into something we can store in memory
  - syntax trees
- the operands of + are always expressions and contents of while loop aren't
- split expressions and statement into seperate classes
- global variables -- rather simple since we don't have to care about scope
- variable syntax
  - if (monday) var beverage = "espresso" --> let's disallow this like C + Java
  - new kind of statement: declaration
  - program -> declaration* EOF;
  - declaration -> varDecl | statement;
  - statement ->exprStmt | printStmt;
  - right now, we only include variables, but later we'll add functions and classes
  - varDecl -> "var" IDENTIFIER ("=" expression)? ";";
  - primary -> "true" | "false" | "nil" | NUMBER | STRING | "(" expression ")" \
      | IDENTIFIER
  - IDENTIFIER is the name of the token being accessed
  - variable needs to be stored somewhere
  - data structure can be called an environment
    - key: variable name, value: values
    - we'll chose to not differentiate between reassignment and assignment
      - this is nice for catching bugs but not helpful for repl
- some languages don't allow reassignment
  - mutating a variable is a side effect and some people think it's dirty or inelegant
- expression -> assignment;
- assignment -> IDENTIFIER "=" assignment | equality;
- x = 5; => we have an l-value and r-value here
- l-value is a storage location that you can assign to
- now that we have assignment (x = 2) -- after x has been defined
- our language is as complicated as basic
- next, we need to add scope so we dont have programs accidently step on eachother
- there is dynamic scoping and static scoping {} verus functions/classes
- scope and environments are close cousins
- in lox, we use C-style so block scope {}
- basic algorithm
  - as we visit each statement inside a block, keep track of any variables declared
  - after the lastement is executed
  - tell the environment to delete all of those variables
  - misses one case, volume = 0 { volume = 1} etc
  - we need to chain the environments together
    - when we need a variable, we start from innermost until we find var
    - BLOCK -> TOP LEVEL (GLOBAL)
- statement -> exprStmt | printStmt | block;
- block -> "{" declaration* "}";
