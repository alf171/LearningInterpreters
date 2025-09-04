# Chapteer 25 - Calls and Functions

- the most interesting change we will have to make to support functions and calls
- is to our stack
- we already have a stack for local variables and temporaries
- however, we have notion of a call stack
- let's start with representing functions from a vm standpoint
  - a function is abod that can be executed
  - we could compile everything into a monolith chunk
  - each fn would point to first instruction
  - this is how native code works but we will abstract
  - lets give each function a chunk
- since function are first class, they will have to be objects
  - arity field stores the number of parameters expected
  - we also store a chunk and function name
