
##  [Chapter 2] A Map of the Territory

Stages:
  - scanning or lexing or even lexical analysis
    - linear stream of chars and chunks into words
    - typically referred to as tokens
    - can be single chars like ( or , or "hi!" or min
    - some have no meaning like spaces or comments
      - scanners will skip these
  - Parsing 
    - where our syntax gets grammar
    - grammar is the ability to compose larger expressions
    - flat sequence of tokens -> builds a tree that mirrors the nested nature
    - these trees are sometimes called "parse tree" or "abstract syntax trees" (ASTs)
    - ex) average -> / -> (+ -> (min, max), 2)
    - is also responsible for telling us about syntax errors!
  - Static analysis
    - the first bit of analysis is called binding or resolution
    - for each identifier, we find locate the def and wire together
      - scope matters here > the region of source code where the variable is declared
    - if we have a statically typed language, we would do we type checking
      - ex) a + b >> verify that we can add these two types otherwise throw TypeError
    - we store this information (attributes) on the syntax tree itself
    - also store off to the side in a look-up table (symbol tree)
    - this is the last step in what's considered the front end
  - Intermediate representation
    - an interface between the source language and destination
    - Let's say you want to support Pascal, C, and Fortran compilers and target x86, arm
      - you would have to write 6 compilers (all combinations)
    - a shared IR can reduce this -- you would just need one front end for each that produces each source language
    - But the next step is another reason we might want an IR
  - Optimization
    - Once we understand the program, we are free to swap it out with a different program
    - ** As long as it has the same semantics **
    - ex) constant folding area = 3.14 * (1 / 2) * (7 / 8)
    - the compiler can do the calculation and swap the value
  - Code generation (or code gen)
    - produce assembly-like instructions a CPU runs
    - we are out of middle end into the back end here
    - move more and more into what the machine understands
    - we can either generate instructions for the real CPU ora  virtual one
    - native code is very fast  but generating it is a lot of work
      - tons  of instructions, complex pipelines, and historical baggage
    - you also become tied to the arch.
    - to get around this, hackers made their compiler produce virtual machine code
      - produce code for some ideal machine (was called p-code -- portable)
      - today it's called bytecode bc each instructions is usually one byte long
    - dense binary encoding of the language's low-level operations
  - Virtual Machine
    - If your compiler produces byte code, there is more work left
    - You can write a mini compiler for each target arch that goes byte code -> native code
    - or, write a virtual machine which emulates hypothetical chip supporting your VM at runtime
    - Running bytecode in a VM is slower than translating it to native code ahead of time
      - bc every instruction must be simulated at runtime each time we run
      - but in return, you get portability and simplicity
    - ex) If you implement your VM in C, you can run your language on any platform that has a C compiler
  - Runtime
    - Now that the users code is runnable on our platform, we need to run it
    - If we compiled to machine code, we tell the OS to load the executable and done
    - If we compiled it to bytecode, we start up the VM and load the program into that
    - There will likely be additional things
      - if our program manages memory, we need a GC
      - If our language supports `instance of`, we need to track the type of each object during exec
      - the runtime gets passed into the VM along with byte code

Shortcuts and Alternate Routes
  <!-- - single pass compilers -->
  <!--   - some simple compilers interleave parsing, analysis, and code generation -->

LEFT OFF: Page 16
