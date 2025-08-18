# Chapter 11 Resolving and Bindings

- writing a language is about caring about correctness
- even to the deepest and dampest corners of semantics
- we fix the scope leak introduce by previous chapter
- gain a further understanding of lexical scope
- also learn about semantic analysis
  - powerful tool for extracting meaning with running code
- Static Scope
  - lox uses lexical scoping like most languages
- in our interpreter, environments are the dynamic manifestation of scopes
- a block is not all actually the same as a scope
- { var a; var b; }
  - but we don't capture the scope after defining a and at end (unique points)
- persistent environments
  - persistent data structures
    - can never be directly modified
    - one option is each variable to have its own env
  - semantic analysis
    - our interpreter resolves a variable, tracks down the reference, and evaluates it each time
    - we only need to resolve once actually
    - write something which evaluates code, find every variable, and figure dependence
    - this process is called semantic analysis
    - parser only knows if a program is semantically correct
    - question is when should we do this calculation
    - in the parser (clox does this) but for now let's put in separate pass for learning
- a variable resolution pass
  - things we care about:
      1. a block statement
        - introduces a new scope for the statements it contains
      2. a function declaration
        - introduces a new scope for its body and binds its parameters in that scope
      3. a variable declaration
        - adds a new variable to the current scope
      4. variable and assignment expressions
        - need to have their variables resolved
