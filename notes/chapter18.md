# Chapter 18 - Type Values

- Lox is dynamically typed
  - a single variable can hold a boolean, number, or string at different points
  - right now, everything is a number
  - by end of chapter, we'll support Booleans an nil
- force us to figure out how over value rep can dynamically handle different types
- tagged unions
  - how do we represent the type of a value
  - how do we store the value itself
  - tagged unions is the most simple idea while not the most efficient
  - we define an enum which kind of type we support
  - we need to define different payload size based on the type
  - this can be done with a union
    - union = struct but fields overlap
- currently, our types are immutable
- we need to add OP_NIL, OP_TRUE, OF_FALSE since we added them as types to our scanner
