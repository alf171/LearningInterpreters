# Chapter 30 - Optimization

- taking a working application and improving performance across many axes
- reducing number of resources to do something
- all kinds of optimizations other than runtime speed
  - memory usage, startup time, persistent storage size, or network bandwith

"there was a time when skilled programmers could hold the entire hardware ach and compiler pipeline in their head"

- those days are long gone
- we sort of pretend C is a low level programming language

- different lox programs will stress different areas of the VM
  - not likely to improve all programs or archs
- we will utilize benchmarks to guide us
  - while tests pin down sematic correctnes
  - often you'll find that some benchmarks get faster while others slower
    - chose what types of programs your language is optimized for
- Profiling
  - track resource usage while your program runs
  - helps with the system is too complex to fully reason about
  - simple ones show: how much time in each function
  - more complex show cache misses, instruction cache misses, branch mispredictions, mem
- Faster Hash Table Probing
  - we are spending a lot of our time on `tableGet` doing hash lookups
  - in particular, modulo operator is pretty slow in the get
  - we have some more knowledge of the problem that our CPU does
  - our table starts at size 8 and doubles (always power of two)
    - you can actually get the mod of something by & with power of two (minus 1)
      - this minus 1 gives series of 1s (10000 to 01111)
      - in fact, this minus one happens pretty often so we can cache it wrap in table
- Nan Boxing
  - next optim is a very different feel
  - sometimes called nan tagging
  - Value type takes up 16 bytes, struct has two fields, type tag and a union of payload
  - if we could reduce this, we could press more memory into our system
  - most computer have plenty of RAM now but reducing size helps with our caches
  - part of the issue with dynamic langs is we need to squril away types into our objs
  - particularily good for languages where all nums are double precision floats
    - like JS and Lua and Lox :)
  - almost all machines use IEEE 754 for float representation
  - fraction, mantissa, or sig bits
  - in our case, we care about the special case when all exp bits are set (NAN)
  - there are actually two kinds of NANs (signaling NaNs and quiet NaNs)
    - signalling is for erroneous computation like div by 0
    - quiet NaNs are safer to use (all exp set) leaves some bits unused
    - this means a float has enough room to store NaN plus 51 bits of data we can use
  - dont pointer need 64 bits on 64 bit arch >> most really only use 48 bits
  - using these bits for representing nil, bols, and obj pointers in nan boxing
  - we are actually going to put NaN boxing behind a flag
    - since it wont work for 32 bit systems
  - we will use type punning to trick the compiler to accept bits we pass through
  - code example

```{c}
static inline Value numToValue(double num) {
  Value value;
  memcpy(&value, &num, sizeof(double));
  return value;
}
```

- most compiler know to optimize away the memcpy entirely
- objects are the hardest part to implement with NaN boxing
- small catches about using a == b for floats
  - NaN values are not equal to themselves
  - special case `var nan = 0/0; print nan == nan;` this should be falsevalue

- future places to go with LOX
  - grab a classic compiler book and rebuild the FE of lox with a compilation pipeline
  - can add interesting intermediate representation and optimization passes
  - dynamic typing is somewhat restrictive in this regard
  - maybe add static typing or a type checker to lox

- this book has been correct but not rigorous
