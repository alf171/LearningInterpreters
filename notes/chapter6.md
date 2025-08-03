
## Chapter 6 Parsing Expressions

  - writing a real parser is difficult
  - in this chapter, we will create one
  - we have classes to represent a syntax tree
    - next we will go from tokens into one of these syntax trees
  - parser: given a string (or series of tokens), we map those to terminals in the grammar to figure out which rules could have generated that string
  - ambiguity means the parser may misunderstand the users code 
  - as we parse, we aren't just consider if the string is valid Lox, but we're also tracking rules -> tokens
  - ex) 6 / 3 - 1 -- more than one valid arrangement
    - binary (NUMBER(6), /, binary (3, -, 1))
    - binary (binary(6, /, 3), -, 1)
  - mathematicians solved this by introducing ordering
  - higher "precedence" operations are said to "bind tighter"
  - left-associativity
    - left evaluate before those on the right
    - 5 - 3 - 1 ~= (5 - 3) - 1
  | Name | Operators | Associates |
  | --------------- | --------------- | --------------- |
  | Equality | ==, != | Left |
  | Comparison | >, >=, <, <= | Left |
  | Term | -, + | Left |
  | Factor | / * | Left |
  | Unary | ! - | Left |
--
  - we are going to add some metadata to what our parser generates  
  - expression -> equality -> comparison -> term -> factor -> unary -> primary
  - we can also have rules for precedence levels
    - Recursive Descent Parsing
      - doesn't require a complex parser generator tools like Yacc, Bison, or ANTLR
      - instead we will use some hand written code
      - used by gc, v8, roslyn
      - top down parser
      - start from TOP so expression in our case
      - works into nested subexpressions reaching the leaves
      - called recursive bc a grammar rule can refer to itself
  - Syntax errors
    - given valid tokens, produce corresponding syntax trees
    - invalid sequence, detect any errors and tell the user about it
    - modern IDEs do this a ton to provide syntax highlighting
    - parser cant segfault or hang. all input needs to be valid
    - be fast, parsing is occurring constantly 
    - also should report all errors separately
  - Panic mode error recovery
    - as soon as parser detects error, we enter panic mode
    - we then need to recover in some sort of valid state to continue parsing
      - this is called synchronization
  - 
